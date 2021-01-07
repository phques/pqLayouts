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
#include "LayerAccessAction.h"
#include "Keyboard.h"

LayerAccessAction::LayerAccessAction(KeyDef keydef, Layer::Idx_t layerIdx) : keydef(keydef), layerIdx(layerIdx)
{
}

bool LayerAccessAction::OnkeyDown(Keyboard* kbd)
{
    // going into a new layer
    kbd->GotoLayer(layerIdx);

    // eat access key
    return true;
}

bool LayerAccessAction::OnkeyUp(Keyboard* kbd)
{
    // coming out of the layer
    // return to main layer
    kbd->GotoMainLayer();

    // eat access key
    return true;
}
