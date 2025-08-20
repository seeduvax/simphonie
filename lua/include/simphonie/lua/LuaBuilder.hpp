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
    static Smp::AnySimple anyFromLua(Smp::PrimitiveTypeKind ptk, sol::object val);
    static bool simulatorExportToFMU(Smp::ISimulator* sim, Smp::String8 filename);

protected:
    /** 
     * Publish specialization.
     * shall trig component creation.
     * @param receiver Publication not used since this service does not have
     *        any own field.
     */
    void publish(Smp::IPublication* receiver) override;
    /**
     * Configure specialization.
     * shall trig component fields initiatization.
     */
    void configure() override;
    /**
     * Connect specialization.
     * Shall apply fields connections and components links.
     */
    void connect() override;
private:
    void addSubComponents(Smp::IComposite* node, sol::table t);
    void initComponents(Smp::IObject* node, sol::table t);
    
    Smp::ISimulator* _sim;
    Smp::Services::IResolver* _resolver;

    void loadSmpModels(sol::table models);
    void loadParameters(sol::table parameters);
    void loadInitializations(sol::table initializations);
    void loadSamplers(sol::table initializations);
    void loadConnections(sol::table connections);
    void loadSchedules(sol::table schedules);

    sol::table _config;

    /**
     * local utility to connect fields by name.
     * Fields are search using the resolver witrh absolute (from root simulator
     * node) resolution.
     * @param fromPath source field path.
     * @param toPath destition field path.
     */
    void connect(Smp::String8 from, Smp::String8 to, bool bulk = false);
};

}  // namespace lua
}  // namespace simph
#endif  // __simphonie_lua_LuaBuilder_HPP__
