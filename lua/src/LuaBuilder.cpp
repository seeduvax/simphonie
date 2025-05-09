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

#include <iostream>
#include <string>

#include "Smp/IEntryPoint.h"
#include "Smp/IModel.h"
#include "Smp/IOutputField.h"
#include "Smp/ISimpleArrayField.h"
#include "Smp/ISimpleField.h"
#include "Smp/Publication/IType.h"
#include "Smp/Services/IResolver.h"
#include "Smp/Services/IScheduler.h"
#include "simdeck/ExInvalidType.hpp"
#include "simphonie/sys/Logger.hpp"
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
        std::string name=te.first.as<std::string>();
        sol::table v=te.second;
        std::string type=v["type"];
        std::string description = v.get_or<std::string>("description", "");
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
void LuaBuilder::connect(Smp::String8 fromPath, Smp::String8 toPath, bool bulk) {
    auto resolver = _sim->GetResolver();

    // bulk connection of fields is reversed, to let special fields
    // receiving many connection being published as output field to
    // benefit the Connect interface.  This is used for instance to
    // connect many fields (whatever inputs or outputs to on field
    // recorder, see FieldRecorder in colibry module).
    auto fromField = dynamic_cast<Smp::IOutputField*>(resolver->ResolveAbsolute(bulk ? toPath : fromPath));
    auto toField = dynamic_cast<Smp::IField*>(resolver->ResolveAbsolute(bulk ? fromPath : toPath));
    if (fromField != nullptr && toField != nullptr) {
        std::string msg = "Connecting field ";
        msg += fromPath;
        msg += " to ";
        msg += toPath;
        _sim->GetLogger()->Log(this, msg.c_str(), Smp::Services::ILogger::LMK_Debug);
        fromField->Connect(toField);
    }
    else {
        auto fromComp = dynamic_cast<Smp::IComponent*>(resolver->ResolveAbsolute(fromPath));
        auto toComp = dynamic_cast<Smp::IComponent*>(resolver->ResolveAbsolute(toPath));
        if (fromComp != nullptr && toComp != nullptr) {
            std::string msg = "Linking component ";
            msg += fromPath;
            msg += " to ";
            msg += toPath;
            _sim->GetLogger()->Log(this, msg.c_str(), Smp::Services::ILogger::LMK_Debug);
            auto linkReg = _sim->GetLinkRegistry();
            linkReg->AddLink(fromComp, toComp);
        }
        else {
            std::string msg = "Can't connect fields or add components link";
            if (fromField == nullptr && fromComp == nullptr) {
                msg += ", from object ";
                msg += fromPath;
                msg += " not found";
            }
            if (toField == nullptr && toComp == nullptr) {
                msg += ", to object ";
                msg += toPath;
                msg += " not found";
            }
            _sim->GetLogger()->Log(this, msg.c_str(), Smp::Services::ILogger::LMK_Error);
        }
    }
}
// ..........................................................
void LuaBuilder::configure() {
    // iterate on configuration table component section to init data defined
    // at that level to set related fields value.
    // iterate on init data section to set related fields value.
    // iterate on connection section to connect fields (TODO all kind of
    // connection to be handled, not only field to field connections)
    sol::table components=_config["components"];
    for (auto te : components) {
        std::string name=te.first.as<std::string>();
        sol::table v=te.second;
        std::string type=v["type"];
        
        Smp::IObject* comp=_sim->GetContainer(Smp::ISimulator::SMP_SimulatorModels)->GetChild(name.c_str());
        if(comp==nullptr){
            comp=_sim->GetService(name.c_str());
        }
        initComponents(comp,v);
    }
}
// ..........................................................
void LuaBuilder::connect() {
    // iterate on connections list connect fields.
    sol::table connections = _config["connections"];
    for (auto cnx : connections) {
        if (cnx.second.is<sol::table>()) {
            auto target = cnx.first.as<std::string>();
            for (auto src : cnx.second.as<sol::table>()) {
                connect(src.second.as<std::string>().c_str(), target.c_str(), true);
            }
        }
        else {
            connect(cnx.second.as<std::string>().c_str(), cnx.first.as<std::string>().c_str());
        }
    }

    // iterate on schedul list to schedule entry points
    sol::table schedule = _config["schedule"];
    auto scheduler = _sim->GetScheduler();
    auto resolver = _sim->GetResolver();
    for (auto entry : schedule) {
        sol::table t = entry.second;
        std::string name = t.get_or<std::string>("name", "");
        if (name == "") {
            name = entry.first.as<std::string>();
        }
        Smp::Duration cycleTime = t.get_or("cycleTime_s", 0.0) * 1000000000ULL;
        if (cycleTime == 0) {
            cycleTime = t.get_or("cycleTime_ms", 0.0) * 1000000ULL;
        }
        if (cycleTime == 0) {
            cycleTime = t.get_or("cycleTime_us", 0.0) * 1000ULL;
        }
        if (cycleTime == 0) {
            cycleTime = t.get_or("cycleTime_ns", 0.0);
        }
        Smp::Int64 repeat = t.get_or("repetitions", -1LL);
        Smp::Duration time = t.get_or("offset", 0ULL);
        auto ep = dynamic_cast<Smp::IEntryPoint*>(resolver->ResolveAbsolute(name.c_str()));
        if (ep != nullptr) {
            scheduler->AddSimulationTimeEvent(ep, time, cycleTime, repeat);
            std::ostringstream msg;
            msg << name << " scheduled cycleTime=" << cycleTime << "ns,  repeat=" << repeat
                << " simulationTime=" << time << "ns";
            _sim->GetLogger()->Log(this, msg.str().c_str(), Smp::Services::ILogger::LMK_Debug);
        }
        else {
            std::ostringstream msg;
            msg << "Can't schedule " << name << ": entry point not found";
            _sim->GetLogger()->Log(this, msg.str().c_str(), Smp::Services::ILogger::LMK_Error);
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

Smp::AnySimple anyFromLua(Smp::PrimitiveTypeKind ptk, sol::object val){
    Smp::AnySimple res = Smp::AnySimple(ptk);
    switch (ptk) {
        case Smp::PrimitiveTypeKind::PTK_Int8:
            res.SetValue(ptk,(Smp::Int8)val.as<double>()); // read double to handle scientific notation
            break;
        case Smp::PrimitiveTypeKind::PTK_Int16:
            res.SetValue(ptk,(Smp::Int16)val.as<double>());
            break;
        case Smp::PrimitiveTypeKind::PTK_Int32:
            res.SetValue(ptk,(Smp::Int32)val.as<double>());
            break;
        case Smp::PrimitiveTypeKind::PTK_Int64:
        case Smp::PrimitiveTypeKind::PTK_DateTime:
        case Smp::PrimitiveTypeKind::PTK_Duration:
            res.SetValue(ptk,(Smp::Int64)val.as<double>());
            break;
        case Smp::PrimitiveTypeKind::PTK_UInt8:
            res.SetValue(ptk,(Smp::UInt8)val.as<double>());
            break;
        case Smp::PrimitiveTypeKind::PTK_UInt16:
            res.SetValue(ptk,(Smp::UInt16)val.as<double>());
            break;
        case Smp::PrimitiveTypeKind::PTK_UInt32:
            res.SetValue(ptk,(Smp::UInt32)val.as<double>());
            break;
        case Smp::PrimitiveTypeKind::PTK_UInt64:
            res.SetValue(ptk,(Smp::UInt64)val.as<double>());
            break;
        case Smp::PrimitiveTypeKind::PTK_Bool:
            res.SetValue(ptk,val.as<Smp::Bool>());
            break;
        case Smp::PrimitiveTypeKind::PTK_Char8:
            res.SetValue(ptk,val.as<Smp::Char8>());
            break;
        case Smp::PrimitiveTypeKind::PTK_Float32:
            res.SetValue(ptk,val.as<Smp::Float32>());
            break;
        case Smp::PrimitiveTypeKind::PTK_Float64:
            res.SetValue(ptk,val.as<Smp::Float64>());
            break;
        case Smp::PrimitiveTypeKind::PTK_String8:
            res.SetValue(ptk,val.as<Smp::String8>());
            break;
    }
    return res;
}
// ..........................................................
void LuaBuilder::addSubComponents(Smp::IComposite* node, sol::table t) {
    for (auto te: t) {
        std::string kName=te.first.as<std::string>();
        if (kName!="type" && kName!="description") {
            if(node->GetContainer(kName.c_str()) == nullptr)continue;
            sol::table content=te.second.as<sol::table>();
            for (auto child: content) {
                std::string name=child.first.as<std::string>();
                sol::table v=child.second;
                std::string type=v["type"];
                std::string description = v.get_or<std::string>("description", "");
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


void LuaBuilder::initComponents(Smp::IObject* node, sol::table t) {
    for (auto te: t) {
        std::string kName=te.first.as<std::string>();
        if (kName!="type" && kName!="description") {
            auto c=dynamic_cast<Smp::IComponent*>(node);
            if (c!=nullptr) {
                auto field = c->GetField(kName.c_str());
                if(field!=nullptr){
                    Smp::PrimitiveTypeKind ptk = field->GetType()->GetPrimitiveTypeKind();
                    if(ptk==Smp::PrimitiveTypeKind::PTK_None){
                        throw simdeck::ExInvalidType(field,"No primitive type Found");
                    }
                    auto simpleField = dynamic_cast<Smp::ISimpleField*>(field);
                    if(simpleField!=nullptr){
                        simpleField->SetValue(anyFromLua(ptk, te.second));
                    }
                    auto simpleArrayField = dynamic_cast<Smp::ISimpleArrayField*>(field);
                    if(simpleArrayField!=nullptr){
                        sol::table luaArray = te.second;
                        if(luaArray.size() == simpleArrayField->GetSize()){
                            for(Smp::UInt64 i=0;i<simpleArrayField->GetSize();i++)
                            {
                                simpleArrayField->SetValue(i, anyFromLua(ptk , luaArray[i+1]));
                            }
                        }
                        else{
                            throw simdeck::ExInvalidType(field,"Lua table size does not match array size");
                        }
                    }
                }
            }
            Smp::IComposite* cnode = dynamic_cast<Smp::IComposite*>(node);
            if(cnode == nullptr || cnode->GetContainer(kName.c_str()) == nullptr)continue;
            sol::table content=te.second.as<sol::table>();
            for (auto child: content) {
                initComponents(cnode->GetContainer(kName.c_str())->GetChild(child.first.as<Smp::String8>()),child.second);
            }
        }
    }
}

}  // namespace lua
}  // namespace simph
