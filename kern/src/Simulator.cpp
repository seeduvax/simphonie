/*
 * @file Simulator.cpp
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simphonie/kern/Simulator.hpp"
#include "simphonie/kern/EventManager.hpp"
#include "simphonie/kern/ExDuplicateUuid.hpp"
#include "simphonie/kern/ExFileNotFound.hpp"
#include "simphonie/kern/LinkRegistry.hpp"
#include "simphonie/kern/Logger.hpp"
#include "simphonie/kern/Resolver.hpp"
#include "simphonie/kern/Scheduler.hpp"
#include "simphonie/kern/TimeKeeper.hpp"
#include "simphonie/kern/TypeRegistry.hpp"
#include "simdeck/ExInvalidComponentState.hpp"
#include "simphonie/sys/Synchro.hpp"

#include "Smp/IOutputField.h"
#include "Smp/IModel.h"


// ..........................................................
namespace simphonie {
namespace kern {
// --------------------------------------------------------------------
// ..........................................................
Simulator::Simulator(Smp::String8 name, Smp::String8 descr, Smp::IObject* parent)
    : Parent(name, descr, parent),
      _initEntryPoints("InitEntryPoints", "", this),
      _compFactories("ComponentFactories", "", this) {
    addContainer(Smp::ISimulator::SMP_SimulatorModels);
    _models = GetContainer(Smp::ISimulator::SMP_SimulatorModels);
    addContainer(Smp::ISimulator::SMP_SimulatorServices);
    _services = GetContainer(Smp::ISimulator::SMP_SimulatorServices);
    _logger = new Logger("Logger", "Logging service", this);
    _scheduler = new Scheduler("Scheduler", "Schedule service", this);
    _timeKeeper = new TimeKeeper("TimeKeeper", "Time service", this);
    _eventMgr = new EventManager("EventManager", "Event handling service", this);
    _linkRegistry = new LinkRegistry("LinkRegistry", "Link registry service", this);
    TypeRegistry* tr = new TypeRegistry("TypeRegistry", "Type registry service", this);
    _typeRegistry = tr;

    _epStart = EntryPoint::Create("start","",this,&Simulator::epStart);
    _epStop = EntryPoint::Create("stop","",this,&Simulator::epStop);

    _services->AddComponent(tr);
    _resolver = new Resolver("Resolver", "Objects registry and resolver", this);
    _services->AddComponent(_scheduler);
    _services->AddComponent(_timeKeeper);
    _services->AddComponent(_eventMgr);
    _services->AddComponent(_linkRegistry);
    _services->AddComponent(_resolver);
    // add logger at the end to be sure it will be the last removed. (after scheduler at least)
    _services->AddComponent(_logger);
    setState(Smp::SimulatorStateKind::SSK_Building);

}
// ..........................................................
Simulator::~Simulator() {
    for (auto pub : _publications) {
        // TODO consider delete the publication earlier in the simulator
        // life cycle since publication should not be used further the 
        // simulator building phase.
        delete pub;
    }
    for (auto model : *(_models->GetComponents())) {
        delete model;
    }
    for (auto service : *(_services->GetComponents())) {
        delete service;
    }
    for (auto lib : _libs) {
        auto finalizeFunc = lib->getEntry<bool (*)()>("Finalise");
        if (finalizeFunc != nullptr) {
            finalizeFunc();
        }
        delete lib;
    }
    delete _epStart;
    delete _epStop;
}
// --------------------------------------------------------------------
// ..........................................................
void Simulator::setState(Smp::SimulatorStateKind newState) {
    if (_state != newState) {
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
        _state = newState;
        std::ostringstream msg;
        msg << "state changed: " << _state;
        _logger->Log(this, msg.str().c_str(), Smp::Services::ILogger::LMK_Event);
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
bool Simulator::checkState(Smp::String8 opName, Smp::SimulatorStateKind expState) {
    bool res = _state == expState;
    if (!res) {
        std::ostringstream msg;
        msg << opName << " request ingored, operation not available at current state: " << _state;
        _logger->Log(this, msg.str().c_str(), Smp::Services::ILogger::LMK_Warning);
    }
    return res;
}
// ..........................................................
void Simulator::Initialise() {
    if (checkState("Initialise", Smp::SimulatorStateKind::SSK_Standby)) {
        setState(Smp::SimulatorStateKind::SSK_Initialising);
        for (auto ep : _initEntryPoints) {
            ep->Execute();
        }
        setState(Smp::SimulatorStateKind::SSK_Standby);
    }
}
// ..........................................................
void Simulator::doPublish(Smp::IComponent* comp) {
    if (comp->GetState() == Smp::ComponentStateKind::CSK_Created) {
        LOGI("Publishing component : " << comp->GetName() << " " << comp->GetState());
        Smp::IPublication* pub=new Publication(comp,_typeRegistry);
        _publications.push_back(pub);
        comp->Publish(pub);
        // forward publication to children if component is a composite.
        auto composite=dynamic_cast<Smp::IComposite*>(comp);
        if (composite!=nullptr) {
            for (auto container: *(composite->GetContainers())) {
                for (auto subcomp: *(container->GetComponents())) {
                    doPublish(subcomp);
                }
            }
        }
    }
    else {
        throw simdeck::ExInvalidComponentState(comp,comp->GetState(),
                Smp::ComponentStateKind::CSK_Created);
    }
}

// ..........................................................
void Simulator::Publish() {
    if (checkState("Publish", Smp::SimulatorStateKind::SSK_Building)) {
        // TODO: shall also check SMP_LeavingBuilding event handling is not in progress.
        // TODO: ECSS-E-ST-40-07C document requests in §5.3.7 to switch into
        // SMP_Publishing state from here until it is completed with related
        // enter/leave event emission, but:
        // - related state/event identifiers are missing from the SMP header files.
        // - at this step, the event manager itself may not be ready yet (itself published).
        // publish services.
        for (auto service : *(_services->GetComponents())) {
            doPublish(service);
        }
        // publish models
        for (auto model : *(_models->GetComponents())) {
            doPublish(model);
        }
    }
}
// ..........................................................
void Simulator::doConfigure(Smp::IComponent* comp) {
    if (comp->GetState() == Smp::ComponentStateKind::CSK_Created) {
        doPublish(comp);
    }
    if (comp->GetState() == Smp::ComponentStateKind::CSK_Publishing) {
        LOGI("Configuring component : " << comp->GetName());
        comp->Configure(_logger);
        // forward configuration to children if component is a composite.
        auto composite=dynamic_cast<Smp::IComposite*>(comp);
        if (composite!=nullptr) {
            for (auto container: *(composite->GetContainers())) {
                for (auto subcomp: *(container->GetComponents())) {
                    doConfigure(subcomp);
                }
            }
        }
    }
    else {
        throw simdeck::ExInvalidComponentState(comp,comp->GetState(),
                Smp::ComponentStateKind::CSK_Publishing);
    }
}
// ..........................................................
void Simulator::Configure() {
    if (checkState("Configure", Smp::SimulatorStateKind::SSK_Building)) {
        for (auto service : *(_services->GetComponents())) {
            doConfigure(service);
        }
        for (auto model : *(_models->GetComponents())) {
            doConfigure(model);
        }
    }
}
// ..........................................................
void Simulator::doConnect(Smp::IComponent* comp) {
    if (    comp->GetState() == Smp::ComponentStateKind::CSK_Created ||
            comp->GetState() == Smp::ComponentStateKind::CSK_Publishing) {
        doConfigure(comp);
    }
    if (comp->GetState() == Smp::ComponentStateKind::CSK_Configured) {
        LOGI("Connecting component : " << comp->GetName());
        comp->Connect(this);
        // forward connect to children if component is a composite.
        auto composite=dynamic_cast<Smp::IComposite*>(comp);
        if (composite!=nullptr) {
            for (auto container: *(composite->GetContainers())) {
                for (auto subcomp: *(container->GetComponents())) {
                    doConnect(subcomp);
                }
            }
        }
    }
    else {
        throw simdeck::ExInvalidComponentState(comp,comp->GetState(),
                Smp::ComponentStateKind::CSK_Configured);
    }
}
// ..........................................................
void Simulator::Connect() {
    if (checkState("Connect", Smp::SimulatorStateKind::SSK_Building)) {
        setState(Smp::SimulatorStateKind::SSK_Connecting);
        for (auto service : *(_services->GetComponents())) {
            doConnect(service);
        }
        for (auto model : *(_models->GetComponents())) {
            doConnect(model);
        }
        setState(Smp::SimulatorStateKind::SSK_Standby);
        Initialise();
    }
}
// ..........................................................
void Simulator::Run() {
    if (checkState("Run", Smp::SimulatorStateKind::SSK_Standby)) {
        _scheduler->AddImmediateEvent(_epStart);
        setState(Smp::SimulatorStateKind::SSK_Executing);
    }
}
// ..........................................................
void Simulator::Hold(Smp::Bool immediate) {
    // TODO manage immediate...
    // But not sure it will be so easy for a multi-threaded scheduler...
    if (checkState("Hold", Smp::SimulatorStateKind::SSK_Executing)) {
        if (immediate) {
            _scheduler->AddImmediateEvent(_epStop);
        }
        else {
            // schedule 1ns after now to ensure all event scheduled at now
            // that may also schedule events at now are executed before closing.
            _scheduler->AddSimulationTimeEvent(_epStop,1);
        }
        if (simphonie::sys::Thread::GetCurrentThreadId()!=_schedulerThreadId) {
            // Wait for stop process completion only when caller thread is
            // not the scheduler thread to not hang the scheduler itself.
            Synchronized(_mutex)
            _waitingStop=true;
            while (_waitingStop) {
                MonitorWait(_monitor);
            }    
        }
    }
}
// ..........................................................
void Simulator::Store(Smp::String8 filename) {
    if (checkState("Store", Smp::SimulatorStateKind::SSK_Standby)) {
        setState(Smp::SimulatorStateKind::SSK_Storing);
        // TODO serialize models states in file
        _logger->Log(this, "Simulator::Store(filename) not implemented yet!", Smp::Services::ILogger::LMK_Error);
        setState(Smp::SimulatorStateKind::SSK_Standby);
    }
}
// ..........................................................
void Simulator::Restore(Smp::String8 filename) {
    if (checkState("Restore", Smp::SimulatorStateKind::SSK_Standby)) {
        setState(Smp::SimulatorStateKind::SSK_Restoring);
        // TODO deserialize models states from file
        _logger->Log(this, "Simulator::Restore(filename) not implemented yet!", Smp::Services::ILogger::LMK_Error);
        setState(Smp::SimulatorStateKind::SSK_Standby);
    }
}
// ..........................................................
void Simulator::Reconnect(Smp::IComponent* root) {
    if (checkState("Reconnect", Smp::SimulatorStateKind::SSK_Standby)) {
        // TODO is it needed to change state while running this?
        // Page 89 : After returning from the SMP_LeavingStandby global event, the simulation state is changed to
        // “Reconnecting” state.
        doConnect(root);
    }
}
// ..........................................................
void Simulator::Exit() {
    if (checkState("Exit", Smp::SimulatorStateKind::SSK_Standby)) {
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
    // replace the default mandatory service when service to add is a
    // remplacement implementation.
    overrideService(&_logger,service);
    overrideService(&_eventMgr,service);
    overrideService(&_timeKeeper,service);
    overrideService(&_scheduler,service);
    overrideService(&_resolver,service);
    overrideService(&_linkRegistry,service);
    // finally add service.
    _services->AddComponent(service);
}
// ..........................................................
Smp::IService* Simulator::GetService(Smp::String8 name) const {
    return dynamic_cast<Smp::IService*>(_services->GetComponent(name));
}
// ..........................................................
void Simulator::AddInitEntryPoint(Smp::IEntryPoint* ep) {
    if (_state == Smp::SimulatorStateKind::SSK_Building || _state == Smp::SimulatorStateKind::SSK_Connecting
        || _state == Smp::SimulatorStateKind::SSK_Standby) {
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
    return _resolver;
}
// ..........................................................
void Simulator::RegisterFactory(Smp::IFactory* componentFactory) {
    for (auto fac : _compFactories) {
        if (fac->GetUuid() == componentFactory->GetUuid()) {
            throw ExDuplicateUuid(this, fac->GetName(), componentFactory->GetName());
        }
    }
    _compFactories.push_back(componentFactory);
    std::ostringstream msg;
    msg << "Registered component factory: " << componentFactory->GetName() << " Uuid: " << componentFactory->GetUuid();
    _logger->Log(this, msg.str().c_str(), Smp::Services::ILogger::LMK_Information);
}
// ..........................................................
Smp::IComponent* Simulator::CreateInstance(Smp::Uuid uuid, Smp::String8 name, Smp::String8 description,
                                           Smp::IComposite* parent) {
    Smp::IComponent* res = nullptr;
    for (auto fac : _compFactories) {
        if (fac->GetUuid() == uuid) {
            res = fac->CreateInstance(name, description, parent == nullptr ? this : parent);
            // TODO is it required to add new instance in a container when
            // the parent is set?
            // may trouble publication/configure/connect loops since the same
            // component may be processed twice: one from the "root" containers
            // processing, and one through the recusive composite component
            // processing.
            if (dynamic_cast<Smp::IModel*>(res)) {
                _models->AddComponent(res);
            }
            if (dynamic_cast<Smp::IService*>(res)) {
                _services->AddComponent(res);
            }
            break;
        }
    }

    // When no factory is found, Smp header tells to return null. So nothing
    // particular to do since res is initialized as nullptr.
    return res;
}
// ..........................................................
Smp::IFactory* Simulator::GetFactory(Smp::Uuid uuid) const {
    for (auto fac : _compFactories) {
        if (fac->GetUuid() == uuid) {
            return fac;
        }
    }
    return nullptr;
}
// ..........................................................
void Simulator::LoadLibrary(Smp::String8 name, Smp::LibraryLoadFlag loadFlag) {
    // TODO take care of loadFlag
    std::string libName = name;
    simphonie::sys::DLib* fLib = nullptr;
    for (auto lib : _libs) {
        if (lib->getName() == libName) {
            fLib = lib;
        }
    }
    if (fLib == nullptr) {
        try {
            fLib = new simphonie::sys::DLib(name);
            auto init =
                fLib->getEntry<bool (*)(Smp::ISimulator*, Smp::Publication::ITypeRegistry * tReg)>("Initialise");
            if (init != nullptr) {
                init(this, _typeRegistry);
            }
            _libs.push_back(fLib);
        }
        catch (std::runtime_error ex) {
            throw simphonie::kern::ExFileNotFound(this, name);
        }
    }
}
// ..........................................................
const Smp::FactoryCollection* Simulator::GetFactories() const {
    return &_compFactories;
}
// ..........................................................
Smp::Publication::ITypeRegistry* Simulator::GetTypeRegistry() const {
    return _typeRegistry;
}

/*
// ..........................................................
// TODO moved that to a dedicated connection service.
void Simulator::connect(std::string inputFieldPath, std::string outputFieldPath) {
    auto outputField = dynamic_cast<Smp::IOutputField*>(GetResolver()->ResolveAbsolute(outputFieldPath.c_str()));
    auto inputField = dynamic_cast<Smp::IField*>(GetResolver()->ResolveAbsolute(inputFieldPath.c_str()));

    if (inputField == nullptr) {
        // TODO add macro in sys module to ease this kind of throw
        // ex: S_THROW(std::runtime_error, "my reason:" << reason)
        // TODO throw right SMP exception rather than std::runtime error
        std::stringstream ss;
        ss << "Input field not found: " << inputFieldPath;
        throw std::runtime_error(ss.str().c_str());
    }
    if (outputField == nullptr) {
        std::stringstream ss;
        ss << "Output field not found: " << outputFieldPath;
        throw std::runtime_error(ss.str().c_str());
    }

    outputField->Connect(inputField);
}
*/
// ..........................................................
/*
void Simulator::schedule(std::string modelName, std::string entryPoint, uint32_t period) {
    // TODO handle errors
    auto model = dynamic_cast<Smp::IEntryPointPublisher*>(GetResolver()->ResolveAbsolute(modelName.c_str()));
    if (model == nullptr) {
        std::stringstream ss;
        ss << "Model " << modelName << " not found";
        throw std::runtime_error(ss.str().c_str());
    }
    auto ep = model->GetEntryPoint(entryPoint.c_str());
    if (ep == nullptr) {
        std::stringstream ss;
        ss << "EntryPoint " << entryPoint << " not found";
        throw std::runtime_error(ss.str().c_str());
    }
    GetScheduler()->AddSimulationTimeEvent(ep, 0, period, -1);
};
*/
// ..........................................................
/*
Smp::IComponent* Simulator::createSmpModel(Smp::String8 typeName, Smp::String8 name, Smp::String8 description) {
    Smp::IComponent* res = nullptr;
    for (auto fac : _compFactories) {
        if (std::string(fac->GetTypeName()) == std::string(typeName)
            || std::string(fac->GetName()) == std::string(typeName)) {
            res = fac->CreateInstance(name, description, this);
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
*/

// ..........................................................
void Simulator::epStart() {
    _schedulerThreadId=simphonie::sys::Thread::GetCurrentThreadId();
}
// ..........................................................
void Simulator::epStop() {
    setState(Smp::SimulatorStateKind::SSK_Standby);
    bool shallNotify=false;
    {
        Synchronized(_mutex);
        if (_waitingStop) {
            _waitingStop=false;
            shallNotify=true;
        }
    }
    if (shallNotify) {
        MonitorNotifyAll(_monitor);
    }
}

// ..........................................................
}  // namespace kern
}  // namespace simph
