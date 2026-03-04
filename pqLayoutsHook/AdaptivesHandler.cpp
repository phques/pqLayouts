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
#include "AdaptivesHandler.h"
#include "Keyboard.h"

AdaptivesHandler::AdaptivesHandler()
{
}

bool AdaptivesHandler::ParseAdaptives(const StringPairList& txtAdaptives, const Layer* mainLayer)
{
    for (const auto& pair : txtAdaptives)
    {
        // adaptives are positional by nature.
        // they are saved with the 'physical'/qwerty key as the lookup values.

        // so: Convert first item string (the 'from') into a VeeKeeVector using ReverseMapping
        VeeKeeVector triggerVks;
        const std::string& triggerChars = pair.first;

        if (!mainLayer->VksFromString(triggerChars, true, triggerVks))
        {
            Printf("Skipping adaptive for keys sequence '%s' due to unmapped character.\n",
                triggerChars.c_str());
            continue;
        }

        // Insert into adapts2 or adapts3 based on vector size
        std::string output = std::string{ pair.second };

        if (triggerVks.size() == 2)
        {
            // add a backspace in front to get rid of the 1st typed char
            adapts2[triggerVks] = "\b" + output;
        }
        else if (triggerVks.size() == 3)
        {
            // add backspaces in front to get rid of the 1st typed char
            adapts3[triggerVks] = "\b\b" + output;
        }
        else
        {
            Printf("Warning: Adaptive for key string '%s' has unsupported size (%zu), skipping.\n",
                triggerChars.c_str(), triggerVks.size());
        }
    }

    return false;
}

bool AdaptivesHandler::ProcessAdaptives(const KbdHookEvent& event, IKeyboard* kbd)
{

    // adaptives, delay between each key allowed
    //##pq todo: this could be by layer / read from kbd file etc etc

    // do adaptives (only on main layer at the moment)
    if (event.Down() && lastDownEvent.vkCode != 0)
    {
        if (event.time - lastDownEvent.time > 175) // ms
        {
            Printf("reset lastDownEvent\n");
            prevlastDownEvent.vkCode = 0;
            lastDownEvent.vkCode = 0;
        }
        else
        {
            Printf("checking for adaptive\n");

            std::map<VeeKeeExVector, std::string>::iterator foundAdaptIt;

            // adaptives are positional by nature.
            // they were saved with the 'physical'/qwerty key as the lookup values.
            VeeKeeExVector vkeys3{ prevlastDownEvent.vkCode, lastDownEvent.vkCode, event.vkCode };
            VeeKeeExVector vkeys2{ lastDownEvent.vkCode, event.vkCode };

            if ((foundAdaptIt = adapts3.find(vkeys3)) != adapts3.end() ||
                (foundAdaptIt = adapts2.find(vkeys2)) != adapts2.end())
            {
                Printf("found adaptive!\n");
                kbd->SendString(foundAdaptIt->second);
                Printf("done sending adapt\n");

                prevlastDownEvent = lastDownEvent;
                lastDownEvent = event;
                return true; // eat key
            }
        }
    }

    if (event.Down())
    {
        prevlastDownEvent = lastDownEvent;
        lastDownEvent = event;
    }


    return false;
}
