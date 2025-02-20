/*
 * @file LuaBuilder.hpp
 *
 * Copyright 2021 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simphonie_lua_LuaBuilder_HPP__
#define __simphonie_lua_LuaBuilder_HPP__

#include "sol/sol.hpp"
#include "Smp/ISimulator.h"

namespace simphonie {
namespace lua {

class LuaBuilder {
public:
    LuaBuilder(Smp::ISimulator* sim);
    ~LuaBuilder();

    void setConfiguration(sol::table config);

private:
// TODO builder to be reconsidered
//    simphonie::kern::Builder _builder;
    Smp::ISimulator* _sim;

    void loadSmpModels(sol::table models);
    void loadParameters(sol::table parameters);
    void loadInitializations(sol::table initializations);
    void loadSamplers(sol::table initializations);
    void loadConnections(sol::table connections);
    void loadSchedules(sol::table schedules);
};

}  // namespace lua
}  // namespace simph
#endif  // __simphonie_lua_LuaBuilder_HPP__
