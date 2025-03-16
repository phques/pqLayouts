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
#include "ChordingData.h"
#include "Chord.h"
#include "util.h"


ChordingData::ChordingData()
{
    bool isShifted;
    WORD vk;

    // populate handDelimiters keys set
    for (const char* ptr = "AEOU*-"; *ptr; ++ptr)
    {
        VkUtil::CharToVk(*ptr, vk, isShifted);
        handDelimiters.insert(vk);
    }

    // populate consonants keys set
    for (const char* ptr = "qzwsxdcrfvtgbhnjmklp"; *ptr; ++ptr)
    {
        VkUtil::CharToVk(*ptr, vk, isShifted);
        consonants.insert(vk);
    }

}

void ChordingData::Init(const ChordingKeys& _stenoKeys)
{
    // build list + dictio of steno keys
    // will assign steno order

    // make enough room to hold all entries (important, otherwise vector might resize and our ptrs will be invalidated!)
    stenoKeys.reserve(_stenoKeys.size());

    int stenoOrder = 0;
    for (const ChordingKey& stenoKey : _stenoKeys)
    {
        bool rightHand = (stenoKey.handId == HandId::HAND_RIGHT);
        VeeKee stenoVK = AdjustForHandedness(stenoKey.steno, rightHand);

        if (stenoMap.find(stenoVK) == stenoMap.end())
        {
            // add / create new entry in steno keys list
            stenoKeys.push_back(stenoKey);

            // assign steno order to it
            ChordingKey* key = &stenoKeys.back();
            key->stenoOrder = stenoOrder++;

            // and save in steno keys dictio
            stenoMap[stenoVK] = key;
        }
    }

    // build qwertyToSteno to point to stenoKeys entries
    //nb: this will be n:1 for 'S' and '*' (for ex.)
    for (auto& stenoKey : _stenoKeys)
    {
        bool rightHand = (stenoKey.handId == HandId::HAND_RIGHT);
        VeeKee stenoVK = AdjustForHandedness(stenoKey.steno, rightHand);

        // lookup steno key
        auto stenoKeyIt = stenoMap.find(stenoVK);
        if (stenoKeyIt != stenoMap.end())
        {
            qwertyMap[stenoKey.qwerty] = stenoKeyIt->second;
        }
    }
}

// lookup a qwerty key in the keys map
const ChordingKey* ChordingData::GetChordingKeyFromQwerty(VeeKee qwertyVk) const
{
    // lookup the qwerty key
    auto foundit = qwertyMap.find(qwertyVk);
    if (foundit == qwertyMap.end())
        return nullptr;

    return foundit->second;
}

// lookup a steno key in the keys map
const ChordingKey* ChordingData::GetChordingKeyFromSteno(VeeKee stenoVk) const
{
    // lookup the steno key
    auto foundit = stenoMap.find(stenoVk);
    if (foundit == stenoMap.end())
        return nullptr;

    return foundit->second;
}



bool ChordingData::IsConsonant(VeeKee vk) const
{
    return consonants.find(vk) != consonants.end();
}

bool ChordingData::IsHandDelimiterKey(VeeKee vk) const
{
    return handDelimiters.find(vk) != handDelimiters.end();
}

std::string ChordingData::ToString(const Kord& kord) const
{
    const Kord::Keys& chordKeys = kord.ChordKeys();

    std::string left;
    std::string right;
    std::string middle;
    for (int i = 0; i < (int)chordKeys.size(); i++)
    {
        // get steno def from steno order idx
        if (chordKeys.test(i))
        {
            auto& stenoKey = this->stenoKeys[i];

            // add the steno key to the string parts
            if (stenoKey.handId == HandId::HAND_LEFT)
                left += stenoKey.stenoChar;
            else if (stenoKey.handId == HandId::HAND_RIGHT)
                right += stenoKey.stenoChar;
            else
                middle += stenoKey.stenoChar;
        }
    }

    std::string results = left + (middle.size() > 0 ? middle : "-") + right;
    return results;
}

// make right hand consonants unique
PQHOOK_API VeeKee ChordingData::AdjustForRightHand(VeeKee vk) const
{
    return IsConsonant(vk) ? vk | RIGHTHANDFLAG : vk;
}

// make right hand consonants unique
PQHOOK_API VeeKee ChordingData::AdjustForHandedness(VeeKee vk, bool rightHand) const
{
    return rightHand ? AdjustForRightHand(vk) : vk;
}
