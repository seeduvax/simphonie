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

namespace simph {
	namespace kern {
// --------------------------------------------------------------------
// ..........................................................
Simulator::Simulator() {
}
// ..........................................................
Simulator::~Simulator() {
}
// --------------------------------------------------------------------
// ..........................................................
void Simulator::Initialise() {
    if (_state!=Smp::SimulatorStateKind::SSK_Standby) {
        // TODO throw exception
        return;
    }
    _state=Smp::SimulatorStateKind::SSK_Initialising;
    for (auto ep: _initEntryPoints) {
        ep->Execute();
    }
    _state=Smp::SimulatorStateKind::SSK_Standby;
}
// ..........................................................
void Simulator::publish(Smp::IComponent* comp) {
    if (comp->GetState()==Smp::ComponentStateKind::CSK_Created) {
// TODO            service->Publish(this);
// TODO recursive call on childs.
    }
}
// ..........................................................
void Simulator::Publish() {
    if (_state!=Smp::SimulatorStateKind::SSK_Building) {
        // TODO throw exception
        return;
    }
    for (auto service: _services) {
        publish(service);
    }
    for (auto model: _models) {
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
    for (auto service: _services) {
        configure(service);
    }
    for (auto model: _models) {
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
    _state=Smp::SimulatorStateKind::SSK_Connecting;
    for (auto service: _services) {
        connect(service);
    }
    for (auto model: _models) {
        connect(model);
    }
    _state=Smp::SimulatorStateKind::SSK_Standby;
    Initialise();
}
// ..........................................................
void Simulator::Run() {
    if (_state!=Smp::SimulatorStateKind::SSK_Standby) {
        // TODO throw exception
        return;
    }
// TODO start sequencer...
    _state=Smp::SimulatorStateKind::SSK_Executing;
}
// ..........................................................
void Simulator::Hold() {
    if (_state!=Smp::SimulatorStateKind::SSK_Executing) {
        // TODO throw exception
        return;
    }
// TODO stop sequencer...
    _state=Smp::SimulatorStateKind::SSK_Standby;
}
// ..........................................................
void Simulator::Store(Smp::String8 filename) {
    if (_state!=Smp::SimulatorStateKind::SSK_Standby) {
        // TODO throw exception
        return;
    }
    _state=Smp::SimulatorStateKind::SSK_Storing;
// TODO serialize models states in file
    _state=Smp::SimulatorStateKind::SSK_Standby;
}
// ..........................................................
void Simulator::Restore(Smp::String8 filename) {
    if (_state!=Smp::SimulatorStateKind::SSK_Standby) {
        // TODO throw exception
        return;
    }
    _state=Smp::SimulatorStateKind::SSK_Restoring;
// TODO deserialize models states from file
    _state=Smp::SimulatorStateKind::SSK_Standby;
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
    _state=Smp::SimulatorStateKind::SSK_Exiting;
// TODO shutdown everything...
}
// ..........................................................
void Simulator::Abort() {
    _state=Smp::SimulatorStateKind::SSK_Aborting;
    // TODO force stop of anything that is runnning.
}
// ..........................................................
Smp::SimulatorStateKind Simulator::GetState() const {
    return _state;
}
// ..........................................................
void Simulator::AddService(Smp::IService* service) {
    if (_services.at(service->GetName())==NULL) {
        _services.push_back(service);
    }
    else {
        // TODO throw Smp::DumplicateName
    }
}
// ..........................................................
Smp::IService* Simulator::GetService(Smp::String8 name) const {
    return _services.at(name);
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
    if (_models.at(model->GetName())==NULL) {
        _models.push_back(model);
    }
    else {
        // TODO throw Smp::DumplicateName
    }
}
// ..........................................................
Smp::Services::ILogger* Simulator::GetLogger() const {
    return _logger;
}
// ..........................................................
Smp::Services::ITimeKeeper* Simulator::GetTimeKeeper() const {
// TODO implement!
return nullptr;
}
// ..........................................................
Smp::Services::IScheduler* Simulator::GetScheduler() const {
// TODO implement!
return nullptr;
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
// TODO implement!
return nullptr;
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
