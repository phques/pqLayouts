// combos.cpp

// Copyright 2026 Philippe Quesnel  
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
#include "keyboard.h"
#include "combos.h"
#include "KeyParser.h"
#include "util.h"

//--------

StringCombo::StringCombo(const VeeKeeVector& triggers, const std::string& output) : ComboBase(triggers), output(output)
{
}

void StringCombo::Fire(Keyboard & kbd)
{
    Printf("firing StringCombo\n");
    kbd.SendString(output);
}

ICombo* StringCombo::New(const VeeKeeVector& triggers, const std::string& output) const
{
    return new StringCombo(triggers, output);
}


//--------

KeysCombo::KeysCombo(const VeeKeeVector& triggers, const std::vector<KeyValue>& outKeys) 
    : ComboBase(triggers),
    outKeys(outKeys)
{
}

void KeysCombo::Fire(Keyboard& kbd)
{
    Printf("firing KeysCombo\n");
    for (const auto& key : outKeys)
    {
        kbd.TapVk(key);
    }
}

ICombo* KeysCombo::New(const VeeKeeVector& triggers, const std::string& output) const
{
    // parse vks from output as separate keys, supporting cltr,shift etc
    StringTokener tokener(output, 0);
    KeyParser parser(tokener, "");
    std::vector<KeyValue> keys;
    
    if (parser.ParseKeys(keys))
    {
        return new KeysCombo(triggers, keys);
    }

    return nullptr;
}

//------

CommandCombo::CommandCombo(const VeeKeeVector& triggers, Actions action) : ComboBase(triggers), action(action)
{
}

void CommandCombo::Fire(Keyboard& kbd)
{
    Printf("firing CommandCombo\n");
    kbd.HandleActionCode(action);
}

ICombo* CommandCombo::New(const VeeKeeVector& triggers, const std::string& command) const
{
    Actions action = LookupActionName(command);

    return new CommandCombo(triggers, action);
}
