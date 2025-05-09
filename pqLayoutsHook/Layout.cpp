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
#include "Notifier.h"
#include "util.h"
#include "../pqLayouts/LoLevelKbdFile.h"

using namespace KeyActions;

Layout::Layout() : currentLayer(nullptr)
{
    // Layout always has a main layer, starts as the current layer
    Layer::Idx_t layerIdx=0;
    AddLayer(MainLayerName, layerIdx);
    currentLayer = layersById[MainLayerName];
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


bool Layout::SetLayerAccessKey(const Layer::Id_t& layerId, KeyDef accessKey, bool toggleOnTap, KeyValue keyOnTap)
{
    // find layer
    auto foundLayer = layersById.find(layerId);
    if (foundLayer == layersById.end())
        return false;

    Layer* layer = foundLayer->second;

    // create key action object
    KeyValue accessKeyValue(accessKey.Vk(), accessKey.Scancode());
    IKeyAction* actionTo;
    actionTo = new LayerAccessAction(accessKey, layer->LayerIdx(), toggleOnTap, keyOnTap);

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

bool Layout::GetLayerIndex(const Layer::Id_t& layerId, Layer::Idx_t& layerIdx)
{
    auto foundit = layersById.find(layerId);
    if (foundit == layersById.end())
        return false;

    layerIdx = foundit->second->LayerIdx();
    return true;
}


bool Layout::GotoLayer(Layer* layer)
{
    currentLayer = layer;
    Printf("switching to layer [%s](%d)\n", currentLayer->Name().c_str(), currentLayer->LayerIdx());

    // notify main app window
    HookKbd::Notify(HookKbd::LayerChanged, currentLayer->LayerIdx());

    return true;
}

bool Layout::GotoMainLayer()
{
    // main layer always 1st
    return GotoLayer(layers[0]);
}

bool Layout::GotoLayer(Layer::Idx_t layerIdx)
{
    if (layerIdx >= layers.size())
        return false;

    return GotoLayer(layers[layerIdx]);
}

bool Layout::GotoLayer(const Layer::Id_t& layerId)
{
    auto foundit = layersById.find(layerId);
    if (foundit == layersById.end())
        return false;

    return GotoLayer(foundit->second);

}

const CaseMapping* Layout::Mapping(VeeKee vk) const
{
    if (currentLayer == nullptr)
        return nullptr;

    return currentLayer->Mapping(vk);
}

const CaseMapping* Layout::Mapping(Layer::Idx_t layerIdx, VeeKee vk) const
{
    if (layerIdx >= layers.size()  || layers[layerIdx] == nullptr)
        return nullptr;

    return layers[layerIdx]->Mapping(vk);
}

bool Layout::AddMapping(KeyValue from, KeyValue to)
{
    if (currentLayer == nullptr)
        return false;

    return currentLayer->AddMapping(from, to);
}

bool Layout::AddDualModeModifier(KeyDef key, KeyValue modifierKey, KeyValue tapKey)
{
    if (currentLayer == nullptr)
        return false;

    return currentLayer->AddDualModeModifier(key, modifierKey, tapKey);
}



bool Layout::AddChord(Kord& chord, const std::list<KeyActions::KeyActionPair>& keyActions)
{
    if (currentLayer == nullptr)
        return false;

    return currentLayer->AddChord(chord, keyActions);
}

// lookup 'chord' and return its asociated key action if found, else null 
const std::list<KeyActions::KeyActionPair>* Layout::GetChordActions(const Kord& chord)
{
    if (currentLayer != nullptr)
        return currentLayer->GetChordActions(chord);

    // return empty actions
    return nullptr;
}

void Layout::SetImageView(Layer::ImageView imageView, Layer::ImageView imageViewShift) const
{
    if (currentLayer != nullptr)
        currentLayer->SetImageView(imageView, imageViewShift);
}

Layer::ImageView Layout::GetImageView(bool shiftDown) const
{
    if (currentLayer != nullptr)
        return currentLayer->GetImageView(shiftDown);

    return {};
}

