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

//#include "framework.h"

//--------


// Easy access to KBDLLHOOKSTRUCT info
class KbdHookEvent : public KBDLLHOOKSTRUCT
{
public:
    // Init KbdHookEvent from the recvd LPARAM of LowLevelKeyboardProc
    // ## lParam is cast to KBDLLHOOKSTRUCT*, MUST be from LowLevelKeyboardProc ##
    KbdHookEvent(LPARAM lParam);

    inline bool Extended() const { return (flags & LLKHF_EXTENDED) != 0; }
    inline bool Injected() const { return (flags & LLKHF_INJECTED) != 0; }
    inline bool InjectedLow() const { return (flags & LLKHF_LOWER_IL_INJECTED) != 0; }
    inline bool AltDown() const { return (flags & LLKHF_ALTDOWN) != 0; }
    inline bool Up() const { return (flags & LLKHF_UP) != 0; }
    inline bool Down() const { return (flags & LLKHF_UP) == 0; }

public:
    //const KBDLLHOOKSTRUCT& info;
};


//--------

