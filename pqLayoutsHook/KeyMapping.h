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

#include "keydef.h"

//------------

class Keyboard;

class IKeyAction
{
public:
    virtual ~IKeyAction() {}

    // returns true to 'eat' the original received key (ie do not forward to next kbd hook)
    PQHOOK_API virtual bool OnkeyDown(Keyboard*) = 0;
    PQHOOK_API virtual bool OnkeyUp(Keyboard*) = 0;
};

//------------


class KeyMapping
{
public:
    KeyMapping();
    KeyMapping(KeyDef key, IKeyAction* mapping);

    const KeyDef& Key() const { return key; }
    IKeyAction* Mapping() const { return mapping; }

private:
    KeyDef key;        // from
    IKeyAction* mapping;    // to
};

//------------


// holds the mappings for both shifted and non shifted version of a key
struct CaseMapping
{
    KeyMapping nonShifted;
    KeyMapping shifted;
};
