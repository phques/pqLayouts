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

class IKeyboard;

struct TextComboDefs
{
    StringPairList& txtCombos;
    StringPairList& txtCombosQwerty;
    StringPairList& txtKeysCombosQwerty;
    StringPairList& txtCmdCombosQwerty;
};


class ICombo
{
public:
    virtual const VeeKeeVector& GetTriggerVks() const = 0;
    virtual void Fire(IKeyboard* kbd) = 0;
    virtual ICombo* New(const VeeKeeVector& triggers, const std::string& output) const = 0;
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

    virtual void Fire(IKeyboard* kbd);
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

    virtual void Fire(IKeyboard* kbd);
    virtual ICombo * New(const VeeKeeVector& triggers, const std::string& output) const;

private:
    std::vector<KeyValue> outKeys;
};

//------

// for commands, like CapsWord
class CommandCombo : public ComboBase
{
public:
    CommandCombo(const VeeKeeVector& triggers, Commands command);

    virtual void Fire(IKeyboard* kbd);
    virtual ICombo* New(const VeeKeeVector& triggers, const std::string& command) const;

private:
    Commands command;
};

//------


enum class ComboState
{
    Idle, Constructing, Holding, ReadyToFire, NbStates
};


// tracking of a combo's construction's as keys are typed
class ComboStateInfo
{
public:
    ComboStateInfo(ICombo* combo);
    std::string ToString() const;
    ComboState GetState() const { return state; }
    void SetState(ComboState newState);
    int StateIndex() const { return (int)state; }

    void Fire(IKeyboard* kbd);
    void Reset();

    // State machine methods
    bool OnKeyDown(const KbdHookEvent& event, IKeyboard* kbd);
    bool OnKeyUp(const KbdHookEvent& event, IKeyboard* kbd);

    bool IsHolding() const;
    bool IsConstructing() const;
    bool IsIdle() const;
    bool IsTimedOut(DWORD currentTick, DWORD timeOut) const;
    bool ShouldFire() const;

    ICombo* GetCombo() const { return combo; }
    const VeeKeeVector& GetTriggerVks() const { return combo->GetTriggerVks(); }

private:
    ComboState state{};
    VeeKeeVector pressedVks;
    VeeKeeVector releasedVks;
    DWORD firstDownTick{};
    ICombo* combo{};
};


class CombosHandler
{
public:
    CombosHandler();

    void Prepare(TextComboDefs textCombos, const Layer* mainLayer);
    bool HandleKbdEvent(const KbdHookEvent& event, IKeyboard* kbd);

    bool FireReadyCombo(IKeyboard* kbd);

private:
    void Reset();

    void Parse(const StringPairList& inputTextCombos, const ICombo& refCombo, bool reverseMap, const Layer* mainLayer);
    bool ExecuteCombo(const std::vector<KbdHookEvent>& events, const VeeKeeVector& vks, IKeyboard* kbd);
    bool HandleOrig(const KbdHookEvent& event, IKeyboard* kbd);

private:
    std::map<VeeKeeVector, ICombo*> combos;
    std::list<ComboStateInfo> trackedCombos;

    std::vector<KbdHookEvent > eventsDown;

    std::set<VeeKee> comboKeys;
    VeeKeeVector vksDown;
    bool cumulating{};
};

