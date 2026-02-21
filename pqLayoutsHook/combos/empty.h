#pragma once

// hard coded combos for Enthium
// VeeKeeVector must be sorted!
// These are sent with our SendString, which will capitalize the 1st char if alpha & Shift is down 
static std::map<VeeKeeVector, const char*> combos = {
};

// sent directly w. SendVk, so to support, for ex, shift-(ctrl-z),
// we will need to add in the Shift in the KeyValue when Shift is down
// (nb: normal 'KeyOut' key entries will have the shift=true for entries on the shift layer !)
// VeeKeeVector must be sorted!
static std::map<VeeKeeVector, std::vector<KeyValue>> combos2 = {
};