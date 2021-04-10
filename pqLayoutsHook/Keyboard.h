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
#include "Notification.h"
#include "chord.h"
#include "chording.h"

class KbdHook; // fwd


class Keyboard
{
public:
    Keyboard(DWORD injectedFromMeValue);
    void SetMainWnd(HWND hMainWindow);
    void SetMainWndMsg(int mainWndMsg);

    bool AddLayer(const Layer::Id_t&, Layer::Idx_t& newLayerIdx);
    bool SetLayerAccessKey(const Layer::Id_t& layerId, KeyDef accessKey, bool isToggle);

    bool GotoMainLayer();
    bool GotoLayer(Layer::Idx_t layerIdx);
    bool GotoLayer(const Layer::Id_t& layerId);
    const Layer* CurrentLayer() const;

    const KeyMapping* Mapping(VeeKee vk);
    bool AddMapping(KeyValue vkFrom, KeyValue vkTo);
    bool AddStickyMapping(KeyValue vk);

    bool AddChord(Kord& chord, const std::list<KeyActions::KeyActionPair>& keyActions);
    bool InitChordingKeys(const ChordingKeys& chordingKeys);

    bool OnKeyEvent(const KbdHookEvent & event, DWORD injectedFromMeValue);
    // dbg
    void OutNbKeysDn();

    bool MyIsPrint(VeeKee vk) { return isprint.find(vk) != isprint.end(); }

    bool SendVk(const KeyValue& key, bool pressed);
    void TrackModifiers(VeeKee vk, bool pressed);
    void TrackMappedKeyDown(VeeKee physicalVk, KeyActions::IKeyAction* mapped, bool pressed);

    bool ToggleSuspend();
    bool Suspended();
    void SuspendKey(VeeKee);
    void QuitKey(VeeKee);
    void MakeSticky(VeeKee);
    VeeKee MakeSticky() const;

    void SetImageFilename(const WCHAR* filename);
    const std::wstring& GetImageFilename() const;

    void SetImageView(Layer::ImageView imageView, Layer::ImageView imageViewShift) const;
    Layer::ImageView GetImageView() const;

    void Notify(HookKbd::Notif, LPARAM);

protected:

    void MappedKeyDown(VeeKee physicalVk, KeyActions::IKeyAction* mapped, bool pressed);
    KeyActions::IKeyAction* MappedKeyDown(VeeKee vk) const;

    void ModifierDown(VeeKee vk, bool pressed);
    bool ModifierDown(VeeKee vk) const;

    bool ShiftDown() const;

    KeyActions::IKeyAction* GetMappingValue(const KbdHookEvent & event);

    void SetupInputKey(INPUT& input, VeeKee vk, bool pressed, DWORD injectedFromMeValue);

    void HandleChording(const KbdHookEvent& event, const DWORD& injectedFromMeValue);
    void OnCompletedChord(const DWORD& injectedFromMeValue);
    void ResumeChording();
    void SuspendChording();
    void ReplayCancelledChord(Kord&, DWORD injectedFromMeValue);

    static bool IsModifier(VeeKee vk);
    static bool IsExtended(VeeKee vk);
    static bool IsShift(VeeKee vk);

private:
    // first=pressed *physical* key, second=what we do on that key(ie mapped value)
    std::map<VeeKee, KeyActions::IKeyAction*> downMappedKeys;

    // at a logical level, whatever the source
    VeeKeeSet downModifiers;

    Kord chord;             // current chord being built / cumulated as keys are pressed
    bool chordingSuspended;
    Chording chording;
    VeeKeeSet chordStars;
    VeeKee star;

    DWORD lastKeypressTick; // time tick of the last key press event

    Layout layout;
    bool suspended;
    VeeKee makeSticky;
    VeeKee suspendKey;
    VeeKee quitKey;

    DWORD injectedFromMeValue;
    HWND hMainWindow;
    int mainWndMsg;
    VeeKeeSet isprint;

    std::wstring imageFilename;

    static VeeKeeSet modifiers;
    static VeeKeeSet extended;
};
