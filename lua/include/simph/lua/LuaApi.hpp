/*
 * @file LuaApi.hpp
 *
 * Copyright 2021 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simph_lua_LuaApi_HPP__
#define __simph_lua_LuaApi_HPP__


extern "C" 
{
    #include <lua.h>
    #include <lauxlib.h>
    #include <lualib.h>
}

#include "LuaBridge/LuaBridge.h"


namespace simph {
	namespace lua {

class LuaApi {
public:
    LuaApi(lua_State* L);
    virtual ~LuaApi();
};

}} // namespace simph::lua
#endif // __simph_lua_LuaApi_HPP__
