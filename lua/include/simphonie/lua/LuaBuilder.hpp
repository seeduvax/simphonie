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
#include "simdeck/Service.hpp"

namespace simphonie {
namespace lua {

class LuaBuilder: public simdeck::Service {
    typedef simdeck::Service Parent;
public:
    LuaBuilder(Smp::String8 name, Smp::String8 description, Smp::IObject* parent);
    virtual ~LuaBuilder();
    /**
     * set builder lua configuration table
     * @param config configuration table
     */ 
    void setConfiguration(sol::table config);

    static Smp::IComponent* simulatorCreateComponent(
                                            Smp::ISimulator* sim,
                                            Smp::String8 typeName,
                                            Smp::String8 name,
                                            Smp::String8 description);
    static Smp::IComponent* componentCreateComponent(
                                            Smp::ISimulator* sim,
                                            Smp::IComposite* composite,
                                            Smp::String8 typeName,
                                            Smp::String8 container,
                                            Smp::String8 name,
                                            Smp::String8 description);
protected:
    /** 
     * Publish specialization.
     * shall trig component creation.
     * @param receiver Publication not used since this service does not have
     *        any own field.
     */
    void publish(Smp::IPublication* receiver) override;
    /**
     * Connect specialization.
     * Shall:
     *  - init data.
     *  - apply connections.
     */  
    void connect() override;
private:
    void addSubComponents(Smp::IComposite* node, sol::table t);
    
    Smp::ISimulator* _sim;
    Smp::Services::IResolver* _resolver;

    void loadSmpModels(sol::table models);
    void loadParameters(sol::table parameters);
    void loadInitializations(sol::table initializations);
    void loadSamplers(sol::table initializations);
    void loadConnections(sol::table connections);
    void loadSchedules(sol::table schedules);

    sol::table _config;
};

}  // namespace lua
}  // namespace simph
#endif  // __simphonie_lua_LuaBuilder_HPP__
