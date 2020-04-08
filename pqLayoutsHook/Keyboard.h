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

#include "util.h"


class Keyboard
{
public:
    Keyboard();

    bool OnKeyEVent(KbdHookEvent&, bool down);

    size_t DownModifiers() const { return downModifiers.size(); }  // for dbg

protected:
    void KeyDown(DWORD vk, bool down);

    void ModifierDown(DWORD vk, bool down);
    bool ModifierDown(DWORD vk) const { return downModifiers.find(vk) != downModifiers.end(); }

    bool IsModifier(DWORD vk);
    DWORD Mapping(DWORD vk);

    void SendVk(DWORD vk, bool down);

private:
    // all VKs
    std::unordered_set<DWORD> downModifiers;
    std::unordered_set<DWORD> downKeys;
    std::unordered_map<DWORD, DWORD> mappings;
};
