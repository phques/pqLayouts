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
        void KbdEVent(const KbdHookEvent& event, WPARAM wParam)
        {
            ostringstream os;
            os << hex
                << '@' << event.info.time
                << " wparam: " << wParam
                << " vkCode: " << event.info.vkCode
                << " scanCode: " << event.info.scanCode
                << (event.Up() ? " up " : " dn ")
                << " flags: " << bitset<8>(event.info.flags)
                << endl;

            OutputDebugStringA(os.str().c_str());
        }


        // outputdbg message for GetLastError()
        void LastError(const char* func)
        {
            ostringstream os;
            os << func << " error: " << GetLastError() << endl;
            OutputDebugStringA(os.str().c_str());


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

    } // namespace Out 

} // namespace Dbg

