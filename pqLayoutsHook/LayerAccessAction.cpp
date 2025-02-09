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

namespace KeyActions
{

LayerAccessAction::LayerAccessAction(KeyDef keydef, Layer::Idx_t layerIdx, bool toggleOnTap, KeyValue keyOnTap) :
    keydef(keydef),
    layerIdx(layerIdx),
    layerIdxOnKeypress(0),
    toggleOnTap(toggleOnTap),
    keyOnTap(keyOnTap)
{
}

bool LayerAccessAction::OnKeyDown(Keyboard* kbd)
{
    // save which layer is active on initial keypress
    const Layer* currLayer = kbd->CurrentLayer();
    layerIdxOnKeypress = currLayer->LayerIdx();

    // going into a new layer
    kbd->GotoLayer(layerIdx);

    // eat access key
    return true;
}

bool LayerAccessAction::OnKeyUp(Keyboard* kbd, bool isTap)
{
    Printf("layer action, toggleOnTap %d isTap %d\n", toggleOnTap, isTap);

    if (toggleOnTap && isTap)
    {
        Printf("layer action, toggle\n");

        // switch between main and this layer
        if (layerIdxOnKeypress == layerIdx)
            kbd->GotoMainLayer();
        else
            kbd->GotoLayer(layerIdx);
    }
    else if (keyOnTap.Vk() != 0 && isTap)
    {
        // return to main layer
        kbd->GotoMainLayer();

        // send the key
        kbd->SendVk(keyOnTap, true);
        kbd->SendVk(keyOnTap, false);
    }
    else
    {
        // coming out of the layer
        // return to main layer
        kbd->GotoMainLayer();
    }

    // eat access key
    return true;
}

}
