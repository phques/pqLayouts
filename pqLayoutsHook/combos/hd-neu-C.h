#pragma once

// hard coded combos for HD neuC
// VeeKeeVector must be sorted!
// These are sent with our SendString, which will capitalize the 1st char if alpha & Shift is down 
static std::map<VeeKeeVector, const char*> combos = {
    { {'R','W'}, "qu"},
    { {'F','S'}, "z"},
    { {'V','X'}, "x"},

    { {'U','Y'}, "@"},
    { {'I','U'}, ".com"},

    { {'O','U'}, ":" },
    { {'I','O'}, ";" },
    //{ {'I','O'}, "/" },

    { {VK_OEM_COMMA, VK_OEM_PERIOD}, "="},
    { {'M', VK_OEM_PERIOD}, "_"},

    { {'L',VK_OEM_1}, "I "},    // ih -> "I " 
    { {'L','X'}, "I'll"},      // il -> "I'll" 
    { {'E','L'}, "I'm"},       // im -> "I'm" 
    { {'X',VK_OEM_PERIOD}, "you'll"},   // yl -> "you'll" 
    { {'A',VK_OEM_PERIOD}, "you're"},   // yr -> "you're" 
    { {'R',VK_OEM_PERIOD}, "your"},     // yp -> "your" (P for 'possessive')
    { {'F','Q'}, "their"},              // tw -> their
    { {'F',VK_OEM_PERIOD}, "they"},     // ty -> they
    { {'F','W'}, "there"},      // tf -> there
    { {'A',VK_OEM_1}, "here"},  // hr -> here
    { {'F','P'}, "they're"},    // t' -> they're
    { {'Q',VK_OEM_1}, "where"}, // wh -> where
    { {'B','X'}, "we'll"},      // xl -> we'll
    { {'B','P'}, "we're"},      // x' -> we're

    { {'D','F'}, "th" },    // 
    { {'D','S'}, "ch" },    // 
    { {'Q','W'}, "wh" },    // 
    { {'A','S'}, "sh" },    // 
    { {'E','R'}, "ph" },    // 
    { {'B','V'}, "gh" },    // 
    { {'A','D'}, "sch"},    // 
    //{ {'A','D','S'}, "sch"},

    { {'F','L'}, "tion"},    // it -> tion
    { {'D','L'}, "ing"},    // in -> ing

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
    { {'K','L'}, {KeyValue(VK_BACK,0)} }, // BACK SPACE
    { {'R','T'}, {KeyValue(VK_TAB, 0, false, false, true)} }, // alt-tab (next window)
};