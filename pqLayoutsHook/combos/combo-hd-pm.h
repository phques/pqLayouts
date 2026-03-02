#pragma once


// Combos definitions for HD PM


//(positional)
// sent directly w. SendVk, so to support, for ex, shift-(ctrl-z),
// we will need to add in the Shift in the KeyValue when Shift is down
// (nb: normal 'KeyOut' key entries will have the shift=true for entries on the shift layer !)
// VeeKeeVector must be sorted!
static std::map<VeeKeeVector, std::vector<KeyValue>> charsCombos = {
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

// these will be parsed to populate combos above
static std::list<std::pair<std::string, std::string>> txtCombos = {
    {"th", "th"},
    {"nt", "ch"},
    {"wg", "wh"},
    {"sn", "sh"},
    {"pl", "ph"},
    {"gm", "gh"},
    //snt sch

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

// these will be parsed to populate combos above, 
// the trigger keys are positional / qwerty, no remapping done
static std::list<std::pair<std::string, std::string>> txtCombosQwerty = {
    { "./", "\01a"},    // ./ => capsWord: capsWord
    { "m/", "\01b"},    // m/ => capsWord: camelCase
    { "zb", "\01c"},    // zb => select word HD_LB4, HD_LB0

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

//## PQ TODO (need to be able to parse "^C", etc)
// these will be parsed to populate charsCombos above, 
// the trigger keys are 'physical' / qwerty, no remapping done
static std::list< std::pair<std::string, std::string>> txtCharsCombos = {

};
