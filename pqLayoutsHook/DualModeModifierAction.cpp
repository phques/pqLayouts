// Copyright 2025 Philippe Quesnel  
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
#include "DualModeModifierAction.h"
#include "Keyboard.h"

namespace KeyActions
{

DualModeModifierAction::DualModeModifierAction(KeyDef keydef, KeyValue modifierKey, KeyValue keyOnTap) :
        modifierKey(modifierKey),
        keyOnTap(keyOnTap)
{
}

bool DualModeModifierAction::OnKeyDown(Keyboard* kbd)
{
    // send the modifier down
    kbd->SendVk(modifierKey, true);

    // eat original key
    return true;
}

bool DualModeModifierAction::OnKeyUp(Keyboard* kbd, bool isTap)
{
    if (isTap)
    {
        // send the modifier up
        kbd->SendVk(modifierKey, true);
        
        // send the onTap key
        kbd->SendVk(keyOnTap, true);
        kbd->SendVk(keyOnTap, false);
    }
    else
    {
        // coming out of the layer
        // return to main layer
        kbd->GotoMainLayer();
    }

    // eat original key
    return true;
}

}
