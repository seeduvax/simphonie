/*
 * @file FieldPublisher.cpp
 *
 * Copyright 2026 Sebastien Devaux. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simdeck/FieldPublisher.hpp"
#include "Smp/IField.h"
#include "Smp/ISimpleArrayField.h"
#include "simdeck/SimpleField.hpp"
#include "simdeck/SimpleType.hpp"
#include <iostream>
#include <mutex>
#include <sstream>
#include <iostream>

namespace simdeck {
SimpleType* uint8Type = new SimpleType(Smp::Uuids::Uuid_UInt8, Smp::PrimitiveTypeKind::PTK_UInt8, "uint8",
                    "Eight bytes signed double data type");

// --------------------------------------------------------------------
// ..........................................................
FieldPublisher::FieldPublisher(Smp::IObject* pubObj) :
 _pubObj(pubObj), _published() {
}
// ..........................................................
FieldPublisher::~FieldPublisher() {
}

void FieldPublisher::addField(Smp::IField* field) {
    _published.push_back(field);
}

// --------------------------------------------------------------------
// IPublication implementation
// ..........................................................
Smp::Publication::ITypeRegistry* FieldPublisher::GetTypeRegistry() const {
    return nullptr;
}
// ..........................................................
Smp::IField* FieldPublisher::PublishField(Smp::String8 name, Smp::String8 description, Smp::Char8* address, Smp::ViewKind view,
                               Smp::Bool state, Smp::Bool input, Smp::Bool output) {
    auto f = SimpleField::Create(name, description, view, uint8Type, address,
                                 state, input, output, _pubObj);
    addField(f);
    return f;
}
// ..........................................................
Smp::IField* FieldPublisher::PublishField(Smp::String8 name, Smp::String8 description, Smp::Bool* address, Smp::ViewKind view,
                               Smp::Bool state, Smp::Bool input, Smp::Bool output) {
    auto f = SimpleField::Create(name, description, view, uint8Type, address, state,
                                 input, output, _pubObj);
    addField(f);
    return f;
}
// ..........................................................
Smp::IField* FieldPublisher::PublishField(Smp::String8 name, Smp::String8 description, Smp::Int8* address, Smp::ViewKind view,
                               Smp::Bool state, Smp::Bool input, Smp::Bool output) {
    auto f = SimpleField::Create(name, description, view, uint8Type, address, state,
                                 input, output, _pubObj);
    addField(f);
    return f;
}
// ..........................................................
Smp::IField* FieldPublisher::PublishField(Smp::String8 name, Smp::String8 description, Smp::Int16* address, Smp::ViewKind view,
                               Smp::Bool state, Smp::Bool input, Smp::Bool output) {
    auto f = SimpleField::Create(name, description, view, uint8Type, address,
                                 state, input, output, _pubObj);
    addField(f);
    return f;
}
// ..........................................................
Smp::IField* FieldPublisher::PublishField(Smp::String8 name, Smp::String8 description, Smp::Int32* address, Smp::ViewKind view,
                               Smp::Bool state, Smp::Bool input, Smp::Bool output) {
    auto f = SimpleField::Create(name, description, view, uint8Type, address,
                                 state, input, output, _pubObj);
    addField(f);
    return f;
}
// ..........................................................
Smp::IField* FieldPublisher::PublishField(Smp::String8 name, Smp::String8 description, Smp::Int64* address, Smp::ViewKind view,
                               Smp::Bool state, Smp::Bool input, Smp::Bool output) {
    auto f = SimpleField::Create(name, description, view, uint8Type, address,
                                 state, input, output, _pubObj);
    addField(f);
    return f;
}
// ..........................................................
Smp::IField* FieldPublisher::PublishField(Smp::String8 name, Smp::String8 description, Smp::UInt8* address, Smp::ViewKind view,
                               Smp::Bool state, Smp::Bool input, Smp::Bool output) {
    auto f = SimpleField::Create(name, description, view, uint8Type, address,
                                 state, input, output, _pubObj);
    addField(f);
    return f;
}
// ..........................................................
Smp::IField* FieldPublisher::PublishField(Smp::String8 name, Smp::String8 description, Smp::UInt16* address, Smp::ViewKind view,
                               Smp::Bool state, Smp::Bool input, Smp::Bool output) {
    auto f = SimpleField::Create(name, description, view, uint8Type, address,
                                 state, input, output, _pubObj);
    addField(f);
    return f;
}
// ..........................................................
Smp::IField* FieldPublisher::PublishField(Smp::String8 name, Smp::String8 description, Smp::UInt32* address, Smp::ViewKind view,
                               Smp::Bool state, Smp::Bool input, Smp::Bool output) {
    auto f = SimpleField::Create(name, description, view, uint8Type, address,
                                 state, input, output, _pubObj);
    addField(f);
    return f;
}
// ..........................................................
Smp::IField* FieldPublisher::PublishField(Smp::String8 name, Smp::String8 description, Smp::UInt64* address, Smp::ViewKind view,
                               Smp::Bool state, Smp::Bool input, Smp::Bool output) {
    auto f = SimpleField::Create(name, description, view, uint8Type, address,
                                 state, input, output, _pubObj);
    addField(f);
    return f;
}
// ..........................................................
Smp::IField* FieldPublisher::PublishField(Smp::String8 name, Smp::String8 description, Smp::Float32* address, Smp::ViewKind view,
                               Smp::Bool state, Smp::Bool input, Smp::Bool output) {
    auto f = SimpleField::Create(name, description, view, uint8Type, address,
                                 state, input, output, _pubObj);
    addField(f);
    return f;
}
// ..........................................................
Smp::IField* FieldPublisher::PublishField(Smp::String8 name, Smp::String8 description, Smp::Float64* address, Smp::ViewKind view,
                               Smp::Bool state, Smp::Bool input, Smp::Bool output) {
    auto f = SimpleField::Create(name, description, view, uint8Type, address,
                                 state, input, output, _pubObj);
    addField(f);
    return f;
}

// ..........................................................
Smp::IField*  FieldPublisher::PublishField(Smp::String8 name, Smp::String8 description, void* address, Smp::Uuid typeUuid,
                               Smp::ViewKind view, Smp::Bool state, Smp::Bool input, Smp::Bool output) {
    
    return nullptr;                                    
}
// ..........................................................
void FieldPublisher::PublishField(Smp::IField* field){                       
    addField(field);
}

// ..........................................................
Smp::ISimpleArrayField*  FieldPublisher::PublishArray(Smp::String8 name, Smp::String8 description, Smp::Int64 count, void* address,
                               Smp::PrimitiveTypeKind type, Smp::ViewKind view, Smp::Bool state, Smp::Bool input,
                               Smp::Bool output) {
    return nullptr;
}
// ..........................................................
Smp::IPublication*  FieldPublisher::PublishArray(Smp::String8 name, Smp::String8 description, Smp::ViewKind view,
                                             Smp::Bool state) {
    return nullptr;
}
// ..........................................................
Smp::IPublication*  FieldPublisher::PublishStructure(Smp::String8 name, Smp::String8 description, Smp::ViewKind view,
                                                 Smp::Bool state) {
    return nullptr;
}
// ..........................................................
Smp::Publication::IPublishOperation*  FieldPublisher::PublishOperation(
                    Smp::String8 name,
                    Smp::String8 description,
                    Smp::ViewKind view) {
    // TODO    
    return nullptr;
}
// ..........................................................
void FieldPublisher::PublishOperation(Smp::IOperation* operation) {
    // TODO
}
// ..........................................................
Smp::IProperty*  FieldPublisher::PublishProperty(
                        Smp::String8 name,
                        Smp::String8 description,
                        Smp::Uuid typeUuid,
                        Smp::AccessKind accessKind,
                        Smp::ViewKind view) {
    return nullptr;
}
// ..........................................................
void FieldPublisher::PublishProperty(Smp::IProperty* property) {
    return;
}
// ..........................................................
void FieldPublisher::Unpublish() {
    // TO DO remove 
    _published.clear();
}
// ..........................................................
Smp::IField*  FieldPublisher::GetField(Smp::String8 fullName) const {
    return _published.at(fullName);
}
// ..........................................................
const Smp::FieldCollection* FieldPublisher::GetFields() const {
    return &_published;
}
// ..........................................................
Smp::IProperty* FieldPublisher::GetProperty(Smp::String8 name) const {
    return nullptr;
}
// ..........................................................
const Smp::PropertyCollection* FieldPublisher::GetProperties() const {
     return nullptr;
}
// ..........................................................
Smp::IOperation* FieldPublisher::GetOperation(Smp::String8 name) const {
    return nullptr;
}
// ..........................................................
const Smp::OperationCollection* FieldPublisher::GetOperations() const {
    return nullptr;
}
} // namespace simdeck
