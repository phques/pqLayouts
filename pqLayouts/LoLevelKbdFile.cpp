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

#include <algorithm>

#include "pqLayoutsHook.h"
#include "../StaticLib1/util.h"
#include "KeyOutAction.h"
#include "Chord.h"


std::map<std::string, WORD> LoLevelKbdFile::keyNames = {
    {"CAPS", VK_CAPITAL},
    {"ESC", VK_ESCAPE},
    {"TAB", VK_TAB},
    {"SPACE", VK_SPACE},            {"SP", VK_SPACE},
    {"ENTER", VK_RETURN},           {"CR", VK_RETURN},
    {"BACKSPACE", VK_BACK},         {"BS", VK_BACK},
    {"DEL", VK_DELETE},
    {"LSHIFT", VK_LSHIFT},          {"LSH", VK_LSHIFT},
    {"RSHIFT", VK_RSHIFT},          {"RSH", VK_RSHIFT},
    {"LCONTROL", VK_LCONTROL},      {"LCTRL", VK_LCONTROL},
    {"RCONTROL", VK_RCONTROL},      {"RCTRL", VK_RCONTROL},
    {"LALT", VK_LMENU},
    {"RALT", VK_RMENU},
    {"LWIN", VK_LWIN},
    {"RWIN", VK_RWIN},
    {"LEFT", VK_LEFT},
    {"RIGHT", VK_RIGHT},
    {"UP", VK_UP},
    {"DOWN", VK_DOWN},
    {"HOME", VK_HOME},
    {"END", VK_END},
    {"PGUP", VK_PRIOR},
    {"PGDN", VK_NEXT},
    {"INS", VK_INSERT},
    {"APPS", VK_APPS},
    {"F1", VK_F1},  {"F2", VK_F2},  {"F3", VK_F3},  {"F4", VK_F4},  {"F5", VK_F5}, {"F6", VK_F6},
    {"F7", VK_F7},  {"F8", VK_F8},  {"F9", VK_F9},  {"F10", VK_F10},  {"F11", VK_F11}, {"F12", VK_F12},

};



//--------------


StringTokener::StringTokener(const std::string& line, int lineNo) : lineNo(lineNo)
{
    str(line);
}

StringTokener::StringTokener(File& f) : StringTokener(f.line, f.lineNo)
{
}

