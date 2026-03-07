#pragma once

// adaptives for Carbyne (PQortho-f VMirrVowP UO / PQortho-f VMirrVowP UO)

static StringPairList txtAdaptives = {
    { "kh", "kl" },
    { "vh", "vl" },
    { "lf", "lv" },
    { "w/", "wl" },

    { ",g", "ig"}, // ,g -> ig (for ver w BYOU on bottom / G on top)
    { "g,", "gi"}, // g, -> gi (for ver w BYOU on bottom / G on top)
    { "g'", "go"}, // g' -> go (for ver w BYOU on bottom / G on top)
    { "gj", "gth" },// gj -> gth (for ver w BYOU on bottom / G on top)

    { "vw", "vs"}, // vm -> vs
    { "wv", "wn"}, // wv -> wn 

    { "mwm", "mpl"}, // mwm -> mpl (copied from HD PM)
    { "wmw", "lml" },
    { "wm", "lm"}, // wm -> lm
    { "mw", "mp" },

    //{ "lp", "s"}, // lp -> ls (not as good a tradeOff, but still better)
    { "tn", "tl"}, // tn -> tl b
    { "fp", "sp" },
    { "pf", "ps" },

    // taken & adapted from HD PM !
    { "mk", "lk" },
    { "kt", "kn" },
    { "tk", "nk" },

    { "yb", "ib" },

    { "ae", "au" },
    // let's try this, even though a,u to x is i bit of a stretch
    { "ax", "au" },
    { "ux", "ua" },
    { "ex", "eo" },
    { "ox", "oe" },
    // not sure about these, same pos as HD PM.. let's try them iso _X
    { "ag", "au" },
    { "ug", "ua" },
    { "eg", "eo" },
    { "og", "oe" },

    { ".'", ".com" },
    { "'.", "!" },
    { ".,", ".edu" },
    { "',", "?" },
    { ".g", ".org" },

    // 'magic' adaptive trailer, on q, for "macros" (q is on qwerty spot usually used for this, like #
    { "cq", "cgi" },
    { "gq", "gmail" },
    { "qq", "Quesnel" },
    { "pq", "Philippe" },
    { "q.q", "philippe.quesnel" },
    { "dq", "device" },
    { "iq", "integration\\" },
    { "sq", "select * from " },
};
