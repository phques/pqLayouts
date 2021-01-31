// Copyright 2021 Philippe Quesnel  
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

#pragma once

#include <lua.hpp>
#include <LuaBridge.h>


// for easier access to lua globals (global vars / funcs)
//   state["myFunc2'](123);
//   int val = state["value1"];
class LuaState
{
public:
    LuaState(lua_State* L) : luaL(L) {}

    inline luabridge::LuaRef operator[](const char* name) { return luabridge::getGlobal(luaL, name); }
    inline luabridge::LuaRef GetRef(const char* name) { return luabridge::getGlobal(luaL, name); }

    std::string PopString()
    {
        std::string s = lua_tostring(luaL, -1);
        lua_pop(luaL, 1);
        return s;
    }

    lua_State* luaL;
};