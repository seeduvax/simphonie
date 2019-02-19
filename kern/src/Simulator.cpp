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

#include "simph/kern/DuplicateName.hpp"
#include "simph/kern/Logger.hpp"
#include "simph/kern/ObjectsRegistry.hpp"
#include "simph/kern/Scheduler.hpp"
#include "simph/kern/TimeKeeper.hpp"

// --------------------------------------------------------------------
// ..........................................................
namespace Smp {
constexpr Smp::Char8 Smp::ISimulator::SMP_SimulatorServices[];
constexpr Smp::Char8 Smp::ISimulator::SMP_SimulatorModels[];

std::ostream& operator << (std::ostream& os, 
                                const Smp::SimulatorStateKind& obj) {
    os << (Smp::Int32)obj;
    switch (obj) {
        case Smp::SimulatorStateKind::SSK_Building:
            os << "(SSK_Building)";
            break;
        case Smp::SimulatorStateKind::SSK_Connecting:
            os << "(SSK_Connecting)";
            break;
        case Smp::SimulatorStateKind::SSK_Initialising:
            os << "(SSK_Initialising)";
            break;
        case Smp::SimulatorStateKind::SSK_Standby:
            os << "(SSK_Standby)";
            break;
        case Smp::SimulatorStateKind::SSK_Executing:
            os << "(SSK_Executing)";
            break;
        case Smp::SimulatorStateKind::SSK_Storing:
            os << "(SSK_Storing)";
            break;
        case Smp::SimulatorStateKind::SSK_Restoring:
            os << "(SSK_Restoring)";
            break;
        case Smp::SimulatorStateKind::SSK_Reconnecting:
            os << "(SSK_Reconnecting)";
            break;
        case Smp::SimulatorStateKind::SSK_Exiting:
            os << "(SSK_Exiting)";
            break;
        case Smp::SimulatorStateKind::SSK_Aborting:
            os << "(SSK_Aborting)";
            break;
        default:
            os << "()";
            break;
    }
    return os;
}
}
// ..........................................................
namespace simph {
	namespace kern {
// --------------------------------------------------------------------
// ..........................................................
Simulator::Simulator(Smp::String8 name,Smp::String8 descr,
                Smp::IObject* parent): Object(name,descr,parent),
                            Composite(name,descr,parent),
                    _initEntryPoints("InitEntryPoints","",this),
                    _compFactories("ComponentFactories","",this) {
    addContainer(Smp::ISimulator::SMP_SimulatorModels);
    _models=GetContainer(Smp::ISimulator::SMP_SimulatorModels);
    addContainer(Smp::ISimulator::SMP_SimulatorServices);
    _services=GetContainer(Smp::ISimulator::SMP_SimulatorServices);
    _logger=new Logger("Logger","Logging service",_services);
    _scheduler=new Scheduler("Scheduler","Schedule service",_services);
    _timeKeeper=new TimeKeeper("TimeKeeper","Time service",_services);
    _services->AddComponent(_logger);
    _services->AddComponent(_scheduler);
    _services->AddComponent(_timeKeeper);
    _registry=new ObjectsRegistry("reg","Objects registry and resolver",_services);
    _services->AddComponent(_registry);
    _registry->add(this);
    _registry->add(_models);
    _registry->add(_services);
    setState(Smp::SimulatorStateKind::SSK_Building);
}
// ..........................................................
Simulator::~Simulator() {
    delete _logger;
}
// --------------------------------------------------------------------
// ..........................................................
void Simulator::setState(Smp::SimulatorStateKind newState) {
    _state=newState;
    std::ostringstream msg;
    msg << "state changed: " << _state;
    _logger->Log(this,msg.str().c_str());
}
// ..........................................................
void Simulator::Initialise() {
    if (_state!=Smp::SimulatorStateKind::SSK_Standby) {
        // TODO throw exception
        return;
    }
    setState(Smp::SimulatorStateKind::SSK_Initialising);
    for (auto ep: _initEntryPoints) {
        ep->Execute();
    }
    setState(Smp::SimulatorStateKind::SSK_Standby);
}
// ..........................................................
void Simulator::publish(Smp::IComponent* comp) {
    if (comp->GetState()==Smp::ComponentStateKind::CSK_Created) {
        _registry->add(comp);
        comp->Publish(_registry);
    }
}
// ..........................................................
void Simulator::Publish() {
    if (_state!=Smp::SimulatorStateKind::SSK_Building) {
        // TODO throw exception
        return;
    }
    for (auto service: *(_services->GetComponents())) {
        publish(service);
    }
    for (auto model: *(_models->GetComponents())) {
        publish(model);
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
    if (_state!=Smp::SimulatorStateKind::SSK_Building) {
        // TODO throw exception
        return;
    }
    for (auto service: *(_services->GetComponents())) {
        configure(service);
    }
    for (auto model: *(_models->GetComponents())) {
        configure(model);
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
    if (_state!=Smp::SimulatorStateKind::SSK_Building) {
        // TODO throw exception
        return;
    }
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
// ..........................................................
void Simulator::Run() {
    if (_state!=Smp::SimulatorStateKind::SSK_Standby) {
        // TODO throw exception
        return;
    }
// TODO start sequencer...
    setState(Smp::SimulatorStateKind::SSK_Executing);
}
// ..........................................................
void Simulator::Hold() {
    if (_state!=Smp::SimulatorStateKind::SSK_Executing) {
        // TODO throw exception
        return;
    }
// TODO stop sequencer...
    setState(Smp::SimulatorStateKind::SSK_Standby);
}
// ..........................................................
void Simulator::Store(Smp::String8 filename) {
    if (_state!=Smp::SimulatorStateKind::SSK_Standby) {
        // TODO throw exception
        return;
    }
    setState(Smp::SimulatorStateKind::SSK_Storing);
// TODO serialize models states in file
_logger->Log(this,"Simulator::Store(filename) not implemented yet!",Smp::Services::ILogger::LMK_Error);
    setState(Smp::SimulatorStateKind::SSK_Standby);
}
// ..........................................................
void Simulator::Restore(Smp::String8 filename) {
    if (_state!=Smp::SimulatorStateKind::SSK_Standby) {
        // TODO throw exception
        return;
    }
    setState(Smp::SimulatorStateKind::SSK_Restoring);
// TODO deserialize models states from file
_logger->Log(this,"Simulator::Restore(filename) not implemented yet!",Smp::Services::ILogger::LMK_Error);
    setState(Smp::SimulatorStateKind::SSK_Standby);
}
// ..........................................................
void Simulator::Reconnect(Smp::IComponent* root) {
    if (_state!=Smp::SimulatorStateKind::SSK_Standby) {
        // TODO throw exception
        return;
    }
    // TODO is it needed to change state while running this?
    connect(root);
}
// ..........................................................
void Simulator::Exit() {
    if (_state!=Smp::SimulatorStateKind::SSK_Standby) {
        // TODO throw exception
        return;
    }
    setState(Smp::SimulatorStateKind::SSK_Exiting);
// TODO shutdown everything...
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
    if (_models->GetComponent(model->GetName())==nullptr) {
        _models->AddComponent(model);
    }
    else {
        throw DuplicateName(_models,model->GetName());
    }
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
// TODO implement!
return nullptr;
}
// ..........................................................
Smp::Services::ILinkRegistry* Simulator::GetLinkRegistry() const {
// TODO implement!
return nullptr;
}
// ..........................................................
Smp::Services::IResolver* Simulator::GetResolver() const {
    return _registry;
}
// ..........................................................
void Simulator::RegisterFactory(Smp::IFactory* componentFactory) {
// TODO implement!
}
// ..........................................................
Smp::IComponent* Simulator::CreateInstance(Smp::Uuid uuid,
                            Smp::String8 name,
                            Smp::String8 description,
                            Smp::IComposite* parent) {
// TODO implement!
return nullptr;
}
// ..........................................................
Smp::IFactory* Simulator::GetFactory(Smp::Uuid uuid) const {
// TODO implement!
return nullptr;
}

}} // namespace simph::kern
