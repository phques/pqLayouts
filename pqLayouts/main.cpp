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
    SHORT charToVk[256] = {0};

    // checks bit 0 of hibyte of ret val from VkKeyScanA
    bool HasShiftBit(SHORT scanExVal) 
    { 
        return ((scanExVal >> 8) & 0x01) != 0; 
    }

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

        AddMapping(kfrom, kto);
    }

    void addCtrlMapping(WORD fromVk, bool shiftedFrom, WORD toVk, bool shiftedTo)
    {
        KeyValue kfrom(fromVk, 0, shiftedFrom);
        KeyValue kto(toVk, 0, shiftedTo);

        AddCtrlMapping(kfrom, kto);
    }

    void addMapping(CHAR from, CHAR to)
    {
        // this returns flags for shift etc in upper byte
        SHORT qwertyVk = charToVk[from];
        SHORT outputVk = charToVk[to];

        addMapping(qwertyVk & 0xFF, HasShiftBit(qwertyVk),
                   outputVk & 0xFF, HasShiftBit(outputVk));
    }

    void addMappingSh(CHAR from, CHAR to)
    {
        // this returns flags for shift etc in upper byte
        SHORT qwertyVk = charToVk[from];
        SHORT outputVk = charToVk[to];

        // on shift layer
        addMapping(qwertyVk & 0xFF, true,
                   outputVk & 0xFF, HasShiftBit(outputVk));
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

    void createPLLTx1dMapping()
    {
        // PLLTx1(e) 'wide mod'
        // main
        addMapping(false,
            "weriopasdfgjkl;'zcvm,./",
            "iuomdnye aglrtsp.,..hcf"
        );

        // main shift
        addMapping(true,
            "WERIOPASDFGJKL;'ZCVM,./",
            "IUOMDNYE\"AGLRTSP:;..HCF"
        );

        // alt/secondary layer
        Layer::Idx_t layerIdx = 0;
        if (AddLayer("alt", layerIdx))
        {
            KeyDef accessKey(VK_SPACE,0);
            SetLayerAccessKey("alt", accessKey);

            GotoLayer(layerIdx);
            {
                // secondary layer 
                addMapping(false,
                    "weriopasdfgjkl;'zcvm,./",
                    "q'jv!#?(-)$+{=}&*/:zkxb"
                );

                // secondary layer shift
                addMapping(true,
                    "WERIOPASDFGJKL;'ZCVM,./",
                    "Q`JV|.<<_>-~[@]%^\\.ZKXB"
                );
            }
            GotoMainLayer();
        }

        // shift on CapsLock and Enter
        addMapping(VK_CAPITAL, false, VK_LSHIFT, false);
        addMapping(VK_CAPITAL, true, VK_CAPITAL, false);

        addMapping(VK_RETURN,  false, VK_RSHIFT, false);
        addMapping(VK_RETURN,  true, VK_CAPITAL, false);


        addMapping(charToVk['x'],  false, VK_RETURN, false);
        addMapping(charToVk['x'],  true, VK_RETURN, true);

        addMapping(charToVk['q'],  false, VK_TAB, false);
        addMapping(charToVk['q'],  true, VK_TAB, true);

        addMapping(charToVk['['],  false, VK_BACK, false);
        addMapping(charToVk['['],  true, VK_DELETE, false);

        addMapping(charToVk['y'],  false, VK_LEFT, false);
        addMapping(charToVk['u'],  false, VK_RIGHT,false);
        addMapping(charToVk['h'],  false, VK_UP,   false);
        addMapping(charToVk['n'],  false, VK_DOWN, false);
        addMapping(charToVk['y'],  true, VK_LEFT,  true);
        addMapping(charToVk['u'],  true, VK_RIGHT, true);
        addMapping(charToVk['h'],  true, VK_UP,    true);
        addMapping(charToVk['n'],  true, VK_DOWN,  true);

        addCtrlMapping(charToVk['v'],  false, 'C', false); // ctrl-c Copy
        addCtrlMapping(charToVk['b'],  false, 'V', false); // ctrl-v Paste
        addCtrlMapping(charToVk['t'],  false, 'X', false); // ctrl-x Cut
        addCtrlMapping(charToVk['`'],  false, 'Z', false); // ctrl-z UNdo

        /*todo
            Complete support for Ctrl-??

                  y u             Left Right
                  h               Up    
            v b   n      ^c ^v    Down   

         Put Fn keys on 1234 .., use Shift to access numbers ?
         (no numpad layer avail in this one)
         .. might need ShiftLock mode, will get annoying sometimes when lots of nums to enter

         French layout !

         Maybe support for layout definitions in text files ! ;-)
        */
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
    InitCharToVk();
    //testMappings();
    createPLLTx1dMapping();

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
