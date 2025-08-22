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

#include "Smp/IModel.h"
#include "Smp/IOutputField.h"
#include "simdeck/ExInvalidComponentState.hpp"
#include "simdeck/ExInvalidFile.hpp"
#include "simphonie/kern/EventManager.hpp"
#include "simphonie/kern/ExDuplicateUuid.hpp"
#include "simphonie/kern/LinkRegistry.hpp"
#include "simphonie/kern/Logger.hpp"
#include "simphonie/kern/LoggerAsync.hpp"
#include "simphonie/kern/LoggerFile.hpp"
#include "simphonie/kern/LoggerOStream.hpp"
#include "simphonie/kern/Resolver.hpp"
#include "simphonie/kern/Scheduler.hpp"
#include "simphonie/kern/StorageReader.hpp"
#include "simphonie/kern/StorageWriter.hpp"
#include "simphonie/kern/TimeKeeper.hpp"
#include "simphonie/kern/TypeRegistry.hpp"
#include "simphonie/sys/Synchro.hpp"

#define SMPLOGEV(msg) _logger->Log(this, msg, Smp::Services::ILogger::LMK_Event);
#define SMPLOGD(msg) _logger->Log(this, msg, Smp::Services::ILogger::LMK_Debug);
#define SMPLOGI(msg) _logger->Log(this, msg, Smp::Services::ILogger::LMK_Information);
#define SMPLOGW(msg) _logger->Log(this, msg, Smp::Services::ILogger::LMK_Warning);
#define SMPLOGE(msg) _logger->Log(this, msg, Smp::Services::ILogger::LMK_Error);
// ..........................................................
namespace simphonie {
namespace kern {
// --------------------------------------------------------------------
// ..........................................................
Simulator::Simulator(Smp::String8 name, Smp::String8 descr, Smp::IObject* parent)
    : Parent(name, descr, parent),
      _initEntryPoints(),
      _compFactories() {
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

    _ownedFactories.emplace_back(new simdeck::Factory<Logger>("Logger", "Logger", this));
    _ownedFactories.emplace_back(new simdeck::Factory<LoggerOStream>("LoggerOStream", "LoggerOStream", this));
    _ownedFactories.emplace_back(new simdeck::Factory<LoggerFile>("LoggerFile", "LoggerFile", this));
    _ownedFactories.emplace_back(new simdeck::Factory<LoggerAsync>("LoggerAsync", "LoggerAsync", this));
    for (auto& factory : _ownedFactories) {
        RegisterFactory(factory.get());
    }

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
        SMPLOGEV(msg.str().c_str());
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
        msg << opName << " request ignored, operation not available at current state: " << _state;
        SMPLOGW(msg.str().c_str());
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
        std::ostringstream msg;
        msg <<"Publishing component : " << comp->GetName();
        SMPLOGI(msg.str().c_str());
        Smp::IPublication* pub = new Publication(comp, this);
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
        bool completed=false;
        while (!completed) {
            auto refsize = _services->GetComponents()->size();
            for (auto service : *(_services->GetComponents())) {
                if (service->GetState() == Smp::ComponentStateKind::CSK_Created) {
                    doPublish(service);
                }
            }
            // service publication is completed when no new service has been
            // added during publication. Otherwise, a publication cycle shall
            // be done again.
            completed = refsize == _services->GetComponents()->size();
        }

        // publish models
        // Same publish loop policy apllied to service is not applied againd to
        // models, because models should not have strong coupling over other
        // models until Connec(), then it is not an issue to have models added
        // during publish or configured being published only from connect.
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
        std::ostringstream msg;
        msg <<"Configuring component : " << comp->GetName();
        SMPLOGI(msg.str().c_str());
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
        std::ostringstream msg;
        msg <<"Connecting component : " << comp->GetName();
        SMPLOGI(msg.str().c_str());
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
        std::ostringstream msg;
        msg << "Storing simulator state to " << filename;
        SMPLOGI(msg.str().c_str());
        StorageWriter writer(this, filename);
        writer.store();
        setState(Smp::SimulatorStateKind::SSK_Standby);
    }
}
// ..........................................................
void Simulator::Restore(Smp::String8 filename) {
    if (checkState("Restore", Smp::SimulatorStateKind::SSK_Standby)) {
        setState(Smp::SimulatorStateKind::SSK_Restoring);
        std::ostringstream msg;
        msg << "Restoring simulator state from " << filename;
        SMPLOGI(msg.str().c_str());
        StorageReader reader(this, filename, "StorageReader");
        reader.restore();
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
        // TODO Tres bien uniquement quand on utilise ISimulator comme un singleton
        this->~Simulator(); /* cleanup owned objects */
        exit(EXIT_SUCCESS); /* cleanup static objects and syscall to end program execution */
    }
}
// ..........................................................
void Simulator::Abort() {
    setState(Smp::SimulatorStateKind::SSK_Aborting);
    abort(); /* syscall to end program execution */
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
    SMPLOGI(msg.str().c_str());
}
// ..........................................................
Smp::IComponent* Simulator::CreateInstance(Smp::Uuid uuid, Smp::String8 name, Smp::String8 description,
                                           Smp::IComposite* parent) {
    Smp::IComponent* res = nullptr;
    for (auto fac : _compFactories) {
        if (fac->GetUuid() == uuid) {
            res = fac->CreateInstance(name, description, parent == nullptr ? this : parent);
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
void Simulator::LoadLibrary(Smp::String8 name, Smp::LibraryLoadingFlag loadFlag) {
    std::string libName = name;
    simphonie::sys::DLib* fLib = nullptr;
    for (auto lib : _libs) {
        if (lib->getName() == libName) {
            fLib = lib;
        }
    }
    if (fLib == nullptr) {
        try {
            bool global;
            switch (loadFlag) {
                case Smp::LibraryLoadingFlag::LLF_Auto:
                case Smp::LibraryLoadingFlag::LLF_Global:
                    global = true;
                    break;
                case Smp::LibraryLoadingFlag::LLF_Local:
                    global = false;
                    break;
            }
            fLib = new simphonie::sys::DLib(name, global);
            auto init =
                fLib->getEntry<bool (*)(Smp::ISimulator*, Smp::Publication::ITypeRegistry * tReg)>("Initialise");
            if (init != nullptr) {
                init(this, _typeRegistry);
            }
            _libs.push_back(fLib);
        }
        catch (std::runtime_error ex) {
            throw simdeck::ExInvalidFile(this, name, ex.what());
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

extern "C" {
Smp::ISimulator* CreateSimulator(Smp::String8 name, Smp::String8 descr, Smp::IObject* parent) {
    return new simphonie::kern::Simulator(name, descr, parent);
};
}
