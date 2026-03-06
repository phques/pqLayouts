#pragma once

// adaptives for HD PM

static StringPairList txtAdaptives = {
{ "pd", "pwd" },
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
{ "nh", "nd" },// PQ ND is a bit difficult on stag

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
{ "dp", "pg" },
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

// need alternative for this, find it hard to reach '
{ "a'", "au" },
{ "u'", "ua" },
{ "e'", "eo" },
{ "o'", "oe" },
// let's try this, even though a,u to z is i bit of a stretch
{ "az", "au" },
{ "uz", "ua" },
{ "ez", "eo" },
{ "oz", "oe" },

// 'magic' adaptive trailer, on #, for "macros"
{ "c#", "cgi" },
{ "g#", "gmail" },
{ "q#", "Quesnel" },
{ "p#", "Philippe" },
{ "q.#", "philippe.quesnel" },
{ "d#", "device" },
{ "i#", "integration\\" },
{ "s#", "select * from " },
};
