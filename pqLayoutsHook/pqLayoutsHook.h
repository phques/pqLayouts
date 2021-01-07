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

#include "dllExport.h"
#include "Keydef.h"
#include "Layer.h"

PQHOOK_API bool HookKbdLL();
PQHOOK_API bool UnhookKbdLL();

PQHOOK_API bool AddMapping(KeyValue from, KeyValue to);
PQHOOK_API bool AddCtrlMapping(KeyValue from, KeyValue to);

PQHOOK_API bool AddLayer(const char* layerId, Layer::Idx_t& newLayerIdx);
PQHOOK_API bool SetLayerAccessKey(const char* layerId, KeyDef);

PQHOOK_API bool GotoLayer(const char* layerId);
PQHOOK_API bool GotoLayer(Layer::Idx_t layerIdx);
PQHOOK_API bool GotoMainLayer();
