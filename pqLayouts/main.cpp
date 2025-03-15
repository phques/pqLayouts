// Copyright 2021 Philippe Quesnel  
//
// This file is part of pqLayouts.
//
// pqLayouts is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// pqLayouts is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with pqLayouts.  If not, see <http://www.gnu.org/licenses/>.


#include "stdafx.h"
#include "pqLayoutsHook.h"
#include "LoLevelKbdFile.h"
#include "KbdDisplayWnd.h"
#include "resource.h"
#include "../StaticLib1/util.h"

// we need commctrl v6 for LoadIconMetric()
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")


namespace
{
    const TCHAR* const windowTitle = TEXT("PQLayouts");
    HINSTANCE hAppInstance = NULL;
    KbdDisplayWnd* helpWnd = nullptr;

    enum WndMessages
    {
        WMAPP_NOTIFYCALLBACK = WM_USER,
        WMAPP_KBDHOOK_NOTIF 
    };
}

// debug
namespace
{
    SHORT charToVk[256] = {0};

    void InitCharToVk()
    {
        // not all are valid, but simpler to let API handle it
        for (size_t i = 0; i <= 127; i++)
        {
            charToVk[i] = VkKeyScanA(static_cast<CHAR>(i));
        }
    }

    void addMapping(WORD fromVk, bool shiftedFrom, WORD toVk, bool shiftedTo)
    {
        KeyValue kfrom(fromVk, 0, shiftedFrom);
        KeyValue kto(toVk, 0, shiftedTo);

        HookKbd::AddMapping(kfrom, kto);
    }

    void addCtrlMapping(WORD fromVk, bool shiftedFrom, WORD toVk, bool shiftedTo)
    {
        KeyValue kfrom(fromVk, 0, shiftedFrom);
        KeyValue kto(toVk, 0, shiftedTo, true);

        HookKbd::AddMapping(kfrom, kto);
    }

    void addMapping(CHAR from, CHAR to)
    {
        // this returns flags for shift etc in upper byte
        SHORT qwertyVk = charToVk[from];
        SHORT outputVk = charToVk[to];

        addMapping(qwertyVk & 0xFF, VkUtil::HasShiftBit(qwertyVk),
                   outputVk & 0xFF, VkUtil::HasShiftBit(outputVk));
    }

    void addMappingSh(CHAR from, CHAR to)
    {
        // this returns flags for shift etc in upper byte
        SHORT qwertyVk = charToVk[from];
        SHORT outputVk = charToVk[to];

        // on shift layer
        addMapping(qwertyVk & 0xFF, true,
                   outputVk & 0xFF, VkUtil::HasShiftBit(outputVk));
    }

    void addMapping(bool shiftLayer, const char* mask, const char* map)
    {
        while (*mask != 0)
        {
            if (shiftLayer)
                addMappingSh(*mask++, *map++);
            else
                addMapping(*mask++, *map++);
        }
    }


}; // namespace

namespace
{
    //---------

    void DeleteIcon(HWND hwnd) 
    {
        NOTIFYICONDATA nid = {0};
        nid.cbSize = sizeof(nid);
        nid.hWnd = hwnd;
        nid.uID = 0;
        Shell_NotifyIcon(NIM_DELETE, &nid);
    }

    void AddIcon(HWND hwnd)
    {
        NOTIFYICONDATA nid = {0};
        nid.cbSize = sizeof(nid);
        nid.hWnd = hwnd;
        nid.uID = 0;
        nid.uCallbackMessage = WMAPP_NOTIFYCALLBACK;
        nid.uFlags = NIF_MESSAGE;
        nid.uVersion = NOTIFYICON_VERSION_4;

        // add icon & set version
        Shell_NotifyIcon(NIM_ADD, &nid);
        Shell_NotifyIcon(NIM_SETVERSION, &nid);
    }

    void SetIcon(HWND hwnd, bool enabled)
    {
        NOTIFYICONDATA nid = {0};
        nid.cbSize = sizeof(nid);
        nid.hWnd = hwnd;
        nid.uID = 0;
        nid.uFlags = NIF_ICON;

        // set the icon 
        int iconId = (enabled ? IDI_ICON1 : IDI_ICON2);
        LoadIconMetric(hAppInstance, MAKEINTRESOURCE(iconId), LIM_SMALL, &nid.hIcon);
        //nid.hIcon = LoadIcon(hAppInstance, MAKEINTRESOURCE(iconId));

        Shell_NotifyIcon(NIM_MODIFY, &nid);

    }

