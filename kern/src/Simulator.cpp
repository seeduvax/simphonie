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
}} // namespace simph::kern
