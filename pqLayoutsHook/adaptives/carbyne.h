#pragma once

//##PQ todo hard coded as test, to go with Carbyne layout. combos based on HandsDown

static std::map<VeeKeeVector, const char*> adapts3 = {
    // adaptives
    { {'R','W','R'}, "\bpl"}, // mwm -> mpl (copied from HD PM)
};

static std::map<VeeKeeVector, const char*> adapts2 = {
    // adaptives
//{ {'R','W','R'}, "mpl"}, // mwm -> mpl (copied from HD PM)

//{ {'J','K'}, "u"}, // ae -> au

{ {'O','P'}, "\big"}, // ,g -> ig (for ver w BYOU on bottom / G on top)
{ {'P','O'}, "i"}, // g, -> gi (for ver w BYOU on bottom / G on top)
{ {'P','I'}, "o"}, // g' -> go (for ver w BYOU on bottom / G on top)
{ {'P',VK_OEM_4}, "th"}, // gj -> gth (for ver w BYOU on bottom / G on top)

{ {'Q','E'}, "\bph"}, // vh -> ph
{ {'Q','W'}, "s"}, // vw -> vs
{ {'W','Q'}, "n"}, // wv -> wn 

{ {'E','R'}, "\blm"}, // hm -> lm

{ {'R','W'}, "p"}, // mw -> mp

{ {'E','Q'}, "\blv"}, // hv -> lv

//{ {'X','Z'}, "s"}, // lp -> ls (not as good a tradeOff, but still better)
{ {'F','S'}, "l"}, // tn -> tl b
{ {'A','S'}, "p"}, // sn -> sp
{ {'X','S'}, "\bps"}, // ln -> ps !! (actually parallel to each other)

// taken & adapted from HD PM !
{ {'R','G'}, "\blk"}, // MK -> LK
{ {'F','G'}, "\bnk"}, // TK -> NK
{ {'G','F'}, "n"}, // KT -> KN

{ {'S','D'}, "g"}, // NR -> ng

// use 'q' (qwerty Y) as 'magic adaptive key' 
// \ = VK_OEM_5
{ {'Z','Y'}, "\bphilippe.quesnel"},  // P
{ {'L','Y'}, "\bintegration\\"},     // I
{ {'P','Y'}, "\bbig"},               // G 'adaptive', b_g is hard, (big) the hardest?
{ {'A','Y'}, "\bselect * from "},    // S

// 'combos' done as adaptives until we have actual combos
//{ {'L',VK_OEM_1}, "\bI "}, //ic -> "I " (bad idea?)
//{ {'U','Y'}, "\b@" },
{ {'I','U'}, "\b!" },
{ {'I','O'}, "\b?" },
{ {'O','I'}, "\b/" }, // since '/' is on qwerty T, for progming twas nice on I in HD
//{ {'U','O'}, "\b:" }, //not requied with Carbyne (?)
//{ {VK_OEM_COMMA,
//   VK_OEM_PERIOD}, "\b="},
//{ {'M', VK_OEM_PERIOD}, "\b_"},
//{ {'W', 'R'}, "\bqu"},

//{ {'U','I'}, "\b.com"},
{ {'Y','U'}, "\bgmail"},
{ {'U','Y'}, "\bcgi"},
};
