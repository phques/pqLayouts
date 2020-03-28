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
#include "pqLayoutsHook.h"
#include "util.h"
#include "OutDbg.h"

using namespace std;


namespace
{
    HHOOK hKbdHook = NULL;
}


namespace
{
    // called for each KBDLLHOOK event
    LRESULT CALLBACK LowLevelKeyboardProc(
        _In_ int    nCode,
        _In_ WPARAM wParam,
        _In_ LPARAM lParam
        )
    {
        // should we process this?
        if (nCode == HC_ACTION)
        {
            // get params
            KbdHookEvent event(lParam);

            // dbg output info on event
            Dbg::Out::KbdEVent(event, wParam);
        }

        // forward to next hook
        return CallNextHookEx(hKbdHook, nCode, wParam, lParam);
    }

}

//------ [exports... --------

// hook our low level kbd procedure
bool HookKbdLL()
{
    hKbdHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, GetModuleHandle(0), 0);
    if (hKbdHook == NULL)
    {
        Dbg::Out::LastError("SetWindowsHookEx");
        return false;
    }
    return true;
}

// unhook our low level kbd procedure
bool UnhookKbdLL()
{
    if (hKbdHook == NULL)
        return false;

    if (UnhookWindowsHookEx(hKbdHook))
        return true;

    Dbg::Out::LastError("UnhookWindowsHookEx");
    return false;
}

//------ ... exports] --------


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}


