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
#include "MakeStickyAction.h"
#include "Keyboard.h"


namespace KeyActions
{

MakeStickyAction::MakeStickyAction(VeeKee vk) : vk(vk)
{
}

#if 1
bool MakeStickyAction::OnKeyDown(Keyboard* kbd)
{
    kbd->MakeSticky(vk);

    // take note of down original input / mapped keys 
    kbd->TrackMappedKeyDown(vk, this, true);

    return true;
}

bool MakeStickyAction::OnKeyUp(Keyboard* kbd, bool isTap)
{
    kbd->MakeSticky(0);
    return true;
}

#else
bool MakeStickyAction::OnKeyDown(Keyboard* kbd)
{
    kbd->MakeSticky(vk);

    // take note of down original input / mapped keys 
    kbd->TrackMappedKeyDown(vk, this, true);

    return true;
}

bool MakeStickyAction::OnKeyUp(Keyboard* kbd, bool isTap)
{
    kbd->MakeSticky(0);
    return true;
}

#endif // 1


}
