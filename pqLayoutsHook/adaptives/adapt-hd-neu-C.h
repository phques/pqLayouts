#pragma once

//##PQ todo hard coded as test, to go with HD neu-C layout. 

static std::list<std::pair<std::string, std::string>> txtAdaptives;

static std::map<VeeKeeVector, std::string> adapts3 = {
    // adaptives
    { {'E','Q','E'}, "\b\blml"},   // mwm -> lml (calmly)
};

static std::map<VeeKeeVector, std::string> adapts2 = {
// adaptives

    // left hand
    { {'E','T'}, "l"},      // mv -> ml
    { {'R','E'}, "l"},      // pm -> pl
    { {'R','T'}, "d"},      // pv -> pd
    { {'W','E'}, "l"},      // fm -> fl
    { {'Q','E'}, "\blm"},   // wm -> lm
    { {'Q','R'}, "\blml"},  // wp -> lml (calmly)
    { {'Q','W'}, "s"},      // wf -> ws
    { {'T','R'}, "l"},      // vp -> vl
    { {'G','F'}, "l"},      // bt -> bl
    { {'V','F'}, "ht"},     // gt -> ght
    { {'F','G'}, "\bmb"},   // tb -> mb
    { {'C','V'}, "v"},      // dg -> dv (actually 433: dg 15746 / 501: dv 10975, but since dv is faaar)

    // right/vowels hand
    { {'J','K'}, "u"},          // ae -> au

    // 'magic H'
    { {'M',VK_OEM_1}, "a"},     // uh -> ua
    { {'K',VK_OEM_1}, "o"},     // eh -> eo
    { {VK_OEM_COMMA,
        VK_OEM_1}, "\boe"},     // oh -> oe
    { {VK_OEM_PERIOD,
           VK_OEM_1}, "i"},     // yh -> yi
    // 'other' 'magic H'
    { {'X',VK_OEM_1}, "n"},     // lh -> ln
    { {'E',VK_OEM_1}, "n"},     // mh -> mn
    { {'D',VK_OEM_1}, "l"},     // nh -> nl

    { {VK_OEM_PERIOD,
           VK_OEM_2}, "i"},     // yk -> yi


    // use '/' (qwerty Y) as 'magic adaptive key' 
    { {'R','Y'}, "\bphilippe.quesnel"}, // P
    { {'D','Y'}, "\bintegration\\"},    // N

    { {'S','Y'}, "\bselect * from "},   // S

    { {'Z','Y'}, "\bcircuit"},          // C)ircuit
    { {'C','Y'}, "\bdevice"},           // D)evice
    { {'L','Y'}, "\bincident"},         // I)ncident
    { {'X','Y'}, "\blocation"},         // L)ocation
    { {'K','Y'}, "nergize"},            // E)nergize

    // 'combos' done as adaptives until we have actual combos
    //{ {'L',VK_OEM_1}, "\bI "}, //ic -> "I " (bad idea?)
    //{ {'U','Y'}, "\b@" },
    { {'I','U'}, "\b!" },
    { {'I','O'}, "\b?" },
    //{ {'O','I'}, "\b/" },
    //{ {'U','O'}, "\b:" },
    //{ {VK_OEM_COMMA,
    //   VK_OEM_PERIOD}, "\b="},
    //{ {'M', VK_OEM_PERIOD}, "\b_"},
    //{ {'W', 'R'}, "\bqu"},

    //{ {'U','I'}, "\b.com"},
    { {'Y','U'}, "\bgmail"},
    { {'U','Y'}, "\bcgi"},
};
