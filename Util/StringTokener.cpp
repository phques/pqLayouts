// StringTokener.cpp

// Copyright 2026 Philippe Quesnel  
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

#include "pch.h"

#include "StringTokener.h"
#include "File.h"


StringTokener::StringTokener(const std::string& line, int lineNo)
    : lineNo(lineNo)
{
    str(line);
}

StringTokener::StringTokener(File& f)
    : StringTokener(f.line, f.lineNo)
{
}

bool StringTokener::ReadParam(const char* paramName, std::string& param)
{
    if (eof()) {
        std::cerr << paramName << ", line " << LineNo() << std::endl;
        return false;
    }
    *this >> param;
    return true;
}
