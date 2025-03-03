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
    this->time = info.time;     // nb, this will be the same as GetTickCount() at the moment we are called
    this->dwExtraInfo = info.dwExtraInfo;

    QueryPerformanceCounter(&perfoCounter);
}

KbdHookEvent::KbdHookEvent()
{
    memset(this, 0, sizeof(KbdHookEvent));
}


DWORD KbdHookEvent::TimeDiff(const KbdHookEvent& startEvent)
{
    return TickCountDiff(startEvent.time, this->time);
}

void KbdHookEvent::QpcDiff(const KbdHookEvent& startEvent, LARGE_INTEGER& diff) const
{
    ::QpcDiff(startEvent.perfoCounter, this->perfoCounter, diff);
}

//-------------


// calculates the time difference between two values from GetTickCount / GetMessageTime / KBDLLHOOKSTRUCT.time
/* msdn:
  The time is a long integer that specifies the elapsed time, in milliseconds, 
  from the time the system was started to the time the message was created 
  (that is, placed in the thread's message queue).

  To calculate time delays between messages, 
  subtract the time of the first message from the time of the second message (ignoring overflow) 
  and compare the result of the subtraction against the desired delay amount.
*/
DWORD TickCountDiff(DWORD start, DWORD end)
{   
    LONG diff = static_cast<LONG>(end) - static_cast<LONG>(start);
    return abs(diff);
}

// convert elapsed number of ticks from 2 QueryPerformanceCounter into elapsed microseconds
void QpcDiff(const LARGE_INTEGER& start, const LARGE_INTEGER& end, LARGE_INTEGER& diff)
{
    static bool frequencyInitialized = false;
    static LARGE_INTEGER frequency;

    if (!frequencyInitialized)
    {
        QueryPerformanceFrequency(&frequency); 
        frequencyInitialized = true;
    }
    diff.QuadPart = end.QuadPart - start.QuadPart;
    diff.QuadPart *= 1000000;
    diff.QuadPart /= frequency.QuadPart;
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

    return true;
}

char VkUtil::VkToChar(WORD vk, WORD scancode)
{
    // get displayable ascii character from vk
    static BYTE keystates[256] = { 0 };
    WORD asciiChars[16] = { 0 };
    ToAscii(vk, scancode, keystates, asciiChars, 0);

    char asciiChar = ' ';
    if (asciiChars[0] > 32 && asciiChars[0] < 127)
        asciiChar = static_cast<char>(asciiChars[0]);

    return asciiChar;
}

