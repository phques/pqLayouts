#pragma once


// Combos for Carbyne (PQangle-f VMirrVowP UO / PQortho-f VMirrVowP UO)


// these will be parsed to populate combos 
static StringPairList txtCombos = {
    {"th", "th"},
    {"nr", "ch"},
    {"wh", "wh"},
    {"sn", "sh"},
    {"pd", "ph"},
    {"g.", "gh"},
    {"snr", "sch"},

    {"it", "tion"},
    {"in", "ing"},

    {"ic", "I "},
    {"id", "I'd "},
    {"il", "I'll "},
    {"im", "I'm "},

    {"yd", "you'd "},
    {"yl", "you'll "},
    {"yf", "you're "},
    {"yv", "you've "},
    {"yr", "your "},

    {"tn", "their "},
    {"tf", "they're "},
    {"tw", "they "},
    {"tl", "they'll "},
    {"td", "they'd "},

    {"vr", "where "},
    {"v'", "we're "},
    {"vm", "we'd "},
    {"vh", "we'll "},
    {"v/", "we've "},

    {"hm", "here "},
    {"tr", "there "},

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

// the trigger keys are 'physical' / qwerty, no remapping done
static StringPairList txtKeysCombosQwerty = {
    {"qw", "Tab"},  // HD_LT4, HD_LT3     // tab
    {"qwe","+Tab"}, // HD_LT4, HD_LT3, HD_LT2  // Shift-tab
    {"fg", "SP"},   // HD_LM1, HD_LM0     // SPACE
    {"vb", "CR"},   // HD_LB0, HD_LB1     // ENTER
    {"nm", "CR"},   // PQ HD_RB0, HD_RB1  // ENTER on left hand

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
};

// the trigger keys are positional / qwerty, no remapping done
// the outputs are commands
static StringPairList txtCmdCombosQwerty = {
    { "./", "CapsWord"},    // ./ HD_RB3, HD_RB4 => capsWord: capsWord
    { "m/", "CamelCase"},   // m/ HD_RB1, HD_RB4 => capsWord: camelCase
    { "zb", "SelectWord"},  // zb HD_LB4, HD_LB0 => select word 
};
