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
#include "pqLayoutsHook.h"
#include "util.h"
#include "OutDbg.h"
#include "Keyboard.h"
#include "Notification.h"
#include "Notifier.h"


namespace
{
    // a 'unique' value to identify values we injected with SendInput
    const ULONG_PTR injectedFromMe = 0x0E5FA78A; // <Guid("0E5FA78A-82FE-4557-956E-320702FEB659")>

    HHOOK hKbdHook = NULL;
    Keyboard theKbd(injectedFromMe);

}


namespace HookKbd
{

    // the 'Notifier'
    void Notify(Notif notif, LPARAM lParam)
    {
        theKbd.Notify(notif, lParam);
    }

}

// put this in a class since we cannot debug this code,
// so we'll use friend class KbdHook to access data for dbg output
class KbdHook
{
public:
    // called for each KBDLLHOOK event
    static LRESULT CALLBACK LowLevelKeyboardProc(
        _In_ int    nCode,
        _In_ WPARAM wParam,
        _In_ LPARAM lParam
    )
    {
        bool eat = false;

        // should we process this?
        if (nCode == HC_ACTION)
        {
            KbdHookEvent event(lParam);

            // dbg output
            // comment out the IF to see debug output for injected kbd inputs
#ifdef _DEBUG
           //if (!SelfInjected(event))
            KbdProcDebugOut(event, wParam, SelfInjected(event));
#endif // !_DEBUG

            // process this key
            if (!SelfInjected(event))
                eat = theKbd.OnKeyEvent(event);
        }

        // dont forward to next hook
        if (eat)
            return 1;

        // forward to next hook
        return CallNextHookEx(hKbdHook, nCode, wParam, lParam);
    }

    static bool SelfInjected(const KbdHookEvent& event)
    {
        return (event.Injected() && event.dwExtraInfo == injectedFromMe);
    }

    // dbg output
    static void KbdProcDebugOut(KbdHookEvent& event, const WPARAM& wParam, bool selfInjected)
    {
        // use SCrollLock key on my kbd to outut ------ in dbg to help separate events
        // or F6 (no scrollLock on Mac kbd ;-)
        if (event.vkCode == 0x91 || event.vkCode == 0x75) {
            if (event.Down() && !selfInjected)
                Printf("---------\n");
            return;
        }

        Dbg::Out::KbdEVent(event, wParam, selfInjected);

        //theKbd.OutNbKeysDn();

        //if (event.Down())
        //{
        //    UINT scancode = MapVirtualKeyExA(event.vkCode, MAPVK_VK_TO_VSC_EX, NULL);

        //    std::ostringstream os;
        //    os << std::hex << " scan " << scancode << std::endl;
        //    Dbg::Out::DebugString(os);
        //}
    }

};

//------ [exports... --------

namespace HookKbd
{

// hook our low level kbd procedure
PQHOOK_API bool HookKbdLL()
{
    hKbdHook = SetWindowsHookEx(WH_KEYBOARD_LL, KbdHook::LowLevelKeyboardProc, GetModuleHandle(0), 0);

    if (hKbdHook == NULL)
        return Dbg::Out::LastError("SetWindowsHookEx");

    return true;
}

// unhook our low level kbd procedure
PQHOOK_API bool UnhookKbdLL()
{
    if (hKbdHook == NULL)
        return false;

    if (!UnhookWindowsHookEx(hKbdHook))
        return Dbg::Out::LastError("UnhookWindowsHookEx"); 

    return true;
}

void SetNotificationMessage(HWND hMainWindow, int mainWndMsg)
{
    theKbd.SetMainWnd(hMainWindow);
    theKbd.SetMainWndMsg(mainWndMsg);
}


PQHOOK_API bool AddMapping(KeyValue from, KeyValue to)
{
    return theKbd.AddMapping(from, to);
}

PQHOOK_API bool AddMappingP(KeyValueP fromP, KeyValueP toP)
{
    //    PQHOOK_API KeyValue(VeeKee, UINT scancode, bool shift=false, bool control=false, bool alt=false);

    KeyValue from(fromP.vk, fromP.scancode, fromP.shift, fromP.control, fromP.alt);
    KeyValue to(toP.vk, toP.scancode, toP.shift, toP.control, toP.alt);
    return theKbd.AddMapping(from, to);
}

bool HookKbd::AddDualModeModifier(KeyDef key, KeyValue modifierKey, KeyValue tapKey)
{
    return theKbd.AddDualModeModifier(key, modifierKey, tapKey);
}

PQHOOK_API bool AddChord(Kord& chord, const std::list<KeyActions::KeyActionPair>& keyActions)
{
    return theKbd.AddChord(chord, keyActions);
}

PQHOOK_API bool InitChordingKeys(const ChordingKeys& chordingKeys)
{
    return theKbd.InitChordingKeys(chordingKeys);
}

PQHOOK_API void SetLeftHandPrefix(
    Layer::Id_t lpsteaksLayerName1, Layer::Id_t lpsteaksLayerName2, 
    std::string lpsteaksPrefix1, std::string lpsteaksPrefix2)
{
    theKbd.SetLeftHandPrefix(lpsteaksLayerName1, lpsteaksLayerName2, lpsteaksPrefix1, lpsteaksPrefix2);
}

PQHOOK_API bool AddLayer(const char* layerId, Layer::Idx_t& newLayerIdx)
{
    return theKbd.AddLayer(layerId, newLayerIdx);
}

PQHOOK_API bool SetLayerAccessKey(const char* layerId, KeyDef keydef, bool canTap, KeyValue keyOnTap)
{
    return theKbd.SetLayerAccessKey(layerId, keydef, canTap, keyOnTap);
}


PQHOOK_API bool GotoLayer(const char* layerId)
{
    return theKbd.GotoLayer(layerId);
}

PQHOOK_API bool GotoLayerByIdx(Layer::Idx_t layerIdx)
{
    return theKbd.GotoLayer(layerIdx);
}

PQHOOK_API bool GotoMainLayer()
{
    return theKbd.GotoMainLayer();
}

PQHOOK_API void SetImageFilename(const WCHAR* filename)
{
    theKbd.SetImageFilename(filename);
}

PQHOOK_API const WCHAR* GetImageFilename()
{
    return theKbd.GetImageFilename().c_str();
}

PQHOOK_API void SetImageView(Layer::ImageView imageView, Layer::ImageView imageViewShift)
{
    theKbd.SetImageView(imageView, imageViewShift);
}

void HookKbd::GetImageView(Layer::ImageView* imageView)
{
    *imageView = theKbd.GetImageView();
}

PQHOOK_API void SuspendKey(VeeKee suspendKey, VeeKee quitKey)
{
    theKbd.SuspendKey(suspendKey);
    theKbd.QuitKey(quitKey);
}

PQHOOK_API void ToggleSuspend()
{
    theKbd.ToggleSuspend();
}

PQHOOK_API bool Suspended()
{
    return theKbd.Suspended();
}

}; // namespace HookKbd


//------ ... exports] --------


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}




