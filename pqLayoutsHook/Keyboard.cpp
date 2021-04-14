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

#include "pch.h"
#include "Keyboard.h"
#include "OutDbg.h"
#include "pqLayoutsHook.h"

using namespace KeyActions;


//----------


VeeKeeSet Keyboard::modifiers = {
    VK_LSHIFT, VK_RSHIFT, VK_SHIFT,
    VK_LCONTROL, VK_RCONTROL, VK_CONTROL,
    VK_LMENU, VK_RMENU, VK_MENU,         // Alt !
    VK_LWIN, VK_RWIN
};


VeeKeeSet Keyboard::extended = {
    VK_LEFT, VK_RIGHT, VK_UP, VK_DOWN,
    VK_HOME, VK_END, VK_PRIOR, VK_NEXT, 
    VK_INSERT, VK_DELETE, VK_DIVIDE, VK_NUMLOCK,
    VK_RCONTROL, VK_RMENU, VK_APPS, 
    VK_PAUSE, VK_SNAPSHOT
};

//----------

Keyboard::Keyboard(DWORD injectedFromMeValue) : 
    injectedFromMeValue(injectedFromMeValue), 
    hMainWindow(NULL),
    mainWndMsg(0),
    suspended(false), 
    makeSticky(0),
    suspendKey(0), 
    quitKey(0),
    lastKeypressTick(0),
    chordingSuspended(false)
{
    //init isprint 
    for (char c = 0x20; c <= 0x7E; c++)
    {
        SHORT vk = VkKeyScanA(c);
        isprint.insert(vk);
    }

    star = VkKeyScanA('*') & 0xFF;
}

void Keyboard::SetMainWnd(HWND hMainWindow)
{
    this->hMainWindow = hMainWindow;
}

void Keyboard::SetMainWndMsg(int mainWndMsg)
{
    this->mainWndMsg = mainWndMsg;
}

bool Keyboard::AddLayer(const Layer::Id_t& layerId, Layer::Idx_t& newLayerIdx)
{
    return layout.AddLayer(layerId, newLayerIdx);
}

bool Keyboard::SetLayerAccessKey(const Layer::Id_t& layerId, KeyDef accessKey, bool canTap)
{
    return layout.SetLayerAccessKey(layerId, accessKey, canTap);
}

bool Keyboard::GotoMainLayer()
{
    return layout.GotoMainLayer();
}

bool Keyboard::GotoLayer(Layer::Idx_t layerIdx)
{
   return layout.GotoLayer(layerIdx);
}

bool Keyboard::GotoLayer(const Layer::Id_t& layerId)
{
    return layout.GotoLayer(layerId);
}

const Layer* Keyboard::CurrentLayer() const
{
    return layout.CurrentLayer();
}

bool Keyboard::IsModifier(VeeKee vk)
{
    return modifiers.find(vk) != modifiers.end();
}

bool Keyboard::IsShift(VeeKee vk)
{
    return vk == VK_LSHIFT || vk == VK_RSHIFT || vk == VK_SHIFT;
}

bool Keyboard::IsExtended(VeeKee vk)
{
    return extended.find(vk) != extended.end();
}

// update the up/down modifiers collection
// notifies main app when shift goes from up/down ('layer change')
void Keyboard::ModifierDown(VeeKee vk, bool pressed)
{
    const bool isShiftDownBefore = ShiftDown();

    if (pressed)
        downModifiers.insert(vk);
    else
        downModifiers.erase(vk); 

    const bool isShiftDownAfter = ShiftDown();

    // notify main app when shift goes from up/down ('layer change')
    if (isShiftDownBefore != isShiftDownAfter)
    {
        const auto* const currentLayer = CurrentLayer();
        Notify(HookKbd::LayerChanged, currentLayer->LayerIdx());
    }
}


bool Keyboard::ModifierDown(VeeKee vk) const
{
    return downModifiers.find(vk) != downModifiers.end();

}

bool Keyboard::ShiftDown() const
{
    return ModifierDown(VK_LSHIFT) || 
        ModifierDown(VK_RSHIFT) || 
        ModifierDown(VK_SHIFT);
}

void Keyboard::TrackMappedKeyDown(VeeKee physicalVk, IKeyAction* mapped, bool pressed)
{
    MappedKeyDown(physicalVk, mapped, pressed);
}

bool Keyboard::ToggleSuspend()
{
    bool currentSuspended = suspended;
    suspended = !suspended;
    Printf("suspended = %d\n", (suspended ? 1 : 0));

    // notify main app window
    Notify(HookKbd::SuspendStateUpdated, (suspended ? 1 : 0));

    return currentSuspended;
}

