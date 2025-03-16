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
#include "Layer.h"
#include "KeyOutAction.h"
#include "DualModeModifierAction.h"
#include "OutDbg.h"

using namespace KeyActions;


Layer::Layer(const Id_t& name, Idx_t layerIdx) : name(name), layerIdx(layerIdx)
{
}

const CaseMapping* Layer::Mapping(VeeKee vk) const
{
    const auto found = mappings.find(vk);

    if (found != mappings.end())
        return &(found->second);

    return nullptr;
}

bool Layer::AddMapping(KeyValue from, IKeyAction* actionTo)
{
    CaseMapping& caseMapping = mappings[from.Vk()];
    if (from.Shift())
        caseMapping.shifted = KeyMapping(from, actionTo);
    else
        caseMapping.nonShifted = KeyMapping(from, actionTo);

    return true;
}

bool Layer::AddMapping(KeyValue from, KeyValue to)
{
    //Printf("Add mapping from %02X, to %02X\n", from.Vk(), to.Vk());

    IKeyAction* action = new KeyOutAction(from, to);

    return AddMapping(from,  action);
}

bool Layer::AddDualModeModifier(KeyDef key, KeyValue  modifierKey, KeyValue tapKey)
{
    // create key action object
    IKeyAction* action = new DualModeModifierAction(key, modifierKey, tapKey);
    KeyValue keyValue(key.Vk(), key.Scancode());

    return AddMapping(keyValue, action);
}

bool Layer::AddChord(Kord& chord, const std::list<KeyActions::KeyActionPair>& keyActions)
{
    // give a warning if this already exists
    const std::list<KeyActions::KeyActionPair>* existingKeyActions = GetChordActions(chord);
    if (existingKeyActions != nullptr)
        Printf("chord [%s] already registered\n", chord.GetDisplay().c_str());

    // add chord/keyaction pair to list of chords
    ChordActions chordAction = { chord, keyActions };
    chords.push_back(chordAction);

    return true;
}

// lookup 'chord' and return its asociated key action if found, else null 
// if !found returns a pair with no actions
const std::list<KeyActions::KeyActionPair>* Layer::GetChordActions(const Kord& chord)
{
    auto found = find(chords.begin(), chords.end(), chord);
    if (found != chords.end())
        return &found->keyActions;

    return nullptr;
}


void Layer::SetImageView(ImageView imageView, ImageView imageViewShift)
{
    this->imageView = imageView;
    this->imageViewShift = imageViewShift;
}

Layer::ImageView Layer::GetImageView(bool shiftDown) const
{
    return shiftDown ? imageViewShift : imageView;
}
