/*
 * @file LuaBuilder.cpp
 *
 * Copyright 2021 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simphonie/lua/LuaBuilder.hpp"
#include "Smp/IModel.h"
#include "Smp/IOutputField.h"
#include "Smp/Services/IResolver.h"

namespace simphonie {
namespace lua {
// --------------------------------------------------------------------
// ..........................................................
LuaBuilder::LuaBuilder(Smp::String8 name, Smp::String8 description, Smp::IObject* parent):
                    Parent(name, description, parent) { 
    auto sim=dynamic_cast<Smp::ISimulator*>(parent);
    if (sim!=nullptr) {
        _sim=sim;
    }
    else {
        // TODO send exception.
    }
}
// ..........................................................
LuaBuilder::~LuaBuilder() {}
// ..........................................................
void LuaBuilder::setConfiguration(sol::table config) {
    _config=config;
}
// --------------------------------------------------------------------
// ..........................................................
void LuaBuilder::publish(Smp::IPublication* receiver) {
    // iterate on configuration table component section to create and add
    // components to the simulator.
    sol::table components=_config["components"];
    for (auto te: components) {
        //std::string name=te.first;    
        std::string name=te.first.as<std::string>();
        sol::table v=te.second;
        std::string type=v["type"];
        std::string description=v["description"];
        auto comp=simulatorCreateComponent(_sim, type.c_str(), name.c_str(),
                                 description.c_str());
        // TODO recursively scan to build child components.
        auto composite=dynamic_cast<Smp::IComposite*>(comp);
        if (composite!=nullptr) {
            addSubComponents(composite,v);
        }
    }
}


// ..........................................................
void LuaBuilder::connect() {
    // iterate on configuration table component section to init data defined
    // at that level to set related fields value.
    // iterate on init data section to set related fields value.
    // iterate on connection section to connect fields (TODO all kind of
    // connection to be handled, not only field to field connections)
    auto resolver=_sim->GetResolver();
    sol::table components=_config["connections"];
    for (auto cnx: components) {
        std::string toPath=cnx.first.as<std::string>();
        std::string fromPath=cnx.second.as<std::string>();
        auto from=dynamic_cast<Smp::IOutputField*>(resolver->ResolveAbsolute(fromPath.c_str()));
        auto to=dynamic_cast<Smp::IField*>(resolver->ResolveAbsolute(toPath.c_str()));
        if (from!=nullptr && to!=nullptr) {
            from->Connect(to);
        }
    }
}
// --------------------------------------------------------------------
// ..........................................................
Smp::IComponent* LuaBuilder::simulatorCreateComponent(Smp::ISimulator* sim,
                                               Smp::String8 typeName,
                                               Smp::String8 name,
                                               Smp::String8 description) {
    for (auto fac: *(sim->GetFactories())) {
        if (strcmp(typeName,fac->GetTypeName())==0) {
            auto comp=fac->CreateInstance(name,description,sim);
            if (comp!=nullptr) {
                auto service=dynamic_cast<Smp::IService*>(comp);
                if (service!=nullptr) {
                    sim->AddService(service);
                    return comp;
                }
                auto model=dynamic_cast<Smp::IModel*>(comp);
                if (model!=nullptr) {
                    sim->AddModel(model);
                    return comp;
                }
                // from here built component is neither a service or a model
                // then it can't be added to the simulator a shall be dropped
                sim->GetLogger()->Log(comp,
                        "Component is neither a service or a model. "
                        "Can't add it to the simulator",
                        Smp::Services::ILogger::LMK_Error);
                delete comp;
                return nullptr;
            }
        }
    }
    std::string msg="No factory found to build instances of component type ";
    msg+=typeName;
    sim->GetLogger()->Log(sim, msg.c_str(), Smp::Services::ILogger::LMK_Error);
    return nullptr;;
}
// ..........................................................
Smp::IComponent* LuaBuilder::componentCreateComponent(
                                           Smp::ISimulator* sim,
                                           Smp::IComposite* composite,
                                           Smp::String8 typeName,
                                           Smp::String8 container,
                                           Smp::String8 name,
                                           Smp::String8 description) {
    auto cnt=composite->GetContainer(container);
    if (cnt!=nullptr) {
        for (auto fac: *(sim->GetFactories())) {
            if (strcmp(typeName,fac->GetTypeName())==0) {
                auto comp=fac->CreateInstance(name,description,composite);
                if (comp!=nullptr) {
                    cnt->AddComponent(comp);
                    return comp;
                }
            }
        }
    }
    return nullptr;
}
// ..........................................................
void LuaBuilder::addSubComponents(Smp::IComposite* node, sol::table t) {
    for (auto te: t) {
        std::string kName=te.first.as<std::string>();
        if (kName!="type" && kName!="description") {
            sol::table content=te.second;
            for (auto child: content) {
                std::string name=child.first.as<std::string>();
                sol::table v=child.second;
                std::string type=v["type"];
                std::string description=v["description"];
                auto sub = componentCreateComponent(_sim, 
                                                    node,
                                                    type.c_str(),
                                                    kName.c_str(),
                                                    name.c_str(),
                                                    description.c_str());
                auto composite = dynamic_cast<Smp::IComposite*>(sub);
                if (composite!=nullptr) {
                    addSubComponents(composite,v);
                }
            }
        }
    }
}



/*
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

//        _builder.AddLoadParamCfg({path, val});
    }
}
// ..........................................................
void LuaBuilder::loadSchedules(sol::table schedules) {
    for (const auto& obj : schedules) {
        sol::table value = obj.second;
        std::string path = value["path"];
        auto period = Smp::AnySimple(Smp::PrimitiveTypeKind::PTK_UInt64, (uint32_t)value["period"]);
//        _builder.AddLoadScheduleCfg({path, period});
    }
}
// ..........................................................
void LuaBuilder::loadInitializations(sol::table initializations) {
    for (const auto& obj : initializations) {
        sol::table value = obj.second;
        std::string path = value["path"];
        // TODO identify value type : bool, float, int, string, uint32...
        auto val = Smp::AnySimple(Smp::PrimitiveTypeKind::PTK_Float64, (double)value["value"]);
//        _builder.AddLoadInitCfg({path, val});
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
//        _builder.AddLoadSamplerCfg({name, description, mode, fields});
    }
}
// ..........................................................
void LuaBuilder::loadConnections(sol::table connections) {
    for (const auto& obj : connections) {
        sol::table value = obj.second;
        std::string from = value["from"];
        std::string to = value["to"];
//        _builder.AddLoadConnectionCfg({from, to});
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
//        _builder.AddLoadSmpModelCfg({library, type, name, description});
    }
}
// ..........................................................
*/
}  // namespace lua
}  // namespace simph
