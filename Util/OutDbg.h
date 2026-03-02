#pragma once
// Copyright 2020 Philippe Quesnel  
//
// This file is part of pqLayouts.
//
// pqLayouts is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// pqLayouts is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with pqLayouts.  If not, see <http://www.gnu.org/licenses/>.

#include "util.h"

//---------

namespace Dbg
{
    namespace Out
    {
        bool LastError(const char* func);
        void KbdEVent(const KbdHookEvent&, WPARAM, bool selfInjected);
        void WmKey(MSG msg);
        void DebugString(std::ostringstream&);

    } // namespace Out 

} // namespace Dbg


//---------
