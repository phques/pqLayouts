#pragma once

// combos.h

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

#include "keydef.h"


class Keyboard;

class ICombo
{
public:
    virtual const VeeKeeVector& GetTriggerVks() const = 0;
    virtual void Fire(Keyboard & kbd) = 0;
    virtual ICombo* New(const VeeKeeVector& triggers, const std::string& output) const = 0;
};

//----

enum class ComboState
{
    Idle, Constructing, Complete, Fired
};

// tracking of a combo's construction's as keys are typed
class ComboStateInfo
{
private:
    ComboState comboState{};
    VeeKeeVector pressedVks;
    VeeKeeVector releasedVks;
    DWORD firstDownTick{}; // time of 1st pressed key of the combo
    ICombo* combo{};
};

//--------

class ComboBase : public ICombo
{
public:
    ComboBase(const VeeKeeVector& triggers) : triggers(triggers)
    {
    }

    virtual const VeeKeeVector& GetTriggerVks() const
    {
        return triggers;
    }

private:
    VeeKeeVector triggers;
};

//---------

// for string output
// this can also start with '\01' signaling a command, 
// identified by a single '\0' prefix: "\01a", "\01b"
class StringCombo : public ComboBase
{
public:
    StringCombo(const VeeKeeVector& triggers, const std::string& output);

    virtual void Fire(Keyboard & kbd);
    virtual ICombo* New(const VeeKeeVector& triggers, const std::string& output) const;


private:
    std::string output;
};

//------

// for special characters output e.g. ctrl-c
class KeysCombo : public ComboBase
{
public:
    KeysCombo(const VeeKeeVector& triggers, const std::vector<KeyValue>& outKeys);

    virtual void Fire(Keyboard & kbd);
    virtual ICombo * New(const VeeKeeVector& triggers, const std::string& output) const;

private:
    std::vector<KeyValue> outKeys;
};

//------

// for commands, like CapsWord
class CommandCombo : public ComboBase
{
public:
    CommandCombo(const VeeKeeVector& triggers, Actions action);

    virtual void Fire(Keyboard& kbd);
    virtual ICombo* New(const VeeKeeVector& triggers, const std::string& command) const;

private:
    Actions action;
};
