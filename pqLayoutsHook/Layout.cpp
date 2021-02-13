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
#include "LayerAccessAction.h"
#include "LayerToggleAction.h"
#include "util.h"

using namespace KeyActions;

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


bool Layout::SetLayerAccessKey(const Layer::Id_t& layerId, KeyDef accessKey, bool toggleOnTap)
{
    // find layer
    auto foundLayer = layersById.find(layerId);
    if (foundLayer == layersById.end())
        return false;

    Layer* layer = foundLayer->second;

    // create key action object
    KeyValue accessKeyValue(accessKey.Vk(), accessKey.Scancode());
    IKeyAction* actionTo;
    actionTo = new LayerAccessAction(accessKey, layer->LayerIdx(), toggleOnTap);

    // and register mapping on main layer
    // AND on the layer itself, so we can return on access key UP
    layers[0]->AddMapping(accessKeyValue, actionTo);
    layer->AddMapping(accessKeyValue, actionTo);

    // also register access key shifted
    accessKeyValue.Shift(true);
    layers[0]->AddMapping(accessKeyValue, actionTo);
    layer->AddMapping(accessKeyValue, actionTo);

    return true;
}

bool Layout::GotoMainLayer()
{
    // main layer always 1st
    currentLayer = layers[0];
    Printf("switching to layer [%s](%d)\n", currentLayer->Name().c_str(), currentLayer->LayerIdx());

    return true;
}

bool Layout::GotoLayer(Layer::Idx_t layerIdx)
{
    if (layerIdx >= layers.size())
        return false;

    currentLayer = layers[layerIdx];
    Printf("switching to layer [%s](%d)\n", currentLayer->Name().c_str(), currentLayer->LayerIdx());

    return true;
}

bool Layout::GotoLayer(const Layer::Id_t& layerId)
{
    auto foundit = layersById.find(layerId);
    if (foundit == layersById.end())
        return false;

    currentLayer = foundit->second;
    Printf("switching to layer [%s](%d)\n", currentLayer->Name().c_str(), currentLayer->LayerIdx());

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

    return currentLayer->AddMapping(from, to, false);
}

bool Layout::AddCtrlMapping(KeyValue from, KeyValue to)
{
    if (currentLayer == nullptr)
        return false;

    return currentLayer->AddMapping(from, to, true);
}

bool Layout::AddStickyMapping(KeyValue vk)
{
    if (currentLayer == nullptr)
        return false;

    return currentLayer->AddStickyMapping(vk);
}
