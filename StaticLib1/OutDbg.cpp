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
#include "OutDbg.h"

using namespace std;

namespace Dbg
{
    namespace Out
    {
        
        // outputdbg KBDLLHOOK event
        void KbdEVent(const KbdHookEvent& event, WPARAM wParam, bool selfInjected)
        {
            // get displayable ascii character from vk
            static BYTE keystates[256] = {0};
            WORD asciiChars[16] = {0};
            ToAscii(event.vkCode, event.scanCode, keystates, asciiChars, 0);

            char asciiChar = ' ';
            if (asciiChars[0] > 32 && asciiChars[0] < 127)
                asciiChar = static_cast<char>(asciiChars[0]);

            Printf("@%0X %c wparam: %04X vk: %02X scan: %02X %c %s %s flags: %s\n",
                event.time, 
                (selfInjected ? 'I' : (event.Injected() ? 'i' : ' ')),
                wParam, event.vkCode, event.scanCode,
                asciiChar,
                (event.Up() ? " up" : " dn"), 
                (event.Extended() ? " EX" : " ex"),
                bitset<8>(event.flags).to_string().c_str());
        }

        // outputdbg WM_KEYDOWN/UP message
        void WmKey(MSG msg)
        {
            WmKeyLPARAM lparam(msg.lParam);

            Printf("vk %02x scan %0X %c (%c) %c\n",
                msg.wParam,
                lparam.u.values.scanCode,
                lparam.Up() ? 'u' : 'd',
                lparam.u.values.previousState ? 'd' : 'u',
                lparam.u.values.extended ? 'X' : ' '
                );
        }


        // outputdbg message for GetLastError()
        // returns false
        bool LastError(const char* funcName)
        {
            // show called funcName & error#
            Printf(" error: %d\n", GetLastError());


            // show error descr
            LPVOID lpMsgBuf;
            DWORD dwLastError = GetLastError();

            FormatMessageA(
                FORMAT_MESSAGE_ALLOCATE_BUFFER |
                FORMAT_MESSAGE_FROM_SYSTEM |
                FORMAT_MESSAGE_IGNORE_INSERTS,
                NULL,
                dwLastError,
                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                (LPSTR)&lpMsgBuf,
                0, NULL);

            Printf((LPCSTR)lpMsgBuf); 

            LocalFree(lpMsgBuf);

            return false;
        }



    } // namespace Out 

} // namespace Dbg

