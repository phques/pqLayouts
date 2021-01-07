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

#pragma once

#include "KeyMapping.h"

class Layer
{
public:
    typedef std::string Id_t;
    typedef unsigned int Idx_t;

public:
    Layer(const Id_t& name, Idx_t layerIdx);

    const Id_t& Name() const { return name; }
    Idx_t LayerIdx() const { return layerIdx;}

    const CaseMapping* Mapping(VeeKee) const;
    bool AddMapping(KeyValue from, KeyValue to, bool controlMapping);

private:
    Id_t name;
    Idx_t layerIdx;
    std::map<VeeKee, CaseMapping> mappings;
};

