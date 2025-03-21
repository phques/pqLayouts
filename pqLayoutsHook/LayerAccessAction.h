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
#include "Layer.h"
#include "KeyMapping.h"

namespace KeyActions
{

class LayerAccessAction : public IKeyAction
{
public:
    LayerAccessAction(KeyDef keydef, Layer::Idx_t layerIdx, bool toggleOnTap, KeyValue keyOnTap);
    virtual bool OnKeyDown(Keyboard*);
    virtual bool OnKeyUp(Keyboard*, bool isTap);
    virtual bool SkipDownRepeats(Keyboard*) const { return true; }
    virtual bool IsLayerAccess() const { return true; }

protected:
    KeyDef keydef;
    Layer::Idx_t layerIdx;
    Layer::Idx_t layerIdxOnKeypress; // the active layer when the key was pressed
    bool toggleOnTap;
    KeyValue keyOnTap;  // output this key when access is tapped
};

}
