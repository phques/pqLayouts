// KeyParser.cpp

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

#include "pch.h"

#include "KeyParser.h"
#include "util.h"

KeyParser::KeyParser(StringTokener& tokener, const char* paramName) :
    tokener(tokener), paramName(paramName)
{
}

bool KeyParser::ReadFromTokener()
{
    // read key param
    if (tokener.eof()) {
        std::cerr << "missing param '" << paramName << "', line " << tokener.LineNo() << std::endl;
        return false;
    }

    tokener >> token;
    return true;
}

bool KeyParser::operator ()()
{
    // read key param
    if (!ReadFromTokener())
        return false;

    // parse key
    return ParseKey(true);
}

// reads multiple keys from current line (from tokener), no modifiers on keys
// can be space separated or as a string:  A S D F tab QWERTY  
// comments are done with "!!" and end the line
// read keys are *added* to param 'keys'
bool KeyParser::GetKeys(std::list<KeyValue>& keys, std::vector<char>& stenoChars)
{

    while (!tokener.eof())
    {
        token.clear();
        tokener >> token;

        // rest of line is comment 
        if (token == "!!")
            break;

        if (!GetKeysFromToken(keys, stenoChars))
            return false;
    }

    return true;
}

bool KeyParser::GetKeysFromToken(std::list<KeyValue>& keyValues, std::vector<char>& stenoChars)
{
    // could be a known key name
    const auto vk = VkUtil::LookupKeyName(token);
    if (vk != 0)
    {
        // key name, save key's VK
        keyValues.push_back(KeyValue(vk, 0));
        stenoChars.push_back('?'); // unknown char
    }
    else
    {
        // either a single key represented by its character 'a' or a chain of characters 'asdf'
        // add each to the chord definition
        for (char ch : token)
        {
            WORD vk = 0;
            bool isShifted = false;

            if (!VkUtil::CharToVk(ch, vk, isShifted))
            {
                std::cerr << "non valid key [" << token << "], line " << tokener.LineNo() << std::endl;
                return false;
            }

            keyValues.push_back(KeyValue(vk, 0, isShifted));
            stenoChars.push_back(ch);
        }
    }

    return true;
}

KeyValue KeyParser::ToKeyValue() const
{
    if (isCommand)
    {
        return KeyValue(cmd, isShifted || hasShiftPrefix, hasControlPrefix, hasAltPrefix);
    }

    return KeyValue(vk, 0, isShifted || hasShiftPrefix, hasControlPrefix, hasAltPrefix);
}


bool KeyParser::ParseKey(bool showError)
{
    // check for prefix '+' for shifted key
    const char* keytext = token.c_str();

    bool stop = false;
    while (!stop && strlen(keytext) > 1)
    {
        switch (keytext[0])
        {
        case '+':
            hasShiftPrefix = true;
            keytext++;
            break;
        case '^':
            hasControlPrefix = true;
            keytext++;
            break;
        case '@':
            hasAltPrefix = true;
            keytext++;
            break;
        case '#':
            isCommand  = true;
            keytext++;
            break;
        default:
            stop = true;
            break;
        }
    }

    if (isCommand)
    {
        // command name ?
        cmd = LookupCommandName(keytext);
        if (cmd == Commands::None)
        {
            if (showError)
            {
                std::cerr << "unknown command [" << token << "], line " << tokener.LineNo() << std::endl;
            }
            return false;
        }
    }
    else
    {
        // keyname ?
        if (strlen(keytext) > 1)
        {
            vk = VkUtil::LookupKeyName(keytext);
            if (vk == 0)
            {
                if (showError)
                {
                    std::cerr << "unknown key [" << token << "], line " << tokener.LineNo() << std::endl;
                }
                return false;
            }
        }
        else
        {
            // just a character representing the key 'w', '{' etc
            if (!VkUtil::CharToVk(*keytext, vk, isShifted))
            {
                if (showError)
                {
                    std::cerr << "non valid key [" << token << "], line " << tokener.LineNo() << std::endl;
                }
                return false;
            }
        }
    }

    return true;
}

bool KeyParser::ParseKeys(std::vector<KeyValue>& keys)
{
    while (!tokener.eof())
    {
        token.clear();
        tokener >> token;

        // rest of line is comment 
        if (token == "!!")
            break;

        if (!ParseKey(true))
            return false;

        keys.push_back(ToKeyValue());
    }

    return true;
}