bool Keyboard::Suspended()
{
    return suspended;
}

void Keyboard::MakeSticky(VeeKee makeSticky)
{
    Printf("make sticky 0x%02X\n", makeSticky);
    this->makeSticky = makeSticky;
}

// if != 0then make keys sticky
VeeKee Keyboard::MakeSticky() const
{
    return makeSticky;
}

void Keyboard::SuspendChording()
{
    chordingSuspended = true;
}

void Keyboard::ResumeChording()
{
    chordingSuspended  = false;
}


void Keyboard::SetImageFilename(const WCHAR* filename)
{
    if (filename != nullptr)
        imageFilename = filename;
}

const std::wstring& Keyboard::GetImageFilename() const
{
    return imageFilename;
}

void Keyboard::SetImageView(Layer::ImageView imageView, Layer::ImageView imageViewShift) const
{
    layout.SetImageView(imageView, imageViewShift);
}

Layer::ImageView Keyboard::GetImageView() const
{
    const bool isShiftDown = ShiftDown();
    return layout.GetImageView(isShiftDown);
}

void Keyboard::Notify(HookKbd::Notif notif, LPARAM lparam)
{
    // notify main app window
    PostMessage(this->hMainWindow, mainWndMsg, notif, lparam);

}

void Keyboard::SuspendKey(VeeKee vk)
{
    suspendKey = vk;
}

void Keyboard::QuitKey(VeeKee vk)
{
    quitKey = vk;
}

void Keyboard::MappedKeyDown(VeeKee physicalVk, IKeyAction* mapped, bool pressed)
{
    if (pressed)
        downMappedKeys[physicalVk] = mapped;
    else
        downMappedKeys.erase(physicalVk); 
}

IKeyAction* Keyboard::MappedKeyDown(VeeKee vk) const
{
    auto found = downMappedKeys.find(vk);
    return (found != downMappedKeys.end()) ? found->second : nullptr;

}

const KeyMapping* Keyboard::Mapping(VeeKee vk)
{
    auto caseMapping = layout.Mapping(vk);
    if (caseMapping == nullptr)
        return nullptr;

    return (ShiftDown() ? &caseMapping->shifted : &caseMapping->nonShifted); 
}

bool Keyboard::AddMapping(KeyValue from, KeyValue to)
{
    return layout.AddMapping(from, to);
}

bool Keyboard::AddStickyMapping(KeyValue vk)
{
    return layout.AddStickyMapping(vk);
}

bool Keyboard::AddChord(Kord& chord, const std::list<KeyActions::KeyActionPair>& keyActions)
{
    return layout.AddChord(chord, keyActions);
}



//------

IKeyAction* Keyboard::GetMappingValue(const KbdHookEvent & event)
{
    const CaseMapping* caseMapping = layout.Mapping(event.vkCode);

    if (caseMapping == nullptr) 
       return nullptr;

    // drill down to out mapping
    const KeyMapping& mapping = ShiftDown() ? caseMapping->shifted : caseMapping->nonShifted;
    IKeyAction* valueOut = mapping.Mapping();

    return valueOut;
}


bool Keyboard::InitChordingKeys(const ChordingKeys& chordingKeys)
{
    chording.Init(chordingKeys);
    return true;
}

