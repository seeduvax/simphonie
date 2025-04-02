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
#include "Smp/ISimpleField.h"
#include "Smp/ISimpleArrayField.h"
#include "Smp/Publication/IType.h"

#include "simdeck/ExInvalidType.hpp"
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
    std::cout<<"Initializations"<<std::endl;
    sol::table components=_config["components"];
    for (auto te: components) {
        //std::string name=te.first;    
        std::string name=te.first.as<std::string>();
        sol::table v=te.second;
        std::string type=v["type"];
        std::string description=v["description"];
        auto comp=_sim->GetContainer(Smp::ISimulator::SMP_SimulatorModels)->GetChild(name.c_str());
        // TODO recursively scan to build child components.
        auto composite=dynamic_cast<Smp::IComposite*>(comp);
        if (composite!=nullptr) {
            initSubComponents(composite,v);
        }
    }
    std::cout<<"Connections"<<std::endl;
    auto resolver=_sim->GetResolver();
    components=_config["connections"];
    for (auto cnx: components) {

        
        std::string toPath=cnx.first.as<std::string>();
        std::string fromPath=cnx.second.as<std::string>();
        auto from=dynamic_cast<Smp::IOutputField*>(resolver->ResolveAbsolute(fromPath.c_str()));
        auto to=dynamic_cast<Smp::IField*>(resolver->ResolveAbsolute(toPath.c_str()));
        if (from!=nullptr && to!=nullptr) {
            std::cout<<from->GetName()<<"=>"<<to->GetName()<<std::endl;
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

Smp::AnySimple anyFromLua(Smp::PrimitiveTypeKind ptk, sol::object val){
    Smp::AnySimple res = Smp::AnySimple(ptk);
    switch (ptk) {
        case Smp::PrimitiveTypeKind::PTK_Int8:
            res.SetValue(ptk,val.as<Smp::Int8>());
            break;
        case Smp::PrimitiveTypeKind::PTK_Int16:
            res.SetValue(ptk,val.as<Smp::Int16>());
            break;
        case Smp::PrimitiveTypeKind::PTK_Int32:
            res.SetValue(ptk,val.as<Smp::Int32>());
            break;
        case Smp::PrimitiveTypeKind::PTK_Int64:
        case Smp::PrimitiveTypeKind::PTK_DateTime:
        case Smp::PrimitiveTypeKind::PTK_Duration:
            res.SetValue(ptk,val.as<Smp::Int64>());
            break;
        case Smp::PrimitiveTypeKind::PTK_UInt8:
            res.SetValue(ptk,val.as<Smp::UInt8>());
            break;
        case Smp::PrimitiveTypeKind::PTK_UInt16:
            res.SetValue(ptk,val.as<Smp::UInt16>());
            break;
        case Smp::PrimitiveTypeKind::PTK_UInt32:
            res.SetValue(ptk,val.as<Smp::UInt32>());
            break;
        case Smp::PrimitiveTypeKind::PTK_UInt64:
            res.SetValue(ptk,val.as<Smp::UInt64>());
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


void LuaBuilder::initSubComponents(Smp::IComposite* node, sol::table t) {
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
            if(node->GetContainer(kName.c_str()) == nullptr)continue;
            sol::table content=te.second.as<sol::table>();
            for (auto child: content) {
                Smp::IObject* childNode = node->GetContainer(kName.c_str())->GetChild(child.first.as<Smp::String8>());
                initSubComponents(dynamic_cast<Smp::IComposite*>(childNode),child.second);
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
