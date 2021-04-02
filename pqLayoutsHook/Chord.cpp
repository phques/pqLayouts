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


Kord::Kord()
{
    Reset();
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
        KeyDown(event.vkCode, realEvent);
    else
        KeyUp(event.vkCode);
}


void Kord::KeyDown(VeeKee vk, const KbdHookEvent& event)
{
    switch (state)
    {
    case State::New:
        state = State::Chording;
        firstKeyDownEvent = event;
        break;
    case State::Unchording: // key pressed before all keys are up, cancel chord
        //pq 2021-04-01 try to allow adding subsequent keys to chord
        //   similar to what Plover does.
        //state = State::Cancelled;
        //return;
        break;
    case State::Cancelled:  // ignore
    case State::Completed:
        return;
    default:
        break;
    }

    inChord[vk] = true;
    pressed[vk] = true;

    lastKeyDownEvent = event;
}

void Kord::KeyUp(VeeKee vk)
{
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
        switch (inChord.count())
        {
        case 0:
        case 1:
            state = State::Cancelled;
            break;
        case 2:
        {
            // to prevent accidental trigger of short (2 keys) chords,
            // consider it complete only if the 2 keys were pressed almost at the same time
            // pq-todo: configurable ? enabled/disabled/delay
            // pq ? needs to be a very short time ! the precision of tickDiff is +- 10-16ms !! (x 2 ?)
            DWORD tickDiff = lastKeyDownEvent.TimeDiff(firstKeyDownEvent);
            
            LARGE_INTEGER qpcDiff;
            lastKeyDownEvent.QpcDiff(firstKeyDownEvent, qpcDiff);

            Printf("2keys chord tickdiff = %d, qpc diff = %lld\n", tickDiff, qpcDiff.QuadPart);

            //if (tickDiff < 32)
            if (qpcDiff.QuadPart < 50000) // 50ms
                state = State::Completed;
            else
                state = State::Cancelled;
            break;
        }
        default:
            state = State::Completed;
        }
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
    memset(&firstKeyDownEvent, 0, sizeof(firstKeyDownEvent));
    memset(&lastKeyDownEvent, 0, sizeof(lastKeyDownEvent));
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
        // convert VK to char and add to output string
        if (inChord.test(i))
            str += VkUtil::VkToChar(i, 0);
    }

    return str;
}

bool Kord::operator==(const Kord& other) const
{
    return this->inChord == other.inChord;
}
