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

namespace KeyActions
{

// class that output a value (another key) for a mapped key
class MakeStickyAction : public IKeyAction
{
public:
    MakeStickyAction(VeeKee vk);

    // returns true to 'eat' the original received key (ie do not forward to next kbd hook)
    virtual bool OnKeyDown(Keyboard*);
    virtual bool OnKeyUp(Keyboard*, bool isTap);
    virtual bool SkipDownRepeats(Keyboard*) const { return true; }

protected:
    VeeKee vk;
};

}
