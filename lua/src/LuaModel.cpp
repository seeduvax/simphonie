/*
 * @file LuaModel.cpp
 *
 * Copyright 2025 Sebastien Devaux. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simphonie/lua/LuaModel.hpp"

namespace simphonie {
namespace lua {
// --------------------------------------------------------------------
// ..........................................................
LuaModel::LuaModel(Smp::String8 name, Smp::String8 description, Smp::IObject* parent):
        Parent(buildName(name).c_str(), description, parent) {
    std::string s=name;
    auto loc=s.find('=');
    if ( loc != std::string::npos ) {
        _scriptPath=s.substr(loc+1);
    }

    _lua.open_libraries(sol::lib::base, sol::lib::package, sol::lib::string, sol::lib::os, sol::lib::math,
                       sol::lib::table, sol::lib::debug);
    _lua.safe_script_file(_scriptPath.c_str());
    sol::table g=_lua.globals();
    _luaModel=g["model"];
}
// ..........................................................
LuaModel::~LuaModel() {
}

// --------------------------------------------------------------------
// ..........................................................
void LuaModel::publish(Smp::IPublication* receiver) {
    sol::function pub=_luaModel["publish"];
    if ( pub != sol::nil ) {
        pub();
    }
}
// ..........................................................
void LuaModel::configure() {
    sol::function cfg=_luaModel["configure"];
    if ( cfg != sol::nil ) {
        cfg();
    }
}
// ..........................................................
void LuaModel::connect() {
    sol::function cnx=_luaModel["connect"];
    if ( cnx != sol::nil ) {
        cnx();
    }
}

// --------------------------------------------------------------------
// ..........................................................
std::string LuaModel::buildName(Smp::String8 spec) {
    std::string s=spec;
    auto loc=s.find('=');
    if ( loc != std::string::npos ) {
        return s.substr(0,loc);
    }
    return "LuaModel";
}

}} // namespace simphonie::lua
