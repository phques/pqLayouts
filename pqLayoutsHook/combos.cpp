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

static std::vector<const char*> stateNames = 
{
    "Idle", "Constructing", "Holding", "ReadyToFire", "Holding"
};

//--------

StringCombo::StringCombo(const VeeKeeVector& triggers, const std::string& output) : ComboBase(triggers), output(output)
{
}

void StringCombo::Fire(IKeyboard* kbd)
{
    Printf("firing StringCombo\n");
    kbd->SendString(output);
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

void KeysCombo::Fire(IKeyboard* kbd)
{
    Printf("firing KeysCombo\n");
    for (const auto& key : outKeys)
    {
        kbd->TapVk(key);
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

CommandCombo::CommandCombo(const VeeKeeVector& triggers, Commands command) : ComboBase(triggers), command(command)
{
}

void CommandCombo::Fire(IKeyboard* kbd)
{
    Printf("firing CommandCombo\n");
    kbd->HandleCommandCode(command);
}

ICombo* CommandCombo::New(const VeeKeeVector& triggers, const std::string& commandName) const
{
    Commands command = LookupCommandName(commandName);

    return new CommandCombo(triggers, command);
}

//--------

ComboStateInfo::ComboStateInfo(ICombo* combo) : combo(combo)
{
}

std::string ComboStateInfo::ToString() const
{
    return VkUtil::VksToString(combo->GetTriggerVks());
}

void ComboStateInfo::SetState(ComboState newState)
{
    Printf("combo %s, state %s -> %s\n", 
        ToString().c_str(), stateNames[(int)state], stateNames[(int)newState]);
    state = newState;
}

void ComboStateInfo::Reset()
{
    state = ComboState::Idle;
    pressedVks.clear();
    releasedVks.clear();
    firstDownTick = 0;
}

bool ComboStateInfo::OnKeyDown(const KbdHookEvent& event, IKeyboard* kbd)
{
    switch (state)
    {
    case ComboState::Idle:
        if (VkUtil::Contains(combo->GetTriggerVks(), event.vkCode))
        {
            Printf("starting combo %s construction %c\n",
                ToString().c_str(), VkUtil::VkToChar((WORD)event.vkCode));

            pressedVks.push_back(event.vkCode);
            SetState(ComboState::Constructing);
            firstDownTick = event.time ;
            return true;
        }
        return false; // ignore

    case ComboState::Constructing:
        if (!VkUtil::Contains(combo->GetTriggerVks(), event.vkCode))
        {
            Printf("unexpected key down %c while constructing combo %s, resetting combo\n",
                VkUtil::VkToChar((WORD)event.vkCode), ToString().c_str());

            Reset();
            return false; // ignore
        }

        // new combo key down
        pressedVks.push_back(event.vkCode);
        std::sort(pressedVks.begin(), pressedVks.end());

        // got all keys?
        if (pressedVks == combo->GetTriggerVks())
        {
            SetState(ComboState::Holding);
        }
        return true;

    case ComboState::Holding:
        if (VkUtil::Contains(releasedVks, event.vkCode) ||
            !VkUtil::Contains(combo->GetTriggerVks(), event.vkCode))
        {
            Printf("unexpected key down %c while holding combo %s, resetting\n",
                VkUtil::VkToChar((WORD)event.vkCode), ToString().c_str());

            Reset();
            return false;
        }
        return true; // eat auto-repeat
    }

    return false;
}

bool ComboStateInfo::OnKeyUp(const KbdHookEvent& event, IKeyboard* kbd)
{
    switch (state)
    {
        case ComboState::Idle:
            return false;

        case ComboState::Constructing:
            Printf("key up %c while constructing combo %s, resetting\n",
                VkUtil::VkToChar((WORD)event.vkCode), ToString().c_str());

            Reset();
            return false;

        case ComboState::Holding:
            // should not happen!
            if (VkUtil::Contains(releasedVks, event.vkCode) ||
                !VkUtil::Contains(combo->GetTriggerVks(), event.vkCode))
            {
                Printf("unexpected key up %c while holding combo %s, resetting\n",
                    VkUtil::VkToChar((WORD)event.vkCode), ToString().c_str());

                Reset();
                return false;
            }

            // add to released
            Printf("key up %c while holding combo %s, adding to released\n", 
                VkUtil::VkToChar((WORD)event.vkCode), ToString().c_str());

            releasedVks.push_back(event.vkCode);
            std::sort(releasedVks.begin(), releasedVks.end());

            // got all keys?
            if (releasedVks == combo->GetTriggerVks())
            {
                Printf("combo ready to fire! %s\n", ToString().c_str());
                SetState(ComboState::ReadyToFire);
                return true;
            }
    }

    return false;
}

bool ComboStateInfo::IsHolding() const 
{ 
    return state == ComboState::Holding; 
}

bool ComboStateInfo::IsConstructing() const 
{ 
    return state == ComboState::Constructing; 
}

bool ComboStateInfo::IsIdle() const 
{ 
    return state == ComboState::Idle; 
}

bool ComboStateInfo::IsTimedOut(DWORD currentTick, DWORD timeOut) const
{
    return TickCountDiff(currentTick, firstDownTick) > timeOut;
}

bool ComboStateInfo::ShouldFire() const
{
    return state == ComboState::ReadyToFire;
}

void ComboStateInfo::Fire(IKeyboard* kbd)
{
    Printf("Firing combo! %s\n", ToString().c_str());
    combo->Fire(kbd);
    SetState(ComboState::Idle);
}

//--------

CombosHandler::CombosHandler()
{
}

bool CombosHandler::Prepare(TextComboDefs& textCombos, const Layer* mainLayer)
{
    StringCombo stringCombo({}, ""); // dummy, we just need it to call New() to create new combos
    Parse(textCombos.txtCombos, stringCombo, true, mainLayer);
    Parse(textCombos.txtCombosQwerty, stringCombo, false, mainLayer);

    CommandCombo cmdCombo({}, Commands::None); // dummy, we just need it to call New() to create new combos
    Parse(textCombos.txtCmdCombosQwerty, cmdCombo, false, mainLayer);

    KeysCombo keysCombo({}, {}); // dummy, we just need it to call New() to create new combos
    Parse(textCombos.txtKeysCombosQwerty, keysCombo, false, mainLayer);

    // Save all combo trigger keys
    for (const auto& combo : combos)
    {
        for (const auto& vk : combo.first)
        {
            comboKeys.insert(vk);
        }
    }

    // Populate trackedCombos with the combos to track
    trackedCombos.clear();

    for (const auto& pair : combos)
    {
        const VeeKeeVector& triggers = pair.first;
        ICombo* combo = pair.second;

        trackedCombos.push_back(ComboStateInfo(combo));
    }

    return true;
}

void CombosHandler::Parse(const StringPairList& inputTextCombos, const ICombo& refCombo, bool reverseMap, const Layer* mainLayer)
{
    // Dont clear combos, add to them

    for (const auto& pair : inputTextCombos)
    {
        // saved with the 'physical'/qwerty key as the lookup values.

        // so: Convert first item string (the 'from') into a VeeKeeVector 
        VeeKeeVector triggerVks;
        const std::string triggerChars = pair.first;

        if (!mainLayer->VksFromString(triggerChars, reverseMap, triggerVks))
        {
            Printf("Skipping combo for keys sequence '%s' due to unmapped character.\n", triggerChars.c_str());
            continue;
        }

        // vks need to be sorted for combos
        std::sort(triggerVks.begin(), triggerVks.end());

        // Create newcombo and save
        ICombo* newCombo = refCombo.New(triggerVks, pair.second);
        if (newCombo != nullptr)
        {
            combos[triggerVks] = newCombo;
        }
    }
}

bool CombosHandler::ExecuteCombo(const std::vector<KbdHookEvent>& events, const VeeKeeVector& vks, IKeyboard* kbd)
{
    auto foundComboIt = combos.find(vks);
    if (foundComboIt != combos.end())
    {
        Printf("found combo!\n");
        foundComboIt->second->Fire(kbd);

        return true;
    }

    // no combo found
    return false;
}

bool CombosHandler::HandleOrig(const KbdHookEvent& event, IKeyboard* kbd)
{
    // --- Combos handling ---

    // ##NB: it is important that we let non combo keys through here,
    //       because some might not be mapped, and ReplayEvents() will have no effects for those!!
    const bool isComboKey = (comboKeys.find(event.vkCode) != comboKeys.end());
    const bool isComboKeyDown = isComboKey && event.Down();

    if (!cumulating && !isComboKeyDown)
    {
        Printf("not cumulating, not comboKeyDown\n");
        return false;
    }

    if (!cumulating && isComboKeyDown)
    {
        Printf("1st combo key\n");
        cumulating = true;
        eventsDown.push_back(event);
        vksDown.push_back(event.vkCode);
        std::sort(vksDown.begin(), vksDown.end());
        return true;
    }

    if (cumulating)
    {
        const bool isAlreadyDown = isComboKeyDown && VkUtil::Contains(vksDown, event.vkCode);

        if (!isComboKeyDown || isAlreadyDown || event.TimeDiff(eventsDown[0]) > 90) //ms (old=50,75)
        {
            Printf("cancel cumul\n");
            kbd->ReplayEvents(eventsDown);
            cumulating = false;
            eventsDown.clear();
            vksDown.clear();
            return false;
        }

        eventsDown.push_back(event);
        vksDown.push_back(event.vkCode);
        std::sort(vksDown.begin(), vksDown.end());

        if (ExecuteCombo(eventsDown, vksDown, kbd))
        {
            // we simulated keys, so lastVkCodeDown is not correct anymore 
            kbd->SetLastVkCodeDown(0); 
        }
        else
        {
            Printf("cancel cumul\n");
            kbd->ReplayEvents(eventsDown);
        }

        cumulating = false;
        eventsDown.clear();
        vksDown.clear();
        return true;
    }

    return false;
}

void CombosHandler::Reset()
{
    eventsDown.clear();
    vksDown.clear();
    cumulating = false;

    for (auto& comboInfo : trackedCombos)
    {
        comboInfo.Reset();
    }
}

bool CombosHandler::HandleKbdEvent(const KbdHookEvent& event, IKeyboard* kbd)
{
    //return HandleOrig(event, kbd);

    bool eatKey{};
    bool hasNonIdle{};
    bool hadNonIdle{};
    bool hadState[(int)ComboState::NbStates] = {};
    bool hasState[(int)ComboState::NbStates] = {};

    // check current states
    for (auto& comboInfo : trackedCombos)
    {
        hadNonIdle |= comboInfo.GetState() != ComboState::Idle;
        hadState[comboInfo.StateIndex()] = true;
    }

    for (auto& comboInfo : trackedCombos)
    {
        // check for timeout
        if (comboInfo.IsConstructing() && comboInfo.IsTimedOut(event.time, 50)) //??
        {
            Printf("%c %s, cons combo %s timed out\n", 
                VkUtil::VkToChar((WORD)event.vkCode), event.Down() ? "down" : "up",
                comboInfo.ToString().c_str());
            comboInfo.Reset();
            continue;
        }

        // check for timeout
        if (comboInfo.IsHolding() && comboInfo.IsTimedOut(event.time, 250)) //??
        {
            Printf("%c %s, rel combo %s timed out\n",
                VkUtil::VkToChar((WORD)event.vkCode), event.Down() ? "down" : "up",
                comboInfo.ToString().c_str());
            comboInfo.Reset();
            continue;
        }

        // if we had a combo in Holding state, wait for it to finish,
        // don't start a new combo
        if (hadState[(int)ComboState::Holding] && comboInfo.IsIdle())
        {
            continue;
        }

        // process key event
        if (event.Down())
        {
            eatKey |= comboInfo.OnKeyDown(event, kbd);
        }
        else
        {
            eatKey |= comboInfo.OnKeyUp(event, kbd);
        }

        hasNonIdle |= !comboInfo.IsIdle();
        hasState[comboInfo.StateIndex()] = true;
    }

    bool holdingOrConstructing = hasState[(int)ComboState::Constructing] || 
                                 hasState[(int)ComboState::Holding];

    // can we fire a combo now? nb: there might be a longer one still constructing/holding
    if (hasState[(int)ComboState::ReadyToFire] && !holdingOrConstructing)
    {
        if (FireReadyCombo(kbd))
            return true; // eat key, we fired a combo
    }
    
    // should we save the current event for replaying later if combo is cancelled?
    if (holdingOrConstructing)
    {
        Printf("saving event for replaying %c\n", VkUtil::VkToChar((WORD)event.vkCode));
        eventsDown.push_back(event);
    }

    // reset if everything is cancelled
    if (hadNonIdle && !hasNonIdle)
    {
        Printf("cancel combo(s), replaying keys\n");
        kbd->ReplayEvents(eventsDown);
        Reset();
    }

    return eatKey;
}

bool CombosHandler::FireReadyCombo(IKeyboard* kbd)
{
    // find the combo to fire, there should be onlyh one
    for (auto& comboInfo : trackedCombos)
    {
        if (comboInfo.ShouldFire())
        {
            // fire it
            comboInfo.Fire(kbd);

            // we simulated keys, so lastVkCodeDown is not correct anymore 
            kbd->SetLastVkCodeDown(0);
            Reset();
            return true; // eat key, we fired a combo
        }
    }

    return false;
}
