#pragma once

//##PQ todo hard coded as test, to go with Enthium layout. 

static std::map<VeeKeeVector, const char*> adapts3 = {
    // adaptives
    { {'M', VK_OEM_PERIOD, 'M'}, "\bpl"},   // mfm -> mpl
};

static std::map<VeeKeeVector, const char*> adapts2 = {
// adaptives
    { {'Z',VK_SNAPSHOT}, "\b, "},               // ,r  -> ,SP
    { {'X',VK_SNAPSHOT}, "\b. "},               // .r  -> .SP
    { {'W','Q'}, ","},                          //  yb -> y, 
    { {'W','R'}, "."},                          //  yu -> y. 
    { {VK_OEM_COMMA,'M'}, "l"},                 //  gm -> gl 
    { {'M',VK_OEM_COMMA}, "\blm"},              //  mg -> lm 
    { {'X','Z'}, "\bo,"},    //  ., -> o, 
    { {'Z','X'}, "\bo."},    //  ,. -> o. 

    // positional/qwerty, same as HD neu layout
    { {'I','U'}, "\b!" },   // IU !
    { {'I','O'}, "\b?" },   // IO ?

    // vowels, special case for AU
    { {'D','F'}, "u"},      // ae -> au

    // vowels, 'magic C'
    { {'R','A'}, "a"},   // uc -> ua
    { {'W','A'}, "i"},     // yc -> yi


    // use '/' 'j' (qwerty vn) as 'magic adaptive key'
    // nb: we use two keys in the same way as for shift keys: use opposite hand
    { {'O','V'}, "\bphilippe.quesnel"}, // P
    { {'K','V'}, "\bintegration\\"},    // T
    { {VK_OEM_1,'V'}, "\bselect * from "}, // S
    { {'A','V'}, "\bcircuit"},          // C)ircuit
    { {'I','V'}, "\bdevice"},           // D)evice
    { {'S','N'}, "\bincident"},         // I)ncident
    { {'U','V'}, "\blocation"},         // L)ocation
    { {'F','N'}, "nergize"},            // E)nergize

    { {'Y','U'}, "\bgmail"},
    { {'U','Y'}, "\bcgi"},
};
