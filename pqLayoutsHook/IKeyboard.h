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
#pragma once

#include <string>
#include <list>
#include "CommonTypes.h"
#include "KeyMapping.h"
#include "layout.h"
#include "Notification.h"
#include "chord.h"
#include "ChordingData.h"

// Forward declarations
class KbdHookEvent;
class KeyMapping;
class KeyDef;
class Layer;

//namespace KeyActions {
//    class IKeyAction;
//    class KeyActionPair;
//}

class IKeyboard
{
public:
    virtual ~IKeyboard() = default;

    virtual void SetMainWnd(HWND hMainWindow) = 0;
    virtual void SetMainWndMsg(int mainWndMsg) = 0;

    virtual bool AddLayer(const Layer::Id_t&, Layer::Idx_t& newLayerIdx) = 0;
    virtual bool SetLayerAccessKey(const Layer::Id_t& layerId, KeyDef accessKey, bool isToggle, KeyValue keyOnTap) = 0;

    virtual void PrepareAdaptives() = 0;
    virtual void PrepareCombos() = 0;

    virtual const Layer* GetMainLayer() = 0;

    virtual bool GotoMainLayer() = 0;
    virtual bool GotoLayer(Layer::Idx_t layerIdx) = 0;
    virtual bool GotoLayer(const Layer::Id_t& layerId) = 0;
    virtual const Layer* CurrentLayer() const = 0;

    virtual const KeyMapping* Mapping(VeeKee vk) = 0;
    virtual KeyValue VkMapping(VeeKee vk) const = 0;
    virtual VeeKeeEx ReverseMapping(VeeKeeEx vk) const = 0;

    virtual bool AddMapping(KeyValue vkFrom, KeyValue vkTo) = 0;
    virtual KeyActions::IKeyAction* GetKeyAction(VeeKee vk) const = 0;
    virtual KeyActions::IKeyAction* GetKeyAction(VeeKee vk, Layer::Idx_t layerIdx) const = 0;

    virtual bool AddDualModeModifier(KeyDef key, KeyValue modifierKey, KeyValue tapKey) = 0;

    virtual bool AddChord(Kord& chord, const std::list<KeyActions::KeyActionPair>& keyActions) = 0;
    virtual bool InitChordingKeys(const ChordingKeys& chordingKeys) = 0;
    virtual void SetLeftHandPrefix(Layer::Id_t lpsteaksLayerName1, Layer::Id_t lpsteaksLayerName2, std::string lpsteaksPrefix1, std::string lpsteaksPrefix2) = 0;

    virtual bool CheckForSuspendKey(const KbdHookEvent& event) = 0;
    virtual bool ProcessKeyAction(const KbdHookEvent& event, KeyActions::IKeyAction* action, bool wasDown) = 0;

    virtual bool ProcessCapsWord(const KbdHookEvent& event) = 0;
    virtual bool OnKeyEvent(const KbdHookEvent& event) = 0;

    virtual void OutNbKeysDn() = 0;


    virtual bool TapVk(const KeyValue& key) = 0;
    virtual bool SendVk(const KeyValue& key, bool pressed) = 0;
    virtual void SendString(const std::string& textString) = 0;
    virtual bool HandleCommandCode(Commands command) = 0;

    virtual void TrackModifiers(VeeKee vk, bool pressed) = 0;
    virtual void TrackMappedKeyDown(VeeKee physicalVk, KeyActions::IKeyAction* mapped, bool pressed) = 0;

    virtual bool ToggleSuspend() = 0;
    virtual bool Suspended() = 0;
    virtual void SuspendKey(VeeKee) = 0;
    virtual void QuitKey(VeeKee) = 0;

    virtual void SetImageFilename(const WCHAR* filename) = 0;
    virtual const std::wstring& GetImageFilename() const = 0;

    virtual void SetImageView(Layer::ImageView imageView, Layer::ImageView imageViewShift) const = 0;
    virtual Layer::ImageView GetImageView() const = 0;

    virtual void Notify(HookKbd::Notif, LPARAM) = 0;
};