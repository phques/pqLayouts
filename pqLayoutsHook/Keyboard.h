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

#include <cassert>
#include "util.h"

class KbdHook; // 

class Keyboard
{
    friend class KbdHook;

public:
    Keyboard();

protected:
    bool OnKeyEVent(KbdHookEvent&);

    void KeyDown(DWORD vk, bool down);
    bool KeyDown(DWORD vk) const;

    void ModifierDown(DWORD vk, bool down);
    bool ModifierDown(DWORD vk) const;
    bool ShiftDown() const;

    static bool IsModifier(DWORD vk);
    static bool IsExtended(DWORD vk);

    DWORD Mapping(DWORD vk);

    void SendVk(DWORD vk, bool down);

    // dbg
    void OutNbKeysDn();

private:
    // all VKs
    DWORD downModifiers[256];
    DWORD downKeys[256];
    DWORD mappings[2][256];

    static std::unordered_set<DWORD> modifiers;
    static std::unordered_set<DWORD> extended;

    // a 'unique' value to identify values we injected with SendInput
    const ULONG_PTR injectedFromMe = 0x0E5FA78A; // <Guid("0E5FA78A-82FE-4557-956E-320702FEB659")>
};
