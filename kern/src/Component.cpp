/*
 * @file Component.cpp
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simph/kern/Component.hpp"
#include "simph/kern/InvalidComponentState.hpp"
#include <iostream>

// --------------------------------------------------------------------
// ..........................................................
namespace Smp {
std::ostream& operator << (std::ostream& os, 
                                const ComponentStateKind& obj) {
    os << (Smp::Int32)obj;
    switch (obj) {
        case ComponentStateKind::CSK_Created:
            os << "(CSK_Created)";
            break;
        case ComponentStateKind::CSK_Publishing:
            os << "(CSK_Publishing)";
            break;
        case ComponentStateKind::CSK_Configured:
            os << "(CSK_Configured)";
            break;
        case ComponentStateKind::CSK_Connected:
            os << "(CSK_Connected)";
            break;
        default:
            os << "()";
            break;
    }
    return os;
}
}
namespace simph {
	namespace kern {
// --------------------------------------------------------------------
// ..........................................................
Component::Component() {
    _state=Smp::ComponentStateKind::CSK_Created;
}
// ..........................................................
Component::~Component() {
}
// --------------------------------------------------------------------
// ..........................................................
Smp::ComponentStateKind Component::GetState() const {
    return _state;
}
// ..........................................................
void Component::publish(Smp::IPublication* receiver) {
}
void Component::Publish(Smp::IPublication* receiver) {
    if (_state!=Smp::ComponentStateKind::CSK_Created) {
        throw InvalidComponentState(this,_state,Smp::ComponentStateKind::CSK_Created);
    }
    publish(receiver);
    _state=Smp::ComponentStateKind::CSK_Publishing;
}
// ..........................................................
void Component::configure() {
}
void Component::Configure(Smp::Services::ILogger* logger) {
    if (_state!=Smp::ComponentStateKind::CSK_Publishing) {
        throw InvalidComponentState(this,_state,Smp::ComponentStateKind::CSK_Publishing);
    }
    _logger=logger;
    configure();
    _state=Smp::ComponentStateKind::CSK_Configured;
}
// ..........................................................
void Component::connect() {
}
void Component::Connect(Smp::ISimulator* simulator) {
    if (_state!=Smp::ComponentStateKind::CSK_Configured) {
        throw InvalidComponentState(this,_state,Smp::ComponentStateKind::CSK_Configured);
    }
    _simulator=simulator;
    connect();
    _state=Smp::ComponentStateKind::CSK_Connected;
}
// ..........................................................
Smp::IField* Component::GetField(Smp::String8 fullName) const {
    return _fields.at(fullName);
}
// ..........................................................
const Smp::FieldCollection* Component::GetFields() const {
    return &_fields;
}
// ..........................................................
const Smp::Uuid& Component::GetUuid() const {
    return _uuid;
}

}} // namespace simph::kern
