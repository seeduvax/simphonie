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
#include "simph/kern/EventManager.hpp"
#include "simph/kern/ExDuplicateUuid.hpp"
#include "simph/kern/ExLibraryNotFound.hpp"
#include "simph/kern/LinkRegistry.hpp"
#include "simph/kern/Logger.hpp"
#include "simph/kern/Resolver.hpp"
#include "simph/kern/Scheduler.hpp"
#include "simph/kern/TimeKeeper.hpp"
#include "simph/kern/TypeRegistry.hpp"
#include "simph/smpdk/ExDuplicateName.hpp"

#include "Smp/IDataflowField.h"

#include "simph/kern/Builder.hpp"

// ..........................................................
namespace simph {
namespace kern {
// --------------------------------------------------------------------
// ..........................................................
Simulator::Simulator(Smp::String8 name, Smp::String8 descr, Smp::IObject* parent)
    : Composite(name, descr, parent),
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

    _services->AddComponent(tr);
    _resolver = new Resolver("Resolver", "Objects registry and resolver", this);
    _services->AddComponent(_logger);
    _services->AddComponent(_scheduler);
    _services->AddComponent(_timeKeeper);
    _services->AddComponent(_eventMgr);
    _services->AddComponent(_linkRegistry);
    _services->AddComponent(_resolver);
    // TODO make the simulator appear in resolver itself ?
    //  _resolver->publish(this);
    setState(Smp::SimulatorStateKind::SSK_Building);
}
// ..........................................................
Simulator::~Simulator() {
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
        auto pub = dynamic_cast<Publication*>(_resolver->publish(comp));
        comp->Publish(pub);
        Smp::IEntryPointPublisher* epp = dynamic_cast<Smp::IEntryPointPublisher*>(comp);
        if (epp != nullptr) {
            for (auto ep : *(epp->GetEntryPoints())) {
                pub->addChild(ep);
            }
        }
    }
    else {
        // TODO add Smpc exception ExInvalidComponentState
    }
}
// ..........................................................
void Simulator::Publish() {
    if (checkState("Publish", Smp::SimulatorStateKind::SSK_Building)) {
        for (auto service : *(_services->GetComponents())) {
            doPublish(service);
        }
        for (auto model : *(_models->GetComponents())) {
            doPublish(model);
        }
    }
}
// ..........................................................
void Simulator::doConfigure(Smp::IComponent* comp) {
    if (comp->GetState() == Smp::ComponentStateKind::CSK_Publishing) {
        LOGI("Configuring component : " << comp->GetName());
        comp->Configure(_logger);
        // TODO recursive call on childs.
    }
    else {
        // TODO add Smpc exception ExInvalidComponentState
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
    if (comp->GetState() == Smp::ComponentStateKind::CSK_Configured) {
        LOGI("Connecting component : " << comp->GetName());
        comp->Connect(this);
        // TODO recursive call on childs.
    }
    else {
        // TODO add Smpc expcetion ExInvalidComponentState
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
        _scheduler->start();
        setState(Smp::SimulatorStateKind::SSK_Executing);
    }
}
// ..........................................................
void Simulator::Hold(Smp::Bool immediate) {
    // TODO manage immediate...
    // But not sure it will be so easy for a multi-threaded scheduler...
    if (checkState("Hold", Smp::SimulatorStateKind::SSK_Executing)) {
        _scheduler->stop();
        setState(Smp::SimulatorStateKind::SSK_Standby);
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
    for (auto fac : _compFactories) {
        if (fac->GetUuid() == uuid) {
            return fac;
        }
    }
    return nullptr;
}
// ..........................................................
void Simulator::LoadLibrary(Smp::String8 name) {
    std::string libName = name;
    simph::sys::DLib* fLib = nullptr;
    for (auto lib : _libs) {
        if (lib->getName() == libName) {
            fLib = lib;
        }
    }
    if (fLib == nullptr) {
        try {
            fLib = new simph::sys::DLib(name);
            auto init =
                fLib->getEntry<bool (*)(Smp::ISimulator*, Smp::Publication::ITypeRegistry * tReg)>("Initialise");
            if (init != nullptr) {
                init(this, _typeRegistry);
            }
            _libs.push_back(fLib);
        }
        catch (std::runtime_error ex) {
            throw simph::kern::ExLibraryNotFound(this, name);
        }
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
// ..........................................................
void Simulator::connect(std::string inputFieldPath, std::string outputFieldPath) {
    // TODO handle input/output field inversion
    auto output_field = dynamic_cast<Smp::IDataflowField*>(GetResolver()->ResolveAbsolute(outputFieldPath.c_str()));
    auto input_field = dynamic_cast<Smp::IDataflowField*>(GetResolver()->ResolveAbsolute(inputFieldPath.c_str()));

    if (input_field == nullptr) {
        // TODO add macro in sys module to ease this kind of throw
        // ex: S_THROW(std::runtime_error, "my reason:" << reason)
        std::stringstream ss;
        ss << "Input field not found: " << inputFieldPath;
        throw std::runtime_error(ss.str().c_str());
    }
    if (output_field == nullptr) {
        std::stringstream ss;
        ss << "Output field not found: " << outputFieldPath;
        throw std::runtime_error(ss.str().c_str());
    }

    // TODO handle connection error
    // output_field->Connect(input_field);
    input_field->Connect(output_field);
}
// ..........................................................
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
// ..........................................................
void Simulator::setValue(std::string fieldPath, float value) {
    // TODO handle errors
    auto field = dynamic_cast<simph::kern::Field*>(GetResolver()->ResolveAbsolute(fieldPath.c_str()));
    Smp::AnySimple SmpVal;
    SmpVal.SetValue(Smp::PrimitiveTypeKind::PTK_Float64, value);
    field->SetValue(SmpVal);
};
// ..........................................................
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

}  // namespace kern
}  // namespace simph
