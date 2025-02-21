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
#include "Keydef.h"

KeyDef::KeyDef() : vk(0), scancode(0)
{
}

KeyDef::KeyDef(VeeKee vk, UINT scancode) : vk(vk), scancode(scancode)
{
}

//-------

KeyValue::KeyValue() : shift(false), control(false)
{
}

KeyValue::KeyValue(VeeKee vk, UINT scancode, bool shift, bool control, bool alt) :  
    KeyDef(vk, scancode), 
    shift(shift), control(control), alt(alt)
{
}
