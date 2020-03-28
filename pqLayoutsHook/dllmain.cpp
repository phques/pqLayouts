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

#include "pch.h"
#include "pqLayoutsHook.h"

using namespace std;

namespace
{
    HHOOK hKbdHook = NULL;
}

// To access KBDLLHOOKSTRUCT.flags as individual values / flags

/*  https://docs.microsoft.com/en-us/windows/win32/api/winuser/ns-winuser-kbdllhookstruct?redirectedfrom=MSDN
 0  Specifies whether the key is an extended key, such as a function key or a key on the numeric keypad.
    The value is 1 if the key is an extended key; otherwise, it is 0.
 1  Specifies whether the event was injected from a process running at lower integrity level.
    The value is 1 if that is the case; otherwise, it is 0.
    Note that bit 4 is also set whenever bit 1 is set.
 2-3 Reserved.
 4  Specifies whether the event was injected.
    The value is 1 if that is the case; otherwise, it is 0.
    Note that bit 1 is not necessarily set when bit 4 is set.
 5  The context code.
    The value is 1 if the ALT key is pressed; otherwise, it is 0.
 6  Reserved.
 7  The transition state.
    The value is 0 if the key is pressed and 1 if it is being released.
*/
struct KbdHookFlagsBits
{
    unsigned int extended : 1;
    unsigned int injectedLow : 1;
    unsigned int reserved : 2;
    unsigned int injected : 1;
    unsigned int contextCode : 1;    // 1= Alt down
    unsigned int reserved2 : 1;
    unsigned int up : 1;             // transitin state, 0-dn, 1=up
};

union KbdHookFlagsUnion
{
    KbdHookFlagsBits bits;
    DWORD dword;
    BYTE byte;
};


namespace
{

    LRESULT CALLBACK LowLevelKeyboardProc(
        _In_ int    nCode,
        _In_ WPARAM wParam,
        _In_ LPARAM lParam
        )
    {
        // should we process this?
        if (nCode == HC_ACTION)
        {
            KBDLLHOOKSTRUCT& hookStruct = *reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);

            KbdHookFlagsUnion flags;
            flags.dword = hookStruct.flags;

            // dbg output info on event
            ostringstream os;
            os  << hex
                << hookStruct.time
                << " wparam: " << wParam
                << " vkCode: " << hookStruct.vkCode 
                << " scanCode: " << hookStruct.scanCode
                << " up: " << flags.bits.up
                << " flags: " << bitset<8>(flags.byte) 
                << endl;

            OutputDebugStringA(os.str().c_str());
        }

        // forward to next hook
        return CallNextHookEx(hKbdHook, nCode, wParam, lParam);

    }


    void ShowLastError()
    {
        LPVOID lpMsgBuf;
        DWORD dw = GetLastError();

        FormatMessage(
            FORMAT_MESSAGE_ALLOCATE_BUFFER |
            FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL,
            dw,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPTSTR)&lpMsgBuf,
            0, NULL);

        OutputDebugString((LPCTSTR)lpMsgBuf);
        LocalFree(lpMsgBuf);
    }

}

//--------------


bool HookKbdLL()
{
    hKbdHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, GetModuleHandle(0), 0);
    if (hKbdHook == NULL)
    {
        // dbg output error
        ostringstream os;
        os << "SetWindowsHookEx error: " << GetLastError() << endl;
        OutputDebugStringA(os.str().c_str());

        ShowLastError();
        return false;
    }
    return true;
}


bool UnhookKbdLL()
{
    bool ret = false;

    if (hKbdHook != NULL)
    {
        if (!UnhookWindowsHookEx(hKbdHook))
        {
            // dbg output error
            ostringstream os;
            os << "UnhookWindowsHookEx error: " << GetLastError() << endl;
            OutputDebugStringA(os.str().c_str());

            ShowLastError();
        }
        else
            ret = true;
    }
    return ret;
}

//--------------


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


