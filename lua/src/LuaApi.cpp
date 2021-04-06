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

#include "simph/kern/Field.hpp"
#include "simph/kern/EntryPoint.hpp"
#include "simph/kern/IEntryPointPublisher.hpp"
#include "simph/kern/Scheduler.hpp"


namespace simph {
	namespace lua {
    
class SimProxy {
public:
    SimProxy(): _sim() {
    };
    ~SimProxy() {
    };
    void Initialise() {
        _sim.Initialise();
    };
    void Publish() 
    {
        _sim.Publish();
    }
    void Configure()
    {
        _sim.Configure();
    };
    void Connect()
    {
        _sim.Connect();
    };
    void Run()
    {
        _sim.Run();
    };
    void Hold(const bool immediate)
    {
        _sim.Hold(immediate);
    };
    void Store(const char* filename)
    {
        _sim.Store(filename);
    };
    void Restore(const char* filename)
    {
        _sim.Restore(filename);
    };
    void Exit()
    {
        _sim.Exit();
    };
    void Abort()
    {
        _sim.Abort();
    };

    void loadSmpLib(const char* library)
    {
        _sim.LoadLibrary(library);
    };

    void createSmpModel(const char* model, const char* model_name, const char* description)
    {
        _sim.CreateInstance(Smp::Uuid(model),model_name,description, &_sim);
    };

    void connect(std::string output, std::string input)
    {
        auto output_field = dynamic_cast<Smp::IDataflowField*>(_sim.GetResolver()->ResolveAbsolute(output.c_str()));
        auto input_field = dynamic_cast<Smp::IDataflowField*>(_sim.GetResolver()->ResolveAbsolute(input.c_str()));

        if(input_field == nullptr)
        {
            std::stringstream ss;
            ss<< "Input field not find (" << input << ")";
            throw std::runtime_error(ss.str().c_str());
        }
        if(output_field == nullptr)
        {
            std::stringstream ss;
            ss<< "Output field not find (" << output << ")";
            throw std::runtime_error(ss.str().c_str());
        }

        output_field->Connect(input_field);
    };

    void schedule(std::string modelName, std::string entryPoint, uint32_t period) {
        auto model = dynamic_cast<Smp::IEntryPointPublisher*>(_sim.GetResolver()->ResolveAbsolute(modelName.c_str()));
        _sim.GetScheduler()->AddSimulationTimeEvent(model->GetEntryPoint(entryPoint.c_str()), 0, period, -1);
    };

    void stepTime(uint32_t duration) {
        auto scheduler = dynamic_cast<simph::kern::Scheduler*>(_sim.GetScheduler());
        scheduler->step(duration);
    };

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
private:
    simph::kern::Simulator _sim;
};



// --------------------------------------------------------------------
// ..........................................................
LuaApi::LuaApi(lua_State* luaState) {
    luabridge::getGlobalNamespace(luaState)
       .beginClass<SimProxy>("Simulator")
          .addConstructor<void(*) ()>()
          .addFunction("Initialise", &SimProxy::Initialise)
          .addFunction("Publish", &SimProxy::Publish)
          .addFunction("Configure", &SimProxy::Configure)
          .addFunction("Connect", &SimProxy::Connect)
          .addFunction("Run", &SimProxy::Run)
          .addFunction("Hold", &SimProxy::Hold)
          .addFunction("Store", &SimProxy::Store)
          .addFunction("Restore", &SimProxy::Restore)
          .addFunction("Exit", &SimProxy::Exit)
          .addFunction("Abort", &SimProxy::Abort)
          .addFunction("connect", &SimProxy::connect)
          .addFunction("stepTime", &SimProxy::stepTime)
          .addFunction("schedule", &SimProxy::schedule)
          .addFunction("loadSmpLib", &SimProxy::loadSmpLib)
          .addFunction("createSmpModel", &SimProxy::createSmpModel)
          .addFunction("getData", &SimProxy::getData)
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
