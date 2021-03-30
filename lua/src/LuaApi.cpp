/*
 * @file LuaApi.cpp
 *
 * Copyright 2021 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simph/lua/LuaApi.hpp"
#include "simph/kern/Simulator.hpp"

namespace simph {
	namespace lua {


class SimProxy {
public:
    SimProxy(): _sim() {
        std::cout << "Construction SimProxy" << std::endl;
    };
    ~SimProxy() {
        std::cout << "Destruction SimProxy" << std::endl;
    };
    void Initialise() {
        _sim.Initialise();
    };
    //void Publish() 
    //void Configure() 
    //void Connect() 
    //void Run() 
    //void Hold(Smp::Bool immediate) 
    //void Store(Smp::String8 filename) 
    //void Restore(Smp::String8 filename) 
    //void Reconnect(Smp::IComponent* root) 
    //void Exit() 
    //void Abort() 
    //bool 
    void printRef() {
        std::cout << "sim:" <<  &_sim << std::endl;
    };
private:
    simph::kern::Simulator _sim;
};



// --------------------------------------------------------------------
// ..........................................................
LuaApi::LuaApi(lua_State* luaState) {
    std::cout << "Loading simph_lua api" << std::endl;
    luabridge::getGlobalNamespace(luaState)
       .beginClass<SimProxy>("Simulator")
          .addConstructor<void(*) ()>()
          .addFunction("Initialise", &SimProxy::Initialise)
          .addFunction("printRef", &SimProxy::printRef)
    .endClass(); 

}
// ..........................................................
LuaApi::~LuaApi() {
}
// ..........................................................
extern "C" { 
   int luaopen_libsimph_lua(lua_State* L) {
        static LuaApi api(L);
        return 1;
   }
   int luaopen_lua(lua_State* L) {
        return luaopen_libsimph_lua(L);
   }
}



}} // namespace simph::lua