bool StringTokener::ReadParam(const char* paramName, std::string& param)
{
    if (eof()) {
        std::cerr << paramName << ", line " << LineNo() << std::endl;
        return false;
    }

    *this >> param;
    return true;
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
bool KeyParser::GetKeys(std::list<KeyValue> & keys, std::vector<char>& stenoChars)
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
    const auto vk = LoLevelKbdFile::LookupKeyName(token);
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
    return KeyValue(vk, 0, isShifted || hasShiftPrefix, hasControlPrefix);
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
        default:
            stop = true;
            break;
        }
    }


    // keyname ?
    if (strlen(keytext) > 1)
    {
        vk = LoLevelKbdFile::LookupKeyName(keytext);
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
    // get VK value of '-'
    bool isShifted;
    VkUtil::CharToVk('-', hyphenVk, isShifted);
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

    std::cout << "reading from file [" << filename << "]" << std::endl;

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
            if (!doSteakPower(kbdfile))
                return false;
        }
        else if (cmd == "steaks")
        {
            if (!doSteaks(kbdfile))
                return false;
        }
        else if (cmd == "lpsteaks")
        {
            if (!doLpSteaks(stringTokener))
                return false;
        }
        //else if (cmd == "kord")
        //{
        //    if (!doKord(stringTokener))
        //        return false;
        //}
        else if (cmd == "include")
        {
            if (!doInclude(stringTokener, filename))
                return false;

            std::cout << "back to file [" << filename << "]" << std::endl;
        }
        else 
        {
            std::cerr << "expecting a command, line " << kbdfile.lineNo << std::endl;
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

/* Read qwerty/steno keys for chording

! order is important, left hand, left vowels, stars, right vowelsm right hand
! stars separate left / right hand !
steakpower 
 SSTKPWHR AO ** ** EU FRPBLGTS
 QAWSEDRF CV TG UJ M, IKOLP;[' 

*/
bool LoLevelKbdFile::doSteakPower(File& kbdfile)
{
    std::list<KeyValue> stenoKeys;
    std::list<KeyValue> qwertyKeys;
    std::vector<char> stenoChars;
    std::vector<char> qwertyChars; // unused

    // 1st read steno keys
    if (!readKeysList(kbdfile, "steno chording keys", stenoKeys, stenoChars ))
        return false;
    
    // 2nd read qwerty keys
    if (!readKeysList(kbdfile, "qwerty chording keys", qwertyKeys, qwertyChars ))
        return false;

    // check nbr of keys
    if (stenoKeys.size() != qwertyKeys.size())
    {
        std::cerr << "nbr of steno and qwerty keys don't match, line " << kbdfile.lineNo-2 << std::endl;
        return false;
    }

    // parse qwerty keys / steno keys to create the steno keys mapping
    auto stenoCharsIt = stenoChars.begin();
    auto stenoKeysIt = stenoKeys.begin();
    auto qwertyKeysIt = qwertyKeys.begin();
    bool leftHandDone = false;
    ChordingKeys chordingKeys;

    for (; stenoKeysIt != stenoKeys.end(); ++stenoKeysIt, ++qwertyKeysIt, ++stenoCharsIt)
    {
        // detect left vs right hand, 
        // stars are defined in the middle, separating left / right hands
        bool isStar = (*stenoCharsIt == '*');
        if (isStar)
            leftHandDone = true;

        // save steno key mapping
        ChordingKey key;
        key.handId = isStar ? HandId::HAND_STARS : 
                    (leftHandDone ? HandId::HAND_RIGHT : HandId::HAND_LEFT);
        key.qwerty = qwertyKeysIt->Vk();
        key.steno = stenoKeysIt->Vk();
        key.stenoChar = *stenoCharsIt;
        key.stenoOrder = 0;  // will be setup in chording.init

        chordingKeys.push_back(key);
    }

    chording.Init(chordingKeys);
    return HookKbd::InitChordingKeys(chordingKeys);
}

bool LoLevelKbdFile::readKeysList(File& kbdfile, const char* paramName, std::list<KeyValue>& stenoKeys, std::vector<char>& stenoChars)
{
    // read next line
    if (kbdfile.f.eof())
    {
        std::cerr
            << "unexpected end of file, expecting keys list, line "
            << kbdfile.lineNo << std::endl;
        return false;
    }

    kbdfile.GetLine();
    StringTokener tokener(kbdfile);
    KeyParser keyParser(tokener, paramName);

    return keyParser.GetKeys(stenoKeys, stenoChars);
}

#if 0
bool LoLevelKbdFile::doKord(StringTokener& tokener)
{
    // the output of the chord
    KeyParser chordOutput(tokener, "chord definition");
    if (!chordOutput())
        return false;

    KeyValue outKey = chordOutput.ToKeyValue();

    // now build the chord / read the keys of the chord
    Kord chord;
    KeyActions::KeyActionPair actions = parseChordValue(tokener, chordOutput, chord);
    if (actions == KeyActions::nullActionPair)
        return false;

    return HookKbd::AddChord(chord, actions);

}
#endif // 0

// reads list of chord definitions 'steaks'
// end of list = 'endsteaks'
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
        if (chordDef.token == "endsteaks")
            return true;

        // skip empty line / comment
        if (tokener.eof() || chordDef.token == "!!")
            continue;

        // parse the chord keys definition
        Kord chord;
        if (!parseChordValue(tokener, chordDef, chord))
            return false;

        // parse the output key of the chord
        
        // actions for chord output
        std::list<KeyValue> keys;

        // read the next token
        if (!chordDef.ReadFromTokener())
            return false;

        // Try as a simple one key, supports +^ prefixes
        if (chordDef.ParseKey(false))
        {
            KeyValue outKey = chordDef.ToKeyValue();
            keys.push_back(outKey);
        }
        else
        {
            // read a list of keys
            std::vector<char> chars;
            if (!chordDef.GetKeysFromToken(keys, chars))
                return false;
        }

        // create actions for chord output from list of keys
        std::list<KeyActions::KeyActionPair> actionPairs;
        for (auto& key : keys)
        {
            KeyActions::KeyActionPair actionPair(KeyActions::nullActionPair);
            SetChordKeyActionPair(key, actionPair);

            actionPairs.push_back(actionPair);
        }

        chord.SetDisplay(chording.ToString(chord));
        //Printf("chord %s\n", chodr.GetDisplay().c_str());

        if (!HookKbd::AddChord(chord, actionPairs))
            return false;
    }

}

bool LoLevelKbdFile::doLpSteaks(StringTokener& tokener)
{
    std::string layerName1, layerName2, prefix1, prefix2;
    if (!tokener.ReadParam("layerName1", layerName1) ||
        !tokener.ReadParam("layerName2", layerName2) ||
        !tokener.ReadParam("left hand prefix1", prefix1) ||
        !tokener.ReadParam("left hand prefix2", prefix2))
    {
        return false;
    }

    HookKbd::SetLeftHandPrefix(layerName1, layerName2, prefix1, prefix2);
    return true;
}


