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

//--------

CombosHandler::CombosHandler()
{
}

void CombosHandler::Prepare(TextComboDefs textCombos, const Layer* mainLayer)
{
    StringCombo stringCombo({}, ""); // dummy, we just need it to call New() to create new combos
    ParseCombos(textCombos.txtCombos, stringCombo, true, mainLayer);
    ParseCombos(textCombos.txtCombosQwerty, stringCombo, false, mainLayer);

    CommandCombo cmdCombo({}, Commands::None); // dummy, we just need it to call New() to create new combos
    ParseCombos(textCombos.txtCmdCombosQwerty, cmdCombo, false, mainLayer);

    KeysCombo keysCombo({}, {}); // dummy, we just need it to call New() to create new combos
    ParseCombos(textCombos.txtKeysCombosQwerty, keysCombo, false, mainLayer);

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
}

void CombosHandler::ParseCombos(const StringPairList& inputTextCombos, const ICombo& refCombo, bool reverseMap, const Layer* mainLayer)
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

bool CombosHandler::Handle(const KbdHookEvent& event, IKeyboard* kbd)
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
