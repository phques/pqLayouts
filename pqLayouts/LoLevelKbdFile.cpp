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

std::string LoLevelKbdFile::steakPower("SSTKPWHRAOEUFRPBLGTSDZ");


//--------------


StringTokener::StringTokener(const std::string& line, int lineNo) : lineNo(lineNo)
{
    str(line);
}

StringTokener::StringTokener(File& f) : StringTokener(f.line, f.lineNo)
{
}


//--------------


KeyParser::KeyParser(StringTokener& tokener, const char* paramName) : 
    hasShiftPrefix(false), hasControlPrefix(false),
    vk(0), isShifted(false),
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

    tokener >> param;
    return true;
}

bool KeyParser::operator ()()
{
    // read key param
    if (!ReadFromTokener())
        return false;

    // parse key
    return ParseKey();
}

// reads multiple keys from current line (from tokener), no modifiers on keys
// can be space separated or as a string:  A S D F tab QWERTY  
// comments are done with "!!" and end the line
// read keys are *added* to param 'keys'
bool KeyParser::GetKeys(std::list<VeeKee>& keys)
{

    while (!tokener.eof())
    {
        std::string tok;
        tokener >> tok;

        // rest of line is comment 
        if (tok == "!!")
            break;

        // could be a known key name
        auto foundit = LoLevelKbdFile::KeyNames().find(tok);
        if (foundit != LoLevelKbdFile::KeyNames().end())
        {
            keys.push_back(foundit->second);
        }
        else
        {
            // either a single key represented by its character 'a' or a chain of characters 'asdf'
            // add each to the chord definition
            for (auto* ptr = tok.c_str(); *ptr; ++ptr)
            {
                WORD vk = 0;
                bool isShifted = false;

                if (!VkUtil::CharToVk(*ptr, vk, isShifted))
                {
                    std::cerr << "non valid key [" << param << "], line " << tokener.LineNo() << std::endl;
                    return false;
                }

                keys.push_back(vk);
            }
        }
    }

    return true;
}

KeyValue KeyParser::ToKeyValue() const
{
    return KeyValue(vk, 0, isShifted || hasShiftPrefix, hasControlPrefix);
}

bool KeyParser::ParseKey()
{
    // check for prefix '+' for shifted key
    const char* keytext = param.c_str();

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
        default:
            stop = true;
            break;
        }
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

//----

void File::GetLine()
{
    lineNo++;
    std::getline(f, line);
}


//--------------------------------


LoLevelKbdFile::LoLevelKbdFile()
{
}

