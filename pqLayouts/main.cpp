// Copyright 2020 Philippe Quesnel  
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
#include "resource.h"


namespace
{
    const wchar_t* const windowTitle = L"PQLayouts";

}

// debug
namespace
{
    bool addMapping(char from, char to, bool shifted)
    {
        // this returns flags for shift etc in upper byte
        SHORT qwertyVk = VkKeyScanExA(from, NULL);
        SHORT outputVk = VkKeyScanExA(to, NULL);
        printf("VkKeyScanExA %c = %0X\n", to, outputVk);

        bool ret = AddMapping("main", qwertyVk & 0xFF, outputVk & 0xFF, shifted);
        return ret;
    }

    void testMappings()
    {
        addMapping('A', 'G', false);
        addMapping('S', 'é', false);
        addMapping('d', 'à', false);
        addMapping('f', ' ', false);

        //AddMapping("main", 'A', 'G', false);
        //AddMapping("main", 'S', 'I', false);
        //AddMapping("main", 'D', 'O', false);
        //AddMapping("main", 'F', VK_SPACE, false);

        AddMapping("main", 'A', 'G', true);
        AddMapping("main", 'S', 'I', true);
        AddMapping("main", 'D', 'O', true);
        AddMapping("main", 'F', VK_OEM_7, true); // want '"' .. but we map *VIRTUAL KEYS*, need to specify if needs Shift

        AddMapping("main", VK_CAPITAL, VK_SHIFT, false);
        AddMapping("main", VK_RETURN, VK_RSHIFT, false);
        AddMapping("main", VK_CAPITAL, VK_SHIFT, true);
        AddMapping("main", VK_RETURN, VK_RSHIFT, true);
    }
}


INT_PTR CALLBACK DialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) 
{

    switch (message)
    {
    case WM_INITDIALOG:
        SetWindowText(hDlg, windowTitle);
        return TRUE;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDOK:
        case IDCANCEL:
            SendMessage(hDlg, WM_CLOSE, 0, 0);
            return TRUE;
        }
        break;

    case WM_CLOSE: 
        DestroyWindow(hDlg);
        return TRUE; 

    case WM_DESTROY:
        PostQuitMessage(0);
        return TRUE;

    }

    return FALSE;
}

int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR commandLine, int nCmdShow) 
{

    // Check if there is already an instance running.
    // mutex auto closed on exit
    CreateMutex(0, 0, L"_PqLayouts Main_");
    if (GetLastError() == ERROR_ALREADY_EXISTS) 
    {
        MessageBox(0, L"Oooppps", L"Ooops", MB_OK);
        return 0;
    }

    // Create our dialog
    HWND hDlg = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), 0, DialogProc);
    ShowWindow(hDlg, nCmdShow);

    //## dbg
    testMappings();

    HookKbdLL();
    //refreshIconState(hDlg);
    //SetTimer(hDlg, 1, 500, 0);

    MSG msg;
    int ret = 0;
    while (ret = GetMessage(&msg, 0, 0, 0) > 0) 
    {
        if (!IsDialogMessage(hDlg, &msg)) 
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    UnhookKbdLL();
    //refreshIconState(hDlg, true);

    return ret;
}


// we build app in console subsystem so we can use printf & cie to debug !
// compile with windows subsystem in Release
// call WinMain
int main(int argc, char* argv[])
{
    WinMain(GetModuleHandle(0), 0, NULL, SW_SHOW);
    return 0;
}
