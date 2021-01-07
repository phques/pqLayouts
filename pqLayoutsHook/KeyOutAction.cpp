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
#include "KeyOutAction.h"
#include "Keyboard.h"


KeyOutAction::KeyOutAction(KeyDef inKey, KeyValue outKey) : outKey(outKey), inKey(inKey)
{
}

bool KeyOutAction::OnkeyDown(Keyboard* kbd)
{
    bool ret = kbd->SendVk(outKey, true);
    if (ret)
    {
        // take note of down original input / mapped keys 
        kbd->TrackMappedKeyDown(inKey.Vk(), this, true);

        // and modifiers
        kbd->TrackModifiers(outKey.Vk(), true);
    }

    return ret;
}

bool KeyOutAction::OnkeyUp(Keyboard* kbd)
{
    // no output on UP for ctrl-x type keys
    if (outKey.Control())
    {
        return true; // eat input key (control-xx map
    }
    
    bool ret = kbd->SendVk(outKey, false);
    if (ret)
    {
        // take note of down *original input* / mapped keys 
        kbd->TrackMappedKeyDown(inKey.Vk(), this, false);

        // and modifiers
        kbd->TrackModifiers(outKey.Vk(), false);
    }

    return ret;
}
