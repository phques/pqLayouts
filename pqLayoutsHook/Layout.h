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

#include "Keydef.h"
#include "KeyMapping.h"
#include "layer.h"


class Layout
{
public:
    Layout();
    ~Layout();

    bool AddLayer(const Layer::Id_t&, Layer::Idx_t& newLayerIdx);
    bool SetLayerAccessKey(const Layer::Id_t& layerId, KeyDef accessKey, bool isToggle, KeyValue keyOnTap);

    bool GotoMainLayer();
    bool GotoLayer(Layer::Idx_t layerIdx);
    bool GotoLayer(const Layer::Id_t& layerId);
    bool GetLayerIndex(const Layer::Id_t& layerId, Layer::Idx_t& layerIdx);
    const Layer* CurrentLayer() const { return currentLayer; }

    const CaseMapping* Mapping(VeeKee) const;
    const CaseMapping* Mapping(Layer::Idx_t layerIdx, VeeKee vk) const;
    bool AddMapping(KeyValue from, KeyValue to);
    bool AddDualModeModifier(KeyDef key, KeyValue modifierKey, KeyValue tapKey);

    bool AddChord(Kord& chord, const std::list<KeyActions::KeyActionPair>& keyActions);
    const std::list<KeyActions::KeyActionPair>* GetChordActions(const Kord& chord);

    void SetImageView(Layer::ImageView imageView, Layer::ImageView imageViewShift) const;
    Layer::ImageView GetImageView(bool shiftDown) const;

private:
    bool GotoLayer(Layer* layer);

private:
    std::map<Layer::Id_t, Layer*> layersById;  
    std::vector<Layer*> layers;  
    Layer* currentLayer;
};