void LoLevelKbdFile::SetChordKeyActionPair(const KeyValue& outKey, KeyActions::KeyActionPair& actionPair)
{
    // create key actions for this chord
    KeyDef inKey(0, 0);

    // shifted output 
    KeyValue outKeySh = outKey;
    outKeySh.Shift(true);

    actionPair.first = new KeyActions::KeyOutAction(inKey, outKey);
    actionPair.second = new KeyActions::KeyOutAction(inKey, outKeySh);
}

// reads the chord itself ("P-P")
bool LoLevelKbdFile::parseChordValue(StringTokener& tokener, KeyParser& chordOutputParser, Kord& chord)
{
    // build the chord / read the keys of the chord
    std::list<KeyValue> chordKeys;
    std::vector<char> keysChars;
    if (!chordOutputParser.GetKeysFromToken(chordKeys, keysChars))
        return false;

    bool leftHandDone = false;

    // place chords keys into chord definition
    for (auto& key : chordKeys)
    {
        VeeKee vk = key.Vk();

        // detect left vs right hand
        if (!leftHandDone && chording.IsHandDelimiterKey(vk))
        {
            leftHandDone = true;
        }

        // '-' is not part of the chord !
        if (vk == hyphenVk)
            continue;

        // use steno order to define chord
        if (leftHandDone)
        {
            vk = chording.AdjustForRightHand(vk);
        }

        const ChordingKey* chordingKey = chording.GetChordingKeyFromSteno(vk);
        if (chordingKey != nullptr)
        {
            chord.AddInChordKey(chordingKey->stenoOrder);
        }
    }

    return true;
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

    if (!GetRelativeFilePath(pcScriptFilename, imageFilename, fullImagePath)) 
    {
        std::cerr << "failed to get image file path" << std::endl;
        return false;
    }

    HookKbd::SetImageFilename(fullImagePath);
    return true;
}

bool LoLevelKbdFile::GetRelativeFilePath(const char* pcScriptFilename, std::string& otherFilename, WCHAR  fullOtherPath[MAX_PATH])
{
    // calc complete path of the kbd def file we are reading
    DWORD  retval = 0;
    WCHAR  scriptPath[MAX_PATH] = TEXT("");
    WCHAR  fullScriptPath[MAX_PATH] = TEXT("");
    WCHAR* lppPart = { NULL };

    // get complete path of this script
    std::string scriptName(pcScriptFilename);
    std::wstring wscriptName = std::wstring(scriptName.begin(), scriptName.end());
    std::copy(wscriptName.begin(), wscriptName.end(), scriptPath);

    retval = GetFullPathNameW(scriptPath,
        MAX_PATH,
        fullScriptPath,
        &lppPart);

    if (retval == 0 || retval >= MAX_PATH)
        return false;

    // replace the script filename with the other filename

    // remove script filename, to get directory
    if (!PathRemoveFileSpecW(fullScriptPath))
        return false;

    // add other filename to directory
    std::wstring wotherFilename = std::wstring(otherFilename.begin(), otherFilename.end());

    HRESULT res = PathCchCombine(fullOtherPath, MAX_PATH, fullScriptPath, wotherFilename.c_str());
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

// read / process from an include file
// ## warning: no check for recursive includes, will go into infinite loop if it happens
bool LoLevelKbdFile::doInclude(StringTokener& tokener, const char * pcScriptFilename)
{
    // read include filename
    if (tokener.eof()) {
        std::cerr << "include filename, line " << tokener.LineNo() << std::endl;
        return false;
    }

    std::string includeFilename;
    tokener >> includeFilename;

    // calculate te path of the image file
    // image will be in same directory as this script
    WCHAR  fullIncludePath[MAX_PATH]=TEXT(""); 

    if (!GetRelativeFilePath(pcScriptFilename, includeFilename, fullIncludePath)) 
    {
        std::cerr << "failed to get include file path" << std::endl;
        return false;
    }

    // convert to non-wide string
    // better way to do this ???
    //std::string otherFilename = std::string(fullIncludePath, fullIncludePath + wcslen(fullIncludePath));
    //char* nstring = (char*)_malloca(wcslen(fullIncludePath) * 2 + 4);
    WCHAR* wptr = fullIncludePath;
    char nstring[MAX_PATH*2+2] = {0};
    for (char* pstr = nstring; *wptr; )
    {
        *pstr++ = (char)*wptr++;
    }

    // recursively call ReadKeyboardFile!
    return ReadKeyboardFile(nstring);
}

 WORD LoLevelKbdFile::LookupKeyName(const std::string& keyText) 
{
    // make uppercase versino of the string
    std::string keyStr(keyText);
    std::transform(keyStr.begin(), keyStr.end(), keyStr.begin(),
                   [](const unsigned char c) { return std::toupper(c); });

    // lookup in key names
    const auto foundIt = keyNames.find(keyStr);

    return foundIt == keyNames.end() ? 0 : foundIt->second;
}
