// KeyParser.h

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

#include "pch.h"
#include "StringTokener.h"
#include "../pqLayoutsHook/Keydef.h"

class KeyParser
{
public:
    KeyParser(StringTokener& tokener, const char* paramName);

    bool operator ()();
    bool ReadFromTokener();
    bool ParseKey(bool showError);

    bool GetKeys(std::list<KeyValue>& keys, std::vector<char>& stenoChars);
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
