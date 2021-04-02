// Copyright 2021 Philippe Quesnel  
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

#pragma once

#include "Keydef.h"
#include "util.h"


// 'Chord' is a Windows function grrr, so use 'Kord'
class Kord
{
public:
    enum class State
    {
        New,
        Chording, 
        Unchording,
        Cancelled, 
        Completed
    };

    typedef std::bitset<256> Keys;

public:
    PQHOOK_API Kord();
    PQHOOK_API void AddInChordKey(VeeKee);
    PQHOOK_API std::string ToChars() const;

    void Reset();

    State GetState() const { return state; }
    bool IsConstructing() const;

    void OnEvent(const KbdHookEvent& event, const KbdHookEvent& realEvent);

    bool IsKeyInChord(VeeKee vk);

    size_t NbKeysDown() const { return pressed.count(); }

    const std::vector<KbdHookEvent>& KeysSequence() const { return keysSequence; }

    std::string ChordValue() const { return inChord.to_string(); }

    bool operator ==(const Kord& other) const;

private:
    void KeyDown(VeeKee vk, const KbdHookEvent& event);
    void KeyUp(VeeKee vk);

private:
    //##pq-todo?, for perfo
    //int  nbDown;        // nb of keys currently still down
    Keys pressed;       // keys that are currently down/depressed
    Keys inChord;       // keys that were pressed

    std::vector<KbdHookEvent> keysSequence;  // keys that were pressed, in order (to 'replay'/send if chord fails)

    State state;
    KbdHookEvent firstKeyDownEvent;
    KbdHookEvent lastKeyDownEvent;
};

