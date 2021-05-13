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
#include "simph/kern/Scheduler.hpp"
#include "simph/kern/Simulator.hpp"
#include "simph/lua/LuaBuilder.hpp"
#include "simph/smpdk/Utils.hpp"
#include "sol/sol.hpp"

// exemple de meta new_index
// On ne fait rien, on se content de regarder ce qu'on reçoit en paramètre.
void myNewIndex(Smp::ISimulator& th, sol::stack_object k, sol::stack_object v, sol::this_state L) {
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

/*
luabridge::LuaRef getData(const char * name, lua_State* L) {
    luabridge::LuaRef res(L);

    Smp::ISimpleField* object = dynamic_cast<Smp::ISimpleField*>(_sim.GetResolver()->ResolveAbsolute(name));
    if(object == nullptr)
    {
        return res;
    }

    // Associate corresponding primitive type
    switch (object->GetPrimitiveTypeKind())
    {
    case Smp::PrimitiveTypeKind::PTK_Char8:
        res = (Smp::Char8)(object->GetValue());
        break;
    case Smp::PrimitiveTypeKind::PTK_Bool:
        res = (Smp::Bool)(object->GetValue());
        break;
    case Smp::PrimitiveTypeKind::PTK_Int8:
        res = (Smp::Int8)(object->GetValue());
        break;
    case Smp::PrimitiveTypeKind::PTK_UInt8:
        res = (Smp::UInt8)(object->GetValue());
        break;
    case Smp::PrimitiveTypeKind::PTK_Int16:
        res = (Smp::Int16)(object->GetValue());
        break;
    case Smp::PrimitiveTypeKind::PTK_UInt16:
        res = (Smp::UInt16)(object->GetValue());
        break;
    case Smp::PrimitiveTypeKind::PTK_Int32:
        res = (Smp::Int32)(object->GetValue());
        break;
    case Smp::PrimitiveTypeKind::PTK_UInt32:
        res = (Smp::UInt32)(object->GetValue());
        break;
    case Smp::PrimitiveTypeKind::PTK_Int64:
        res = (Smp::Int64)(object->GetValue());
        break;
    case Smp::PrimitiveTypeKind::PTK_UInt64:
        res = (Smp::UInt64)(object->GetValue());
        break;
    case Smp::PrimitiveTypeKind::PTK_Float32:
        res = (Smp::Float32)(object->GetValue());
        break;
    case Smp::PrimitiveTypeKind::PTK_Float64:
        res = (Smp::Float64)(object->GetValue());
        break;
    case Smp::PrimitiveTypeKind::PTK_Duration:
        res = (Smp::Duration)(object->GetValue());
        break;
    case Smp::PrimitiveTypeKind::PTK_DateTime:
        res = (Smp::DateTime)(object->GetValue());
        break;
    case Smp::PrimitiveTypeKind::PTK_String8:
        res = (Smp::String8)(object->GetValue());
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
    t["GenerateUuid"] = [](std::string c) { return simph::smpdk::Utils::generateUuid(c.c_str()); };
    auto nsSmp = t["Smp"].get_or_create<sol::table>();

    // clang-format off
    nsSmp.new_usertype<Smp::IObject>( "IObject",
        "name", sol::property(&Smp::IObject::GetName),
        "description", sol::property(&Smp::IObject::GetDescription),
        "parent", sol::property(&Smp::IObject::GetParent),
        "type", sol::property([](Smp::IObject* o) { return typeid(*o).name(); })  // TODO add some demangling here
    );
    nsSmp.new_usertype<Smp::IComponent>("IComponent", 
        sol::meta_function::index,
        [](Smp::IComponent* m, Smp::String8 n, sol::this_state L) {
            // look up for fields first
            Smp::IField* f = m->GetField(n);
            if (f != nullptr) {
                return sol::object(L, sol::in_place, f);
            }
            // then look up for entry points if it's an EntryPointPublisher
            Smp::IEntryPoint* ep = nullptr;
            if (dynamic_cast<Smp::IEntryPointPublisher*>(m) != nullptr) {
                ep = dynamic_cast<Smp::IEntryPointPublisher*>(m)->GetEntryPoint(n);
            }
            return sol::object(L, sol::in_place, ep);
        },
        "GetState", &Smp::IComponent::GetState,
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
        sol::base_classes, sol::bases<Smp::IObject>()
    );
    nsSmp.new_usertype<Smp::IService>("IService",
        sol::base_classes, sol::bases<Smp::IObject, Smp::IComponent>()
    );
    nsSmp.new_usertype<Smp::IModel>("IModel",
        sol::base_classes, sol::bases<Smp::IObject, Smp::IComponent>()
    );
    nsSmp.new_usertype<Smp::IComposite>("IComposite",
        "GetContainer", &Smp::IComposite::GetContainer,
        sol::base_classes, sol::bases<Smp::IObject>()
    );
    nsSmp.new_usertype<Smp::Services::IScheduler>("IScheduler",
        sol::base_classes, sol::bases<Smp::IObject, Smp::IComponent, Smp::IService>()
    );
    nsSmp.new_usertype<Smp::ISimulator>("ISimulator",
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
        "GetScheduler", [](Smp::ISimulator& sim) {
            return dynamic_cast<simph::kern::Scheduler*>(sim.GetScheduler());
        },
        sol::base_classes, sol::bases<Smp::IObject, Smp::IComposite>()
    );
    nsSmp.new_usertype<Smp::Services::ITimeKeeper>("ITimeKeeper",
        "GetSimulationTime", &Smp::Services::ITimeKeeper::GetSimulationTime,
        sol::base_classes, sol::bases<Smp::IObject, Smp::IComponent>()
    );

    auto nsSimphonie = t["Simphonie"].get_or_create<sol::table>();
    nsSimphonie.new_usertype<simph::kern::Simulator>("Simulator",
        sol::constructors<simph::kern::Simulator()>(),
        sol::meta_function::construct, [](std::string name) {
          return new simph::kern::Simulator(name.c_str());
        },
        sol::meta_function::index, [](Smp::ISimulator& th, std::string k, sol::this_state L) {
            auto o = th.GetResolver()->ResolveAbsolute(k.c_str());
            // from most concrete to most abstract:
            if (false) {}
            else if (dynamic_cast<Smp::IService*>(o) != nullptr) {
                return sol::object(L, sol::in_place, dynamic_cast<Smp::IService*>(o));
            }
            else if (dynamic_cast<Smp::IModel*>(o) != nullptr) {
                return sol::object(L, sol::in_place, dynamic_cast<Smp::IModel*>(o));
            }
            return sol::object(L, sol::in_place, o);
        },
        sol::meta_function::new_index, myNewIndex,
        "connect", &simph::kern::Simulator::connect,
        "schedule", &simph::kern::Simulator::schedule,
        "createSmpModel", &simph::kern::Simulator::createSmpModel,
        "setConfiguration",[](Smp::ISimulator* s, sol::object o) {
            auto b = new simph::lua::LuaBuilder(s);
            b->setConfiguration(o);
            return b;
        },
        sol::base_classes, sol::bases<Smp::IObject, Smp::IComposite, Smp::ISimulator>()
    );
    nsSimphonie.new_usertype<simph::kern::Scheduler>("Scheduler",
        "step", &simph::kern::Scheduler::step,
        sol::base_classes, sol::bases<Smp::IObject, Smp::IComponent, Smp::IService, Smp::Services::IScheduler>()
    );
    // clang-format on
    t.push();
    return 1;
}
}
