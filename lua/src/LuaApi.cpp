/*
 * @file LuaApi.cpp
 *
 * Copyright 2021 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "Smp/IEntryPointPublisher.h"
#include "Smp/IModel.h"
#include "Smp/IOutputField.h"
#include "Smp/ISimpleArrayField.h"
#include "Smp/ISimpleField.h"
#include "Smp/ISimulator.h"
#include "Smp/Publication/IType.h"
#include "Smp/Services/IEventManager.h"
#include "Smp/Services/IResolver.h"
#include "Smp/Services/IScheduler.h"
#include "Smp/Services/ITimeKeeper.h"
#include "simdeck/Utils.hpp"
#include "simphonie/lua/LuaBuilder.hpp"
#include "simphonie/lua/LuaModel.hpp"
#include "simphonie/sys/DLib.hpp"
#include "sol/sol.hpp"

// TODO check calling sim.Publish is allowed many times (and simulator
// implementation shall be robust to that), in order to let services be pre
// published. May not be required for LuaBuilder since it should finally receive
// the configuration table through property binding to lua.

// --------------------------------------------------------------------
// ..........................................................

sol::object solCastObject(Smp::IObject* obj, sol::this_state L) {
    auto ofield=dynamic_cast<Smp::IOutputField*>(obj);
    if (ofield!=nullptr) {
        return sol::object(L, sol::in_place, ofield);
    }
    auto field=dynamic_cast<Smp::IField*>(obj);
    if (field!=nullptr) {
        return sol::object(L, sol::in_place, field);
    }
    auto ep=dynamic_cast<Smp::IEntryPoint*>(obj);
    if (ep!=nullptr) {
        return sol::object(L, sol::in_place, ep);
    }
    auto resolver=dynamic_cast<Smp::Services::IResolver*>(obj);
    if (resolver!=nullptr) {
        return sol::object(L, sol::in_place, resolver);
    }
    auto simulator=dynamic_cast<Smp::ISimulator*>(obj);
    if (simulator!=nullptr) {
        return sol::object(L, sol::in_place, simulator);
    }
    auto comp=dynamic_cast<Smp::IComponent*>(obj);
    if (comp!=nullptr) {
        return sol::object(L, sol::in_place, comp);
    }
    // default, return object as generic SMP::IObject
    return sol::object(L, sol::in_place, obj);
}

// ..........................................................
// exemple de meta new_index
// On ne fait rien, on se content de regarder ce qu'on reçoit en paramètre.
void simulatorNewIndex(Smp::ISimulator& th, sol::stack_object k, sol::stack_object v, sol::this_state L) {
    std::cout << "Debug myNewIndex: " << th.GetName() << std::endl;
    auto kIsString = k.as<sol::optional<std::string> >();
    if (kIsString) {
        std::string kstr = k.as<std::string>();
        if (kstr == "testTK") {
            std::cout << "Debug myNewIndex: Test assign object" << std::endl;
            // just un truc debile pour verifier qu'on recup bien notre
            // instance d'objet et qu'on peut jouer avec...
            auto obj = v.as<Smp::Services::ITimeKeeper*>();
            if (obj != nullptr) {
                std::cout << "Debug myNewIndex: " << obj->GetName() << std::endl;
                std::cout << "Debug myNewIndex: " << obj->GetSimulationTime() << std::endl;
            }
        }
    }
}
// ..........................................................
sol::object objectIndex(Smp::IObject* obj, sol::stack_object k, sol::this_state L) {
    std::string kstr;
    if (k.is<std::string>()) {
        kstr = k.as<std::string>();
    }
    else if (k.is<int>()) {
        std::ostringstream oss;
        int i = k.as<int>() - 1;  // -1 because lua index arrays from 1 not 0.
        oss << "[" << i << "]";
        kstr = oss.str();
    }
    return solCastObject(obj->GetChild(kstr.c_str()), L);
}

// ..........................................................
Smp::Bool componentCreateChild(Smp::IComponent* th, Smp::String8 typeName, Smp::String8 container, Smp::String8 name, Smp::String8 description) {
    auto composite=dynamic_cast<Smp::IComposite*>(th);
    if (composite==nullptr) {
        return false;
    }
    Smp::IObject* node=th;
    Smp::ISimulator* sim=nullptr;
    while (node!=nullptr && sim==nullptr) {
        sim=dynamic_cast<Smp::ISimulator*>(node);
        node=node->GetParent();
    }
    if (sim!=nullptr) {
        simphonie::lua::LuaBuilder::componentCreateComponent(sim,
                                                        composite,
                                                        typeName,
                                                        container,
                                                        name,
                                                        description);
    }
    return false;
}

Smp::AnySimple anyFromLua(Smp::PrimitiveTypeKind ptk, sol::object val){ //TODO: maybe deduplicate from luabuilder
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
        default:
            std::stringstream ss;
            ss << "Can't set value, primitive of type" << ptk << " not supported";
            throw std::runtime_error(ss.str().c_str());
            break;
    }
    return res;
}

// ..........................................................
sol::object fieldGetValue(Smp::IField* field, sol::this_state L) {
    sol::object res = sol::nil;
    auto sf = dynamic_cast<Smp::ISimpleField*>(field);
    if (sf != nullptr) {
        // Associate corresponding primitive type
        switch (sf->GetPrimitiveTypeKind()) {
            case Smp::PrimitiveTypeKind::PTK_Char8:
                res = sol::object(L, sol::in_place, (Smp::Char8)(sf->GetValue()));
                break;
            case Smp::PrimitiveTypeKind::PTK_Bool:
                break;
            case Smp::PrimitiveTypeKind::PTK_Int8:
                res = sol::object(L, sol::in_place, (Smp::Int8)(sf->GetValue()));
                break;
            case Smp::PrimitiveTypeKind::PTK_UInt8:
                res = sol::object(L, sol::in_place, (Smp::UInt8)(sf->GetValue()));
                break;
            case Smp::PrimitiveTypeKind::PTK_Int16:
                res = sol::object(L, sol::in_place, (Smp::Int16)(sf->GetValue()));
                break;
            case Smp::PrimitiveTypeKind::PTK_UInt16:
                res = sol::object(L, sol::in_place, (Smp::UInt16)(sf->GetValue()));
                break;
            case Smp::PrimitiveTypeKind::PTK_Int32:
                res = sol::object(L, sol::in_place, (Smp::Int32)(sf->GetValue()));
                break;
            case Smp::PrimitiveTypeKind::PTK_UInt32:
                res = sol::object(L, sol::in_place, (Smp::UInt32)(sf->GetValue()));
                break;
            case Smp::PrimitiveTypeKind::PTK_Int64:
                res = sol::object(L, sol::in_place, (Smp::Int64)(sf->GetValue()));
                break;
            case Smp::PrimitiveTypeKind::PTK_UInt64:
                res = sol::object(L, sol::in_place, (Smp::UInt64)(sf->GetValue()));
                break;
            case Smp::PrimitiveTypeKind::PTK_Float32:
                res = sol::object(L, sol::in_place, (Smp::Float32)(sf->GetValue()));
                break;
            case Smp::PrimitiveTypeKind::PTK_Float64:
                res = sol::object(L, sol::in_place, (Smp::Float64)(sf->GetValue()));
                break;
            case Smp::PrimitiveTypeKind::PTK_Duration:
                res = sol::object(L, sol::in_place, (Smp::Duration)(sf->GetValue()));
                break;
            case Smp::PrimitiveTypeKind::PTK_DateTime:
                res = sol::object(L, sol::in_place, (Smp::DateTime)(sf->GetValue()));
                break;
            case Smp::PrimitiveTypeKind::PTK_String8:
                res = sol::object(L, sol::in_place, (Smp::String8)(sf->GetValue()));
                break;
            default:
                std::stringstream ss;
                ss << "Can't get value, primitive type of" << field->GetName() << " not supported";
                throw std::runtime_error(ss.str().c_str());
                break;
        }
    }
    return res;
};
// ..........................................................
void fieldSetValue(Smp::IField* field, sol::object value) {
    auto sf = dynamic_cast<Smp::ISimpleField*>(field);
    auto ptk = field->GetType()->GetPrimitiveTypeKind();
    if (sf != nullptr) {
        auto v = sf->GetValue();
        sf->SetValue(anyFromLua(ptk, value));
    }
    auto af = dynamic_cast<Smp::ISimpleArrayField*>(field);
    if (af != nullptr) {
        sol::table luaArray = value.as<sol::table>();
        if(luaArray.size() == af->GetSize()){
            for(Smp::UInt64 i=0;i<af->GetSize();i++)
            {
                af->SetValue(i, anyFromLua(ptk , luaArray[i+1]));
            }
        }
        else{
            std::stringstream ss;
            ss << "Lua table size "<<luaArray.size()<<" does not match field size " << af->GetSize();
            throw std::runtime_error(ss.str().c_str());
        }
    }
}

sol::object CreateSimulator(sol::lua_table cfg, sol::this_state L) {
    // charger symbol CreateSimulator
    std::string libName = cfg["lib"];
    std::string simName = cfg.get_or<std::string>("name", "simulator");
    std::string descr = cfg.get_or<std::string>("description", "");
    try {
        simphonie::sys::DLib simLib(libName.c_str());
        auto createSim =
            simLib.getEntry<Smp::ISimulator* (*)(Smp::String8 name, Smp::String8 description, Smp::IObject * parent)>(
                "CreateSimulator");
        if (createSim != nullptr) {
            Smp::ISimulator* sim = createSim(simName.c_str(), descr.c_str(), nullptr);
            auto b = new simphonie::lua::LuaBuilder("LuaBuilder", "Simulation builder from lua script", sim);
            sim->AddService(b);
            b->setConfiguration(cfg);
            return sol::object(L, sol::in_place, sim);
        }
    }
    catch (std::runtime_error& ex) {
        std::cerr << "Can't create simulator: " << ex.what() << std::endl;
    }
    return sol::nil;
}

// --------------------------------------------------------------------
// ..........................................................
extern "C" {
int luaopen_libsimph_lua(lua_State* L) {
    sol::state_view lua = L;
    lua.open_libraries(sol::lib::base);
    auto t = lua.create_table();
    t["Uuid"] = [](std::string c) { return Smp::Uuid(c.c_str()); };
    t["GenerateUuid"] = [](std::string c) { return simdeck::Utils::GenerateUuid(c.c_str()); };
    t["CreateSimulator"] = [](sol::lua_table cfg, sol::this_state LS) { return CreateSimulator(cfg, LS); };
    auto nsSmp = t["Smp"].get_or_create<sol::table>();

    // clang-format off
    nsSmp.new_usertype<Smp::IObject>( "IObject",
        "Name", sol::property(&Smp::IObject::GetName),
        "Description", sol::property(&Smp::IObject::GetDescription),
        "Parent", sol::property([](Smp::IObject* o,sol::this_state L) { 
                return solCastObject(o->GetParent(),L);
        }),
        "Type", sol::property([](Smp::IObject* o) { return typeid(*o).name(); }),  // TODO add some demangling here
        sol::meta_function::index, &objectIndex 
    );
    nsSmp.new_usertype<Smp::IEntryPoint>("IEntryPoint", 
        "Execute", [](Smp::IEntryPoint* th) {
            th->Execute();
        },
        sol::base_classes, sol::bases<Smp::IObject>()
    );
    nsSmp.new_usertype<Smp::IComponent>("IComponent", 
        sol::meta_function::index, &objectIndex,
        "State", sol::property(&Smp::IComponent::GetState),
        "GetField", &Smp::IComponent::GetField,
        "GetFields", &Smp::IComponent::GetFields,
        "GetEntryPoint", [](Smp::IComponent* m, Smp::String8 n) {
            Smp::IEntryPoint* ep = nullptr;
            if (dynamic_cast<Smp::IEntryPointPublisher*>(m) != nullptr) {
                ep = dynamic_cast<Smp::IEntryPointPublisher*>(m)->GetEntryPoint(n);
            }
            return ep;
        },
        "GetEntryPoints", [](Smp::IComponent* m) {
            const Smp::EntryPointCollection* epCollection = nullptr;
            if (dynamic_cast<Smp::IEntryPointPublisher*>(m) != nullptr) {
                epCollection =
                    dynamic_cast<Smp::IEntryPointPublisher*>(m)->GetEntryPoints();
            }
            return epCollection;
        },
        "GetContainer", [](Smp::IComponent* m, Smp::String8 n) {
            Smp::IContainer* cont = nullptr;
            auto c=dynamic_cast<Smp::IComposite*>(m);
            if (c != nullptr) {
                cont = c->GetContainer(n);
            }
            return cont;
        },
        "GetContainers", [](Smp::IComponent* m) {
            const Smp::ContainerCollection* cl = nullptr;
            auto c=dynamic_cast<Smp::IComposite*>(m);
            if (c != nullptr) {
                cl = c->GetContainers();
            }
            return cl;
        },
        "CreateChild", &componentCreateChild, 
        sol::base_classes, sol::bases<Smp::IObject>()
    );
    nsSmp.new_usertype<Smp::IField>("IField",
        sol::meta_function::index, &objectIndex,
        "Value", sol::property(&fieldGetValue, &fieldSetValue), 
        sol::base_classes, sol::bases<Smp::IObject>()
    );
    nsSmp.new_usertype<Smp::IOutputField>("IOutputField",
        sol::meta_function::index, &objectIndex,
        "Connect", &Smp::IOutputField::Connect,
        sol::base_classes, sol::bases<Smp::IField>()
    );

    nsSmp.new_usertype<Smp::Services::IScheduler>("IScheduler",
        sol::meta_function::index, &objectIndex,
        sol::base_classes, sol::bases<Smp::IObject, Smp::IComponent>()
    );
    nsSmp.new_usertype<Smp::ISimulator>("ISimulator",
        sol::meta_function::index, &objectIndex,
        sol::meta_function::new_index, simulatorNewIndex,
        "State", sol::property(&Smp::ISimulator::GetState),
        "Publish", &Smp::ISimulator::Publish,
        "Configure", &Smp::ISimulator::Configure,
        "Connect", &Smp::ISimulator::Connect,
        "Run", &Smp::ISimulator::Run,
        "Hold", &Smp::ISimulator::Hold,
        "Store", &Smp::ISimulator::Store,
        "Restore", &Smp::ISimulator::Restore,
        "Exit", &Smp::ISimulator::Exit,
        "Abort", &Smp::ISimulator::Abort,
        "LoadLibrary", &Smp::ISimulator::LoadLibrary,
        "CreateInstance", &Smp::ISimulator::CreateInstance,
        "GetTimeKeeper", &Smp::ISimulator::GetTimeKeeper,
        "AddService", &Smp::ISimulator::AddService,
        "GetScheduler", &Smp::ISimulator::GetScheduler,
        "GetResolver", &Smp::ISimulator::GetResolver,
        "GetEventManager", &Smp::ISimulator::GetEventManager,
        "CreateComponent", &simphonie::lua::LuaBuilder::simulatorCreateComponent,
        sol::base_classes, sol::bases<Smp::IObject, Smp::IComposite>()
    );
    nsSmp.new_usertype<Smp::Services::ITimeKeeper>("ITimeKeeper",
        sol::meta_function::index, &objectIndex,
        "GetSimulationTime", &Smp::Services::ITimeKeeper::GetSimulationTime,
        sol::base_classes, sol::bases<Smp::IObject, Smp::IComponent>()
    );
    // IScheduler binding
    nsSmp.new_usertype<Smp::Services::IScheduler>("IScheduler",
        sol::meta_function::index, &objectIndex,
        "AddImmediateEvent", &Smp::Services::IScheduler::AddImmediateEvent,
        "AddSimulationTimeEvent", &Smp::Services::IScheduler::AddSimulationTimeEvent,
        sol::base_classes, sol::bases<Smp::IObject, Smp::IComponent>()
    );
    // IResolver binding
    nsSmp.new_usertype<Smp::Services::IResolver>("IResolver",
        sol::meta_function::index, &objectIndex,
        "ResolveAbsolute", &Smp::Services::IResolver::ResolveAbsolute,
        "ResolveRelative", &Smp::Services::IResolver::ResolveRelative,
        sol::base_classes, sol::bases<Smp::IObject, Smp::IComponent>()
    );
    // IEventManager binding
    nsSmp.new_usertype<Smp::Services::IEventManager>("IEventManager",
        sol::meta_function::index, &objectIndex,
        "Emit", &Smp::Services::IEventManager::Emit,
        "QueryEventId", &Smp::Services::IEventManager::QueryEventId,
        sol::base_classes, sol::bases<Smp::IObject, Smp::IComponent>()
    );
    nsSmp.new_usertype<simphonie::lua::LuaModel>("LuaModel", 
        sol::meta_function::index, &objectIndex,
        "AddEntryPoint", &simphonie::lua::LuaModel::addEntryPoint,
        sol::base_classes, sol::bases<Smp::IObject, Smp::IComponent>()
    );

    t.push();
    return 1;
}
// according to lib name resolution policy, let short lib name works.
int luaopen_simphonie_lua(lua_State* L) {
    return luaopen_libsimph_lua(L);
}
}
