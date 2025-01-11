/*
 * @file LuaBuilder.hpp
 *
 * Copyright 2021 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simph_lua_LuaBuilder_HPP__
#define __simph_lua_LuaBuilder_HPP__

#include "sol/sol.hpp"
#include "Smp/ISimulator.h"

namespace simph {
namespace lua {

class LuaBuilder {
public:
    LuaBuilder(Smp::ISimulator* sim);
    ~LuaBuilder();

    void setConfiguration(sol::table config);

private:
// TODO builder to be reconsidered
//    simph::kern::Builder _builder;
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
#endif  // __simph_lua_LuaBuilder_HPP__
