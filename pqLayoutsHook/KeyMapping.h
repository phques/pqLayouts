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

namespace KeyActions
{

// action classes interface / base class
class IKeyAction
{
    friend class Keyboard;

public:
    IKeyAction() : downTimeTick(0), upTimeTick(0) {}
    virtual ~IKeyAction() {}

    // returns true to 'eat' the original received key (ie do not forward to next kbd hook)
    virtual bool OnKeyDown(Keyboard*) = 0;
    virtual bool OnKeyUp(Keyboard*, bool isTap) = 0;

    // return true to automatically skip repeats of key down
    virtual bool SkipDownRepeats(Keyboard*) const = 0;

protected:
    DWORD downTimeTick; // timetick at which the key was pressed
    DWORD upTimeTick;   // timetick at which the key was released
};


}  // namespace KeyActions


//------------


class KeyMapping
{
public:
    KeyMapping();
    KeyMapping(KeyDef key, KeyActions::IKeyAction* mapping);

    const KeyDef& Key() const { return key; }
    KeyActions::IKeyAction* Mapping() const { return mapping; }

private:
    KeyDef key;        // from
    KeyActions::IKeyAction* mapping;    // to
};

//------------


// holds the mappings for both shifted and non shifted version of a key
struct CaseMapping
{
    KeyMapping nonShifted;
    KeyMapping shifted;
};
