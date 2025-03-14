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
    hMainWindow(nullptr),
    mainWndMsg(0),
    suspended(false), 
    makeSticky(0),
    suspendKey(0), 
    quitKey(0),
    lastKeypressTick(0),
    lastDownEvent{},
    prevlastDownEvent{},
    lastVkCodeDown(0),
    chordingSuspended(false),
    lpsteaksLayer1(0),
    lpsteaksLayer2(0)
{
    //init isprint 
    for (char c = 0x20; c <= 0x7E; c++)
    {
        SHORT vk = VkKeyScanA(c);
        isprint.insert(vk);
    }
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

bool Keyboard::SetLayerAccessKey(const Layer::Id_t& layerId, KeyDef accessKey, bool canTap, KeyValue keyOnTap)
{
    return layout.SetLayerAccessKey(layerId, accessKey, canTap, keyOnTap);
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

bool Keyboard::AddDualModeModifier(KeyDef  key, KeyValue modifierKey, KeyValue tapKey)
{
    return layout.AddDualModeModifier(key, modifierKey, tapKey);
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

IKeyAction* Keyboard::GetMappingValue(VeeKee vk) const
{
    const CaseMapping* caseMapping = layout.Mapping(vk);
    if (caseMapping == nullptr) 
       return nullptr;

    // drill down to out mapping
    const KeyMapping& mapping = ShiftDown() ? caseMapping->shifted : caseMapping->nonShifted;
    return mapping.Mapping();
}

IKeyAction* Keyboard::GetMappingValue(VeeKee vk, Layer::Idx_t layerIdx) const 
{
    const CaseMapping* caseMapping = layout.Mapping(layerIdx, vk);
    if (caseMapping == nullptr) 
       return nullptr;

    // drill down to out mapping
    const KeyMapping& mapping = ShiftDown() ? caseMapping->shifted : caseMapping->nonShifted;
    return mapping.Mapping();
}

bool Keyboard::InitChordingKeys(const ChordingKeys& chordingKeys)
{
    chording.Init(chordingKeys);
    return true;
}

void Keyboard::SetLeftHandPrefix(Layer::Id_t lpsteaksLayerName1, Layer::Id_t lpsteaksLayerName2,
    std::string lpsteaksPrefix1, std::string lpsteaksPrefix2)
{
    this->lpsteaksLayerName1 = lpsteaksLayerName1;
    this->lpsteaksLayerName2 = lpsteaksLayerName2;
    this->lpsteaksPrefix1 = lpsteaksPrefix1;
    this->lpsteaksPrefix2 = lpsteaksPrefix2;
}

bool Keyboard::CheckForSuspendKey(const KbdHookEvent& event)
{
    //##pq debug, cant use Paause key in VMWare on MacOS !! also try ScrollLock
    if (event.Down() && (event.vkCode == VK_F1 && event.AltDown() || event.vkCode == VK_SCROLL))
    {
        adaptivesOn = !adaptivesOn;
        Printf("alt-suspend key pressed, adaptives now %s\n", adaptivesOn ? " ""on" : "off");
    }

    // is it the suspend key ?
    if (event.vkCode == suspendKey)
    {
        if (event.Down())
        {
            if (event.AltDown())
            {
                adaptivesOn = !adaptivesOn;
                Printf("alt-suspend key pressed, adaptives now %s\n", adaptivesOn ? " ""on" : "off");
            }
            else
            {
                Printf("suspend key pressed, %ssuspending pqLayouts\n", (Suspended() ? "un" : ""));
                ToggleSuspend();
            }
        }
        return true; // 'eat' this key
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

    return false;
}

bool Keyboard::ProcessKeyEvent(const KbdHookEvent& event, IKeyAction* action, const bool wasDown)
{
    // (possibly) eat key down repeats (e.g. layerAccess key)
    if (wasDown &&  event.Down() && action->SkipDownRepeats(this))
        return true;

    // make sticky by eating key up event
#if 0
    if (makeSticky != 0)
    {
        // eat keys Up, making them sticky
        const bool isStickyMaker = (makeSticky == event.vkCode);
        Printf("makeSticky != 0, event.vkCode=0x%ld, makeSticky=%ld, isStickyMaker=%d\n", event.vkCode, makeSticky, isStickyMaker);

        if (isStickyMaker && !event.Down())
        {
            Printf("sticky, skip 0x%02X up\n", static_cast<WORD>(event.vkCode));
            return true;
        }
    }
#endif // 0


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
    const bool isTap(this->lastVkCodeDown == event.vkCode);

    bool ret = false;
    if (event.Down())
        ret = action->OnKeyDown(this);
    else
        ret = action->OnKeyUp(this, isTap);

    return ret;
}

bool Keyboard::IsSelfInjected(const KbdHookEvent& event)
{
    return (event.Injected() && event.dwExtraInfo == injectedFromMeValue);
}


const int TIMER_ID = 0xBEBE;
static bool timerWentOff{};

void __stdcall  TimerProc(
  HWND unnamedParam1,
  UINT unnamedParam2,
  UINT_PTR unnamedParam3,
  DWORD unnamedParam4
)
{
    timerWentOff = true;
    KillTimer(unnamedParam1, TIMER_ID);
    Printf("timer\n");
}

void kilTimer(HWND hWnd)
{
    if (!timerWentOff)
    {
        timerWentOff = false;
        KillTimer(hWnd, TIMER_ID);
    }
}

struct Comboer
{
    bool buildingCombo{};
    KbdHookEvent firstEvent;
    std::vector<VeeKee> accumulatedKeys;
    std::vector<VeeKee> sortedKeys;

    bool isPotentialComboKey(const KbdHookEvent& event, const bool isAccumulatedDown)
    {
        //## pq test debug
        static std::string keys = "QWASD";

        return !isAccumulatedDown && 
                event.Down() &&
                keys.find((char*)event.vkCode) != std::string::npos;
    }

    void reset()
    {
        buildingCombo = false;
        accumulatedKeys.clear();
        sortedKeys.clear();
        firstEvent = KbdHookEvent{};
        
        //##todo kill/reset timer
    }

    void replay()
    {
        //## todo
    }

    void addKey(const KbdHookEvent& event)
    {
        accumulatedKeys.push_back(event.vkCode);
        
        sortedKeys.push_back(event.vkCode);
        std::sort(sortedKeys.begin(), sortedKeys.end());
    }

    // return true to 'eat' key, false to continue processing
    bool OnKeyEvent(const KbdHookEvent& event)
    {
        bool isAccumulatedDown = VkUtil::Contains(accumulatedKeys, event.vkCode);

        // not in combo building mode
        if (!buildingCombo)
        {
            if (!isPotentialComboKey(event, isAccumulatedDown))
                return false;
         
            // start building potential combo
            buildingCombo = true;
            firstEvent = event;
            addKey(event);
            return true;
        }

        // -- buildingCombo mode --

        // 'other' key up, let it through
        if (event.Up() && !isAccumulatedDown)
        {
            return false;
        }

        // another potential combo key
        if (isPotentialComboKey(event, isAccumulatedDown))
        {
            addKey(event);
            //todo lookup combos
            // if found then exec
            // else if foundPotential then keep building
            // else cancel combo build
            //  prob want to support trying again after removing 1st key,
            //  ex: potKeys=zwgm for zw, gm;  press wgm, we want to send 'w', then see gm combo 
            return true;
        }

        // 'other' key down, not a combo, cancel combo, replay keys as were
        if (event.Down() && !isAccumulatedDown)
        {
            replay();
            reset();

            // let last key received go through
            return false;
        }
        return false;
    }
};

static Comboer comboer;

void Keyboard::ReplayEvents(const std::vector<KbdHookEvent>& events)
{
    for (const auto& event : events)
    {
        OnKeyEventLevel2(event);
    }
}

void Keyboard::SendString(int nbrKeysIn, const char* textString)
{
    // send output keys 
    // 1st key is shifted if Shift is currently down
    //i.e. shift ae -> Au
    bool shifted = nbrKeysIn == 0 && ShiftDown();
    int nbBS = 0;
    for (const char* ptr = textString; *ptr; ++ptr)
    {
        KeyValue keyValueOut(*ptr);

        if (shifted && isalpha(*ptr))
        {
            keyValueOut.Shift(true);
        }
        shifted = false;

        SendVk(keyValueOut, true);
        SendVk(keyValueOut, false);

        if (*ptr == '\b')
        {
            if (++nbBS == nbrKeysIn - 1)
            {
                shifted = ShiftDown();
            }
        }
    }
}
bool Keyboard::DoCombo(const std::vector<KbdHookEvent>& events, const VeeKeeVector& vks)
{
    // VeeKeeVector must be sorted!
    // combos for HD neuC
    //## need another format to be able to output ctrl-c & cie !!
    static std::map<VeeKeeVector, const char*> combos = {
        { {'R','W'}, "qu"},
        { {'F','S'}, "z"},

        { {'U','Y'}, "@"},
        { {'I','U'}, ".com"},

        { {'O','U'}, ":" },
        { {'I','O'}, "/" },

        { {'L',VK_OEM_1}, "I "}, //ih -> "I " 

        { {VK_OEM_COMMA, VK_OEM_PERIOD}, "="},
        { {'M', VK_OEM_PERIOD}, "_"},
    };

    auto foundComboIt = combos.find(vks);

    if (foundComboIt != combos.end())
    {
        Printf("found combo!\n");

        SendString(0, foundComboIt->second);
        return true;
    }

    return false;
}

bool Keyboard::OnKeyEvent(const KbdHookEvent& event)
{
    if (CheckForSuspendKey(event)) 
        return true;

    // let keys through keys while suspended
    if (suspended)
        return false;

    //if (comboer.OnKeyEvent(event))
    //    return true;

    static std::vector<KbdHookEvent > eventsDown;
    static VeeKeeVector vksDown;
    static std::string keys = std::string("OILYSMURFSW") 
        + (char)VK_OEM_1
        + (char)VK_OEM_COMMA
        + (char)VK_OEM_PERIOD
        ;
    static bool cumulating = false;

    bool isComboKey = (keys.find((char)event.vkCode) != std::string::npos);
    bool isComboKeyDown = isComboKey && event.Down();

    if (!cumulating && !isComboKeyDown)
    {
        Printf("not cumulating, not comboKeyDown\n");
        return OnKeyEventLevel2(event);
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
        bool isAlreadyDown = isComboKeyDown && VkUtil::Contains(vksDown, event.vkCode);

        if (!isComboKeyDown || isAlreadyDown || event.TimeDiff(eventsDown[0]) > 50) //ms
        {
            Printf("cancel cumul\n");
            ReplayEvents(eventsDown);
            cumulating = false;
            eventsDown.clear();
            vksDown.clear();
            return OnKeyEventLevel2(event);
        }

        eventsDown.push_back(event);
        vksDown.push_back(event.vkCode);
        std::sort(vksDown.begin(), vksDown.end());

        if (!DoCombo(eventsDown, vksDown))
        {
            Printf("cancel cumul\n");
            ReplayEvents(eventsDown);
        }
        cumulating = false;
        eventsDown.clear();
        vksDown.clear();
        return true;
    }


    return OnKeyEventLevel2(event);
}

bool Keyboard::_OnKeyEvent(const KbdHookEvent& event)
{
    //## pq test debug combos
    static bool inComboQ = false;
    static bool comboQComplete = false;

    if (timerWentOff && !inComboQ)
    {
        timerWentOff = false;
    }

    if (inComboQ)
    {
        Printf("inComboQ timerWentOff=%d\n", timerWentOff);

        if (timerWentOff && !comboQComplete)
        {
            kilTimer(hMainWindow);
            inComboQ = false;
            timerWentOff = false;
            Printf("cancel ComboQ on timer\n");
            return true;
        }

        if (event.Down())
        {
            if (event.vkCode == 'Q')
            {
                Printf("skip repeat down Q\n");
                return true;
            }

            if (event.vkCode == 'W')
            {
                Printf("ComboQ completed\n");
                kilTimer(hMainWindow);
                inComboQ = false;
                comboQComplete = true;
                return true;
            }

            Printf("cancel ComboQ on other key up\n");
            kilTimer(hMainWindow);
            inComboQ = false;
            return true;

        }

        if ((event.vkCode == 'Q' || event.vkCode == 'W') && event.Up() && !comboQComplete)
        {
            Printf("cancel ComboQ on Q/W up\n");
            kilTimer(hMainWindow);
            inComboQ = false;
            return true;
        }

        // should not get here (bug?)
        return true;
    }

    if (event.Down())
    {
        if (event.vkCode == 'Q' || event.vkCode == 'W')
        {
            Printf("start timer on Q down\n");
            inComboQ = true;
            comboQComplete = false;

            SetTimer(hMainWindow, TIMER_ID, 50, TimerProc);
            return true;
        }
    }
 
    return OnKeyEventLevel2(event);
}

bool Keyboard::OnKeyEventLevel2(const KbdHookEvent & event)
{
    // save time tick of last key press
    if (event.Down())
    {
        this->lastKeypressTick = event.time;  // nb: this is the same as GetTickCount()
        this->lastVkCodeDown = event.vkCode;
    }


    // adaptives, delay between each key allowed
    //##pq todo: this will need to be by layer / read from kbd file etc etc
//#include "adapt-carbyne.h"
#include "adapt-hd-neu-C.h"

    if (adaptivesOn)
    {
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

                std::map<VeeKeeVector, const char*>::iterator foundAdaptIt;

                // look for adaptives
                VeeKeeVector vkeys3{ prevlastDownEvent.vkCode, lastDownEvent.vkCode, event.vkCode };
                VeeKeeVector vkeys2{ lastDownEvent.vkCode, event.vkCode };

                if ((foundAdaptIt = adapts3.find(vkeys3)) != adapts3.end() ||
                    (foundAdaptIt = adapts2.find(vkeys2)) != adapts2.end())
                {
                    Printf("found adaptive!\n");

                    size_t nbrKeysIn = foundAdaptIt->first.size();
                    SendString(nbrKeysIn, foundAdaptIt->second);

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

    }


    // handle possible chording
    // do it here so we track non mapped keys to to be able to replay them for failed chord 
    if (!chordingSuspended && CurrentLayer()->HasChords())
    {
        //pq-todo might need to have a 'repeat' key or a way to suspend chording

        // map qwerty chord key to steno key (by steno order)
        auto* chordingKey = chording.GetChordingKeyFromQwerty(event.vkCode);
        bool isChordingKey = chordingKey != nullptr;

        // only start chording for valid chording keys
        bool chordInProgress = chord.GetState() != Kord::State::New;
        if (chordInProgress || (!chordInProgress && isChordingKey && event.Down()))
        {
            if (HandleChording(event, chordingKey))
                return true;
        }
    }

    // is this key currently down (mapped) ?
    // if so use that action
    IKeyAction* action = MappedKeyDown(event.vkCode);
    const bool wasDown = (action != nullptr);

    // not a mapped key currently down
    if (action == nullptr)
    {
        // do we have a mapped key for this input key?
        action = GetMappingValue(event.vkCode);
    }

    // we have something to act on..
    if (action != nullptr)
    {
        return ProcessKeyEvent(event, action, wasDown);
    }
    else
    {
        // keep track of pressed modifiers for non mapped keys
        TrackModifiers(event.vkCode, event.Down());
    }

    return false; // let key through
}

bool Keyboard::HandleChording(const KbdHookEvent& event, const ChordingKey* chordingKey)
{
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
        ReplayCancelledChord();
        chord.Reset();
        break;
    }

    case Kord::State::Completed:
    {
        OnCompletedChord();
        chord.Reset();
        break;
    }
    }

    // we always eat the passed key event
    return true;
}

bool Keyboard::CheckLpChordsLayers()
{
    // if we have layer names for left hand prefixed chords but no corresponding layer indexes, find the indexes
    if (!lpsteaksLayerName1.empty() && !lpsteaksLayerName2.empty() &&
        lpsteaksLayer1 == 0 && lpsteaksLayer2 == 0)
    {
        if (!layout.GetLayerIndex(lpsteaksLayerName1, lpsteaksLayer1) ||
            !layout.GetLayerIndex(lpsteaksLayerName2, lpsteaksLayer2))
        {
            return false;
        }
    }

    // return true if we have left hand prefixed chord layers
    return (lpsteaksLayer1 != 0 || lpsteaksLayer2 != 0);
}

//prefixed, order-dependent main/alt chords
bool Keyboard::OnPo2LayersChord()
{
    if (!CheckLpChordsLayers())
        return false;

    Layer::Idx_t layerIdx1 = 0;
    Layer::Idx_t layerIdx2 = 0;

    // -- if has correct prefix e.g. "PH-" / "WR-" main->alt / alt->main
    // then this should be a left hand prefixed chord
    const auto txtChord = chording.ToString(chord);
    std::string::size_type prefixLen = 0;

    if (txtChord.substr(0, lpsteaksPrefix1.size()) == lpsteaksPrefix1)
    {
        // layer1 -> layer2
        prefixLen = lpsteaksPrefix1.size();
        layerIdx1 = lpsteaksLayer1;
        layerIdx2 = lpsteaksLayer2;
    }
    else if (txtChord.substr(0, lpsteaksPrefix2.size()) == lpsteaksPrefix2)
    {
        // layer2 -> layer1
        prefixLen = lpsteaksPrefix2.size();
        layerIdx1 = lpsteaksLayer2;
        layerIdx2 = lpsteaksLayer1;
    }
    else
    {
        return false;
    }

    // -- get right hand 1-2keys (Steno key names), that follow left hand prefix
    const auto rightHandKeys = txtChord.substr(prefixLen);
    if (rightHandKeys.size() != 1 && rightHandKeys.size() != 2)
        return false;

    auto vk1 = chording.AdjustForRightHand(rightHandKeys[0]);
    auto vk2 = chording.AdjustForRightHand(rightHandKeys.size() == 2 ? rightHandKeys[1] : rightHandKeys[0]);
    const auto* chordingKey1 = chording.GetChordingKeyFromSteno(vk1);
    const auto* chordingKey2 = chording.GetChordingKeyFromSteno(vk2);

    if (chordingKey1 == nullptr || chordingKey2 == nullptr)
        return false;

    // -- find which chord right hand qwerty key was pressed 1st 
    for (const auto& event : chord.KeysSequence())
    {
        // key1 was 1st pressed: not reversed
        if (event.Down() && event.vkCode == chordingKey1->qwerty)
        {
            break;
        }
        // key2 was 1st pressed: reversed
        if (event.Down() && event.vkCode == chordingKey2->qwerty)
        {
            std::swap(chordingKey1, chordingKey2);
            break;
        }
    }

    // -- map 2 qwerties to IAction from appropriate layers
    auto* keyAction1 = GetMappingValue(chordingKey1->qwerty, layerIdx1);
    auto* keyAction2 = GetMappingValue(chordingKey2->qwerty, layerIdx2);
    if (keyAction1 == nullptr || keyAction2 == nullptr)
        return false;

    // -- play the 2 actions in order found in 5-
    keyAction1->OnKeyDown(this);
    keyAction1->OnKeyUp(this, false);
    keyAction2->OnKeyDown(this);
    keyAction2->OnKeyUp(this, false);

    return true;
}

void Keyboard::OnCompletedChord()
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
        if (!OnPo2LayersChord())
        {
            //not a known chord, cancel it and replay cumulated keys sequence
    //Printf("chord not found, cancelling / replaying keys\n");

            ReplayCancelledChord();

        }
    }
}

// replays the key events accumulated in a failed chord
void Keyboard::ReplayCancelledChord()
{
    Printf("ReplayCancelledChord chord [%s]\n", chording.ToString(chord).c_str());

    SuspendChording();

    for (auto& keyEvent : chord.KeysSequence())
    {
        // check that this key is mapped
        IKeyAction* action = GetMappingValue(keyEvent.vkCode);
        
        // we NEED an action for a key to do something when replaying from here ..
        // so create a one to one action key if it is not mapped
        if (action == nullptr)
        {
            Printf("replaying non mapped key %0X, creating 1-1 mapping\n", static_cast<WORD>(keyEvent.vkCode));

            // non shifted
            KeyValue kfrom(keyEvent.vkCode, 0, false);
            KeyValue kto(keyEvent.vkCode, 0, false);
            HookKbd::AddMapping(kfrom, kto);
            
            // and shifted
            kfrom.Shift(true);
            kto.Shift(true);
            HookKbd::AddMapping(kfrom, kto);
        }

        OnKeyEvent(keyEvent);
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
                SetupInputKey(inputs[idx++], VK_LSHIFT, true);
            }
            else if (!needsShift)
            {
                // send a LShift up before our key
                if (lshiftDown)
                {
                    SetupInputKey(inputs[idx++], VK_LSHIFT, false);
                }
                // send a RShift up before our key
                if (rshiftDown)
                {
                    SetupInputKey(inputs[idx++], VK_RSHIFT, false);
                }
            }
        }
    }

    // add any required down Ctrl
    if (key.Control() && !(ModifierDown(VK_LCONTROL) || ModifierDown(VK_RCONTROL)))
    {
        // send a control down before our key
        SetupInputKey(inputs[idx++], VK_LCONTROL, true);
    }

    // add any required down Alt
    if (key.Alt() && !(ModifierDown(VK_LMENU) || ModifierDown(VK_RMENU)))
    {
        // send a Alt down before our key
        SetupInputKey(inputs[idx++], VK_LMENU, true);
    }

    // how many prefix shift/ctrl key we need to revert
    size_t nbExtras = idx;

    // add the mapped key
    {
        SetupInputKey(inputs[idx++], key.Vk(), pressed);
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

    //##PQ **from touchcursor** ##
    //"Sleep(1) seems to be necessary for mapped Escape events sent to VirtualPC & recent VMware versions.
    // (Sleep(0) is no good)
    // Also for mapped modifiers with Remote Desktop Connection.
    // Dunno why:"
    // PQ 2025-02, indeed I can see that with a Remote Desktop session .. BUT
    //   if I start pqLayouts AFTER the remote desktop, then it works fine.
    // SO, since the Sleep(1) actually causes potential re-entrance in our keyboard hook function,
    // I will NOT use this fix/patch
//    Sleep(1);

    // and finally, track any modifiers up/down we sent (actual output key, not pre/suffix ones !)
    TrackModifiers(key.Vk(), pressed);

    return true;
}

void Keyboard::SetupInputKey(INPUT& input, VeeKee vk, bool pressed)
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


