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


// Used to hold chord definitions AND
// to build chords as user presses keys.
// (nb:'Chord' is a Windows function grrr, so use 'Kord')
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
    PQHOOK_API void AddInChordKey(int stenoOrderIdx);
    PQHOOK_API size_t GetNbrKeysInChord() const { return inChord.count(); }

    void Reset();

    State GetState() const { return state; }
    bool IsConstructing() const;

    void OnEvent(int keyStenoOrder, const KbdHookEvent & event);
    void Cancel() { state = State::Cancelled; }

    const Keys& ChordKeys() const { return inChord; }
    const std::vector<KbdHookEvent>& KeysSequence() const { return keysSequence; }

    PQHOOK_API void SetDisplay(const std::string& display);
    PQHOOK_API const std::string& GetDisplay() const { return display; }

    bool operator ==(const Kord& other) const;

private:
    void KeyDown(int keyStenoOrder, const KbdHookEvent& event);
    void KeyUp(int keyStenoOrder, const KbdHookEvent& event);

private:
    std::string display; // text representation
    //
    // keys that were chorded, kept using stenoOrderIdx
    Keys inChord;       

    // Next two use the original key VK
    // keys that are currently down/depressed
    Keys pressed;
    // keys that were pressed, in order (to 'replay'/send if chord fails)
    std::vector<KbdHookEvent> keysSequence;  

    State state;
    KbdHookEvent firstKeyDownEvent;
    KbdHookEvent lastKeyDownEvent;
};

