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
#include "Layout.h"

Layout::Layout() : currentLayer(nullptr)
{
    // Layout always has a main layer, starts as the current layer
    Layer::Idx_t layerIdx=0;
    AddLayer("main", layerIdx);
    currentLayer = layersById["main"];
}

Layout::~Layout()
{
    for (auto layer : layers)
        delete layer;
}

bool Layout::AddLayer(const Layer::Id_t& layerId, Layer::Idx_t& newLayerIdx)
{
    newLayerIdx = -1;

    // layer already exists?
    if (layersById.find(layerId) != layersById.end())
        return false;

    // create and save new layer
    newLayerIdx = layers.size();

    auto layer = new Layer(layerId, newLayerIdx);
    layersById[layerId] = layer;
    layers.push_back(layer);

    return true;
}

bool Layout::SetLayerAccessKey(const Layer::Id_t& layerId, KeyDef keydef)
{
    auto foundLayer = layersById.find(layerId);
    if (foundLayer == layersById.end())
        return false;

    // mapped vk is  > 0xFF, low byte holds layerIdx
    VeeKee veekee = 0xFF000000 | foundLayer->second->LayerIdx();
    KeyValue accessKey(keydef.Vk(), keydef.Scancode(), false);
    KeyValue mappedKey(veekee, 0, false);

    layers[0]->AddMapping(accessKey, mappedKey);

    return true;
}

bool Layout::GotoMainLayer()
{
    // main layer always 1st
    currentLayer = layers[0];
    return true;
}

bool Layout::GotoLayer(Layer::Idx_t layerIdx)
{
    if (layerIdx >= layers.size())
        return false;

    currentLayer = layers[layerIdx];
    return true;
}

bool Layout::GotoLayer(const Layer::Id_t& layerId)
{
    auto foundit = layersById.find(layerId);
    if (foundit == layersById.end())
        return false;

    currentLayer = foundit->second;
    return true;
}

const CaseMapping* Layout::Mapping(VeeKee vk) const
{
    if (currentLayer == nullptr)
        return nullptr;

    return currentLayer->Mapping(vk);
}

bool Layout::AddMapping(KeyValue from, KeyValue to)
{
    if (currentLayer == nullptr)
        return false;

    return currentLayer->AddMapping(from, to);
}

