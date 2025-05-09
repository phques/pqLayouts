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
#include "Chord.h"
#include "ChordingData.h"

//---------


class File
{
public:
    File(std::ifstream& f) : lineNo(0), f(f)
    {}

    void GetLine();

    int lineNo;
    std::string line;
    std::ifstream& f;
};


//-------------


class StringTokener : public std::istringstream
{
public:
    StringTokener(const std::string& line, int lineNo);
    StringTokener(File&);

    int LineNo() const { return lineNo; }
    bool ReadParam(const char* paramName, std::string& param);

private:
    int lineNo;
};

//---------

class KeyParser
{
public:
    KeyParser(StringTokener& tokener, const char* paramName);

    bool operator ()();
    bool ReadFromTokener();
    bool ParseKey(bool showError);

    bool GetKeys(std::list<KeyValue> & keys, std::vector<char>& stenoChars);
    bool GetKeysFromToken(std::list<KeyValue>& keys, std::vector<char>& stenoChars);
    KeyValue ToKeyValue() const;

public:
    std::string token;  // this is the text value read from file for the key

    bool hasShiftPrefix;
    bool hasControlPrefix;
    bool hasAltPrefix;
    bool isShifted;     // as defined by VkKeyScan()
    WORD vk;

private:
    StringTokener& tokener;
    const char* paramName;
};

//---------

class LoLevelKbdFile
{
public:
    typedef std::map<std::string, WORD> KeyNamesMap;

public:
    LoLevelKbdFile();

    bool ReadKeyboardFile(const char* filename);

    static const KeyNamesMap& KeyNames() { return keyNames; }
    static WORD LookupKeyName(const std::string& keyText);

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
    bool doInclude(StringTokener& tokener, const char * scriptFilename);

private:
    static KeyNamesMap keyNames;
    
    ChordingData chording;
    WORD hyphenVk;
};

