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
#include "util.h"


// Init KbdHookEvent from the recvd LPARAM of LowLevelKeyboardProc
// ## lParam is cast to KBDLLHOOKSTRUCT*, MUST be from LowLevelKeyboardProc ##
KbdHookEvent::KbdHookEvent(LPARAM lParam)
{
    const KBDLLHOOKSTRUCT& info = *reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);
    this->vkCode = info.vkCode;
    this->scanCode = info.scanCode;
    this->flags = info.flags;
    this->time = info.time;
    this->dwExtraInfo = info.dwExtraInfo;
}

//----------


// converts a character to a VK and a 'isShifted' flag
bool VkUtil::CharToVk(char ch, WORD& vk, bool& isShifted)
{
    vk = VkKeyScanA(ch);
    if (vk == 0xFFFF)
        return false;

    isShifted = HasShiftBit(vk);
    vk = vk & 0xFF;
}
