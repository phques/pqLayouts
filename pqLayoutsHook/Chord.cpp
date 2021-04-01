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

#include "pch.h"
#include "Chord.h"


Kord::Kord() : state(State::New)
{
}

bool Kord::IsConstructing() const
{
    return state == State::Chording || state == State::Unchording;
}

void Kord::OnEvent(const KbdHookEvent& event, const KbdHookEvent& realEvent)
{
    // dont add repeats key downs to pressSequence
    if (!pressed.test(event.vkCode) || !event.Down())
    {
        keysSequence.push_back(realEvent);
    }

    if (event.Down())
        KeyDown(event.vkCode);
    else
        KeyUp(event.vkCode);
}


void Kord::KeyDown(VeeKee vk)
{
    // dont add repeats to pressSequence
    //if (!pressed.test(vk))
    //{
    //    KeyEvent keyEvent = { vk, true };
    //    keysSequence.push_back(keyEvent);
    //}

    switch (state)
    {
    case State::New:
        state = State::Chording;
        break;
    case State::Unchording: // key pressed before all keys are up, cancel chord
        state = State::Cancelled;
        return;
    case State::Cancelled:  // ignore
    case State::Completed:
        return;
    default:
        break;
    }

    inChord[vk] = true;
    pressed[vk] = true;

}

void Kord::KeyUp(VeeKee vk)
{
    //KeyEvent keyEvent = { vk, false };
    //keysSequence.push_back(keyEvent);

    switch (state)
    {
    case State::Cancelled:
    case State::Completed:
    case State::New:
        return;             // just ignore
    case State::Chording:   // start unchording
        state = State::Unchording;
        break;
    default:
        break;
    }

    pressed[vk] = false;
    
    // all keys are up, chord is 'complete' (or cancelled if there was only one key)
    if (NbKeysDown() == 0)
    {
        if (inChord.count() > 1)
            state = State::Completed;
        else
            state = State::Cancelled;
    }
}

bool Kord::IsKeyInChord(VeeKee vk)
{
    return inChord.test(vk);
}

void Kord::Reset()
{
    inChord = Keys();
    pressed = Keys();
    keysSequence.clear();
    state = State::New;
}

// used when creating chord definitions
void Kord::AddInChordKey(VeeKee vk)
{
    inChord[vk] = true;
}

std::string Kord::ToChars() const
{
    std::string str;
    for (int i = 0; i < 255; i++)
    {
        if (inChord.test(i))
        {
            str += static_cast<char>(i);
        }
    }

    return str;
}

bool Kord::operator==(const Kord& other) const
{
    return this->inChord == other.inChord;
}
