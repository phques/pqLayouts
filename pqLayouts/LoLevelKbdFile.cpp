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

#include "stdafx.h"
#include "LoLevelKbdFile.h"
#include "pqLayoutsHook.h"


std::map<std::string, WORD> LoLevelKbdFile::keyNames = {
    {"caps", VK_CAPITAL},
    {"esc", VK_ESCAPE},
    {"tab", VK_TAB},
    {"space", VK_SPACE},            {"sp", VK_SPACE},
    {"enter", VK_RETURN},           {"cr", VK_RETURN},
    {"backspace", VK_BACK},         {"bs", VK_BACK},
    {"del", VK_DELETE},
    {"lshift", VK_LSHIFT},
    {"rshift", VK_RSHIFT},
    {"lcontrol", VK_LCONTROL},      {"lctrl", VK_LCONTROL},
    {"rcontrol", VK_RCONTROL},      {"rctrl", VK_RCONTROL},
    {"lalt", VK_LMENU},
    {"ralt", VK_RMENU},
    {"left", VK_LEFT},
    {"right", VK_RIGHT},
    {"up", VK_UP},
    {"down", VK_DOWN},
    {"home", VK_HOME},
    {"end", VK_END},
    {"pgup", VK_PRIOR},
    {"pgdn", VK_NEXT},
    {"ins", VK_INSERT},
    {"F1", VK_F1},  {"F2", VK_F2},  {"F3", VK_F3},  {"F4", VK_F4},  {"F5", VK_F5}, {"F6", VK_F6},
    {"F7", VK_F7},  {"F8", VK_F8},  {"F9", VK_F9},  {"F10", VK_F10},  {"F11", VK_F11}, {"F12", VK_F12},

};

//--------------


StringTokener::StringTokener(const std::string& line, int lineNo) : lineNo(lineNo)
{
    str(line);
}

//--------------


KeyParser::KeyParser(StringTokener& tokener, const char* paramName) : 
    hasShiftPrefix(false), vk(0), isShifted(false),
    tokener(tokener), paramName(paramName)
{
}

bool KeyParser::operator ()()
{
    // read key param
    if (tokener.eof()) {
        std::cerr << "missing param '" << paramName << "', line " << tokener.LineNo() << std::endl;
        return false;
    }

    tokener >> param;

    // parse key
    return parseKey();
}

bool KeyParser::parseKey()
{
    // check for prefix '+' for shifted key
    const char* keytext = param.c_str();
    if (strlen(keytext) > 1 && keytext[0] == '+')
    {
        hasShiftPrefix = true;
        keytext++;
    }

    // keyname ?
    if (strlen(keytext) > 1)
    {
        auto foundit = LoLevelKbdFile::KeyNames().find(keytext);
        if (foundit == LoLevelKbdFile::KeyNames().end())
        {
            std::cerr << "unknown key [" << param << "], line " << tokener.LineNo() << std::endl;
            return false;
        }
        vk = foundit->second;
    }
    else
    {
        // just a character represnting the key 'w', '{' etc
        vk = VkKeyScanA(*keytext);
        if (vk == 0xFFFF)
        {
            std::cerr << "non valid key [" << param << "], line " << tokener.LineNo() << std::endl;
            return false;
        }
        isShifted = HasShiftBit(vk);
        vk = vk & 0xFF;
    }

    return true;
}


//--------------------------------


LoLevelKbdFile::LoLevelKbdFile()
{
}

bool LoLevelKbdFile::ReadKeyboardFile(const char* filename)
{
    // open file
    int lineNo = 0;
    std::string line;
    std::ifstream kbdfile(filename);

    if (!kbdfile)
    {
        std::cerr << "failed to open lo level kbd def file [" << filename << "]" << std::endl;
        return false;
    }

    // read file line by line
    while (!kbdfile.eof())
    {
        lineNo++;
        std::getline(kbdfile, line);

        StringTokener stringTokener(line, lineNo);

        // read the next command (or !comments)
        std::string cmd;
        stringTokener >> cmd;

        // skip comment / empty line
        if (stringTokener.eof() || cmd.c_str()[0] == '!') 
        {
            continue;
        }

        // validate line
        if (cmd == "k2k")
        {
            if (!doK2kCmd(stringTokener))
                return false;
        }
        else if (cmd == "k2ck")
        {
            if (!doK2kcCmd(stringTokener))
                return false;
        }
        else if (cmd == "K2K")
        {
            if (!doK2kWithShCmd(stringTokener))
                return false;
        }
        else if (cmd == "K2CK")
        {
            if (!doK2kcWithShCmd(stringTokener))
                return false;
        }
        else if (cmd == "addlayer")
        {
            if (!addLayer(stringTokener, false))
                return false;
        }
        else if (cmd == "addtogglelayer")
        {
            if (!addLayer(stringTokener, true))
                return false;
        }
        else if (cmd == "stickyer")
        {
            if (!setMakeSticky(stringTokener))
                return false;
        }
        else 
        {
            std::cerr << "expecting a command, line " << lineNo << std::endl;
            return false;
        }

    }

    return true;
}

