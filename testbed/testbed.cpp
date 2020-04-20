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

#include "pch.h"
#include "resource.h"
#include "util.h"

using namespace luabridge;

class MyLua : public LuaState
{
public:
    MyLua() : LuaState(luaL_newstate())
    {
        luaL_openlibs(luaL);
        enableExceptions(luaL);

        // add ../lua to lua path
        LuaRef luaPackage = GetRef("package");

        std::string path = luaPackage["path"];
        path = "../lua/?;../lua/?.lua;" + path;

        luaPackage["path"] = path;
    }

    bool LoadScript(const char* script)
    {
        try
        {
            // load our lua script
            if (luaL_dofile(luaL, script) != LUA_OK)
            {
                // get error from stack
                std::string err = PopString();
                err = "could not load lua script:\n" + err;

                MessageBoxA(NULL, err.c_str(), "testbed", MB_OK);
                return false;
            }
        }
        catch (const LuaException& e)
        {
            Printf("luaexception %s\n", e.what());
            return false;
        }

        return true;
    }
};

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
    // is it a keybd message?
    if (!(msg.message == WM_KEYDOWN || msg.message == WM_SYSKEYDOWN ||
          msg.message == WM_KEYUP || msg.message == WM_SYSKEYUP))
        return false;

    // prevent dialog close on Escape
    if (msg.message == WM_KEYDOWN && msg.wParam == VK_ESCAPE) {
        puts("skip escape");
        return true;
    }

    bool down = (msg.message == WM_KEYDOWN || msg.message == WM_SYSKEYDOWN);

    return false;
}

#ifdef  UNICODE
int __stdcall wWinMain(HINSTANCE hInst, HINSTANCE h0, LPTSTR lpCmdLine, int nCmdShow)
#else
int __stdcall WinMain(HINSTANCE hInst, HINSTANCE h0, LPTSTR lpCmdLine, int nCmdShow)
#endif
{
     // create & show our dialog
    HWND hDlg;
    hDlg = CreateDialogParam(hInst, MAKEINTRESOURCE(IDD_DIALOG1), 0, DialogProc, 0);
    ShowWindow(hDlg, nCmdShow);

    // init LUA
    MyLua lua;
    if (!lua.LoadScript("../lua/mapping.lua"))
        return 0;

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
#ifdef  UNICODE
     wWinMain(GetModuleHandle(0), 0, GetCommandLine(), SW_SHOW);
#else
     WinMain(GetModuleHandle(0), 0, GetCommandLine(), SW_SHOW);
#endif
     return 0;
 }
