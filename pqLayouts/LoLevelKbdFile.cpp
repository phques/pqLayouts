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
#include "../StaticLib1/util.h"
#include "KeyOutAction.h"
#include "Chord.h"


std::map<std::string, WORD> LoLevelKbdFile::keyNames = {
    {"caps", VK_CAPITAL},
    {"esc", VK_ESCAPE},
    {"tab", VK_TAB},
    {"space", VK_SPACE},            {"sp", VK_SPACE},
    {"enter", VK_RETURN},           {"cr", VK_RETURN},
    {"backspace", VK_BACK},         {"bs", VK_BACK},
    {"del", VK_DELETE},
    {"lshift", VK_LSHIFT},          {"lsh", VK_LSHIFT},
    {"rshift", VK_RSHIFT},          {"rsh", VK_RSHIFT},
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
    {"apps", VK_APPS},
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
        // just a character representing the key 'w', '{' etc
        if (!VkUtil::CharToVk(*keytext, vk, isShifted))
        {
            std::cerr << "non valid key [" << param << "], line " << tokener.LineNo() << std::endl;
            return false;
        }
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

        std::string a(filename);
        std::wstring b(a.begin(), a.end());

        MessageBoxW(NULL, b.c_str(), L"failed to open lo level kbd def file", MB_OK);
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
        else if (cmd == "addlayertap")
        {
            if (!addLayer(stringTokener, true))
                return false;
        }
        else if (cmd == "stickyer")
        {
            if (!setMakeSticky(stringTokener))
                return false;
        }
        else if (cmd == "imagefile")
        {
            if (!setImageFile(stringTokener, filename))
                return false;
        }
        else if (cmd == "imageview")
        {
            if (!setImageView(stringTokener))
                return false;
        }
        else if (cmd == "kord")
        {
            if (!doKord(stringTokener))
                return false;
        }
        else 
        {
            std::cerr << "expecting a command, line " << lineNo << std::endl;
            return false;
        }

    }

    // always start on main layer
    HookKbd::GotoMainLayer();

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

bool LoLevelKbdFile::doKord(StringTokener& tokener)
{
    // the output of the chord
    KeyParser chordOutput(tokener, "chord outputKey");
    if (!chordOutput())
        return false;
    
    // now build the chord / read the keys of the chord
    Kord chord;

    while (!tokener.eof())
    {
        std::string tok;
        tokener >> tok;

        // could be a known key name
        auto foundit = LoLevelKbdFile::KeyNames().find(tok);
        if (foundit != LoLevelKbdFile::KeyNames().end())
        {
            chord.AddInChordKey(foundit->second);
        }
        else
        {
            // either a single key represented by its character 'a' or a chain of characters 'asdf'
            // add each to the chord definition
            for (auto* ptr = tok.c_str(); *ptr; ++ptr)
            {
                auto vk = VkKeyScanA(*ptr);
                chord.AddInChordKey(vk & 0xFF);
            }
        }
    }

    // register the chord with its output 'action'
    // create key action
    KeyDef inKey(0, 0);
    KeyValue outKey(chordOutput.vk, 0, chordOutput.isShifted || chordOutput.hasShiftPrefix);
    auto action = new KeyActions::KeyOutAction(inKey, outKey);

    // save chord definition
    return HookKbd::AddChord(chord, action);
}

bool LoLevelKbdFile::addLayer(StringTokener& tokener, bool toggleOnTap)
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
    if (!HookKbd::SetLayerAccessKey(layerName.c_str(), KeyDef(accessKey.vk, 0), toggleOnTap))
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

bool LoLevelKbdFile::setImageFile(StringTokener& tokener, const char * pcScriptFilename)
{
    // read image filename
    if (tokener.eof()) {
        std::cerr << "image filename, line " << tokener.LineNo() << std::endl;
        return false;
    }

    std::string imageFilename;
    tokener >> imageFilename;

    // calculate te path of the image file
    // image will be in same directory as this script
    WCHAR  fullImagePath[MAX_PATH]=TEXT(""); 

    if (!GetImageFilePath(pcScriptFilename, imageFilename, fullImagePath)) 
    {
        std::cerr << "failed to get image file path" << std::endl;
        return false;
    }

    HookKbd::SetImageFilename(fullImagePath);
    return true;
}

bool LoLevelKbdFile::GetImageFilePath(const char* pcScriptFilename, std::string& imageFilename, WCHAR  fullImagePath[260])
{
    // calc complete path of the kbd def file we are reading
    DWORD  retval = 0;
    WCHAR  scriptPath[MAX_PATH] = TEXT("");
    WCHAR  fullScriptPath[MAX_PATH] = TEXT("");
    WCHAR* lppPart = { NULL };

    // get complete path of thos script
    std::string scriptName(pcScriptFilename);
    std::wstring wscriptName = std::wstring(scriptName.begin(), scriptName.end());
    std::copy(wscriptName.begin(), wscriptName.end(), scriptPath);

    retval = GetFullPathNameW(scriptPath,
        MAX_PATH,
        fullScriptPath,
        &lppPart);

    if (retval == 0 || retval >= MAX_PATH)
        return false;

    // replace the script filename with the image filename

    // remove script filename, to get directory
    if (!PathRemoveFileSpecW(fullScriptPath))
        return false;

    // add image filename to directory
    std::wstring wImageFilename = std::wstring(imageFilename.begin(), imageFilename.end());

    HRESULT res = PathCchCombine(fullImagePath, MAX_PATH, fullScriptPath, wImageFilename.c_str());
    if (res != S_OK)
        return false;

    return true;
}

bool LoLevelKbdFile::setImageView(StringTokener& tokener)
{
    // -- read image view topY
    if (tokener.eof()) {
        std::cerr << "image view topY, line " << tokener.LineNo() << std::endl;
        return false;
    }

    int imageViewTopY;
    tokener >> imageViewTopY;


    // -- read image view bottomY
    if (tokener.eof()) {
        std::cerr << "image view bottomY, line " << tokener.LineNo() << std::endl;
        return false;
    }

    std::string imageViewBottomYStr;
    tokener >> imageViewBottomYStr;

    // "+20" is 20 of height
    int imageViewBottomY = 0;
    const char* ptr = imageViewBottomYStr.c_str();
    if (ptr[0] == '+')
        imageViewBottomY = imageViewTopY + atoi(ptr+1);
    else
        imageViewBottomY = atoi(ptr);


    // -- read image view topY shifted
    if (tokener.eof()) {
        std::cerr << "image view topY shifted, line " << tokener.LineNo() << std::endl;
        return false;
    }

    int imageViewShiftedTopY;
    tokener >> imageViewShiftedTopY;


    // -- read image view bottomY shifted
    if (tokener.eof()) {
        std::cerr << "image view bottomY shifted, line " << tokener.LineNo() << std::endl;
        return false;
    }

    std::string imageViewShiftedBottomYStr;
    tokener >> imageViewShiftedBottomYStr;

    int imageViewShiftedBottomY = 0;
    ptr = imageViewShiftedBottomYStr.c_str();
    if (ptr[0] == '+')
        imageViewShiftedBottomY = imageViewShiftedTopY + atoi(ptr+1);
    else
        imageViewShiftedBottomY = atoi(ptr);

    HookKbd::SetImageView(
        Layer::ImageView(imageViewTopY, imageViewBottomY),
        Layer::ImageView(imageViewShiftedTopY, imageViewShiftedBottomY));

    return true;
}

