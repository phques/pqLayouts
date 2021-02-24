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
#include "MakeStickyAction.h"
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

bool Layer::AddMapping(KeyValue from, KeyValue to, bool controlMapping)
{
    Printf("Add mapping from %02X, to %02X\n", from.Vk(), to.Vk());

    to.Control(controlMapping);
    IKeyAction* action = new KeyOutAction(from, to);

    return AddMapping(from, action);
}

bool Layer::AddStickyMapping(KeyValue vk)
{
    Printf("Add sticky mapping 0x%02X\n", vk.Vk());

    IKeyAction* action = new MakeStickyAction(vk.Vk());

    return AddMapping(vk, action);
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