bool LoLevelKbdFile::doK2kCmd(StringTokener& tokener)
{
    KeyParser keyFrom(tokener, "fromKey");
    KeyParser keyTo(tokener, "toKey");

    if (!keyFrom() || !keyTo())
        return false;

    KeyValue kfrom(keyFrom.vk, 0, keyFrom.hasShiftPrefix);
    KeyValue kto(keyTo.vk, 0, keyTo.hasShiftPrefix || keyTo.isShifted);

    // call DLL hook to add a new mapping
    return HookKbd::AddMapping(kfrom, kto);
}

bool LoLevelKbdFile::doK2kcCmd(StringTokener& tokener)
{
    KeyParser keyFrom(tokener, "fromKey");
    KeyParser keyTo(tokener, "toKey");

    if (!keyFrom() || !keyTo())
        return false;

    KeyValue kfrom(keyFrom.vk, 0, keyFrom.hasShiftPrefix  || keyFrom.isShifted);
    KeyValue kto(keyTo.vk, 0, keyTo.hasShiftPrefix || keyTo.isShifted);

    // call DLL hook to add a new mapping
    return HookKbd::AddCtrlMapping(kfrom, kto);
}

bool LoLevelKbdFile::doK2kWithShCmd(StringTokener& tokener)
{
    KeyParser keyFrom(tokener, "fromKey");
    KeyParser keyTo(tokener, "toKey");
    KeyParser keyToSh(tokener, "toKeyShift");

    if (!keyFrom() || !keyTo() || !keyToSh())
        return false;

    // from to, non shifted layer
    {
        KeyValue kfrom(keyFrom.vk, 0, false);
        KeyValue kto(keyTo.vk, 0, keyTo.hasShiftPrefix || keyTo.isShifted);

        // call DLL hook to add a new mapping
        if (!HookKbd::AddMapping(kfrom, kto))
            return false;
    }

    // from to, shifted layer
    {
        KeyValue kfrom(keyFrom.vk, 0, true);
        KeyValue kto(keyToSh.vk, 0, keyToSh.hasShiftPrefix || keyToSh.isShifted);

        // call DLL hook to add a new mapping
        if (!HookKbd::AddMapping(kfrom, kto))
            return false;
    }

    return true;
}

bool LoLevelKbdFile::doK2kcWithShCmd(StringTokener& tokener)
{
    KeyParser keyFrom(tokener, "fromKey");
    KeyParser keyTo(tokener, "toKey");
    KeyParser keyToSh(tokener, "toKeyShift");

    if (!keyFrom() || !keyTo() || !keyToSh())
        return false;

    // from to, non shifted layer
    {
        KeyValue kfrom(keyFrom.vk, 0, false);
        KeyValue kto(keyTo.vk, 0, keyTo.hasShiftPrefix || keyTo.isShifted);

        // call DLL hook to add a new mapping
        if (!HookKbd::AddCtrlMapping(kfrom, kto))
            return false;
    }

    // from to, shifted layer
    {
        KeyValue kfrom(keyFrom.vk, 0, true);
        KeyValue kto(keyToSh.vk, 0, keyToSh.hasShiftPrefix || keyToSh.isShifted);

        // call DLL hook to add a new mapping
        if (!HookKbd::AddCtrlMapping(kfrom, kto))
            return false;
    }

    return true;
}

bool LoLevelKbdFile::addLayer(StringTokener& tokener, bool isToggle)
{
    // read layer name
    if (tokener.eof()) {
        std::cerr << "layer name, line " << tokener.LineNo() << std::endl;
        return false;
    }

    std::string layerName;
    tokener >> layerName;
    if (layerName == "main")
    {
        std::cerr << "'main' is a reserved layer name, line " << tokener.LineNo() << std::endl;
        return false;
    }

    // create layer
    Layer::Idx_t layerIdx = 0;
    if (!HookKbd::AddLayer(layerName.c_str(), layerIdx))
    {
        std::cerr << "failed to create layer '" << layerName << "', line " << tokener.LineNo() << std::endl;
        return false;
    }

    // read access key
    KeyParser accessKey(tokener, "layer access key");
    if (!accessKey())
        return false;

    // set access key
    if (!HookKbd::SetLayerAccessKey(layerName.c_str(), KeyDef(accessKey.vk, 0), isToggle))
    {
        std::cerr << "failed to set acccess key for layer '" << layerName << "', line " << tokener.LineNo() << std::endl;
        return false;
    }

    // switch to new layer
    if (!HookKbd::GotoLayer(layerIdx))
    {
        std::cerr << "failed to switch to new layer '" << layerName << "', line " << tokener.LineNo() << std::endl;
        return false;
    }

    return true;
}

bool LoLevelKbdFile::setMakeSticky(StringTokener& tokener)
{
    // read makeSticky key
    KeyParser makeSticky(tokener, "make sticky key");
    if (!makeSticky())
        return false;

    // assign it
    KeyValue honey(makeSticky.vk, 0, false);
    return HookKbd::AddStickyMapping(honey);
}