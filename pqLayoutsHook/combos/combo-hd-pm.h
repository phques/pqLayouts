#pragma once


// Combos definitions for HD PM


//(positional)
// sent directly w. SendVk, so to support, for ex, shift-(ctrl-z),
// we will need to add in the Shift in the KeyValue when Shift is down
// (nb: normal 'KeyOut' key entries will have the shift=true for entries on the shift layer !)
// VeeKeeVector must be sorted!
static std::map<VeeKeeVector, std::vector<KeyValue>> sendkeyCombos = {
    // HD_LB0, HD_LM0   new -- Since cmd/ctrl is on middle
    // HD_RB0, HD_RM0   open -- these must be 2 handed, so combo
    // HD_RT0, HD_RM0   close -- not strictly necessary?
    // HD_LT0, HD_LM0   quit -- because q is on symbol layer
    // HD_LM4, HD_LM0   find selection
    // HD_LB4, HD_LB1   select all
    // HD_LB4, HD_LB0   select word
    // HD_LB4, HD_LB3   undo
    // HD_LB4, HD_LB3, HD_LB2   redo
    // HD_LB3, HD_LB2   copy (hold for cut)
    // HD_LB2, HD_LB1   paste (hold for paste-match)

    { {'B','G'}, {CtrlKeyValue('N')} }, // ctrl-n HD_LB0, HD_LM0   new 
    { {'H','N'}, {CtrlKeyValue('O')} }, // ctrl-o HD_RB0, HD_RM0   open
    //{ {'C','X','Z'}, {CtrlKeyValue('Y')} }, // ctrl-y HD_LB4, HD_LB3, HD_LB2   redo
    { {'V','X'}, {CtrlKeyValue('Y')} }, // ctrl-y redo
    { {'X','Z'}, {CtrlKeyValue('Z')} }, // ctrl-z undo (can use shift to do Redo)
    { {'C','X'}, {CtrlKeyValue('C')} }, // ctrl-c copy
    { {'C','V'}, {CtrlKeyValue('V')} }, // ctrl-v paste
    { {'H','Y'}, {CtrlKeyValue('W')} }, // ctrl-w (close, unfortunately not std, ditto ctrl-f4)
    { {'A','G'}, {CtrlKeyValue('F')} }, // ctrl-f (find)
    { {'V','Z'}, {CtrlKeyValue('A')} }, // ctrl-a (select all)
    { {'K','L'}, {KeyValue(VK_BACK,0)} }, // BACK SPACE
    { {'R','T'}, {KeyValue(VK_TAB, 0, false, false, true)} }, // alt-tab (next window) HD_LT1, HD_LT0  // APPMENU on alpha layer
    // dup of "vm"  
    //HD_LM4, HD_LM1 // ESCape

};

// these will be parsed to populate combos 
static StringPairList txtCombos = {
    {"th", "th"},
    {"nt", "ch"},
    {"wg", "wh"},
    {"sn", "sh"},
    {"pl", "ph"},
    {"gm", "gh"},
    {"snt", "sch"},

    {"it", "tion"},

    {"ic", "I "},
    {"id", "I'd "},
    {"il", "I'll "},
    {"im", "I'm "},

    {"yd", "you'd "},
    {"yl", "you'll "},
    {"yf", "you're "},
    {"yv", "you've "},
    {"yr", "your "},

    {"tr", "their "},
    {"tf", "they're "},
    {"tw", "they "},
    {"tl", "they'll "},
    {"tp", "they'd "},

    {"vr", "where "},
    {"v'", "we're "},
    {"vg", "we'd "},
    {"vm", "we'll "},
    {"vj", "we've "},

    {"hf", "here "},
    {"tm", "there "},

};

