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
    const TCHAR* const windowTitle = TEXT("PQLayouts");

}

// debug
namespace
{
    // checks bit 0 of hibyte of ret val from VkKeyScanExA
    bool ScanExIsShift(SHORT scanExVal) 
    { 
        return ((scanExVal >> 8) & 0x01) != 0; 
    }

    bool addMapping(CHAR from, CHAR to)
    {
        // this returns flags for shift etc in upper byte
        SHORT qwertyVk = VkKeyScanExA(from, NULL);
        SHORT outputVk = VkKeyScanExA(to, NULL);
        printf("VkKeyScanExA %c = 0x%0X\n", to, outputVk);

        KeyValue kfrom(qwertyVk & 0xFF,0, ScanExIsShift(qwertyVk));
        KeyValue kto(outputVk & 0xFF, 0, ScanExIsShift(outputVk));

        return AddMapping(kfrom, kto);
    }

    bool addMapping(WORD from, bool shiftedFrom, WORD to, bool shiftedTo)
    {
        KeyValue kfrom(from, 0, shiftedFrom);
        KeyValue kto(to, 0, shiftedTo);

        return AddMapping(kfrom, kto);
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
        // ? ( - ) $
        Layer::Idx_t layerIdx = 0;
        if (AddLayer("alt", layerIdx))
        {
            KeyDef accessKey(VK_SPACE,0);
            SetLayerAccessKey("alt", accessKey);

            GotoLayer(layerIdx);
            addMapping('a', '?');
            addMapping('s', '(');
            addMapping('d', '-');
            addMapping('f', ')');
            addMapping('g', '$');
            GotoMainLayer();
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
    CreateMutex(0, 0, TEXT("_PqLayouts Main_"));
    if (GetLastError() == ERROR_ALREADY_EXISTS) 
    {
        MessageBox(0, TEXT("Oooppps"), TEXT("Ooops"), MB_OK);
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
