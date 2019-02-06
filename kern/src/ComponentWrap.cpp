/*
 * @file ComponentWrap.cpp
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simph/kern/ComponentWrap.hpp"
#include "simph/kern/Field.hpp"
#include "simph/sys/Logger.hpp"

namespace simph {
	namespace kern {
// --------------------------------------------------------------------
// ..........................................................
ComponentWrap::ComponentWrap(Smp::IComponent* wrapped,
                        Smp::Publication::ITypeRegistry* tReg):
                    _wrapped(wrapped),
                    _typeRegistry(tReg) {
}
// ..........................................................
ComponentWrap::~ComponentWrap() {
}
// --------------------------------------------------------------------
// ..........................................................
Smp::String8 ComponentWrap::GetName() {
    return _wrapped->GetName();
}
// ..........................................................
Smp::String8 ComponentWrap::GetDescription() {
    return _wrapped->GetDescription();
}
// --------------------------------------------------------------------
// ..........................................................
Smp::Publication::ITypeRegistry* ComponentWrap::GetTypeRegistry() const {
    return _typeRegistry;
}
// ..........................................................
void ComponentWrap::PublishField(
        Smp::String8 name,
        Smp::String8 description,
        Smp::Char8* address,
        Smp::ViewKind view,
        Smp::Bool state,
        Smp::Bool input,
        Smp::Bool output) {
    addField(new TField<Smp::Char8>(name,description,view,address,
                    state,input,output,_wrapped));
}
// ..........................................................
void ComponentWrap::PublishField(
        Smp::String8 name,
        Smp::String8 description,
        Smp::Bool* address,
        Smp::ViewKind view,
        Smp::Bool state,
        Smp::Bool input,
        Smp::Bool output) {
    addField(new TField<Smp::Bool>(name,description,view,address,
                    state,input,output,_wrapped));
}
// ..........................................................
void ComponentWrap::PublishField(
        Smp::String8 name,
        Smp::String8 description,
        Smp::Int8* address,
        Smp::ViewKind view,
        Smp::Bool state,
        Smp::Bool input,
        Smp::Bool output) {
    addField(new TField<Smp::Int8>(name,description,view,address,
                    state,input,output,_wrapped));
}
// ..........................................................
void ComponentWrap::PublishField(
        Smp::String8 name,
        Smp::String8 description,
        Smp::Int16* address,
        Smp::ViewKind view,
        Smp::Bool state,
        Smp::Bool input,
        Smp::Bool output) {
    addField(new TField<Smp::Int16>(name,description,view,address,
                    state,input,output,_wrapped));
}
// ..........................................................
void ComponentWrap::PublishField(
        Smp::String8 name,
        Smp::String8 description,
        Smp::Int32* address,
        Smp::ViewKind view,
        Smp::Bool state,
        Smp::Bool input,
        Smp::Bool output) {
    addField(new TField<Smp::Int32>(name,description,view,address,
                    state,input,output,_wrapped));
}
// ..........................................................
void ComponentWrap::PublishField(
        Smp::String8 name,
        Smp::String8 description,
        Smp::Int64* address,
        Smp::ViewKind view,
        Smp::Bool state,
        Smp::Bool input,
        Smp::Bool output) {
    addField(new TField<Smp::Int64>(name,description,view,address,
                    state,input,output,_wrapped));
}
// ..........................................................
void ComponentWrap::PublishField(
        Smp::String8 name,
        Smp::String8 description,
        Smp::UInt8* address,
        Smp::ViewKind view,
        Smp::Bool state,
        Smp::Bool input,
        Smp::Bool output) {
    addField(new TField<Smp::UInt8>(name,description,view,address,
                    state,input,output,_wrapped));
}
// ..........................................................
void ComponentWrap::PublishField(
        Smp::String8 name,
        Smp::String8 description,
        Smp::UInt16* address,
        Smp::ViewKind view,
        Smp::Bool state,
        Smp::Bool input,
        Smp::Bool output) {
    addField(new TField<Smp::UInt16>(name,description,view,address,
                    state,input,output,_wrapped));
}
// ..........................................................
void ComponentWrap::PublishField(
        Smp::String8 name,
        Smp::String8 description,
        Smp::UInt32* address,
        Smp::ViewKind view,
        Smp::Bool state,
        Smp::Bool input,
        Smp::Bool output) {
    addField(new TField<Smp::UInt32>(name,description,view,address,
                    state,input,output,_wrapped));
}
// ..........................................................
void ComponentWrap::PublishField(
        Smp::String8 name,
        Smp::String8 description,
        Smp::UInt64* address,
        Smp::ViewKind view,
        Smp::Bool state,
        Smp::Bool input,
        Smp::Bool output) {
    addField(new TField<Smp::UInt64>(name,description,view,address,
                    state,input,output,_wrapped));
}
// ..........................................................
void ComponentWrap::PublishField(
        Smp::String8 name,
        Smp::String8 description,
        Smp::Float32* address,
        Smp::ViewKind view,
        Smp::Bool state,
        Smp::Bool input,
        Smp::Bool output) {
    addField(new TField<Smp::Float32>(name,description,view,address,
                    state,input,output,_wrapped));
}
// ..........................................................
void ComponentWrap::PublishField(
        Smp::String8 name,
        Smp::String8 description,
        Smp::Float64* address,
        Smp::ViewKind view,
        Smp::Bool state,
        Smp::Bool input,
        Smp::Bool output) {
    addField(new TField<Smp::Float64>(name,description,view,address,
                    state,input,output,_wrapped));
}
// ..........................................................
void ComponentWrap::PublishField(
        Smp::String8 name,
        Smp::String8 description,
        void* address,
        Smp::Uuid typeUuid,
        Smp::ViewKind view,
        Smp::Bool state,
        Smp::Bool input,
        Smp::Bool output) {
LOGE("ComponentWrap::PublishField(...,void*,...) not implemented yet!")
}
// ..........................................................
void ComponentWrap::PublishField(Smp::IField* field) {
LOGE("ComponentWrap::PublishField(Smp::IField*) not implemented yet!")
}
// ..........................................................
void ComponentWrap::PublishArray(
        Smp::String8 name,
        Smp::String8 description,
        Smp::Int64 count,
        void* address,
        Smp::PrimitiveTypeKind type,
        Smp::ViewKind view,
        Smp::Bool state,
        Smp::Bool input,
        Smp::Bool output) {
LOGE("ComponentWrap::PublishArray(...,void*,...) not implemented yet!")
}
// ..........................................................
Smp::IPublication* ComponentWrap::PublishArray(
        Smp::String8 name,
        Smp::String8 description,
        Smp::ViewKind view,
        Smp::Bool state) {
LOGE("ComponentWrap::PublishArray(...) not implemented yet!")
return nullptr;
}
// ..........................................................
Smp::IPublication* ComponentWrap::PublishStructure(
        Smp::String8 name,
        Smp::String8 description,
        Smp::ViewKind view,
        Smp::Bool state) {
LOGE("ComponentWrap::PublishStructure(...) not implemented yet!")
return nullptr;
}
// ..........................................................
Smp::Publication::IPublishOperation* ComponentWrap::PublishOperation(
        Smp::String8 name,
        Smp::String8 description,
        Smp::ViewKind view) {
LOGE("ComponentWrap::PublishOperation(...) not implemented yet!")
return nullptr;
}
// ..........................................................
void ComponentWrap::PublishProperty(
        Smp::String8 name,
        Smp::String8 description,
        Smp::Uuid typeUuid,
        Smp::AccessKind accessKind,
        Smp::ViewKind view) {
LOGE("ComponentWrap::PublishProperty(...) not implemented yet!")
}
// ..........................................................
Smp::IField* ComponentWrap::GetField(Smp::String8 fullName) const {
    return Component::GetField(fullName);
}
// ..........................................................
const Smp::FieldCollection* ComponentWrap::GetFields() const {
    return Component::GetFields();
}
// ..........................................................
Smp::IRequest* ComponentWrap::CreateRequest(Smp::String8 operationName) {
LOGE("ComponentWrap::CreateRequest(...) not implemented yet!")
return nullptr;
}
// ..........................................................
void ComponentWrap::DeleteRequest(Smp::IRequest* request) {
LOGE("ComponentWrap::DeleteRequest(...) not implemented yet!")
}

}} // namespace simph::kern
