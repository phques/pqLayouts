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

// forward decl
class Kord;


enum class HandId
{
    HAND_LEFT,
    HAND_RIGHT,
    HAND_STARS
};

struct ChordingKey
{
    HandId handId;
    VeeKee qwerty;      // VK of the qwerty key the user presses
    VeeKee steno;       // VK of the corresponding steno key
    char stenoChar;     // original character in 'steakpower', used for display
    int stenoOrder;     // steno key steno order (index into stenoKeys member)
};

typedef std::vector<ChordingKey> ChordingKeys;

//-------------

// the 'steno' chording data, 
// ie qwerty to steno mapping and steno order
class ChordingData
{
    // ORed with right hand consonant VKs to make them unique
    const int RIGHTHANDFLAG = 0x1000;

public:
    PQHOOK_API ChordingData();
    PQHOOK_API void Init(const ChordingKeys& _stenoKeys);

    PQHOOK_API bool IsHandDelimiterKey(VeeKee vk) const;
    PQHOOK_API std::string ToString(const Kord& kord) const;
    PQHOOK_API VeeKee AdjustForRightHand(VeeKee vk) const;
    PQHOOK_API VeeKee AdjustForHandedness(VeeKee vk, bool rightHand) const;

    PQHOOK_API const ChordingKey* GetChordingKeyFromQwerty(VeeKee qwertyVk) const;
    PQHOOK_API const ChordingKey* GetChordingKeyFromSteno(VeeKee stenoVk) const;

private:
    bool IsConsonant(VeeKee vk) const;

private:
    VeeKeeSet handDelimiters;
    VeeKeeSet consonants;

    // accessed in steno order
    ChordingKeys stenoKeys;

    // dictio into stenoKeys, index = qwerty VK
    std::map<VeeKee, const ChordingKey*> qwertyMap;

    // dictio into stenoKeys, index = steno VK
    // VeeKee | RIGHTHANDFLAG indicates it is the right hand consonant steno key
    std::map<VeeKee, const ChordingKey*> stenoMap;

};