bool LoLevelKbdFile::ReadKeyboardFile(const char* filename)
{
    // open file
    std::ifstream f(filename);

    if (!f)
    {
        std::cerr << "failed to open lo level kbd def file [" << filename << "]" << std::endl;

        std::string a(filename);
        std::wstring b(a.begin(), a.end());

        MessageBoxW(NULL, b.c_str(), L"failed to open lo level kbd def file", MB_OK);
        return false;
    }

    // read file line by line
    File kbdfile(f);

    while (!kbdfile.f.eof())
    {
        kbdfile.GetLine();

        StringTokener stringTokener(kbdfile);

        // read the next command (or !comments)
        std::string cmd;
        stringTokener >> cmd;

        // skip comment / empty line
        if (cmd.empty() || cmd.c_str()[0] == '!') 
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
        else if (cmd == "steakpower")
        {
            if (!doSteakPower(kbdfile, stringTokener))
                return false;
        }
        else if (cmd == "steakstars")
        {
            if (!doSteakStars(stringTokener))
                return false;
        }
        else if (cmd == "steaks")
        {
            if (!doSteaks(kbdfile))
                return false;
        }
        else if (cmd == "kord")
        {
            if (!doKord(stringTokener))
                return false;
        }
        else 
        {
            std::cerr << "expecting a command, line " << kbdfile.lineNo << std::endl;
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
    KeyValue kto = keyTo.ToKeyValue();

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
    KeyValue kto = keyTo.ToKeyValue();

    // call DLL hook to add a new mapping
    return HookKbd::AddMapping(kfrom, kto);
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
        KeyValue kto = keyTo.ToKeyValue();

        // call DLL hook to add a new mapping
        if (!HookKbd::AddMapping(kfrom, kto))
            return false;
    }

    // from to, shifted layer
    {
        KeyValue kfrom(keyFrom.vk, 0, true);
        KeyValue kto = keyToSh.ToKeyValue();

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
        KeyValue kto = keyTo.ToKeyValue().Control(true);

        // call DLL hook to add a new mapping
        if (!HookKbd::AddMapping(kfrom, kto))
            return false;
    }

    // from to, shifted layer
    {
        KeyValue kfrom(keyFrom.vk, 0, true);
        KeyValue kto = keyToSh.ToKeyValue().Control(true);

        // call DLL hook to add a new mapping
        if (!HookKbd::AddMapping(kfrom, kto))
            return false;
    }

    return true;
}

bool LoLevelKbdFile::doSteakPower(File& kbdfile, StringTokener& tokener)
{
    // read the (steno) chord keys mappings
    // map steno keys SSTKPWHRAO.. .. to qwerty kbd keys
    stenoKbdMap.clear();

    KeyParser chordKeysParser(tokener, "steno chord keys mapping");

    // read the qwerty keys
    std::list<VeeKee> stenoQwertyKeys;
    if (!chordKeysParser.GetKeys(stenoQwertyKeys))
        return false;

    // do we have enough qwerty keys to define the steno kbd?
    if (stenoQwertyKeys.size() < steakPower.size())
    {
        std::cerr << "not enough keys for steno chord mapping, line " << tokener.LineNo() << std::endl;
        return false;
    }

    // parse qwerty keys / steno keys to create the steno keys mapping
    auto stenoQwertyKeyIt = stenoQwertyKeys.begin();
    auto steakPowerIt = steakPower.begin();
    std::string prefix;

    for (; steakPowerIt != steakPower.end(); ++steakPowerIt, ++stenoQwertyKeyIt)
    {
        // right hand steno keys use a '-' prefix : "-F"
        if (*steakPowerIt == 'F')
            prefix = "-";

        // save steno key mapping
        auto stenoKey = prefix + *steakPowerIt;
        stenoKbdMap[stenoKey] = *stenoQwertyKeyIt;
    }

    return true;
}

bool LoLevelKbdFile::doSteakStars(StringTokener& tokener)
{
    KeyParser chordStarsParser(tokener, "chord key stars");

    std::list<VeeKee> chordStarKeys;
    if (!chordStarsParser.GetKeys(chordStarKeys))
        return false;

    // save keys in a vk set and save info
    VeeKeeSet vkset(chordStarKeys.begin(), chordStarKeys.end());
    HookKbd::ChordStars(vkset);

    return true;
}

bool LoLevelKbdFile::doKord(StringTokener& tokener)
{
    // the output of the chord
    KeyParser chordOutput(tokener, "chord definition");
    if (!chordOutput())
        return false;
    
    // now build the chord / read the keys of the chord
    return parseChordValue(tokener, chordOutput);
}

bool LoLevelKbdFile::parseChordValue(StringTokener& tokener, KeyParser& chordOutput)
{
    // build the chord / read the keys of the chord
    std::list<VeeKee> chordKeys;
    if (!chordOutput.GetKeys(chordKeys))
        return false;

    // place chords keys into chord definition
    Kord chord;
    for (auto key : chordKeys)
        chord.AddInChordKey(key);

    // register the chord with its output 'action'
    // create key action
    KeyDef inKey(0, 0);
    KeyValue outKey = chordOutput.ToKeyValue();
    auto action = new KeyActions::KeyOutAction(inKey, outKey);

    // save chord definition
    return HookKbd::AddChord(chord, action);
}

bool LoLevelKbdFile::doSteaks(File& file)
{
    // exit on eof or "endsteaks"
    while (true)
    {
        // read next line
        if (file.f.eof())
        {
            std::cerr << "unexpected end of file, expecting chord definition or 'endsteaks', line " << file.lineNo << std::endl;
            return false;
        }

        file.GetLine();
        StringTokener tokener(file);

        // chord def or "endsteaks"
        KeyParser chordDef(tokener, "chord definition");

        // read the next token
        if (!chordDef.ReadFromTokener())
            return false;

        // "endsteaks" indicates end of chords defintions
        if (chordDef.param == "endsteaks")
            return true;

        // skip empty line / comment
        if (tokener.eof() || chordDef.param.c_str()[0] == '!')
            continue;

        // parse the output key of the chord
        if (!chordDef.ParseKey())
            return false;

        // now complete the chord definition
        if (!parseChordValue(tokener, chordDef))
            return false;

    }

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

