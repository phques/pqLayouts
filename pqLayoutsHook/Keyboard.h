#pragma once
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

#include <cassert>
#include "util.h"
#include "layout.h"

class KbdHook; // fwd

typedef std::unordered_set<VeeKee> VeeKeeSet;


class Keyboard
{
public:
    Keyboard(DWORD injectedFromMeValue);

    bool AddLayer(const Layer::Id_t&, Layer::Idx_t& newLayerIdx);
    bool SetLayerAccessKey(const Layer::Id_t& layerId, KeyDef keydef);

    bool GotoMainLayer();
    bool GotoLayer(Layer::Idx_t layerIdx);
    bool GotoLayer(const Layer::Id_t& layerId);

    const KeyMapping* Mapping(VeeKee vk);
    bool AddMapping(KeyValue vkFrom, KeyValue vkTo);
    bool AddCtrlMapping(KeyValue vkFrom, KeyValue vkTo);

    bool OnKeyEvent(KbdHookEvent&, DWORD injectedFromMeValue);
    // dbg
    void OutNbKeysDn();

    bool MyIsPrint(VeeKee vk) { return isprint.find(vk) != isprint.end(); }

    bool SendVk(const KeyValue& key, bool down);
    void TrackModifiers(VeeKee vk, bool down);
    void TrackMappedKeyDown(VeeKee physicalVk, IKeyAction* mapped, bool down);

protected:

    void MappedKeyDown(VeeKee physicalVk, IKeyAction* mapped, bool down);
    IKeyAction* MappedKeyDown(VeeKee vk) const;

    void ModifierDown(VeeKee vk, bool down);
    bool ModifierDown(VeeKee vk) const;

    bool ShiftDown() const;

    IKeyAction* GetMappingValue(KbdHookEvent& event);

    void SetupInputKey(INPUT& input, VeeKee vk, bool down, DWORD injectedFromMeValue);

    static bool IsModifier(VeeKee vk);
    static bool IsExtended(VeeKee vk);
    static bool IsShift(VeeKee vk);

private:
    // first=pressed *physical* key, second=what we do on that key(ie mapped value)
    std::map<VeeKee, IKeyAction*> downMappedKeys;

    // at a logical level, whatever the source
    VeeKeeSet downModifiers; 

    Layout layout;

    DWORD injectedFromMeValue;
    VeeKeeSet isprint;
    static VeeKeeSet modifiers;
    static VeeKeeSet extended;
};