    void ShowContextMenu(HWND hwnd, POINT pt)
    {
        HMENU hMenu = LoadMenu(hAppInstance, MAKEINTRESOURCE(IDR_MENU1));
        if (hMenu)
        {
            HMENU hSubMenu = GetSubMenu(hMenu, 0);
            if (hSubMenu)
            {
                // our window must be foreground before calling TrackPopupMenu or the menu will not disappear when the user clicks away
                SetForegroundWindow(hwnd);

                // respect menu drop alignment
                UINT uFlags = TPM_RIGHTBUTTON;
                if (GetSystemMetrics(SM_MENUDROPALIGNMENT) != 0)
                {
                    uFlags |= TPM_RIGHTALIGN;
                }
                else
                {
                    uFlags |= TPM_LEFTALIGN;
                }

                TrackPopupMenuEx(hSubMenu, uFlags, pt.x, pt.y, hwnd, NULL);
            }
            DestroyMenu(hMenu);
        }
    }

} // namespace


bool OnAppNotify(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
    switch (LOWORD(lParam))
    {
    case NIN_SELECT:
        // user clicked our taskbar icon, toggle suspended status
        HookKbd::ToggleSuspend();
        return true;

    case WM_LBUTTONDBLCLK:
        //printf("WM_LBUTTONDBLCLK\n");
        break;

    case WM_CONTEXTMENU:
        {
            // open context menu
            POINT const pt = { LOWORD(wParam), HIWORD(wParam) };
            ShowContextMenu(hDlg, pt);
            return true;
        }
    }
    return false;
}

bool OnCommand(HWND hDlg, WPARAM wParam)
{
    switch (LOWORD(wParam))
    {
    case ID_POPUP_TOGGLESUSPEND:
    case IDC_BUTTON_SUSPEND:
        HookKbd::ToggleSuspend();
        return true;

    case ID_POPUP_EXIT:
    case IDOK:
    case IDCANCEL:
        SendMessage(hDlg, WM_CLOSE, 0, 0);
        return true;
    }
    return false;
}

void OnHookNotify(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
    const auto notif = static_cast<HookKbd::Notif>(wParam);

    switch (notif)
    {
    case HookKbd::SuspendStateUpdated:
    {
        // update our icon (for enabled/disabled status)
        const bool suspended = (lParam == 1 ? true : false);

        Printf("main, notif SuspendStateUpdated %d\n", lParam);
        SetIcon(hDlg, !suspended);

        const auto visible = !suspended;
        helpWnd->DisplayWindow(visible);
        break;
    }
    case HookKbd::LayerChanged:
    {
        Printf("main, notif LayerChanged %d\n", lParam);

        const auto imageView = HookKbd::GetImageView();
        helpWnd->SetImageView(imageView);
        break;
    }
    }
}

INT_PTR CALLBACK DialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) 
{

    switch (message)
    {
    case WM_INITDIALOG:
        SetWindowText(hDlg, windowTitle);
        return TRUE;

    case WM_CLOSE:
        DestroyWindow(hDlg);
        return TRUE;

    case WM_DESTROY:
        PostQuitMessage(0);
        return TRUE;

        // taskbar icon notif
    case WMAPP_NOTIFYCALLBACK:
        if (OnAppNotify(hDlg, wParam, lParam)) 
            return TRUE;
        break;

    // hook dll notification
    case WMAPP_KBDHOOK_NOTIF:
        OnHookNotify(hDlg, wParam, lParam);
        return TRUE;

    case WM_COMMAND:
        if (OnCommand(hDlg, wParam)) 
            return TRUE;
        break;

    }

    return FALSE;
}

