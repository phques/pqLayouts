// pqLayouts project  
// Copyright 2020 Philippe Quesnel  
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


using namespace std;

namespace
{
    const wchar_t* const windowTitle = L"PQLayouts";
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

    // Check if there is already an instance running
    CreateMutex(0, 0, L"_PqLayouts Main_");
    if (GetLastError() == ERROR_ALREADY_EXISTS) 
    {
        MessageBox(0, L"Oooppps", L"Ooops", MB_OK);
        return 0;
    }

    // Create our dialog
    HWND hDlg = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), 0, DialogProc);
    ShowWindow(hDlg, nCmdShow);

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