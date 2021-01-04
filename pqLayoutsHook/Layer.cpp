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

bool Layer::AddMapping(KeyValue from, KeyValue to)
{
    CaseMapping& caseMapping = mappings[from.Vk()];
    
    if (from.Shift())
        caseMapping.shifted = KeyMapping(from, to);
    else
        caseMapping.nonShifted = KeyMapping(from, to);

    return true;
}
