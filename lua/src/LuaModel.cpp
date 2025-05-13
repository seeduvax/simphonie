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

#include "Smp/ISimulator.h"

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
    _lua.safe_script("Smp=require 'simphonie_lua'");
    sol::table g=_lua.globals();
    _luaModel=g["model"];
}
// ..........................................................
LuaModel::~LuaModel() {
}

// --------------------------------------------------------------------
// ..........................................................
void LuaModel::call(const char* name) {
    sol::protected_function pf = _luaModel[name];
    if (pf != sol::nil) {
        auto res = pf(sol::object(_lua, sol::in_place, this));
        if (!res.valid()) {
            sol::error err = res;
            getSimulator()->GetLogger()->Log(this, err.what(), Smp::Services::ILogger::LMK_Error);
        }
    }
}
// ..........................................................
void LuaModel::publish(Smp::IPublication* receiver) {
    call("publish");
}
// ..........................................................
void LuaModel::configure() {
    call("configure");
}
// ..........................................................
void LuaModel::connect() {
    call("connect");
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
