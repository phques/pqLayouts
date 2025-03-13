#pragma once

//##PQ todo hard coded as test, to go with HD neu-C layout. 

static std::map<VeeKeeVector, const char*> adapts3 = {
    // adaptives
    { {'E','Q','E'}, "\b\blml"},   // mwm -> lml (calmly)
};

static std::map<VeeKeeVector, const char*> adapts2 = {
// adaptives

    // left hand
    { {'E','T'}, "l"},      // mv -> ml
    { {'R','E'}, "l"},      // pm -> pl
    { {'W','E'}, "l"},      // fm -> fl
    { {'Q','E'}, "\blm"},   // wm -> lm
    { {'Q','R'}, "\blml"},  // wp -> lml (calmly)
    { {'Q','W'}, "s"},      // wf -> ws
    { {'T','R'}, "l"},      // vp -> vl
    { {'G','F'}, "l"},      // bt -> bl
    { {'V','F'}, "ht"},     // gt -> ght

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


    // use 'z' (qwerty Y) as 'magic adaptive key' 
    // \ = VK_OEM_5
    { {'Z','Y'}, "\bphilippe.quesnel"},  // P
    { {'L','Y'}, "\bintegration\\"},     // I
    { {'P','Y'}, "\bbig"},               // G 'adaptive', b_g is hard, (big) the hardest?
    { {'A','Y'}, "\bselect * from "},    // S

    // 'combos' done as adaptives until we have actual combos
    { {'L',VK_OEM_1}, "\bI "}, //ic -> "I " (bad idea?)
    { {'U','Y'}, "\b@" },
    { {'I','U'}, "\b!" },
    { {'I','O'}, "\b?" },
    { {'O','I'}, "\b/" }, // since '/' is on qwerty T, for progming twas nice on I in HD
    { {'U','O'}, "\b:" }, //not requied with Carbyne (?)
    { {VK_OEM_COMMA,
       VK_OEM_PERIOD}, "\b="},
    { {'M', VK_OEM_PERIOD}, "\b_"},
    { {'W', 'R'}, "\bqu"},

    { {'U','I'}, "\b.com"},
    { {'Y','U'}, "\bgmail"},
    { {'U','P'}, "\bcgi"},
};
