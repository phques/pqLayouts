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

Keyboard::Keyboard()
{
    // init all to fale
    memset(downModifiers, 0, sizeof(downModifiers));
    memset(downKeys, 0, sizeof(downKeys));

    //memset(mappings, 0, sizeof(mappings));

    // init mappings for one-to-one (0x00, 0xFF are not VKs)
    //for (DWORD i = 1; i < 0xFF; i++)
    //{
    //    mappings[0][i] = mappings[1][i] = i;
    //}

    //init isprint 
    for (char c = 0x20; c <= 0x7E; c++)
    {
        SHORT vk = VkKeyScanA(c);
        isprint.insert(vk);
    }
}

bool Keyboard::AddLayer(const Layer::Id_t& layerId, Layer::Idx_t& newLayerIdx)
{
    return layout.AddLayer(layerId, newLayerIdx);
}

bool Keyboard::SetLayerAccessKey(const Layer::Id_t& layerId, KeyDef keydef)
{
    return layout.SetLayerAccessKey(layerId, keydef);
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

bool Keyboard::IsModifier(VeeKee vk)
{
    return modifiers.find(vk) != modifiers.end();
}

bool Keyboard::IsExtended(VeeKee vk)
{
    return extended.find(vk) != extended.end();
}

void Keyboard::ModifierDown(VeeKee vk, bool down)
{
    assert(vk < 0xFF);
    downModifiers[vk] = down;
}


bool Keyboard::ModifierDown(VeeKee vk) const
{ 
    assert(vk < 0xff); 
    return downModifiers[vk]; 
}

bool Keyboard::ShiftDown() const
{
    return ModifierDown(VK_LSHIFT) || 
        ModifierDown(VK_RSHIFT) || 
        ModifierDown(VK_SHIFT);
}

void Keyboard::KeyDown(VeeKee vk, bool down)
{
    assert(vk < 0xFF);
    downKeys[vk] = down;
}

bool Keyboard::KeyDown(VeeKee vk) const
{
    assert(vk < 0xff); 
    return downKeys[vk];
}

const KeyMapping* Keyboard::Mapping(VeeKee vk)
{
    assert(vk < 0xFF);

    //int shiftedIdx(ShiftDown() ? 1 : 0);
    //return mappings[shiftedIdx][vk];

    auto caseMapping = layout.Mapping(vk);
    if (caseMapping == nullptr)
        return nullptr;

    return (ShiftDown() ? &caseMapping->shifted : &caseMapping->nonShifted); 
}

bool Keyboard::AddMapping(KeyValue from, KeyValue to)
{
    if (from.Vk() >= 0xFF || to.Vk() >= 0xFF)
        return false;

    // dbg
    Printf("Add mapping from %02X, to %02X\n", from.Vk(), to.Vk());

    //int shiftedIdx(shifted ? 1 : 0);
    //mappings[shiftedIdx][vkFrom] = vkTo;

    return layout.AddMapping(from, to);
}


//------

const KeyValue* Keyboard::GetMappingValue(KbdHookEvent& event)
{
    const CaseMapping* caseMapping = layout.Mapping(event.vkCode);

    if (caseMapping == nullptr) 
       return nullptr;

    // drill down to out mapping
    const KeyMapping& mapping = ShiftDown() ? caseMapping->shifted : caseMapping->nonShifted;
    const KeyValue& valueOut = mapping.Mapping();

    if ( valueOut.Vk() == 0) // not mapped
        return nullptr; 

    return &valueOut;
}


// return true if we should skip / eat this virtual-key
bool Keyboard::OnKeyEvent(KbdHookEvent& event, DWORD injectedFromMeValue)
{
    // get mapped value
    const KeyValue* valueOut = GetMappingValue(event);
    bool isMapped = (valueOut != nullptr);
    VeeKee vkOut = (isMapped ? valueOut->Vk() : event.vkCode);

    // layer access key
    if (vkOut > 0xFF)
    {
        Printf("layer access %0x\n", vkOut);

        // this is a layer access key lower byte indicates which layer
        if (event.Up())
        {
            // coming out of the layer
            // return to main layer
            layout.GotoMainLayer();
        }
        else
        {
            // going into a new layer
            layout.GotoLayer(vkOut & 0xFF);
        }
        // nb: isMapped is true, we will 'eat' the original key
    }
    else // normal key
    {
        // output a ,apped key
        // take note of down keys / modifiers
        if (IsModifier(vkOut))
            ModifierDown(vkOut, event.Down());

        KeyDown(vkOut, event.Down());

        // if not mapped, don't send it ourself here,
        if (isMapped)
            SendVk(*valueOut, event.Down(), injectedFromMeValue);
    }

    // if we sent a mapped value, don't forward original event
    // also eat-up displayable non mapped keys, but be safe and let special keys trough ;-)
    return isMapped || MyIsPrint(vkOut); 
}



void Keyboard::SendVk(const KeyValue& key, bool down, DWORD injectedFromMeValue)
{
    // maybe pre compute this ?
    //##PQ actually a few keys don't seem to properly convert to scan code (kbd specific?)
    //## PrtScrn, Pause don't convert to the scancode that we received in the kbd hook event !?
    //UINT scancode = MapVirtualKeyExA(key.Vk(), MAPVK_VK_TO_VSC_EX, NULL);

    // prepare an array of Inputs to send
    // these include shift/control events surrounding the mapped key to send
    INPUT inputs[10] = { 0 };
    size_t idx = 0;

    // add any required up/down shifts
    if (down)
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

        // add any required down Ctrl
        if (key.Control() && !(ModifierDown(VK_LCONTROL) || ModifierDown(VK_RCONTROL)))
        {
            // send a control down before our key
            SetupInputKey(inputs[idx++], VK_RCONTROL, true, injectedFromMeValue);
        }
    }

    // the mapped key
    {
        SetupInputKey(inputs[idx++], key.Vk(), down, injectedFromMeValue);
    }

    // finaly, undo any shift/control exta events we added above
    if (down)
    {
        for (size_t i = 0; i < idx-1; i++)
        {
            // copy, then invert KEYEVENTF_KEYUP flag
            inputs[idx+i] = inputs[i];
            if (inputs[idx+i].ki.dwFlags & KEYEVENTF_KEYUP)
                inputs[idx+i].ki.dwFlags &= ~KEYEVENTF_KEYUP;
            else
                inputs[idx+i].ki.dwFlags |= KEYEVENTF_KEYUP;
        }
        idx += idx-1;
    }

    // now send the keys
    SendInput(idx, inputs, sizeof(inputs[0]));

    //##PQ from touchcursor##
    // Sleep(1) seems to be necessary for mapped Escape events sent to VirtualPC & recent VMware versions.
    // (Sleep(0) is no good)
    // Also for mapped modifiers with Remote Desktop Connection.
    // Dunno why:
    Sleep(1);
}

void Keyboard::SetupInputKey(INPUT& input, VeeKee vk, bool down, DWORD injectedFromMeValue)
{
    //UINT scancode = MapVirtualKeyExA(vk, MAPVK_VK_TO_VSC_EX, NULL);

    input.type = INPUT_KEYBOARD;
    input.ki.wVk = static_cast<WORD>(vk);
    //input.ki.wScan = scancode;
    input.ki.dwFlags = down ? 0 : KEYEVENTF_KEYUP;
    input.ki.time = 0;
    input.ki.dwExtraInfo = injectedFromMeValue;
    if (IsExtended(vk))
        input.ki.dwFlags |= KEYEVENTF_EXTENDEDKEY;
}

// dbg 
void Keyboard::OutNbKeysDn()
{
    auto NotZero = [](VeeKee vk) {return vk != 0; };
    int nbmodsdn = std::count_if(&downModifiers[1], &downModifiers[0xFF], NotZero);
    int nbkeysdn = std::count_if(&downKeys[1], &downKeys[0xFF], NotZero);

    Printf("keys dn %d mods dn %d\n", nbkeysdn, nbmodsdn);
}

//-------


