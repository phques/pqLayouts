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
#include "Keyboard.h"


Keyboard::Keyboard()
{
 
}

bool Keyboard::IsModifier(DWORD vk)
{
    return
        vk == VK_LSHIFT || vk == VK_RSHIFT || vk == VK_SHIFT ||
        vk == VK_LCONTROL || vk == VK_RCONTROL || vk == VK_CONTROL ||
        vk == VK_LMENU || vk == VK_RMENU || vk == VK_MENU ||         // Alt !
        vk == VK_LWIN || vk == VK_RWIN;
}

void Keyboard::ModifierDown(DWORD vk, bool down)
{
    if (down)
        downModifiers.insert(vk);
    else
        downModifiers.erase(vk);
}


void Keyboard::KeyDown(DWORD vk, bool down)
{
    if (down)
        downKeys.insert(vk);
    else
        downKeys.erase(vk);
}

DWORD Keyboard::Mapping(DWORD vk)
{
    auto foundit = mappings.find(vk);
    if (foundit == mappings.end())
        return vk;

    return foundit->second;
}


// return true if we should skip / eat this virtual-key
bool Keyboard::OnKeyEVent(KbdHookEvent& event, bool down)
{
    DWORD vkout = Mapping(event.vkCode);

    KeyDown(vkout, down);

    if (IsModifier(vkout))
        ModifierDown(vkout, down);

    return false;
}



void Keyboard::SendVk(DWORD vk, bool down)
{
    //DWORD flags = 0;
    //if (up) flags |= KEYEVENTF_KEYUP;
    //if (isExtendedKey(code)) flags |= KEYEVENTF_EXTENDEDKEY;

    //INPUT input;
    //input.type = INPUT_KEYBOARD;
    //input.ki.wVk = code;
    //input.ki.wScan = MapVirtualKey(code, 0);
    //input.ki.dwFlags = flags;
    //input.ki.time = 0;
    //input.ki.dwExtraInfo = injectedFlag;
    //SendInput(1, &input, sizeof(input));

    //##PQ from touchcursor##
    // Sleep(1) seems to be necessary for mapped Escape events sent to VirtualPC & recent VMware versions.
    // (Sleep(0) is no good)
    // Also for mapped modifiers with Remote Desktop Connection.
    // Dunno why:
    Sleep(1);
}