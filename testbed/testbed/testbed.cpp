
#include "pch.h"
#include "resource.h"

//-----

// enable visual styles
#pragma comment(linker, \
  "\"/manifestdependency:type='Win32' "\
  "name='Microsoft.Windows.Common-Controls' "\
  "version='6.0.0.0' "\
  "processorArchitecture='*' "\
  "publicKeyToken='6595b64144ccf1df' "\
  "language='*'\"")

#pragma comment(lib, "ComCtl32.lib")

//-------

INT_PTR CALLBACK DialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    //printf("dialogproc %0x %0x %0x\n", message, wParam, lParam);

    switch (message)
    {
    //case WM_COMMAND:
    //    switch (LOWORD(wParam)) {
    //    case IDOK:
    //    case IDCANCEL:
    //        SendMessage(hDlg, WM_CLOSE, 0, 0);
    //        return TRUE;
    //    }
    //    break;

    case WM_CLOSE:
        DestroyWindow(hDlg);
        return TRUE;

    case WM_DESTROY:
        PostQuitMessage(0);
        return TRUE;

    }

    return FALSE;
}

//-----

// return true to skip this completely
bool handleKbdMsg(MSG& msg)
{
    // ## need to handle keys up too

    if (msg.message == WM_KEYDOWN) {
        printf("wm_keydown %0x\n", msg.wParam);

        // prevent dialog close on Escape
        if (msg.wParam == VK_ESCAPE) {
            puts("skip escape");
            return true;
        }

        // # dbg replace a with b
        if (msg.wParam == 'A')
            msg.wParam = 'B';
    }

    if (msg.message == WM_SYSKEYDOWN) {
        printf("wm_syskeydown %0x\n", msg.wParam);
        //msg.wParam = 'B';
    }

    return false;
}


 int __stdcall wWinMain(HINSTANCE hInst, HINSTANCE h0, LPTSTR lpCmdLine, int nCmdShow)
{
     // create & show our dialog
    HWND hDlg;
    hDlg = CreateDialogParam(hInst, MAKEINTRESOURCE(IDD_DIALOG1), 0, DialogProc, 0);
    ShowWindow(hDlg, nCmdShow);

    // message loop
    BOOL ret;
    MSG msg;
    while ((ret = GetMessage(&msg, 0, 0, 0)) != 0) {
        if (ret == -1) /* error found */
            return -1;

        // handle keyboard messages 
        if (handleKbdMsg(msg))
            continue;  // skip this message


        // normal message processing
        if (!IsDialogMessage(hDlg, &msg)) {
            TranslateMessage(&msg); /* translate virtual-key messages */
            DispatchMessage(&msg); /* send it to dialog procedure */
        }
    }

    return 0;
}



 // we build app in console subsystem so we can use printf & cie to debug !
 // compile with windows subsystem in Release
 // call WinMain
 int main(int argc, char* argv[])
 {
     wWinMain(GetModuleHandle(0), 0, NULL, SW_SHOW);
     return 0;
 }
