/*
 * @file LuaModel.hpp
 *
 * Copyright 2025 Sebastien Devaux. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simphonie_lua_LuaModel_HPP__
#define __simphonie_lua_LuaModel_HPP__

#include "simdeck/EPPModel.hpp"
#include "sol/sol.hpp"

namespace simphonie {
namespace lua {

/**
 *
 */
class LuaModel: public simdeck::EPPModel {
    typedef EPPModel Parent;
public:
    /**
     * Default constructor.
     */
    LuaModel(Smp::String8 name, Smp::String8 description, Smp::IObject* parent);
    /**
     * Destructor.
     */
    virtual ~LuaModel();

protected:
    void publish(Smp::IPublication* receiver) override;
    void configure() override;
    void connect() override;

private:
    sol::table _luaModel;
    sol::state _lua;
    std::string _scriptPath;

    std::string buildName(Smp::String8 spec);
    void call(const char* name);
};

}} // namespace simphonie::lua
#endif // __simphonie_lua_LuaModel_HPP__
