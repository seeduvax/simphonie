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
#include "Smp/ISimulator.h"
#include "Smp/IModel.h"
#include "Smp/IOutputField.h"
#include "simphonie/kern/Resolver.hpp"
#include "simphonie/kern/Scheduler.hpp"
#include "simphonie/kern/Simulator.hpp"
#include "simphonie/lua/LuaBuilder.hpp"
#include "simdeck/Utils.hpp"
#include "sol/sol.hpp"


// TODO check calling sim.Publish is allowed many times (and simulator
// implementation shall be robust to that), in order to let services be pre
// published. May not be required for LuaBuilder since it should finally receive
// the configuration table through property binding to lua.


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

#define TRACE(expr) std::cout << __FILE__ << ":" << __LINE__ << ": " << #expr << " = " << expr << std::endl;
sol::object objectIndex(Smp::IObject* obj, Smp::String8 name, sol::this_state L) {
    TRACE(obj->GetName())
    TRACE(name)
    TRACE((void*)obj->GetChild(name))
    return solCastObject(obj->GetChild(name),L);
}


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

/*
 getData(simphonie::kern::Field* field) {
     res;
    if(field == nullptr)
    {
        return res;
    }

    // Associate corresponding primitive type
    switch (field->GetPrimitiveTypeKind())
    {
    case Smp::PrimitiveTypeKind::PTK_Char8:
        res = (Smp::Char8)(field->GetValue());
        break;
    case Smp::PrimitiveTypeKind::PTK_Bool:
        res = (Smp::Bool)(field->GetValue());
        break;
    case Smp::PrimitiveTypeKind::PTK_Int8:
        res = (Smp::Int8)(field->GetValue());
        break;
    case Smp::PrimitiveTypeKind::PTK_UInt8:
        res = (Smp::UInt8)(field->GetValue());
        break;
    case Smp::PrimitiveTypeKind::PTK_Int16:
        res = (Smp::Int16)(field->GetValue());
        break;
    case Smp::PrimitiveTypeKind::PTK_UInt16:
        res = (Smp::UInt16)(field->GetValue());
        break;
    case Smp::PrimitiveTypeKind::PTK_Int32:
        res = (Smp::Int32)(field->GetValue());
        break;
    case Smp::PrimitiveTypeKind::PTK_UInt32:
        res = (Smp::UInt32)(field->GetValue());
        break;
    case Smp::PrimitiveTypeKind::PTK_Int64:
        res = (Smp::Int64)(field->GetValue());
        break;
    case Smp::PrimitiveTypeKind::PTK_UInt64:
        res = (Smp::UInt64)(field->GetValue());
        break;
    case Smp::PrimitiveTypeKind::PTK_Float32:
        res = (Smp::Float32)(field->GetValue());
        break;
    case Smp::PrimitiveTypeKind::PTK_Float64:
        res = (Smp::Float64)(field->GetValue());
        break;
    case Smp::PrimitiveTypeKind::PTK_Duration:
        res = (Smp::Duration)(field->GetValue());
        break;
    case Smp::PrimitiveTypeKind::PTK_DateTime:
        res = (Smp::DateTime)(field->GetValue());
        break;
    case Smp::PrimitiveTypeKind::PTK_String8:
        res = (Smp::String8)(field->GetValue());
        break;
    default:
        std::stringstream ss;
        ss<< "Primitive type of" << name << "not found";
        throw std::runtime_error(ss.str().c_str());
        break;
    }

    return res;
};
*/


// --------------------------------------------------------------------
// ..........................................................
extern "C" {
int luaopen_libsimph_lua(lua_State* L) {
    sol::state_view lua = L;
    lua.open_libraries(sol::lib::base);
    auto t = lua.create_table();
    t["Uuid"] = [](std::string c) { return Smp::Uuid(c.c_str()); };
    t["GenerateUuid"] = [](std::string c) { return simdeck::Utils::GenerateUuid(c.c_str()); };
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

    auto nsSimphonie = t["Simphonie"].get_or_create<sol::table>();
    nsSimphonie.new_usertype<simphonie::kern::Simulator>("Simulator",
        sol::constructors<simphonie::kern::Simulator()>(),
        sol::meta_function::construct, [](std::string name) {
            return new simphonie::kern::Simulator(name.c_str());
        },
        sol::meta_function::index, &objectIndex,
// TODO bind those services to addition SMP service or helpers that are provided 
// with the lua binding. Meaning it shall work on any SMP complient ISimulator
// implementation.
        sol::meta_function::new_index, simulatorNewIndex,
//        "connect", &simphonie::kern::Simulator::connect,
//        "schedule", &simphonie::kern::Simulator::schedule,
//        "setValue", &simphonie::kern::Simulator::setValue,
//        "createSmpModel", &simphonie::kern::Simulator::createSmpModel,
        "setConfiguration",[](Smp::ISimulator* s, sol::object o) {
            auto b = new simphonie::lua::LuaBuilder("LuaBuilder", "Simulation builder from lua script", s);
            s->AddService(b);
            b->setConfiguration(o);
            return b;
        },
        sol::base_classes, sol::bases<Smp::IObject, Smp::IComposite, Smp::ISimulator>()
    );

    

// TODO, neeed something to bind entry points and published operations to 
// to lua.
    t.push();
    return 1;
}
// according to lib name resolution policy, let short lib name works.
int luaopen_simphonie_lua(lua_State* L) {
    return luaopen_libsimph_lua(L);
}
}
