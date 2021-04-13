/*
 * @file LuaApi.cpp
 *
 * Copyright 2021 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "sol/sol.hpp"
#include "Smp/ISimulator.h"
#include "simph/kern/Simulator.hpp"

// exemple de meta index
// On retourne toujours le même truc quelque soit la clé.
// permet au passage de verifier que le type récupéré coté lua est OK
// (c'est à dire ici, bien du type timekeeper)...
sol::object myIndex(Smp::ISimulator& th, sol::stack_object k,sol::this_state L) {
    std::cout<<"Debug myIndex: "<<th.GetName()<<std::endl;
    auto o=th.GetTimeKeeper();
    return sol::object(L,sol::in_place,o);
}
// exemple de meta new_index
// On ne fait rien, on se content de regarder ce qu'on reçoit en paramètre.
void myNewIndex(Smp::ISimulator& th, sol::stack_object k, sol::stack_object v, sol::this_state L) {
    std::cout<<"Debug myNewIndex: "<<th.GetName()<<std::endl;
    auto kIsString=k.as<sol::optional<std::string> >();
    if (kIsString) {
        std::string kstr=k.as<std::string>();
        if (kstr == "testTK") {
            std::cout<<"Debug myNewIndex: Test assign object"<<std::endl;
            // just un truc debile pour verifier qu'on recup bien notre
            // instance d'objet et qu'on peut jouer avec...
            auto obj=v.as<Smp::Services::ITimeKeeper*>();
            if (obj!=nullptr) {
                std::cout<<"Debug myNewIndex: "<<obj->GetName()<<std::endl;
                std::cout<<"Debug myNewIndex: "<<obj->GetSimulationTime()<<std::endl;
            }
        }
    }
}
// exemple de factory, par exemple pour poir faire un truc
// specifique sur l'objet construit quand il l'est depuis lua.
simph::kern::Simulator* myFactory(std::string name) {
    return new simph::kern::Simulator(name.c_str());
}

// --------------------------------------------------------------------
// ..........................................................
extern "C" {
    int luaopen_libsimph_lua(lua_State* L) {
        sol::state_view lua=L;
        lua.open_libraries(sol::lib::base);
        auto t=lua.create_table();
        auto nsSmp=t["Smp"].get_or_create<sol::table>();
        nsSmp.new_usertype<Smp::IObject>("IObject",
            "name",sol::property(&Smp::IObject::GetName),
            "description",sol::property(&Smp::IObject::GetDescription),
            "parent",sol::property(&Smp::IObject::GetParent)
        );
        nsSmp.new_usertype<Smp::IComponent>("IComponent",
            "GetState",&Smp::IComponent::GetState, 
            sol::base_classes,sol::bases<Smp::IObject>()
        );
        nsSmp.new_usertype<Smp::IComposite>("IComposite",
            "GetContainer",&Smp::IComposite::GetContainer, 
            sol::base_classes,sol::bases<Smp::IObject>()
        );
        nsSmp.new_usertype<Smp::ISimulator>("ISimulator",
            "Publish",&Smp::ISimulator::Publish,
            "Configure",&Smp::ISimulator::Configure,
            "Connect",&Smp::ISimulator::Connect,
            "Run",&Smp::ISimulator::Run,  
            "Hold",&Smp::ISimulator::Hold,
            "Store",&Smp::ISimulator::Store,
            "Restore",&Smp::ISimulator::Restore,
            "Exit",&Smp::ISimulator::Exit,
            "Abort",&Smp::ISimulator::Abort,
            "LoadLibrary",&Smp::ISimulator::LoadLibrary,
            "CreateInstance",&Smp::ISimulator::CreateInstance,
            "GetTimeKeeper",&Smp::ISimulator::GetTimeKeeper,
            sol::base_classes,sol::bases<Smp::IObject,Smp::IComposite>()
        );
        nsSmp.new_usertype<Smp::Services::ITimeKeeper>("ITimeKeeper",
            "GetSimulationTime",&Smp::Services::ITimeKeeper::GetSimulationTime,
            sol::base_classes,sol::bases<Smp::IObject,Smp::IComponent>()
        );
        auto nsSimphonie=t["Simphonie"].get_or_create<sol::table>();
        nsSimphonie.new_usertype<simph::kern::Simulator>("Simulator",
             sol::constructors<simph::kern::Simulator()>(),
             sol::meta_function::construct,myFactory,
             sol::meta_function::index,myIndex,
             sol::meta_function::new_index,myNewIndex,
             sol::base_classes,sol::bases<Smp::IObject,Smp::IComposite,Smp::ISimulator>()
        );
        t.push();
        return 1;
    }
}
