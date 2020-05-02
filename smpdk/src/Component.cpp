/*
 * @file Component.cpp
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simph/smpdk/Component.hpp"
#include "simph/smpdk/ExInvalidComponentState.hpp"
#include <sstream> 

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
	namespace smpdk {
// --------------------------------------------------------------------
// ..........................................................
Component::Component(Smp::String8 name,
                Smp::String8 descr,
                Smp::IObject* parent):
                    Object(name,descr,parent),
                    _state(Smp::ComponentStateKind::CSK_Created),
                    _logger(nullptr),
                    _linkRegistry(nullptr),
                    _fields("Fields","",this),
                    _uuid(),
                    _simulator(nullptr) {
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
        throw ExInvalidComponentState(this,_state,Smp::ComponentStateKind::CSK_Created);
    }
    publish(receiver);
    _state=Smp::ComponentStateKind::CSK_Publishing;
}
// ..........................................................
void Component::configure() {
}
void Component::Configure(Smp::Services::ILogger* logger,
                         Smp::Services::ILinkRegistry* linkRegistry) {
    if (_state!=Smp::ComponentStateKind::CSK_Publishing) {
        throw ExInvalidComponentState(this,_state,Smp::ComponentStateKind::CSK_Publishing);
    }
    _logger=logger;
    _linkRegistry=linkRegistry;
    configure();
    _state=Smp::ComponentStateKind::CSK_Configured;
}
// ..........................................................
void Component::connect() {
}
void Component::Connect(Smp::ISimulator* simulator) {
    if (_state!=Smp::ComponentStateKind::CSK_Configured) {
        throw ExInvalidComponentState(this,_state,Smp::ComponentStateKind::CSK_Configured);
    }
    _simulator=simulator;
    connect();
    _state=Smp::ComponentStateKind::CSK_Connected;
}
// ..........................................................
void Component::disconnect() {
}
void Component::Disconnect() {
    _simulator=nullptr;
    _logger=nullptr;
    _linkRegistry=nullptr;
// TODO to be completed
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

}} // namespace simph::smpdk
