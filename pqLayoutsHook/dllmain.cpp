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

namespace
{

    LRESULT CALLBACK LowLevelKeyboardProc(
        _In_ int    nCode,
        _In_ WPARAM wParam,
        _In_ LPARAM lParam
        )
    {
        if (nCode == HC_ACTION)
        {
            ostringstream os;
            os << "wparam: " << hex << wParam << dec << endl;
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


void HookKbdLL()
{
    hKbdHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, GetModuleHandle(0), 0);
    if (hKbdHook == NULL)
    {
        ostringstream os;
        os << "SetWindowsHookEx error: " << GetLastError() << endl;
        OutputDebugStringA(os.str().c_str());
        ShowLastError();
    }
}


void UnhookKbdLL()
{
    if (hKbdHook != NULL)
    {
        if (!UnhookWindowsHookEx(hKbdHook))
        {
            ostringstream os;
            os << "UnhookWindowsHookEx error: " << GetLastError() << endl;
            OutputDebugStringA(os.str().c_str());
            ShowLastError();
        }
    }
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

