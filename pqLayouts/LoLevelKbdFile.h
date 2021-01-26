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

//---------

class StringTokener : public std::istringstream
{
public:
    StringTokener(const std::string& line, int lineNo);
    int LineNo() const { return lineNo; }

private:
    int lineNo;
};

//---------

class KeyParser
{
public:
    KeyParser(StringTokener& tokener, const char* paramName);

    bool operator ()();

    // checks bit 0 of hibyte of ret val from VkKeyScanA
    static bool HasShiftBit(SHORT scanExVal) 
    { 
        return ((scanExVal >> 8) & 0x01) != 0; 
    }

public:
    std::string param;  // this is the text value read from file for the key
    bool hasShiftPrefix;
    bool isShifted;     // as defined by VkKeyScan()
    WORD vk;

private:
    bool parseKey();
private:
    StringTokener& tokener;
    const char* paramName;
};

//---------

class LoLevelKbdFile
{
public:
    LoLevelKbdFile();

    bool ReadKeyboardFile(const char* filename);

    static const std::map<std::string, WORD>& KeyNames() { return keyNames; }

private:
    bool doK2kCmd(StringTokener& stringTokener);
    bool doK2kWithShCmd(StringTokener& stringTokener);

private:
    static std::map<std::string, WORD> keyNames;
};

