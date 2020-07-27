/*
 * @file Simulator.cpp
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simph/kern/Simulator.hpp"

#include "simph/smpdk/ExDuplicateName.hpp"
#include "simph/kern/ExDuplicateUuid.hpp"
#include "simph/kern/Logger.hpp"
#include "simph/kern/EventManager.hpp"
#include "simph/kern/ObjectsRegistry.hpp"
#include "simph/kern/Scheduler.hpp"
#include "simph/kern/TimeKeeper.hpp"
#include "simph/kern/LinkRegistry.hpp"
#include "simph/kern/TypeRegistry.hpp"
#include "Smp/IEntryPointPublisher.h"

// ..........................................................
namespace simph {
	namespace kern {
// --------------------------------------------------------------------
// ..........................................................
Simulator::Simulator(Smp::String8 name,Smp::String8 descr,
                Smp::IObject* parent): 
                    Composite(name,descr,parent),
                    _initEntryPoints("InitEntryPoints","",this),
                    _compFactories("ComponentFactories","",this) {
    addContainer(Smp::ISimulator::SMP_SimulatorModels);
    _models=GetContainer(Smp::ISimulator::SMP_SimulatorModels);
    addContainer(Smp::ISimulator::SMP_SimulatorServices);
    _services=GetContainer(Smp::ISimulator::SMP_SimulatorServices);
    _logger=new Logger("Logger","Logging service",this);
    _scheduler=new Scheduler("Scheduler","Schedule service",this);
    _timeKeeper=new TimeKeeper("TimeKeeper","Time service",this);
    _eventMgr=new EventManager("EventManager","Event handling service",this);
    _linkRegistry=new LinkRegistry("LinkRegistry","Link registry service",this);
    TypeRegistry* tr=new TypeRegistry("TypeRegistry","Type registry service",this);
    _typeRegistry=tr;
    _registry=new ObjectsRegistry("Resolver","Objects registry and resolver",
                                        this,tr);
    _services->AddComponent(_logger);
    _services->AddComponent(_scheduler);
    _services->AddComponent(_timeKeeper);
    _services->AddComponent(_eventMgr);
    _services->AddComponent(_linkRegistry);
    _services->AddComponent(_registry);
    _registry->add(this);
    setState(Smp::SimulatorStateKind::SSK_Building);
}
// ..........................................................
Simulator::~Simulator() {
    delete _typeRegistry;
    for (auto model: *(_models->GetComponents())) {
        delete model;
    }
    for (auto service: *(_services->GetComponents())) {
        delete service;
    }
    for (auto lib: _libs) {
        auto finalizeFunc=lib->getEntry<bool (*)()>("Finalise");
        if (finalizeFunc!=nullptr) {
            finalizeFunc();
        }
        delete lib;
    }
}
// --------------------------------------------------------------------
// ..........................................................
void Simulator::setState(Smp::SimulatorStateKind newState) {
    if (_state!=newState) {
        switch (_state) {
            case Smp::SimulatorStateKind::SSK_Connecting:
                _eventMgr->Emit(Smp::Services::IEventManager::SMP_LeaveConnectingId);
                break;
            case Smp::SimulatorStateKind::SSK_Reconnecting:
                _eventMgr->Emit(Smp::Services::IEventManager::SMP_LeaveReconnectingId);
                break;
            case Smp::SimulatorStateKind::SSK_Initialising:
                _eventMgr->Emit(Smp::Services::IEventManager::SMP_LeaveInitialisingId);
                break;
            case Smp::SimulatorStateKind::SSK_Standby:
                _eventMgr->Emit(Smp::Services::IEventManager::SMP_LeaveStandbyId);
                break;
            case Smp::SimulatorStateKind::SSK_Executing:
                _eventMgr->Emit(Smp::Services::IEventManager::SMP_LeaveExecutingId);
                break;
            case Smp::SimulatorStateKind::SSK_Storing:
                _eventMgr->Emit(Smp::Services::IEventManager::SMP_LeaveStoringId);
                break;
            case Smp::SimulatorStateKind::SSK_Restoring:
                _eventMgr->Emit(Smp::Services::IEventManager::SMP_LeaveRestoringId);
                break;
            default:
                // no events to send on leaving for other states.
                break;
        }
        _state=newState;
        std::ostringstream msg;
        msg << "state changed: " << _state;
        _logger->Log(this,msg.str().c_str(),Smp::Services::ILogger::LMK_Event);
        switch (_state) {
            case Smp::SimulatorStateKind::SSK_Initialising:
                _eventMgr->Emit(Smp::Services::IEventManager::SMP_EnterInitialisingId);
                break;
            case Smp::SimulatorStateKind::SSK_Standby:
                _eventMgr->Emit(Smp::Services::IEventManager::SMP_EnterStandbyId);
                break;
            case Smp::SimulatorStateKind::SSK_Executing:
                _eventMgr->Emit(Smp::Services::IEventManager::SMP_EnterExecutingId);
                break;
            case Smp::SimulatorStateKind::SSK_Storing:
                _eventMgr->Emit(Smp::Services::IEventManager::SMP_EnterStoringId);
                break;
            case Smp::SimulatorStateKind::SSK_Restoring:
                _eventMgr->Emit(Smp::Services::IEventManager::SMP_EnterRestoringId);
                break;
            case Smp::SimulatorStateKind::SSK_Reconnecting:
                _eventMgr->Emit(Smp::Services::IEventManager::SMP_EnterReconnectingId);
                break;
            case Smp::SimulatorStateKind::SSK_Exiting:
                _eventMgr->Emit(Smp::Services::IEventManager::SMP_EnterExitingId);
                break;
            case Smp::SimulatorStateKind::SSK_Aborting:
                _eventMgr->Emit(Smp::Services::IEventManager::SMP_EnterAbortingId);
                break;
            default:
                // No event to emit when entering to other states.
                break;
        }
    }
}
// ..........................................................
bool Simulator::checkState(Smp::String8 opName,
                            Smp::SimulatorStateKind expState) {
    bool res=_state==expState;
    if (!res) {
        std::ostringstream msg;
        msg <<opName
            <<" request ingored, operation not available at current state: "
            <<_state;
        _logger->Log(this,msg.str().c_str(),
                    Smp::Services::ILogger::LMK_Warning);
    }
    return res;
}
// ..........................................................
void Simulator::Initialise() {
    if (checkState("Initialise",Smp::SimulatorStateKind::SSK_Standby)) {
        setState(Smp::SimulatorStateKind::SSK_Initialising);
        for (auto ep: _initEntryPoints) {
            ep->Execute();
        }
        setState(Smp::SimulatorStateKind::SSK_Standby);
    }
}
// ..........................................................
void Simulator::publish(Smp::IComponent* comp) {
    if (comp->GetState()==Smp::ComponentStateKind::CSK_Created) {
        _registry->add(comp);
        comp->Publish(_registry);
        Smp::IEntryPointPublisher* epp=
                                dynamic_cast<Smp::IEntryPointPublisher*>(comp);
        if (epp!=nullptr) {
            for (auto ep: *(epp->GetEntryPoints())) {
                _registry->add(ep);
            }
        }
    }
}
// ..........................................................
void Simulator::Publish() {
    if (checkState("Publish",Smp::SimulatorStateKind::SSK_Building)) {
        for (auto service: *(_services->GetComponents())) {
            publish(service);
        }
        for (auto model: *(_models->GetComponents())) {
            publish(model);
        }
    }
}
// ..........................................................
void Simulator::configure(Smp::IComponent* comp) {
    if (comp->GetState()==Smp::ComponentStateKind::CSK_Publishing) {
        comp->Configure(_logger);
// TODO recursive call on childs.
    }
}
// ..........................................................
void Simulator::Configure() {
    if (checkState("Configure",Smp::SimulatorStateKind::SSK_Building)) {
        for (auto service: *(_services->GetComponents())) {
            configure(service);
        }
        for (auto model: *(_models->GetComponents())) {
            configure(model);
        }
    }
}
// ..........................................................
void Simulator::connect(Smp::IComponent* comp) {
    if (comp->GetState()==Smp::ComponentStateKind::CSK_Configured) {
        comp->Connect(this);
// TODO recursive call on childs.
    }
}
// ..........................................................
void Simulator::Connect() {
    if (checkState("Connect",Smp::SimulatorStateKind::SSK_Building)) {
        setState(Smp::SimulatorStateKind::SSK_Connecting);
        for (auto service: *(_services->GetComponents())) {
            connect(service);
        }
        for (auto model: *(_models->GetComponents())) {
            connect(model);
        }
        setState(Smp::SimulatorStateKind::SSK_Standby);
        Initialise();
    }
}
// ..........................................................
void Simulator::Run() {
    if (checkState("Run",Smp::SimulatorStateKind::SSK_Standby)) {
        _scheduler->start();
        setState(Smp::SimulatorStateKind::SSK_Executing);
    }
}
// ..........................................................
void Simulator::Hold(Smp::Bool immediate) {
    // TODO manage immediate...
    // But not sure it will be so easy for a multi-threaded scheduler...
    if (checkState("Hold",Smp::SimulatorStateKind::SSK_Executing)) {
        _scheduler->stop();
        setState(Smp::SimulatorStateKind::SSK_Standby);
    }
}
// ..........................................................
void Simulator::Store(Smp::String8 filename) {
    if (checkState("Store",Smp::SimulatorStateKind::SSK_Standby)) {
        setState(Smp::SimulatorStateKind::SSK_Storing);
// TODO serialize models states in file
_logger->Log(this,"Simulator::Store(filename) not implemented yet!",Smp::Services::ILogger::LMK_Error);
        setState(Smp::SimulatorStateKind::SSK_Standby);
    }
}
// ..........................................................
void Simulator::Restore(Smp::String8 filename) {
    if (checkState("Restore",Smp::SimulatorStateKind::SSK_Standby)) {
        setState(Smp::SimulatorStateKind::SSK_Restoring);
    // TODO deserialize models states from file
    _logger->Log(this,"Simulator::Restore(filename) not implemented yet!",Smp::Services::ILogger::LMK_Error);
        setState(Smp::SimulatorStateKind::SSK_Standby);
    }
}
// ..........................................................
void Simulator::Reconnect(Smp::IComponent* root) {
    if (checkState("Reconnect",Smp::SimulatorStateKind::SSK_Standby)) {
        // TODO is it needed to change state while running this?
        connect(root);
    }
}
// ..........................................................
void Simulator::Exit() {
    if (checkState("Exit",Smp::SimulatorStateKind::SSK_Standby)) {
        setState(Smp::SimulatorStateKind::SSK_Exiting);
// TODO shutdown everything...
    }
}
// ..........................................................
void Simulator::Abort() {
    setState(Smp::SimulatorStateKind::SSK_Aborting);
    // TODO force stop of anything that is runnning.
}
// ..........................................................
Smp::SimulatorStateKind Simulator::GetState() const {
    return _state;
}
// ..........................................................
void Simulator::AddService(Smp::IService* service) {
    _services->AddComponent(service);
}
// ..........................................................
Smp::IService* Simulator::GetService(Smp::String8 name) const {
    return dynamic_cast<Smp::IService*>(_services->GetComponent(name));
}
// ..........................................................
void Simulator::AddInitEntryPoint(Smp::IEntryPoint* ep) {
    if ( _state==Smp::SimulatorStateKind::SSK_Building
           ||  _state==Smp::SimulatorStateKind::SSK_Connecting
           ||  _state==Smp::SimulatorStateKind::SSK_Standby) {
        _initEntryPoints.push_back(ep);
    }    
}
// ..........................................................
void Simulator::AddModel(Smp::IModel* model) {
    _models->AddComponent(model);
}
// ..........................................................
Smp::Services::ILogger* Simulator::GetLogger() const {
    return _logger;
}
// ..........................................................
Smp::Services::ITimeKeeper* Simulator::GetTimeKeeper() const {
    return _timeKeeper;
}
// ..........................................................
Smp::Services::IScheduler* Simulator::GetScheduler() const {
    return _scheduler;
}
// ..........................................................
Smp::Services::IEventManager* Simulator::GetEventManager() const {
    return _eventMgr;
}
// ..........................................................
Smp::Services::ILinkRegistry* Simulator::GetLinkRegistry() const {
    return _linkRegistry;
}
// ..........................................................
Smp::Services::IResolver* Simulator::GetResolver() const {
    return _registry;
}
// ..........................................................
void Simulator::RegisterFactory(Smp::IFactory* componentFactory) {
    for (auto fac: _compFactories) {
        if (fac->GetUuid()==componentFactory->GetUuid()) {
            throw ExDuplicateUuid(this,fac->GetName(),
                            componentFactory->GetName());
        }
    }
    _compFactories.push_back(componentFactory);
    std::ostringstream msg;
    msg << "Registered component factory: "<<componentFactory->GetName();
    _logger->Log(this,msg.str().c_str(),Smp::Services::ILogger::LMK_Information);
}
// ..........................................................
Smp::IComponent* Simulator::CreateInstance(Smp::Uuid uuid,
                            Smp::String8 name,
                            Smp::String8 description,
                            Smp::IComposite* parent) {
    Smp::IComponent* res=nullptr;
    for (auto fac: _compFactories) {
        if (fac->GetUuid()==uuid) {
            res=fac->CreateInstance(name,description,
                        parent==nullptr?this:parent);
            // TODO is it required to add new instance in a container when
            // the parent is set?
            if (dynamic_cast<Smp::IModel*>(res)) {
                _models->AddComponent(res);
            }
            if (dynamic_cast<Smp::IService*>(res)) {
                _services->AddComponent(res);
            }
            // TODO check it is needed to pulish/configure/connect immediately
            // according to current simulator state.
            break;
        }
    }
    // When no factory is found, Smp header tels to return null. So nothing
    // particular to do since res is initialized as nullptr.
    return res;
}
// ..........................................................
Smp::IFactory* Simulator::GetFactory(Smp::Uuid uuid) const {
    for (auto fac: _compFactories) {
        if (fac->GetUuid()==uuid) {
            return fac;
        }
    }
    return nullptr;
}
// ..........................................................
void Simulator::LoadLibrary(Smp::String8 name) {
    std::string libName=name;
    simph::sys::DLib* fLib=nullptr;
    for (auto lib: _libs) {
        if (lib->getName()==libName) {
            fLib=lib;
        }
    }
    if (fLib==nullptr) {
        fLib=new simph::sys::DLib(name);
        auto init=fLib->getEntry<bool (*)(Smp::ISimulator*, Smp::Publication::ITypeRegistry* tReg)>("Initialise");
        if (init!=nullptr) {
            init(this,_typeRegistry);
        }
        _libs.push_back(fLib);
    }
}
// ..........................................................
Smp::FactoryCollection* Simulator::GetFactories() const {
// TODO
LOGE("Simulator::Fectories not implemented yet!")
return nullptr;
}
// ..........................................................
Smp::Publication::ITypeRegistry* Simulator::GetTypeRegistry() const {
    return _typeRegistry;
}

}} // namespace simph::kern
