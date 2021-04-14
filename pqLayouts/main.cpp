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

    void testMappings()
    {
        //addMapping('A', 'G', false);
        //addMapping('S', 'é', false);
        //addMapping('d', 'à', false);
        //addMapping('f', ' ', false);

        addMapping('a', 'g');
        addMapping('s', 'i');
        addMapping('d', 'o');
        addMapping('F', false, VK_SPACE, false);
        addMapping('g', '?');

        addMapping('A', 'G');
        addMapping('S', 'I');
        addMapping('D', 'O');
        addMapping('F', true, VK_OEM_7, true); // want '"' .. but we map *VIRTUAL KEYS*, need to specify if needs Shift

        // cannot use CapsLock as LeftShift,
        // when we send LShift on Caps down event,
        // (Windows?) we receive a CapsLock UP !!
        // (how did this work with Autohotkey !?)
        // This is while running in virtualbox on MacOS, could it be a Mac thing ?
        //-- IT WORKS on my Win10 64 host / Win10 64 guest setup,
        //   with my PC (Microsoft) keyboard
        addMapping(VK_CAPITAL, false, VK_LSHIFT, false);
        addMapping(VK_CAPITAL, true, VK_LSHIFT, true);
        //addMapping(VK_CAPITAL, false, 'A', true); // test shifted on non-shifted & vice versa
        //addMapping(VK_CAPITAL, true, 'A', false);

        addMapping(VK_RETURN,  false, VK_RSHIFT, false);
        addMapping(VK_RETURN,  true, VK_RSHIFT, true);

        //------------
        //asdfg: ? ( - ) $
        Layer::Idx_t layerIdx = 0;
        if (HookKbd::AddLayer("alt", layerIdx))
        {
            KeyDef accessKey(VK_SPACE,0);
            HookKbd::SetLayerAccessKey("alt", accessKey, false);

            HookKbd::GotoLayer(layerIdx);
                addMapping('a', '?');
                addMapping('s', '(');
                addMapping('d', '-');
                addMapping('f', ')');
                addMapping('g', '$');
            HookKbd::GotoMainLayer();
        }
    }

    void SetPLLT1xeShifts()
    {
        // shift on CapsLock and Enter
        // shift-shift == CapsLock
        addMapping(VK_CAPITAL, false, VK_LSHIFT, false);
        addMapping(VK_CAPITAL, true, VK_CAPITAL, false);

        addMapping(VK_RETURN,  false, VK_RSHIFT, false);
        addMapping(VK_RETURN,  true, VK_CAPITAL, false);

        // Control on Shift
        addMapping(VK_LSHIFT, false, VK_LCONTROL, false);
        addMapping(VK_LSHIFT, true, VK_LCONTROL, false);

        addMapping(VK_RSHIFT,  false, VK_RCONTROL, false);
        addMapping(VK_RSHIFT,  true, VK_RCONTROL, false);
    }

    void createPLLTx1dMapping()
    {
        // PLLTx1(e) 'wide mod'

        // --- main layer ---

        addMapping(false,
            //"weriuopasdfgjkl;'zc,./",
            //"iuomwdnye aglrtsp.,hcf"
            "weriopasdfgjkl;'zcm,./",
            "iuomdnye aglrtsp.,whcf"
        );

        // main shift
        addMapping(true,
            //"WERIUOPASDFGJKL;'ZC,./",
            //"IUOMWDNYE\"AGLRTSP:;HCF"
            "WERIOPASDFGJKL;'ZCM,./",
            "IUOMDNYE\"AGLRTSP:;WHCF"
        );

        // Fn keys on digits row / digits on shift
        const char* digits = "1234567890-=";
        for (size_t i = 0; i <= strlen(digits); i++)
        {
            WORD vk = VK_F1 + static_cast<WORD>(i);
            char digit = digits[i];
            addMapping(charToVk[digit],  false, vk, false);
            addMapping(charToVk[digit],  true, charToVk[digit], false);
        }

        SetPLLT1xeShifts();

        // Return on v
        addMapping(charToVk['v'],  false, VK_RETURN, false);
        addMapping(charToVk['v'],  true, VK_RETURN, true);

        // add other special keys on main
        addMapping(VK_TAB,  false, VK_ESCAPE, false);
        addMapping(VK_TAB,  true, VK_ESCAPE, true);
        addMapping(charToVk['q'],  false, VK_TAB, false);
        addMapping(charToVk['q'],  true, VK_TAB, true);
        addMapping(charToVk['['],  false, VK_BACK, false);
        addMapping(charToVk['['],  true, VK_DELETE, false);

        // cursor & edit keys
        //addMapping(charToVk['y'],  false, VK_LEFT, false);
        //addMapping(charToVk['y'],  true, VK_LEFT,  true);
        //addMapping(charToVk['u'],  false, VK_RIGHT,false);
        //addMapping(charToVk['u'],  true, VK_RIGHT, true);
        //addMapping(charToVk['h'],  false, VK_UP,   false);
        //addMapping(charToVk['h'],  true, VK_UP,    true);
        //addMapping(charToVk['n'],  false, VK_DOWN, false);
        //addMapping(charToVk['n'],  true, VK_DOWN,  true);

        //addMapping(charToVk['n'],  false, VK_LEFT, false);
        //addMapping(charToVk['n'],  true, VK_LEFT,  true);
        //addMapping(charToVk['m'],  false, VK_RIGHT,false);
        //addMapping(charToVk['m'],  true, VK_RIGHT, true);
        //addMapping(charToVk['y'],  false, VK_UP,   false);
        //addMapping(charToVk['y'],  true, VK_UP,    true);
        //addMapping(charToVk['h'],  false, VK_DOWN, false);
        //addMapping(charToVk['h'],  true, VK_DOWN,  true);

        addMapping(charToVk['u'],  false, VK_RIGHT,false);
        addMapping(charToVk['u'],  true, VK_RIGHT, true);
        addMapping(charToVk['h'],  false, VK_LEFT, false);
        addMapping(charToVk['h'],  true, VK_LEFT,  true);
        addMapping(charToVk['y'],  false, VK_UP,   false);
        addMapping(charToVk['y'],  true, VK_UP,    true);
        addMapping(charToVk['n'],  false, VK_DOWN, false);
        addMapping(charToVk['n'],  true, VK_DOWN,  true);

        addCtrlMapping(charToVk['x'],  false, 'C', false); // ctrl-c Copy
        addCtrlMapping(charToVk['b'],  false, 'V', false); // ctrl-v Paste
        addCtrlMapping(charToVk['t'],  false, 'X', false); // ctrl-x Cut
        addCtrlMapping(charToVk['`'],  false, 'Z', false); // ctrl-z UNdo


        // --- alt/secondary layer ---

        Layer::Idx_t layerIdx = 0;

        // will set layerIdx
        if (HookKbd::AddLayer("alt", layerIdx))
        {
            HookKbd::SetLayerAccessKey("alt", KeyDef(VK_SPACE,0), false);

            HookKbd::GotoLayer(layerIdx);
            {
                // secondary layer 
                addMapping(false,
                    //"weruiopasdfgjkl;'zxcv,./",
                    //"q'jzv!#?(-)@+{=}&*|/_kxb"
                    "weriopasdfgjkl;'zxcvm,./",
                    "q'jv!#?(-)@+{=}&*|/_zkxb"
                );

                // secondary layer shift
                addMapping(true,
                    "WERIOPASDFGJKL;'ZXCVM,./",
                    "Q`JV|.<<_>-~[$]%^=\\.ZKXB"
                );

                // set shift keys on alt/secondary layer 
                SetPLLT1xeShifts();

                //addMapping(charToVk['y'],  false, VK_HOME, false);
                //addMapping(charToVk['y'],  true, VK_HOME,  true);
                //addMapping(charToVk['u'],  false, VK_END,false);
                //addMapping(charToVk['u'],  true, VK_END, true);
                //addMapping(charToVk['h'],  false, VK_PRIOR, false);
                //addMapping(charToVk['h'],  true, VK_PRIOR,  true);
                //addMapping(charToVk['n'],  false, VK_NEXT, false);
                //addMapping(charToVk['n'],  true, VK_NEXT,  true);

                //addMapping(charToVk['n'],  false, VK_HOME, false);
                //addMapping(charToVk['n'],  true, VK_HOME,  true);
                //addMapping(charToVk['m'],  false, VK_END,false);
                //addMapping(charToVk['m'],  true, VK_END, true);
                //addMapping(charToVk['y'],  false, VK_PRIOR, false);
                //addMapping(charToVk['y'],  true, VK_PRIOR,  true);
                //addMapping(charToVk['h'],  false, VK_NEXT, false);
                //addMapping(charToVk['h'],  true, VK_NEXT,  true);

                addMapping(charToVk['h'],  false, VK_HOME, false);
                addMapping(charToVk['h'],  true, VK_HOME,  true);
                addMapping(charToVk['u'],  false, VK_END,false);
                addMapping(charToVk['u'],  true, VK_END, true);
                addMapping(charToVk['y'],  false, VK_PRIOR, false);
                addMapping(charToVk['y'],  true, VK_PRIOR,  true);
                addMapping(charToVk['n'],  false, VK_NEXT, false);
                addMapping(charToVk['n'],  true, VK_NEXT,  true);
            }
            HookKbd::GotoMainLayer();

            //test, \ is the 'make sticky' key
            KeyValue makeSticky(charToVk['\\'], 0, false);
            HookKbd::AddStickyMapping(makeSticky);
        }

        /*todo

         Put Fn keys on 1234 .., use Shift to access numbers ?
         (no numpad layer avail in this one)
         .. might need ShiftLock mode, will get annoying sometimes when lots of nums to enter

         French layout !

         Maybe support for layout definitions in text files ! ;-)
        */
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

void CreateKbdImageWindow(HINSTANCE hInstance, KbdDisplayWnd*& helpWnd)
{
    const auto* pcImageFilename = HookKbd::GetImageFilename();
    if (pcImageFilename != nullptr)
    {
        // Create a 'kbd image' window
        helpWnd = new KbdDisplayWnd(hInstance);

        // create a wide string from imageFilename
        //std::string basicString(pcImageFilename);
        //std::wstring wideString = std::wstring(basicString.begin(), basicString.end());

        //helpWnd->SetImageFile(wideString.c_str());

        helpWnd->SetImageFile(pcImageFilename);
        helpWnd->SetImageView(HookKbd::GetImageView());
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
    if (commandLine != nullptr && strlen(commandLine) != 0)
    {
        // strip prefix/siffix double quotes ! (when called with "Open With" from Windows)
        char* cmdline = commandLine;
        if (commandLine[0] == '"')
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
        CreateKbdImageWindow(hInstance, helpWnd);

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
    WinMain(GetModuleHandle(0), 0, (argc == 2) ? argv[1] : LPSTR(""), SW_SHOW);
    return 0;
}
