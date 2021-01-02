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

    memset(mappings, 0, sizeof(mappings));

    // init mappings for one-to-one (0x00, 0xFF are not VKs)
    //for (DWORD i = 1; i < 0xFF; i++)
    //{
    //    mappings[0][i] = mappings[1][i] = i;
    //}
}

bool Keyboard::AddLayer(const Layer::LayerId_t& layerId)
{
    return layout.AddLayer(layerId);
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
        return 0;

    return (ShiftDown() ? &caseMapping->shifted : &caseMapping->nonShifted); 
}

bool Keyboard::AddMapping(KeyValue from, KeyValue to)
{
    if (from.Vk() >= 0xFF || to.Vk() >= 0xFF)
        return false;

    // dbg
    Printf("Add mapping from %02x, to %02x\n", from.Vk(), to.Vk());

    //int shiftedIdx(shifted ? 1 : 0);
    //mappings[shiftedIdx][vkFrom] = vkTo;

    return layout.AddMapping(from, to);
}


//------


// return true if we should skip / eat this virtual-key
bool Keyboard::OnKeyEVent(KbdHookEvent& event, DWORD injectedFromMe)
{
    const CaseMapping* caseMapping = layout.Mapping(event.vkCode);

    // safer this way,
    // for eg. with Windows layouts that use AltGr, right Alt actually outputs
    //         LCtrl + RAlt .. but with a weird scancode for LCtrl,
    // if we don't send exactly the same vk / scancode, it screws up and LCtrl-up is never generated !!!
    // ## actually, avoid using such Windows layouts with this software for now !
    if (caseMapping == nullptr) // not mapped, dont touch
        return false; // but let it through to next kbd hook

    // drill down to out mapping and vk 
    const KeyMapping& mapping = ShiftDown() ? caseMapping->shifted : caseMapping->nonShifted;
    KeyValue valueOut = mapping.Mapping();
    VeeKee vkOut = valueOut.Vk();

    if (vkOut == 0) // not mapped, dont touch
        return false; // but let it through to next kbd hook

    if (IsModifier(vkOut))
        ModifierDown(vkOut, event.Down());

    KeyDown(vkOut, event.Down());

    SendVk(vkOut, event.Down(), injectedFromMe);
    
    return true;
}



void Keyboard::SendVk(VeeKee vk, bool down, DWORD injectedFromMe)
{
    // maybe pre compute this ?
    //##PQ actually a few keys don't seem to properly convert to scan code (kbd specific?)
    //## PrtScrn, Pause don't convert to the scancode that we received in the kbd hook event !?
    UINT scancode = MapVirtualKeyExA(vk, MAPVK_VK_TO_VSC_EX, NULL);

    INPUT input = { 0 };
    input.type = INPUT_KEYBOARD;
    input.ki.wVk = static_cast<WORD>(vk);
    input.ki.wScan = scancode;
    input.ki.dwFlags = down ? 0 : KEYEVENTF_KEYUP;
    input.ki.time = 0;
    input.ki.dwExtraInfo = injectedFromMe;
    if (IsExtended(vk))
        input.ki.dwFlags |= KEYEVENTF_EXTENDEDKEY;

    SendInput(1, &input, sizeof(input));

    //##PQ from touchcursor##
    // Sleep(1) seems to be necessary for mapped Escape events sent to VirtualPC & recent VMware versions.
    // (Sleep(0) is no good)
    // Also for mapped modifiers with Remote Desktop Connection.
    // Dunno why:
    Sleep(1);
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


