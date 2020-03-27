//-- main.cpp

#include "stdafx.h"
#include "resource.h"

namespace
{
    const wchar_t* const windowTitle = L"PQLayouts";
}


INT_PTR CALLBACK DialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {

    switch (message) {

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
    if (GetLastError() == ERROR_ALREADY_EXISTS) {
        MessageBox(0, L"Oooppps", L"Ooops", MB_OK);
        return 0;
    }

    // Create our dialog
    //Hinst = hInstance;
    HWND hDlg = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), 0, DialogProc);
    ShowWindow(hDlg, nCmdShow);

    //InstallHook();
    //refreshIconState(hDlg);
    //SetTimer(hDlg, 1, 500, 0);

    MSG msg;
    int ret = 0;
    while (ret = GetMessage(&msg, 0, 0, 0) > 0) {
        if (!IsDialogMessage(hDlg, &msg)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    //RemoveHook();
    //refreshIconState(hDlg, true);

    return ret;
}
