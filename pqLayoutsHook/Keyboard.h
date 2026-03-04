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

#include <cassert>
#include "CommonTypes.h"
#include "util.h"
#include "layout.h"
#include "Notification.h"
#include "chord.h"
#include "ChordingData.h"
#include "combos.h"
#include "AdaptivesHandler.h"
#include "IKeyboard.h"
#include "ILayout.h"

class KbdHook; // fwd


class Keyboard : public IKeyboard, public ILayout
{
public:
    Keyboard(DWORD injectedFromMeValue);
    
    // IKeyboard interface
    void SetMainWnd(HWND hMainWindow) override;
    void SetMainWndMsg(int mainWndMsg) override;
    bool CheckForSuspendKey(const KbdHookEvent& event) override;
    bool ProcessKeyAction(const KbdHookEvent& event, KeyActions::IKeyAction* action, bool wasDown) override;
    bool ProcessCapsWord(const KbdHookEvent& event) override;
    bool OnKeyEvent(const KbdHookEvent & event) override;
    void TrackModifiers(VeeKee vk, bool pressed) override;
    void TrackMappedKeyDown(VeeKee physicalVk, KeyActions::IKeyAction* mapped, bool pressed) override;
    bool ToggleSuspend() override;
    bool Suspended() override;
    void SuspendKey(VeeKee) override;
    void QuitKey(VeeKee) override;
    void Notify(HookKbd::Notif, LPARAM) override;
    void ReplayEvents(const std::vector<KbdHookEvent>& events) override;
    void OutNbKeysDn() override;
    void SetLastVkCodeDown(DWORD) override;

    // ILayout interface
    bool AddLayer(const Layer::Id_t&, Layer::Idx_t& newLayerIdx) override;
    bool SetLayerAccessKey(const Layer::Id_t& layerId, KeyDef accessKey, bool isToggle, KeyValue keyOnTap) override;
    void PrepareAdaptives() override;
    void PrepareCombos() override;
    const Layer* GetMainLayer() override;
    bool GotoMainLayer() override;
    bool GotoLayer(Layer::Idx_t layerIdx) override;
    bool GotoLayer(const Layer::Id_t& layerId) override;
    const Layer* CurrentLayer() const override;
    const KeyMapping* Mapping(VeeKee vk) override;
    KeyValue VkMapping(VeeKee vk) const override;
    VeeKeeEx ReverseMapping(VeeKeeEx vk) const override;
    bool AddMapping(KeyValue vkFrom, KeyValue vkTo) override;
    KeyActions::IKeyAction* GetKeyAction(VeeKee vk) const override;
    KeyActions::IKeyAction* GetKeyAction(VeeKee vk, Layer::Idx_t layerIdx) const override;
    bool AddDualModeModifier(KeyDef  key, KeyValue modifierKey, KeyValue tapKey) override;
    bool AddChord(Kord& chord, const std::list<KeyActions::KeyActionPair>& keyActions) override;
    bool InitChordingKeys(const ChordingKeys& chordingKeys) override;
    void SetLeftHandPrefix(Layer::Id_t lpsteaksLayerName1, Layer::Id_t lpsteaksLayerName2, std::string lpsteaksPrefix1, std::string lpsteaksPrefix2) override;
    bool TapVk(const KeyValue& key) override;
    bool SendVk(const KeyValue& key, bool pressed) override;
    void SendString(const std::string& textString) override;
    bool HandleCommandCode(Commands command) override;
    void SetImageFilename(const WCHAR* filename) override;
    const std::wstring& GetImageFilename() const override;
    void SetImageView(Layer::ImageView imageView, Layer::ImageView imageViewShift) const override;
    Layer::ImageView GetImageView() const override;

protected:

    void MappedKeyDown(VeeKee physicalVk, KeyActions::IKeyAction* mapped, bool pressed);
    KeyActions::IKeyAction* MappedKeyDown(VeeKee vk) const;

    void ModifierDown(VeeKee vk, bool pressed);
    bool ModifierDown(VeeKee vk) const;

    bool ShiftDown() const;
    bool CtrlDown() const;

    void SetupInputKey(INPUT& input, VeeKee vk, bool pressed);

    bool HandleChording(const KbdHookEvent& event, const ChordingKey* chordingKey);
    bool CheckLpChordsLayers();
    bool OnPo2LayersChord();
    void OnCompletedChord();
    void ResumeChording();
    void SuspendChording();
    void ReplayCancelledChord();

    bool OnKeyEventLevel2(const KbdHookEvent& event);
    bool IsSelfInjected(const KbdHookEvent& event);

    static bool IsModifier(VeeKee vk);
    static bool IsExtended(VeeKee vk);
    static bool IsShift(VeeKee vk);

    KeyActions::IKeyAction* GetKeyActionFromCaseMapping(const CaseMapping* caseMapping) const;

private:
    // first=pressed *physical* key, second=what we do on that key(ie mapped value)
    std::map<VeeKee, KeyActions::IKeyAction*> downMappedKeys;

    // at a logical level, whatever the source
    VeeKeeSet downModifiers{};

    Kord chord{};             // current chord being built / cumulated as keys are pressed
    bool chordingSuspended{};
    ChordingData chording{};

    // lpsteaks (left hand prefixed chords, right hand is order dependent)
    Layer::Idx_t lpsteaksLayer1{};
    Layer::Idx_t lpsteaksLayer2{};
    Layer::Id_t lpsteaksLayerName1{};
    Layer::Id_t lpsteaksLayerName2{};
    std::string lpsteaksPrefix1{};        // prefix for layer1 -> layer2
    std::string lpsteaksPrefix2{};        // prefix for layer2 -> layer1

    DWORD lastKeypressTick{}; // time tick of the last key press event
    DWORD lastVkCodeDown{};

    Layout layout{};
    bool suspended{};
    bool adaptivesOn{true};
    VeeKee suspendKey{};
    VeeKee quitKey{};

    CapsWordType capsWordType{};
    bool capitalizeNext{};

    DWORD injectedFromMeValue{};
    HWND hMainWindow{};
    int mainWndMsg{};
    VeeKeeSet isprint{};

    std::wstring imageFilename{};

    AdaptivesHandler adaptivesHandler{};
    CombosHandler combosHandler{};

    static VeeKeeSet modifiers;
    static VeeKeeSet extended;

};
