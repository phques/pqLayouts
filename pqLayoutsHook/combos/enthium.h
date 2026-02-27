#pragma once

// hard coded combos for Enthium
// VeeKeeVector must be sorted!
// These are sent with our SendString, which will capitalize the 1st char if alpha & Shift is down 
static std::map<VeeKeeVector, const char*> combos = {
    { {'R','W'}, "qu"},             // (positional, samespot as in HD neu)
    // { {'F','S'}, "z"},
    // { {'V','X'}, "x"},

    { {'U','Y'}, "@"},
    { {'I','U'}, ".com"},

    { {'O','U'}, ":" },
    { {'I','O'}, ";" },
    //{ {'I','O'}, "/" },

    { {VK_OEM_COMMA, VK_OEM_PERIOD}, "="}, //even though we have = on main layer
    { {'M', VK_OEM_PERIOD}, "_"},

    { {'A','S'}, "I "},             // ih -> "I " 
    { {'S','U'}, "I'll "},           // il -> "I'll" 
    { {'M','S'}, "I'm "},            // im -> "I'm" 
    
    { {'U','W'}, "you'll "},         // yl -> "you'll" 
    { {'F','W'}, "you're "},         // ye -> "you're" 
    { {'O','W'}, "your "},           // yp -> "your" (P for 'possessive')
    
    { {'F','J'}, "here "},           // he -> here
    { {'K','W'}, "they "},           // ty -> they
    { {'F','K'}, "there "},          // te -> there
    { {'K','O'}, "their "},          // tp -> their (P for 'possessive')
    { {'C','K'}, "they're "},        // t' -> they're
    { {'J','K'}, "this " },          // th ->this

    // Using S instead of W
    { {'J',VK_OEM_1}, "where "},     // sh -> where
    { {'U',VK_OEM_1}, "we'll "},     // sl -> we'll
    { {'C',VK_OEM_1}, "we're "},     // s' -> we're

    { {'K','L'}, "ch" },    // tn (positional, samespot as in HD neu, but on consonants hand)
    //{ {VK_OEM_1,VK_OEM_7}, "wh" },    // 
    //{ {'A','S'}, "sh" },    // 
    //{ {'E','R'}, "ph" },    // 
    { {'M',VK_OEM_COMMA}, "gh" },    // gm
    //{ {'A','D'}, "sch"},    // 
    //{ {'A','D','S'}, "sch"},

    { {'K','S'}, "tion"},    // it -> tion
    { {'L','S'}, "ing"},    // in -> ing

    // positional ./ m/ are qwerty
    { {VK_OEM_PERIOD, VK_OEM_2 }, "\01a"},  // ./ => capsWord: camelCase
    { {'M', VK_OEM_2}, "\01b"},             // m/ => capsWord: capsWord
};

// sent directly w. SendVk, so to support, for ex, shift-(ctrl-z),
// we will need to add in the Shift in the KeyValue when Shift is down
// (nb: normal 'KeyOut' key entries will have the shift=true for entries on the shift layer !)
// VeeKeeVector must be sorted!
static std::map<VeeKeeVector, std::vector<KeyValue>> combos2 = {
    //{ {'C','X','Z'}, {CtrlKeyValue('Y')} }, // ctrl-y redo
    { {'C','Z'}, {CtrlKeyValue('Y')} }, // ctrl-y redo
    { {'X','Z'}, {CtrlKeyValue('Z')} }, // ctrl-z (can use shift to do Redo)
    { {'C','X'}, {CtrlKeyValue('C')} }, // ctrl-c
    { {'C','V'}, {CtrlKeyValue('V')} }, // ctrl-v
    { {'H','Y'}, {CtrlKeyValue('W')} }, // ctrl-w (close, unfortunately not std, ditto ctrl-f4)
    { {'A','G'}, {CtrlKeyValue('F')} }, // ctrl-f (find)
    { {'V','Z'}, {CtrlKeyValue('A')} }, // ctrl-a (select all)
    { {'D','S'}, {KeyValue(VK_BACK,0)} }, // BACK SPACE 
    { {'R','T'}, {KeyValue(VK_TAB, 0, false, false, true)} }, // alt-tab (next window)
};