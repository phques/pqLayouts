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

#include "KeyMapping.h"
#include "Chord.h"

constexpr const char* MainLayerName = "main";


class Layer
{
public:
    typedef std::string Id_t;
    typedef unsigned int Idx_t;

    // Y offsets into the kbd help image for this layer
    struct ImageView
    {
        int TopY;
        int BottomY;

        ImageView() : TopY(0), BottomY(0) {}
        ImageView(int topY, int bottomY) : TopY(topY), BottomY(bottomY) {}

        int Height() const { return BottomY - TopY; }
    };

public:
    Layer(const Id_t& name, Idx_t layerIdx);

    const Id_t& Name() const { return name; }
    Idx_t LayerIdx() const { return layerIdx;}

    const CaseMapping* Mapping(VeeKee) const;
    bool AddMapping(KeyValue from, KeyActions::IKeyAction* actionTo);
    bool AddMapping(KeyValue from, KeyValue to);
    bool AddDualModeModifier(KeyDef key, KeyValue  modifierKey, KeyValue tapKey);

    bool HasChords() const { return chords.size() > 0; }
    bool AddChord(Kord& chord, const std::list<KeyActions::KeyActionPair>& keyActions);
    const std::list<KeyActions::KeyActionPair>* GetChordActions(const Kord& chord);

    void SetImageView(ImageView imageView, Layer::ImageView imageViewShift);
    ImageView GetImageView(bool shiftDown) const;

private:
    Id_t name;
    Idx_t layerIdx;
    std::map<VeeKee, CaseMapping> mappings;
    ImageView imageView;
    ImageView imageViewShift;

    struct ChordActions
    {
        Kord chord;
        std::list<KeyActions::KeyActionPair> keyActions;

        bool operator==(const Kord& otherChord) const { 
            return otherChord == chord; 
        }
    };

    // pq-todo? might become performance prob if larger qty of chords
    std::vector<ChordActions> chords;

};

