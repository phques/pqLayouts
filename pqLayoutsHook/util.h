#pragma once
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

#include "framework.h"

//--------

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
#if 0
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
#endif // 0



// class for easy access to KBDLLHOOKSTRUCT info
class KbdHookEvent
{
public:
    // Init KbdHookEvent directly from the recvd LPARAM of LowLevelKeyboardProc
    // ## lParam is cast to a ptr, MUST be LPARAM of LowLevelKeyboardProc ##
    KbdHookEvent(LPARAM lParam) : KbdHookEvent(*reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam)) 
    {
    }
    KbdHookEvent(const KBDLLHOOKSTRUCT& info) : info(info)
    {
    }

    inline bool Extended() const { return (info.flags & LLKHF_EXTENDED) != 0; }
    inline bool Injected() const { return (info.flags & LLKHF_INJECTED) != 0; }
    inline bool InjectedLow() const { return (info.flags & LLKHF_LOWER_IL_INJECTED) != 0; }
    inline bool AltDown() const { return (info.flags & LLKHF_ALTDOWN) != 0; }
    inline bool Up() const { return (info.flags & LLKHF_UP) != 0; }
    inline bool Down() const { return (info.flags & LLKHF_UP) == 0; }

public:
    const KBDLLHOOKSTRUCT& info;
};

//--------