bool Keyboard::OnKeyEvent(const KbdHookEvent & event, DWORD injectedFromMeValue)
{
    // is it the suspend key ?
    if (event.vkCode == suspendKey)
    {
        if (event.Down())
        {
            Printf("suspend key pressed, %ssuspending pqLayouts\n", (Suspended() ? "un" : ""));
            ToggleSuspend();
        }
        return true; // 'edat' this key
    }

    // is it the quit key ?
    if (event.vkCode == quitKey)
    {
        if (event.Down())
        {
            Printf("quit key pressed, stopping pqLayouts\n");
            PostMessage(hMainWindow, WM_CLOSE, 0,0);
        }
        return true; // 'eat' this key
    }

     // let keys through keys while suspended
    if (suspended)
        return false;

    // save time tick of last key press
    if (event.Down())
        this->lastKeypressTick = event.time;  // nb: this is the same as GetTickCount()


    //-------------

    // is this key currently down (mapped) ?
    // if so use that action
    IKeyAction* action = MappedKeyDown(event.vkCode);
    const bool wasDown = (action != nullptr);

    // not a mapped key currently down
    if (!wasDown)
    {
        // do we have a mapped key for this input key?
        action = GetMappingValue(event);
    }


    // handle possible chording
    // do it here so we track non mapped keys to to be able to replay them for failed chord 
    bool isLayerAccess = (action != nullptr ? action->IsLayerAccess() : false);

    if (!isLayerAccess && !chordingSuspended && CurrentLayer()->HasChords())
    {
        // restrict chording to only chording keys, since we loose auto-repeat on the chordable keys
        //pq-todo might need to have a 'repeat' key
        //if (MyIsPrint(event.vkCode))
        if (chording.GetChordingKeyFromQwerty(event.vkCode) != nullptr)
        {
            HandleChording(event, injectedFromMeValue);
            return true;
        }
    }

    // we have something to act on..
    if (action != nullptr)
    {
        // eat key down repeats
        if (wasDown &&  event.Down() && action->SkipDownRepeats(this))
            return true;

        // make sticky by eating key up event
        if (makeSticky != 0)
        {
            // eat keys Up, making them sticky
            const bool isStickyMaker = (makeSticky == event.vkCode);

            if (!isStickyMaker && !event.Down())
            {
                Printf("sticky, skip 0x%02X\n", event.vkCode);
                return true;
            }
        }

        // set time of initial down key press
        if (!wasDown && event.Down())
            action->downTimeTick = event.time;

        // set time of the key release
        if (wasDown && !event.Down())
            action->upTimeTick = event.time;
        
        // register up/down input keys (don't re-register key already down)
        if (!(wasDown && event.Down()))
            TrackMappedKeyDown(event.vkCode, action, event.Down());

        // do the action for the key
        const bool isTap(action->downTimeTick == this->lastKeypressTick);

        bool ret = false;
        if (event.Down())
            ret = action->OnKeyDown(this);
        else
            ret = action->OnKeyUp(this, isTap);

        return ret;
    }
    else
    {
        // keep track of pressed modifiers for non mapped keys
        TrackModifiers(event.vkCode, event.Down());
    }

    return false; // let key through
}

void Keyboard::HandleChording(const KbdHookEvent& event, const DWORD& injectedFromMeValue)
{
    // map qwerty chord key to steno key (by steno order)
    const ChordingKey* chordingKey = chording.GetChordingKeyFromQwerty(event.vkCode);
    if (chordingKey != nullptr)
    {
        // debug
        //Printf("chording, qwerty key %02X %c => %c %d\n", 
        //    event.vkCode, 
        //    static_cast<char>(event.vkCode & 0xFF), 
        //    chordingKey->steno, chordingKey->stenoOrder);

        // pass original unmodified event so we can replay as it was typed (no star keys modifications)
        chord.OnEvent(chordingKey->stenoOrder, event);
    }
    else
    {
        // not a chording key! cancel any chording
        // pass original unmodified event so we can replay as it was typed (no star keys modifications)
        chord.OnEvent(0, event);
        chord.Cancel();
    }

    switch (chord.GetState())
    {
    case Kord::State::Cancelled:
    {
        ReplayCancelledChord(chord, injectedFromMeValue);
        chord.Reset();
        break;
    }

    case Kord::State::Completed:
    {
        OnCompletedChord(injectedFromMeValue);
        chord.Reset();
        break;
    }
    }
}

void Keyboard::OnCompletedChord(const DWORD& injectedFromMeValue)
{
    // lookup chord and output its value if found
    // else output original key (cancelled chord)
    Printf("completed chord [%s]\n", chording.ToString(chord).c_str());

    const std::list<KeyActions::KeyActionPair>* chordActions = layout.GetChordActions(chord);
    if (chordActions != nullptr)
    {
        Printf("chord found, executing its action keydown/up\n");

        for (auto actionPair : *chordActions)        
        {
            // get normal / shifted version of the action 
            KeyActions::IKeyAction* action = (ShiftDown() ? actionPair.second : actionPair.first);

            // sanity check
            if (action != nullptr)
            {
                SuspendChording();

                // activate action for this chord
                action->OnKeyDown(this);

                // should this be a Tap ? techniclly it is
                action->OnKeyUp(this, false);

                ResumeChording();
            }

        }
    }
    else
    {
        //not a known chord, cancel it and replay cumulated keys sequence
        //Printf("chord not found, cancelling / replaying keys\n");

        ReplayCancelledChord(chord, injectedFromMeValue);
    }
}

