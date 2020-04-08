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
#include "Keyboard.h"


namespace
{
    HHOOK hKbdHook = NULL;
    Keyboard keyboard;

}


// put this in a class since we cannot debug this code,
// so we'll use friend class KbdHook to access data for dbg output
class KbdHook
{
public:
    // called for each KBDLLHOOK event
    static LRESULT CALLBACK LowLevelKeyboardProc(
        _In_ int    nCode,
        _In_ WPARAM wParam,
        _In_ LPARAM lParam
    )
    {
        bool skip = false;

        // should we process this?
        if (nCode == HC_ACTION)
        {
            KbdHookEvent event(lParam);

            // dbg output
            KbdProcDebugOut(event, wParam);

            // process this key
            skip = keyboard.OnKeyEVent(event);

        }

        // forward to next hook
        if (skip)
            return 1;  // do not send this message / event

        return CallNextHookEx(hKbdHook, nCode, wParam, lParam);
    }

    // dbg output
    static void KbdProcDebugOut(KbdHookEvent& event, const WPARAM& wParam)
    {
        Dbg::Out::KbdEVent(event, wParam);

        keyboard.OutNbKeysDn();

        //if (event.Down())
        //{
        //    UINT scancode = MapVirtualKeyExA(event.vkCode, MAPVK_VK_TO_VSC_EX, NULL);

        //    std::ostringstream os;
        //    os << std::hex << " scan " << scancode << std::endl;
        //    Dbg::Out::DebugString(os);
        //}
    }

};

//------ [exports... --------

// hook our low level kbd procedure
bool HookKbdLL()
{
    hKbdHook = SetWindowsHookEx(WH_KEYBOARD_LL, KbdHook::LowLevelKeyboardProc, GetModuleHandle(0), 0);

    if (hKbdHook == NULL)
        return Dbg::Out::LastError("SetWindowsHookEx");

    return true;
}

// unhook our low level kbd procedure
bool UnhookKbdLL()
{
    if (hKbdHook == NULL)
        return false;

    if (!UnhookWindowsHookEx(hKbdHook))
        return Dbg::Out::LastError("UnhookWindowsHookEx"); 

    return true;
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