// the trigger keys are positional / qwerty, no remapping done
static StringPairList txtCombosQwerty = {
    {"n.", "~"}, // HD_RB0, HD_RB3  ~ tilde (not the deadkey for eñye)
    {"m.", "_"}, // HD_RB1, HD_RB3  _ underscore
    {",.", "="}, // HD_RB2, HD_RB3  = equal (hold for %)HD_RM0, HD_RM1 // ; semicolon
    {"hj", ";"}, // HD_RT1, HD_RT3  ; semicolon
    {"uo", ":"}, // HD_RT1, HD_RT3  : colon
    {"ui", "!"}, // HD_RT1, HD_RT2  !
    {"io", "?"}, // HD_RT2, HD_RT3  ?
    {"ip", "$"}, // HD_RT2, HD_RT4  " DOUBLE QUOTE  // " hijacked for $
    {"op", "`"}, // HD_RT3, HD_RT4  ` tic (not dead key grave)
    {"yu", "@"}, // HD_RT0, HD_RT1  @
    {"wr", "qu"}, // HD_LT3, HD_LT1  TYPE "q" (Qu & Linger deletes u)
};

// the trigger keys are positional / qwerty, no remapping done
// the outputs are commands
static StringPairList txtCmdCombosQwerty = {
    { "./", "CapsWord"},    // ./ HD_RB3, HD_RB4 => capsWord: capsWord
    { "m/", "CamelCase"},   // m/ HD_RB1, HD_RB4 => capsWord: camelCase
    { "zb", "SelectWord"},  // zb HD_LB4, HD_LB0 => select word 
};

// the trigger keys are 'physical' / qwerty, no remapping done
static StringPairList txtKeysCombosQwerty = {
    {"gb", "^n"},   // HD_LB0, HD_LM0   new 
    {"hn", "^o"},   // HD_RB0, HD_RM0   open
    {"yh", "^w"},   // HD_RT0, HD_RM0   close
    {"tg", "@F4"},  // HD_LT0, HD_LM0   quit
    {"ag", "^f"},   // HD_LM4, HD_LM0   find selection
    {"zv", "^a"},   // HD_LB4, HD_LB1   select all
    {"zx", "^z"},   // HD_LB4, HD_LB3   undo
    {"zxc", "^y"},  // HD_LB4, HD_LB3, HD_LB2   redo
    {"xc", "^c"},   // HD_LB3, HD_LB2   copy (hold for cut)
    {"cv", "^v"},   // HD_LB2, HD_LB1   paste (hold for paste-match)
    {"rt", "@Tab"}, // HD_LT1, HD_LT0   alt-tab (next window) APPMENU 
    {"kl", "BS"},   // HD_RM2, HD_RM3   BACK SPACE
/*
    { {'B','G'}, {CtrlKeyValue('N')} }, // ctrl-n HD_LB0, HD_LM0   new
    { {'H','N'}, {CtrlKeyValue('O')} }, // ctrl-o HD_RB0, HD_RM0   open
    //{ {'C','X','Z'}, {CtrlKeyValue('Y')} }, // ctrl-y HD_LB4, HD_LB3, HD_LB2   redo
    { {'V','X'}, {CtrlKeyValue('Y')} }, // ctrl-y redo
    { {'X','Z'}, {CtrlKeyValue('Z')} }, // ctrl-z undo (can use shift to do Redo)
    { {'C','X'}, {CtrlKeyValue('C')} }, // ctrl-c copy
    { {'C','V'}, {CtrlKeyValue('V')} }, // ctrl-v paste
    { {'H','Y'}, {CtrlKeyValue('W')} }, // ctrl-w (close, unfortunately not std, ditto ctrl-f4)
    { {'A','G'}, {CtrlKeyValue('F')} }, // ctrl-f (find)
    { {'V','Z'}, {CtrlKeyValue('A')} }, // ctrl-a (select all)
    { {'K','L'}, {KeyValue(VK_BACK,0)} }, // BACK SPACE
    { {'R','T'}, {KeyValue(VK_TAB, 0, false, false, true)} }, // alt-tab (next window) HD_LT1, HD_LT0  // APPMENU on alpha layer

*/
};


