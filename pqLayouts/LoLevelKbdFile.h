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

#include <sstream>
#include "Keydef.h"
#include "KeyMapping.h"
#include "combos.h"
#include "Chord.h"
#include "ChordingData.h"
#include "KeyParser.h"
#include "nlohmann/json.hpp"

//---------

class LoLevelKbdFile
{
public:
    LoLevelKbdFile();

    bool ReadKeyboardFile(const char* filename);

private:
    bool doK2kCmd(StringTokener& tokener);
    bool doK2kWithShCmd(StringTokener& tokener);
    bool doK2kcCmd(StringTokener& tokener);
    bool doK2kcWithShCmd(StringTokener& tokener);
    bool doDualModifier(StringTokener& tokener);
    bool doSteakPower(File&);
    //bool doKord(StringTokener& tokener);
    bool doSteaks(File& file);
    bool doLpSteaks(StringTokener& tokener);
    void SetChordKeyActionPair(const KeyValue& outKey, KeyActions::KeyActionPair& actionPair);
    bool readKeysList(File& kbdfile, const char* paramName, std::list<KeyValue>& stenoKeys, std::vector<char>& stenoChars);
    bool parseChordValue(StringTokener& tokener, KeyParser& chordOutput, Kord& chord);
    bool addLayer(StringTokener& tokener, bool toggleOnTap, bool outputKeyOnTap);
    bool setImageFile(StringTokener& tokener, const char * scriptFilename);
    bool GetRelativeFilePath(const char* pcScriptFilename, std::string& imageFilename, WCHAR  fullImagePath[MAX_PATH]);
    bool setImageView(StringTokener& tokener);
    bool getIncludeFilePath(StringTokener& tokener, const char * scriptFilename, std::string& includePath);
    void ReadStringPairs(nlohmann::json& json, StringPairList& pairs, const std::string& jsonFilePath);
    bool readStringPairsFromJSONFile(const std::string& jsonFilePath, StringPairList& pairs);
    bool readTextComboDefsFromJSONFile(const std::string& jsonFilePath, TextComboDefs& textComboDefs);
    bool doInclude(StringTokener& tokener, const char* pcScriptFilename);
    bool doAdaptives(StringTokener& tokener, const char* scriptFilename);
    bool doCombos(StringTokener& tokener, const char* scriptFilename);

private:

    ChordingData chording;
    WORD hyphenVk;
};

