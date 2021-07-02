/*
 * @file LuaBuilder.cpp
 *
 * Copyright 2021 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simph/lua/LuaBuilder.hpp"
#include "simph/sys/Logger.hpp"

namespace simph {
namespace lua {
// --------------------------------------------------------------------
// ..........................................................
LuaBuilder::LuaBuilder(Smp::ISimulator* sim) : _builder(sim), _sim(sim) {
    _sim->AddService(&_builder);
}
// ..........................................................
LuaBuilder::~LuaBuilder() {}
// ..........................................................
void LuaBuilder::setConfiguration(sol::table config) {
    LOGI("loading smp models")
    loadSmpModels(config["models"]);
    LOGI("loading parameters")
    loadParameters(config["params"]);
    LOGI("loading init")
    loadInitializations(config["inits"]);
    LOGI("loading connections")
    loadConnections(config["connections"]);
    LOGI("loading schedules")
    loadSchedules(config["schedules"]);
    LOGI("loading samplers")
    loadSamplers(config["samplers"]);
}
// ..........................................................
void LuaBuilder::loadParameters(sol::table parameters) {
    for (const auto& obj : parameters) {
        sol::table value = obj.second;
        std::string path = value["path"];
        // FIXME identify value type : bool, float, int, string, uint32...
        Smp::AnySimple val;
        // if(value["value"] == false || value["value"] == true){
        //    val = Smp::AnySimple(Smp::PrimitiveTypeKind::PTK_Bool, (bool)value["value"]);
        //}
        // else{
        val = Smp::AnySimple(Smp::PrimitiveTypeKind::PTK_Float64, (double)value["value"]);
        //}

        _builder.AddLoadParamCfg({path, val});
    }
}
// ..........................................................
void LuaBuilder::loadSchedules(sol::table schedules) {
    for (const auto& obj : schedules) {
        sol::table value = obj.second;
        std::string path = value["path"];
        auto period = Smp::AnySimple(Smp::PrimitiveTypeKind::PTK_UInt64, (uint32_t)value["period"]);
        _builder.AddLoadScheduleCfg({path, period});
    }
}
// ..........................................................
void LuaBuilder::loadInitializations(sol::table initializations) {
    for (const auto& obj : initializations) {
        sol::table value = obj.second;
        std::string path = value["path"];
        // TODO identify value type : bool, float, int, string, uint32...
        auto val = Smp::AnySimple(Smp::PrimitiveTypeKind::PTK_Float64, (double)value["value"]);
        _builder.AddLoadInitCfg({path, val});
    }
}
// ..........................................................
void LuaBuilder::loadSamplers(sol::table samplers) {
    for (const auto& obj : samplers) {
        sol::table value = obj.second;
        std::string name = value["name"];
        std::string description = value["description"];
        std::string strMode = value["mode"];
        Smp::AnySimple mode;
        mode.SetValue(Smp::PrimitiveTypeKind::PTK_Bool, strMode == "csv" ? true : false);
        std::vector<std::string> fields;
        sol::table valueFields = value["fields"];
        for (int field = 1; field <= valueFields.size(); field++) {
            fields.push_back(valueFields[field]);
        }
        _builder.AddLoadSamplerCfg({name, description, mode, fields});
    }
}
// ..........................................................
void LuaBuilder::loadConnections(sol::table connections) {
    for (const auto& obj : connections) {
        sol::table value = obj.second;
        std::string from = value["from"];
        std::string to = value["to"];
        _builder.AddLoadConnectionCfg({from, to});
    }
}
// ..........................................................
void LuaBuilder::loadSmpModels(sol::table models) {
    for (const auto& obj : models) {
        sol::table value = obj.second;
        std::string library = value["library"];
        std::string type = value["class"];
        std::string name = value["name"];
        std::string description = value["description"];
        _builder.AddLoadSmpModelCfg({library, type, name, description});
    }
}
// ..........................................................

}  // namespace lua
}  // namespace simph
