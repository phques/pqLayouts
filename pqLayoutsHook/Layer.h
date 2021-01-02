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
#include "KeyMapping.h"

class Layer
{
public:
    typedef std::string LayerId_t;

public:
    Layer(const LayerId_t& name);

    const LayerId_t& Name() const { return name; }

    const CaseMapping* Mapping(VeeKee) const;
    bool AddMapping(KeyValue from, KeyValue to);

private:
    LayerId_t name;
    std::map<VeeKee, CaseMapping> mappings;
};