void CreateKbdImageWindow(HINSTANCE hInstance, KbdDisplayWnd*& helpWnd, int screen1Scale, int screen2Scale)
{
    const auto* pcImageFilename = HookKbd::GetImageFilename();
    if (pcImageFilename != nullptr)
    {
        // Create a 'kbd image' window
        helpWnd = new KbdDisplayWnd(hInstance, screen1Scale, screen2Scale);

        // create a wide string from imageFilename
        //std::string basicString(pcImageFilename);
        //std::wstring wideString = std::wstring(basicString.begin(), basicString.end());

        //helpWnd->SetImageFile(wideString.c_str());

        helpWnd->SetImageFile(pcImageFilename);
        helpWnd->SetImageView(HookKbd::GetImageView());

        helpWnd->DisplayWindow(true);

    }
}

int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR commandLine, int nCmdShow) 
{
    // Check if there is already an instance running.
    // mutex auto closed on exit
    CreateMutex(nullptr, 0, TEXT("_PqLayouts Main_"));
    if (GetLastError() == ERROR_ALREADY_EXISTS) 
    {
        MessageBox(0, TEXT("Oooppps"), TEXT("Ooops"), MB_OK);
        return 0;
    }

    hAppInstance = hInstance;

    // Create our dialog
    HWND hDlg = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), nullptr, DialogProc);
    ShowWindow(hDlg, nCmdShow);

    // read keyboard definition
    bool ok = true;
    LoLevelKbdFile lokbdrdr;

    //##pq-debug
    //createPLLTx1dMapping();

    // read kbd def file
    int scale1=100, scale2=100;
    if (commandLine != nullptr && strlen(commandLine) != 0)
    {
        char* cmdline = commandLine;

        //##big patch, 1st param can be something like "-100x125" to read in the scaling factor of screens one and two
        if (sscanf_s(commandLine, "-%dx%d", &scale1, &scale2) == 2)
        {
            printf("scale1 %d scale2 %d\n", scale1, scale2);

            //skip param, then any space between the two params
            while (!isspace(*cmdline) && *cmdline != '\0')
                ++cmdline;
            while (isspace(*cmdline) && *cmdline != '\0')
                ++cmdline;
        }
        // strip prefix/siffix double quotes ! (when called with "Open With" from Windows)
        if (cmdline[0] == '"')
        {
            cmdline[strlen(cmdline)-1] = 0;
            ++cmdline;
        }

        ok = lokbdrdr.ReadKeyboardFile(cmdline);

        // always start on main layer
        HookKbd::GotoMainLayer();
    }

    if (ok)
    {
        // initialize GDI+
        ULONG_PTR gdiplusToken;
        Gdiplus::GdiplusStartupInput gdiplusStartupInput;
        Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, nullptr);

        // setup taskbar icon
        AddIcon(hDlg);
        SetIcon(hDlg, !HookKbd::Suspended());

        // show the kbd help image if we have one
        CreateKbdImageWindow(hInstance, helpWnd, scale1, scale2);

        // pause to suspend key mapping
        // ctrl-pause (VK_CANCEL) to stop/close the app
        HookKbd::SetNotificationMessage(hDlg, WMAPP_KBDHOOK_NOTIF);
        HookKbd::SuspendKey(VK_PAUSE, VK_CANCEL);

        HookKbd::HookKbdLL();

        // message loop
        MSG msg;
        while (GetMessage(&msg, nullptr, 0, 0) > 0)
        {
            if (!IsDialogMessage(hDlg, &msg)) 
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }

        // cleanup
        if (helpWnd != nullptr)
            helpWnd->ClearImage();
        DeleteIcon(hDlg);
        HookKbd::UnhookKbdLL();
        Gdiplus::GdiplusShutdown(gdiplusToken);
    }

    return 0;
}


// we build app in console subsystem so we can use printf & cie to debug !
// compile with windows subsystem in Release
// call WinMain
int main(int argc, char* argv[])
{
    std::string cmdline;

    for (int i = 1; i < argc; ++i)
    {
        cmdline += argv[i];
        if (i + 1 < argc )
            cmdline += " ";
    }

    const size_t len = cmdline.length()+1;
    char* buff = new char[len];
    strcpy_s(buff, len, cmdline.c_str());

    WinMain(GetModuleHandle(nullptr), 0, buff, SW_SHOW);

    delete [] buff;
    return 0;
}