// replays the key events accumulated in a failed chord
void Keyboard::ReplayCancelledChord(Kord& chord, DWORD injectedFromMeValue)
{
    Printf("ReplayCancelledChord chord [%s]\n", chording.ToString(chord).c_str());

    SuspendChording();

    for (auto& keyEvent : chord.KeysSequence())
    {
        // check that this key is mapped
        IKeyAction* action = GetMappingValue(keyEvent);
        
        // we NEED an action for a key to do something when replaying from here ..
        // so create a one to one action key if it is not mapped
        if (action == nullptr)
        {
            Printf("replaying non mapped key %0X, creating 1-1 mapping\n", keyEvent.vkCode);

            // non shifted
            KeyValue kfrom(keyEvent.vkCode, 0, false);
            KeyValue kto(keyEvent.vkCode, 0, false);
            HookKbd::AddMapping(kfrom, kto);
            
            // and shifted
            kfrom.Shift(true);
            kto.Shift(true);
            HookKbd::AddMapping(kfrom, kto);
        }

        OnKeyEvent(keyEvent, injectedFromMeValue);
    }

    ResumeChording();
}



void Keyboard::TrackModifiers(VeeKee vk, bool pressed)
{
    if (IsModifier(vk))
        ModifierDown(vk, pressed);
}


bool Keyboard::SendVk(const KeyValue& key, bool pressed)
{
    // prepare an array of Inputs to send
    // these include shift/control events surrounding the mapped key to send
    INPUT inputs[32] = { 0 };
    size_t idx = 0;

    // add any required up/down shifts
    if (pressed)
    {
        // don't surrounding shift up/down keys if the output key is shift or capslock
        if (!IsShift(key.Vk()) && key.Vk() != VK_CAPITAL)
        {
            bool needsShift = key.Shift();
            bool lshiftDown = ModifierDown(VK_LSHIFT);
            bool rshiftDown = ModifierDown(VK_RSHIFT);
            if (needsShift && !(lshiftDown || rshiftDown))
            {
                // send a Shift down before our key
                SetupInputKey(inputs[idx++], VK_LSHIFT, true, injectedFromMeValue);
            }
            else if (!needsShift)
            {
                // send a LShift up before our key
                if (lshiftDown)
                {
                    SetupInputKey(inputs[idx++], VK_LSHIFT, false, injectedFromMeValue);
                }
                // send a RShift up before our key
                if (rshiftDown)
                {
                    SetupInputKey(inputs[idx++], VK_RSHIFT, false, injectedFromMeValue);
                }
            }
        }
    }

    // add any required down Ctrl
    if (key.Control() && !(ModifierDown(VK_LCONTROL) || ModifierDown(VK_RCONTROL)))
    {
        // send a control down before our key
        SetupInputKey(inputs[idx++], VK_LCONTROL, true, injectedFromMeValue);
    }

    // how many prefix shift/ctrl key we need to revert
    size_t nbExtras = idx;

    // add the mapped key
    {
        SetupInputKey(inputs[idx++], key.Vk(), pressed, injectedFromMeValue);
    }

    // finaly, undo any shift/control exta events we added above
    for (size_t i = 0; i < nbExtras; i++, idx++)
    {
        // copy, then invert KEYEVENTF_KEYUP flag
        inputs[idx] = inputs[i];
        if (inputs[idx].ki.dwFlags & KEYEVENTF_KEYUP)
            inputs[idx].ki.dwFlags &= ~KEYEVENTF_KEYUP;
        else
            inputs[idx].ki.dwFlags |= KEYEVENTF_KEYUP;
    }

    // now send the keys
    SendInput(idx, inputs, sizeof(inputs[0]));

    //##PQ from touchcursor##
    // Sleep(1) seems to be necessary for mapped Escape events sent to VirtualPC & recent VMware versions.
    // (Sleep(0) is no good)
    // Also for mapped modifiers with Remote Desktop Connection.
    // Dunno why:
    Sleep(1);

    // and finally, track any modifiers up/down we sent (actual output key, not pre/suffix ones !)
    TrackModifiers(key.Vk(), pressed);

    return true;
}

void Keyboard::SetupInputKey(INPUT& input, VeeKee vk, bool pressed, DWORD injectedFromMeValue)
{
    //UINT scancode = MapVirtualKeyExA(vk, MAPVK_VK_TO_VSC_EX, NULL);

    input.type = INPUT_KEYBOARD;
    input.ki.wVk = static_cast<WORD>(vk);
    //input.ki.wScan = scancode;
    input.ki.dwFlags = pressed ? 0 : KEYEVENTF_KEYUP;
    input.ki.time = 0;
    input.ki.dwExtraInfo = injectedFromMeValue;
    if (IsExtended(vk))
        input.ki.dwFlags |= KEYEVENTF_EXTENDEDKEY;
}


// dbg 
void Keyboard::OutNbKeysDn()
{
    Printf("keys dn %d mods dn %d\n", downMappedKeys.size(), downModifiers.size());
}

//-------


