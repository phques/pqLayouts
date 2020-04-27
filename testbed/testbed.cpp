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
#include "outDbg.h"

using namespace luabridge;

namespace
{
    const char* myNamespace = "pqLayout";

    // wrapper for VkKeyScanExA
    LuaRef GetVk(char character, lua_State* L)
    {
        SHORT vkandflags = VkKeyScanExA(character, NULL);
        BYTE flags = HIBYTE(vkandflags);
        BYTE vk = LOBYTE(vkandflags);

        LuaRef table = newTable(L);
        table["vk"] = vk;
        table["shiftOn"] = (flags & 1) != 0;
        table["ctrlOn"] = (flags & 2) != 0;
        table["altOn"] = (flags & 4) != 0;
        return table;
    }
    

}

class MyLua : public LuaState
{
//public:
    LuaRef theKbd = nullptr;
    LuaRef kbdOnKey = nullptr;

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

    bool GetLuaGlobals()
    {
        try
        {
            // get 'theKeyboard' lua object
            theKbd = GetRef("theKeyboard");
            if (theKbd.isNil()) {
                MessageBoxA(NULL, "Cannot find 'theKeyboard' in lua script", "testbed", MB_OK);
                return false;
            }

            kbdOnKey = theKbd["onKey"];
        }
        catch (const LuaException& e)
        {
            Printf("GetLuaGlobals luaexception %s\n", e.what());
            return false;
        }

        return true;
    }

    bool SetLuaGlobals()
    {
        try
        {
            // register C funcs
            getGlobalNamespace(luaL)
                .beginNamespace(myNamespace)

                .addFunction("GetVk", &GetVk)

                .endNamespace();

        }
        catch (const LuaException& e)
        {
            Printf("luaexception %s\n", e.what());
            return false;
        }

        return true;
    }

    unsigned int OnKey(WPARAM wParam, bool down)
    {
        LuaRef ret = kbdOnKey(theKbd, wParam, down);
        if (ret.isNil())
            return 0;
        return ret;
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
bool handleKbdMsg(MSG& msg, MyLua& lua)
{
    // is it a keybd message?
    if (!(msg.message == WM_KEYDOWN || msg.message == WM_SYSKEYDOWN ||
          msg.message == WM_KEYUP || msg.message == WM_SYSKEYUP))
        return false;

    // clear console on F11
    if (msg.wParam == VK_F11) {
        if (msg.message == WM_KEYDOWN) {
            Printf("\033[2J"); /*clear screen*/
            Printf("\033[1H"); /*goto line 1 */
        }
        return true;
    }

    // reload LUA script on F12
    if (msg.wParam == VK_F12) {
        if (msg.message == WM_KEYDOWN) {
            puts("--reload script--");
            if (!lua.LoadScript("../lua/mapping.lua"))
                puts("failed to load script");
        }
        return true;
    }

    WmKeyLPARAM lparam(msg.lParam);

    Dbg::Out::WmKey(msg);

    try
    {
        unsigned int newVk = lua.OnKey(msg.wParam, lparam.Down());
        Printf("lua.onkey ret = %0x\n", newVk);

        if (newVk != 0)
            msg.wParam = static_cast<WPARAM>(newVk);
    }
    catch (const LuaException& e)
    {
        Printf("luaexception %s\n", e.what());
        return false;
    }

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
    lua.SetLuaGlobals();

    // need to pass path here, does not seem to use the lua path
    if (!lua.LoadScript("../lua/mapping.lua"))
        return 0;

    if (!lua.GetLuaGlobals())
        return 0;

    Printf("F11 to clear screen\n");
    Printf("F12 to reload script\n");

    // message loop
    BOOL ret;
    MSG msg;
    while ((ret = GetMessage(&msg, 0, 0, 0)) != 0) {
        if (ret == -1) /* error found */
            return -1;

        // handle keyboard messages 
        if (handleKbdMsg(msg, lua))
            continue;  // skip this message

            // prevent dialog close on Escape
        if ((msg.message == WM_KEYDOWN || msg.message == WM_KEYUP) && msg.wParam == VK_ESCAPE) {
            puts("skip escape");
            continue;
        }

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
