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

#include "dllExport.h"

typedef DWORD VeeKee;

typedef std::unordered_set<VeeKee> VeeKeeSet;
typedef std::vector<VeeKee> VeeKeeVector;
typedef std::list<VeeKee> VeeKeeList;

// types for interop use
typedef int BoolP;

struct KeyValueP
{
    VeeKee vk;
    UINT scancode;
    BoolP shift;
    BoolP control;
    BoolP alt;
};

// represents a single key (e.g. the key that holds ';' and ':' characters)
class KeyDef
{
public:

    KeyDef();
    PQHOOK_API KeyDef(VeeKee vk, UINT scancode);
     
    VeeKee Vk() const { return vk; }
    UINT Scancode() const { return scancode; }

protected:
    VeeKee vk;
    UINT scancode;
};

//------------------

// represents a more specific instance of a key,
// e.g. key ";" with shift ON => ':'
class KeyValue : public KeyDef
{
public:
    KeyValue();
    KeyValue(char character);
    PQHOOK_API KeyValue(VeeKee, UINT scancode, bool shift=false, bool control=false, bool alt=false);

    bool Shift() const { return shift; }
    bool Control() const { return control; }
    bool Alt() const { return alt; }
    KeyValue& Shift(bool v) { shift=v; return *this; }
    KeyValue& Control(bool v) { control=v; return *this; }
    KeyValue& Alt(bool v) { alt=v; return *this; }
    
    static std::list<KeyValue> KeyValues(const std::string& str);

private:
    bool shift;
    bool control;
    bool alt;
};

// to ease creating a KeyValue for ctrl-XX
class CtrlKeyValue : public KeyValue
{
public:
    CtrlKeyValue(VeeKee vk) : KeyValue(vk, 0, false, true) {}
};
