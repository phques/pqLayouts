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
#pragma once

#include "dllExport.h"
#include "Keydef.h"
#include "Layer.h"
#include "Chord.h"
#include "KeyMapping.h"
#include "Notification.h"
#include "Chording.h"

namespace HookKbd
{
    PQHOOK_API bool HookKbdLL();
    PQHOOK_API bool UnhookKbdLL();
    PQHOOK_API void SetNotificationMessage(HWND hMainWindow, int mainWndMsg);

    PQHOOK_API bool AddMapping(KeyValue from, KeyValue to);
    PQHOOK_API bool AddDualModeModifier(KeyDef key, KeyValue modifierKey, KeyValue tapKey);

    PQHOOK_API bool AddChord(Kord&, const std::list<KeyActions::KeyActionPair>& keyActions);
    PQHOOK_API bool InitChordingKeys(const ChordingKeys& chordingKeys);
    PQHOOK_API void SetLeftHandPrefix(Layer::Id_t lpsteaksLayerName1, Layer::Id_t lpsteaksLayerName2,
                                      std::string lpsteaksPrefix1, std::string lpsteaksPrefix2);

    PQHOOK_API bool AddLayer(const char* layerId, Layer::Idx_t& newLayerIdx);
    PQHOOK_API bool SetLayerAccessKey(const char* layerId, KeyDef, bool isToggle, KeyValue keyOnTap);

    PQHOOK_API bool GotoLayer(const char* layerId);
    PQHOOK_API bool GotoLayer(Layer::Idx_t layerIdx);
    PQHOOK_API bool GotoMainLayer();

    PQHOOK_API void SetImageFilename(const WCHAR* filename);
    PQHOOK_API const WCHAR* GetImageFilename();

    PQHOOK_API void SetImageView(Layer::ImageView imageView, Layer::ImageView imageViewShift);
    PQHOOK_API Layer::ImageView GetImageView();

    PQHOOK_API void SuspendKey(VeeKee suspendKey, VeeKee quitKey);
    PQHOOK_API void ToggleSuspend();
    PQHOOK_API bool Suspended();
};
