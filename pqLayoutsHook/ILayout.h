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

#include "Layer.h"
#include "KeyDef.h"
#include "KeyMapping.h"
#include <list>

// Base actions for layout
class ILayout
{
public:
    virtual ~ILayout() = default;

    virtual bool AddLayer(const Layer::Id_t& layerId, Layer::Idx_t& newLayerIdx) = 0;
    virtual bool SetLayerAccessKey(const Layer::Id_t& layerId, KeyDef accessKey, bool canTap, KeyValue keyOnTap) = 0;
    
    virtual const Layer* GetMainLayer() = 0;
    virtual bool GotoMainLayer() = 0;
    virtual bool GotoLayer(Layer::Idx_t layerIdx) = 0;
    virtual bool GotoLayer(const Layer::Id_t& layerId) = 0;
    virtual const Layer* CurrentLayer() const = 0;
    
    virtual void SetImageView(Layer::ImageView imageView, Layer::ImageView imageViewShift) const = 0;
    virtual Layer::ImageView GetImageView() const = 0;
    
    virtual KeyValue VkMapping(VeeKee vk) const = 0;
    virtual VeeKeeEx ReverseMapping(VeeKeeEx vkEx) const = 0;
    
    virtual bool AddMapping(KeyValue from, KeyValue to) = 0;
    virtual bool AddDualModeModifier(KeyDef key, KeyValue modifierKey, KeyValue tapKey) = 0;
    virtual bool AddChord(Kord& chord, const std::list<KeyActions::KeyActionPair>& keyActions) = 0;
};