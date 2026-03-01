#pragma once

static std::map<VeeKeeExVector, std::string> adapts3 = {
    // adaptives
};

static std::map<VeeKeeExVector, std::string> adapts2 = {
    // adaptives
};

static std::list<std::pair<const char*, const char*>> adaptives = {
{ "/\"", "?" },

{ "pd", "nd" },
{ "pf", "ps" },
{ "kg", "kl" },
{ "mg", "lg" },
{ "wmg", "lml" },
{ "jg", "jpg" },
{ "wg", "wd" },
{ "kh", "kn" },
{ "gj", "gth" },
{ "vj", "vl" },
{ "wj", "wl" },
{ "mk", "lk" },
{ "hk", "nk" },

// These will give us the opportunity to shift only the first letter when rolling
// So shift-pl  => Pl
{ "pl", "pl" },
{ "bl", "bl" },
{ "sl", "sl" },

{ "gm", "gl" },
{ "vm", "vl" },
{ "wm", "lm" },
{ "mwm", "mpl" },
{ "fp", "sp" },
{ "pp", "pg" },
{ "gv", "gt" },
{ "mv", "lv" },
{ "gw", "gd" },
{ "mw", "mp" },
{ "yb", "ib" },
{ "ae", "au" },
{ "./", ".com" },
{ "/.", "!" },
{ ".\"", ".edu" },
{ "/\"", "?" },
{ ".'", ".org" },
{ "a'", "au" },
{ "u'", "ua" },
{ "e'", "eo" },
{ "o'", "oe" },
};
