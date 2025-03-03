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


//--------

// Easy access to KBDLLHOOKSTRUCT info
class KbdHookEvent : public KBDLLHOOKSTRUCT
{
public:
    // Init KbdHookEvent from the recvd LPARAM of LowLevelKeyboardProc
    // ## lParam is cast to KBDLLHOOKSTRUCT*, MUST be from LowLevelKeyboardProc ##
    KbdHookEvent(LPARAM lParam);
    KbdHookEvent();

    inline bool Extended() const { return (flags & LLKHF_EXTENDED) != 0; }
    inline bool Injected() const { return (flags & LLKHF_INJECTED) != 0; }
    inline bool InjectedLow() const { return (flags & LLKHF_LOWER_IL_INJECTED) != 0; }
    inline bool AltDown() const { return (flags & LLKHF_ALTDOWN) != 0; }
    inline bool Up() const { return (flags & LLKHF_UP) != 0; }
    inline bool Down() const { return (flags & LLKHF_UP) == 0; }

    DWORD TimeDiff(const KbdHookEvent& startEvent);
    void QpcDiff(const KbdHookEvent& startEvent, LARGE_INTEGER& diff) const;

    LARGE_INTEGER perfoCounter;
};

//----------

/* LPARAM bits for wm_keydown/up
Bits 	Meaning
0-15 	The repeat count for the current message. The value is the number of times the keystroke is autorepeated as a result of the user holding down the key. If the keystroke is held long enough, multiple messages are sent. However, the repeat count is not cumulative.
16-23 	The scan code. The value depends on the OEM.
24 	Indicates whether the key is an extended key, such as the right-hand ALT and CTRL keys that appear on an enhanced 101- or 102-key keyboard. The value is 1 if it is an extended key; otherwise, it is 0.
25-28 	Reserved; do not use.
29 	The context code. The value is always 0 for a WM_KEYDOWN message.
30 	The previous key state. The value is 1 if the key is down before the message is sent, or it is zero if the key is up.
31 	The transition state. The value is always 0 for a WM_KEYDOWN message.
*/
struct WmKeyLPARAM {
    WmKeyLPARAM(LPARAM lParam = 0) { u.lParam = lParam; }

    bool Up() const { return u.values.transitionState == 1; }
    bool Down() const { return u.values.transitionState == 0; }
    bool Extended() const { return u.values.extended == 1; }

    union {
        struct {
            unsigned int repeatCount : 16;  // apparently always 1 ??
            unsigned int scanCode : 8;
            unsigned int extended : 1;
            unsigned int reserved : 4;
            unsigned int contextCode : 1;   // always 0
            unsigned int previousState : 1;
            unsigned int transitionState : 1;
        } values;
        LPARAM lParam;
    } u;
};


//--------

struct VkUtil
{
    // checks bit 0 of hibyte of ret val from VkKeyScanA
    static bool HasShiftBit(SHORT scanExVal) 
    { 
        return ((scanExVal >> 8) & 0x01) != 0; 
    }

    // converts a character to a VK and a 'isShifted' flag
    static bool CharToVk(char ch, WORD& vk, bool& isShifted);
    static char VkToChar(WORD vk, WORD scancode);

    template <typename TC, typename  T>
    static bool Contains(TC container, T value)
    { 
        return find(container.begin(), container.end(), value) != container.end();
    }
};


//--------

// debug only Printf (build app in console subsystem)

#ifdef NDEBUG
 #define Printf (__noop)
 #define Printfw (__noop)
#else
 #define Printf printf
 #define Printfw wprintf
#endif


//---

typedef const std::string conststr;

DWORD TickCountDiff(DWORD start, DWORD end);
void QpcDiff(const LARGE_INTEGER& start, const LARGE_INTEGER& end, LARGE_INTEGER& diff);