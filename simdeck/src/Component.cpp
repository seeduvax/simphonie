/*
 * @file Component.cpp
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simdeck/Component.hpp"
#include <iostream>
#include <sstream>
#include "Smp/ISimulator.h"
#include "Smp/ISimpleField.h"
#include "Smp/ISimpleArrayField.h"
#include "Smp/Services/IResolver.h"
#include "simdeck/ExInvalidComponentState.hpp"
#include "simdeck/ExInvalidFieldName.hpp"

// --------------------------------------------------------------------
// ..........................................................
namespace Smp {
std::ostream& operator<<(std::ostream& os, const ComponentStateKind& obj) {
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
}  // namespace Smp
namespace simdeck {
// --------------------------------------------------------------------
// ..........................................................
Component::Component(Smp::String8 name, Smp::String8 descr, Smp::IObject* parent)
    : Object(name, descr, parent),
      _state(Smp::ComponentStateKind::CSK_Created),
      _logger(nullptr),
      _linkRegistry(nullptr),
      _fields("Fields", "", this),
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
void Component::publish(Smp::IPublication* receiver) {}
void Component::Publish(Smp::IPublication* receiver) {
    if (_state != Smp::ComponentStateKind::CSK_Created) {
        throw ExInvalidComponentState(this, _state, Smp::ComponentStateKind::CSK_Created);
    }
    publish(receiver);
    _state = Smp::ComponentStateKind::CSK_Publishing;
}
// ..........................................................
void Component::configure() {}
void Component::Configure(Smp::Services::ILogger* logger, Smp::Services::ILinkRegistry* linkRegistry) {
    if (_state != Smp::ComponentStateKind::CSK_Publishing) {
        throw ExInvalidComponentState(this, _state, Smp::ComponentStateKind::CSK_Publishing);
    }
    _logger = logger;
    _linkRegistry = linkRegistry;
    configure();
    _state = Smp::ComponentStateKind::CSK_Configured;
}
// ..........................................................
Smp::ISimulator* Component::getSimulator() {
    if (_simulator != nullptr) {
        return _simulator;
    }
    Smp::IObject* obj = this;
    while (obj->GetParent() != nullptr) {
        obj = obj->GetParent();
    }
    _simulator = dynamic_cast<Smp::ISimulator*>(obj);
    if (_simulator == nullptr) {
        throw Exception(this,"Component not connected to a simulator.");
    }
    return _simulator;
}
// ..........................................................
void Component::connect() {}
void Component::Connect(Smp::ISimulator* simulator) {
    if (_state != Smp::ComponentStateKind::CSK_Configured) {
        throw ExInvalidComponentState(this, _state, Smp::ComponentStateKind::CSK_Configured);
    }
    _simulator = simulator;
    connect();
    _state = Smp::ComponentStateKind::CSK_Connected;
}
// ..........................................................
void Component::disconnect() {}
void Component::Disconnect() {
    if (_state != Smp::ComponentStateKind::CSK_Connected) {
        throw ExInvalidComponentState(this, _state, Smp::ComponentStateKind::CSK_Connected);
    }
    _simulator = nullptr;
    _logger = nullptr;
    _linkRegistry->RemoveLinks(this);
    _linkRegistry = nullptr;
    disconnect();
}
// ..........................................................
Smp::IField* Component::GetField(Smp::String8 fullName) const {
    return dynamic_cast<Smp::IField*>(resolveChild(fullName,this));
}
// ..........................................................
const Smp::FieldCollection* Component::GetFields() const {
    return &_fields;
}
// ..........................................................
const Smp::Uuid& Component::GetUuid() const {
    return _uuid;
}
// ..........................................................
Smp::AnySimple Component::GetSimpleValue(Smp::String8 fullName) const {
    // TODO such method should be called only from scheduler thread
    //  or any thread only when the simulator is not running.
    //  Or schedule a request to the scheduler and wait for result...
    auto f=dynamic_cast<Smp::ISimpleField*>(GetField(fullName));
    if (f!=nullptr) {
        return f->GetValue();
    }
    // TODO throw InvalidFieldName when found field is not a Simple field.
    throw ExInvalidFieldName(this,fullName);
}
// ..........................................................
void Component::SetSimpleValue(Smp::String8 fullName, Smp::AnySimple value) {
    auto f=dynamic_cast<Smp::ISimpleField*>(GetField(fullName));
    if (f!=nullptr) {
        f->SetValue(value);
    }
}
// ..........................................................
void Component::GetSimpleArrayValue(
        Smp::String8 fullName,
        Smp::UInt64 length,
        Smp::AnySimple* values,
        Smp::UInt64 startIndex) const {
    auto f=dynamic_cast<Smp::ISimpleArrayField*>(GetField(fullName));
    if (f!=nullptr) {
        f->GetValues(length, values, startIndex);
    }
    throw ExInvalidFieldName(this,fullName);
}
// ..........................................................
void Component::SetSimpleArrayValue(
        Smp::String8 fullName,
        Smp::UInt64 length,
        Smp::AnySimple* values,
        Smp::UInt64 startIndex) {
    auto f=dynamic_cast<Smp::ISimpleArrayField*>(GetField(fullName));
    if (f!=nullptr) {
        f->SetValues(length, values, startIndex);
    }
    throw ExInvalidFieldName(this,fullName);
}
// ..........................................................
Smp::Bool Component::AddChild(
        Smp::IObject* child,
        const Smp::ICollectionBase* collection) {
    if ( child==nullptr 
                || collection==nullptr 
                || GetChild(child->GetName())!=nullptr 
                || child->GetParent()!=this) {
        // Can't add child when 
        //   - a child with the same name is already there
        //   - the child to add parent is not this component
        return false; 
    }
    if (collection==&_fields) {
        auto f=dynamic_cast<Smp::IField*>(child);
        if (f!=nullptr) {
            // check this is a valid field insertion request
            _fields.push_back(f);
            return true;
        }
    }
    else {
        auto c=dynamic_cast<Smp::IComposite*>(this);
        auto ch=dynamic_cast<Smp::IComponent*>(child);
        if (c!=nullptr && ch!=nullptr) {
            for (auto ct: *(c->GetContainers())) {
                if (ct->GetComponents()==collection) {
                    ct->AddComponent(ch);
                    return true;
                }
            }
        }
    }
    return false;
}
// ..........................................................
Smp::Bool Component::RemoveChild(
        Smp::IObject* child,
        const Smp::ICollectionBase* collection) {
    if (collection==&_fields) {
        return _fields.remove(dynamic_cast<Smp::IField*>(child));
    }
    else {
        auto c=dynamic_cast<Smp::IComposite*>(this);
        if (c!=nullptr) {
            for (auto ct: *(c->GetContainers())) {
                if (ct->GetComponents()==collection) {
                    auto toDelete=ct->GetComponent(child->GetName());
                    if (toDelete==child) {
                        ct->DeleteComponent(toDelete);
                    }
                    else {
                        return false;
                    }
                }
            }
        }
    }
    return false;
}
// ..........................................................
Smp::IObject* Component::IsChildInCollection(
        Smp::String8 child,
        const Smp::ICollectionBase* collection) const {
    if (collection==&_fields) {
        return GetField(child);
    }
    else {
        auto c=dynamic_cast<const Smp::IComposite*>(this);
        if (c!=nullptr) {
            for (auto ct: *(c->GetContainers())) {
                if (ct->GetComponents()==collection) {
                    return ct->GetComponent(child);
                }
            }
        }
    }
    return nullptr;
}

}  // namespace simdeck
