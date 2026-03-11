/*
 * @file Publication.cpp
 *
 * Copyright 2020 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simphonie/kern/Publication.hpp"

#include <string.h>

#include "Smp/IComponent.h"
#include "Smp/IDynamicInvocation.h"
#include "Smp/IEntryPointPublisher.h"
#include "Smp/Publication/IArrayType.h"
#include "simdeck/ExDuplicateName.hpp"
#include "simdeck/ExInvalidPrimitiveType.hpp"
#include "simdeck/ExInvalidType.hpp"
#include "simdeck/ExTypeNotRegistered.hpp"
#include "simdeck/Property.hpp"
#include "simdeck/SimpleArrayField.hpp"
#include "simdeck/SimpleField.hpp"
#include "simdeck/StringField.hpp"
#include "simdeck/StructureType.hpp"
#include "simdeck/Type.hpp"
#include "simphonie/kern/ExNoDynamicInvocation.hpp"
#include "simphonie/kern/TypeRegistry.hpp"
#include "simphonie/sys/Logger.hpp"
#include "simphonie/sys/RttiUtil.hpp"

namespace simphonie {
namespace kern {

using namespace simdeck;
// --------------------------------------------------------------------
// ..........................................................
class StructurePublication : public Publication {
public:
    StructurePublication(Smp::IObject* toPublish, Smp::ISimulator* sim) : Publication(toPublish, sim) {}
    virtual ~StructurePublication() {}

private:
};
// --------------------------------------------------------------------
// ..........................................................
Publication::Publication(Smp::IObject* toPublish, Smp::ISimulator* sim)
    : _pubObj(dynamic_cast<Smp::IComponent*>(toPublish)),
      _typeRegistry(sim->GetTypeRegistry()),
      _sim(sim),
      _properties() {}
// ..........................................................
Publication::~Publication() {
}
// --------------------------------------------------------------------
// Children management
// ..........................................................
void Publication::addChild(Smp::IObject* pub, const Smp::ICollectionBase* collection) {
    Smp::IObject* p = _pubObj->GetChild(pub->GetName());
    if (p != nullptr) {
        delete pub;
        throw ExDuplicateName(_pubObj, pub->GetName());
    }
    _pubObj->AddChild(pub,collection);
    _published.push_back({pub,collection});
}
void Publication::addField(Smp::IField* field) {
    addChild(field,_pubObj->GetFields());
}

// --------------------------------------------------------------------
// IPublication implementation
// ..........................................................
Smp::Publication::ITypeRegistry* Publication::GetTypeRegistry() const {
    return _typeRegistry;
}
// ..........................................................
Smp::IField* Publication::PublishField(Smp::String8 name, Smp::String8 description, Smp::Char8* address, Smp::ViewKind view,
                               Smp::Bool state, Smp::Bool input, Smp::Bool output) {
    auto f = SimpleField::Create(name, description, view, _typeRegistry->GetType(Smp::Uuids::Uuid_Char8), address,
                                 state, input, output, _pubObj);
    addField(f);
    return f;
}
// ..........................................................
Smp::IField* Publication::PublishField(Smp::String8 name, Smp::String8 description, Smp::Bool* address, Smp::ViewKind view,
                               Smp::Bool state, Smp::Bool input, Smp::Bool output) {
    auto f = SimpleField::Create(name, description, view, _typeRegistry->GetType(Smp::Uuids::Uuid_Bool), address, state,
                                 input, output, _pubObj);
    addField(f);
    return f;
}
// ..........................................................
Smp::IField* Publication::PublishField(Smp::String8 name, Smp::String8 description, Smp::Int8* address, Smp::ViewKind view,
                               Smp::Bool state, Smp::Bool input, Smp::Bool output) {
    auto f = SimpleField::Create(name, description, view, _typeRegistry->GetType(Smp::Uuids::Uuid_Int8), address, state,
                                 input, output, _pubObj);
    addField(f);
    return f;
}
// ..........................................................
Smp::IField* Publication::PublishField(Smp::String8 name, Smp::String8 description, Smp::Int16* address, Smp::ViewKind view,
                               Smp::Bool state, Smp::Bool input, Smp::Bool output) {
    auto f = SimpleField::Create(name, description, view, _typeRegistry->GetType(Smp::Uuids::Uuid_Int16), address,
                                 state, input, output, _pubObj);
    addField(f);
    return f;
}
// ..........................................................
Smp::IField* Publication::PublishField(Smp::String8 name, Smp::String8 description, Smp::Int32* address, Smp::ViewKind view,
                               Smp::Bool state, Smp::Bool input, Smp::Bool output) {
    auto f = SimpleField::Create(name, description, view, _typeRegistry->GetType(Smp::Uuids::Uuid_Int32), address,
                                 state, input, output, _pubObj);
    addField(f);
    return f;
}
// ..........................................................
Smp::IField* Publication::PublishField(Smp::String8 name, Smp::String8 description, Smp::Int64* address, Smp::ViewKind view,
                               Smp::Bool state, Smp::Bool input, Smp::Bool output) {
    auto f = SimpleField::Create(name, description, view, _typeRegistry->GetType(Smp::Uuids::Uuid_Int64), address,
                                 state, input, output, _pubObj);
    addField(f);
    return f;
}
// ..........................................................
Smp::IField* Publication::PublishField(Smp::String8 name, Smp::String8 description, Smp::UInt8* address, Smp::ViewKind view,
                               Smp::Bool state, Smp::Bool input, Smp::Bool output) {
    auto f = SimpleField::Create(name, description, view, _typeRegistry->GetType(Smp::Uuids::Uuid_UInt8), address,
                                 state, input, output, _pubObj);
    addField(f);
    return f;
}
// ..........................................................
Smp::IField* Publication::PublishField(Smp::String8 name, Smp::String8 description, Smp::UInt16* address, Smp::ViewKind view,
                               Smp::Bool state, Smp::Bool input, Smp::Bool output) {
    auto f = SimpleField::Create(name, description, view, _typeRegistry->GetType(Smp::Uuids::Uuid_UInt16), address,
                                 state, input, output, _pubObj);
    addField(f);
    return f;
}
// ..........................................................
Smp::IField* Publication::PublishField(Smp::String8 name, Smp::String8 description, Smp::UInt32* address, Smp::ViewKind view,
                               Smp::Bool state, Smp::Bool input, Smp::Bool output) {
    auto f = SimpleField::Create(name, description, view, _typeRegistry->GetType(Smp::Uuids::Uuid_UInt32), address,
                                 state, input, output, _pubObj);
    addField(f);
    return f;
}
// ..........................................................
Smp::IField* Publication::PublishField(Smp::String8 name, Smp::String8 description, Smp::UInt64* address, Smp::ViewKind view,
                               Smp::Bool state, Smp::Bool input, Smp::Bool output) {
    auto f = SimpleField::Create(name, description, view, _typeRegistry->GetType(Smp::Uuids::Uuid_UInt64), address,
                                 state, input, output, _pubObj);
    addField(f);
    return f;
}
// ..........................................................
Smp::IField* Publication::PublishField(Smp::String8 name, Smp::String8 description, Smp::Float32* address, Smp::ViewKind view,
                               Smp::Bool state, Smp::Bool input, Smp::Bool output) {
    auto f = SimpleField::Create(name, description, view, _typeRegistry->GetType(Smp::Uuids::Uuid_Float32), address,
                                 state, input, output, _pubObj);
    addField(f);
    return f;
}
// ..........................................................
Smp::IField* Publication::PublishField(Smp::String8 name, Smp::String8 description, Smp::Float64* address, Smp::ViewKind view,
                               Smp::Bool state, Smp::Bool input, Smp::Bool output) {
    auto f = SimpleField::Create(name, description, view, _typeRegistry->GetType(Smp::Uuids::Uuid_Float64), address,
                                 state, input, output, _pubObj);
    addField(f);
    return f;
}
// ..........................................................
Smp::IField* Publication::PublishField(Smp::String8 name, Smp::String8 description, void* address, Smp::Uuid typeUuid,
                               Smp::ViewKind view, Smp::Bool state, Smp::Bool input, Smp::Bool output) {
    Type* t = dynamic_cast<Type*>(_typeRegistry->GetType(typeUuid));
    if (t != nullptr) {
        Smp::IField* f = nullptr;
        if (t == _recursivePubGuard) {
            _sim->GetLogger()->Log(_sim, "A PublishField called itself, stop here to not fail on stack overflow.",
                                   Smp::Services::ILogger::LMK_Error);
            _recursivePubGuard = nullptr;
            return nullptr;
        }
        else {
            // At first, delegate the publication to the type, hoping
            // it will not just call back the current Publication method.
            _recursivePubGuard = t;
            f = t->Publish(this, name, description, address, view, state, input, output);
            _recursivePubGuard = nullptr;
        }

        if (f!=nullptr) return f;

        // The type did not succeed into creating the field so we try other simple types in last chance.

        auto at = dynamic_cast<Smp::Publication::IArrayType*>(t);
        if (at != nullptr) {
            Smp::Publication::IType* pt = _typeRegistry->GetType(t->GetPrimitiveTypeKind());
            f = SimpleArrayField::Create(name, description, at->GetSize(), address, pt, view, t, state, input, output,
                                         _pubObj);
        }
        else { 
            f = SimpleField::Create(name, description, view, t, address, state, input, output, _pubObj);
        }
        if (f != nullptr) {
            addField(f);
            return f;
        }
    }
    std::ostringstream oss;
    oss << "Can't publish field " << name << ", no registered type for uuid " << typeUuid;
    throw ExInvalidType(_pubObj, oss.str().c_str());
    return nullptr;
}
// ..........................................................
void Publication::PublishField(Smp::IField* field){
    addField(field);
}
// ..........................................................
Smp::Publication::IType* Publication::getArrayType(Smp::PrimitiveTypeKind ptk, Smp::Int64 count) {
    Type* pt = dynamic_cast<Type*>(_typeRegistry->GetType(ptk));
    if (pt == nullptr) {
        throw ExInvalidPrimitiveType(_pubObj, ptk);
    }
    Smp::Uuid arrayTypeUuid((uint32_t)(count >> 32),
                            {(uint16_t)((count & 0xffff0000) >> 16), (uint16_t)(count & 0xffff), 0},
                            pt->GetUuid().Data3);
    Smp::Publication::IType* t = _typeRegistry->GetType(arrayTypeUuid);
    if (t == nullptr) {
        std::string tname = "A";
        tname = tname + pt->GetName();
        std::string tdescr = "Array of ";
        tdescr = tdescr + pt->GetDescription();
        int itemSize = TypeRegistry::getPrimitiveTypeSize(pt->GetPrimitiveTypeKind());
        t = _typeRegistry->AddArrayType(tname.c_str(), tdescr.c_str(), arrayTypeUuid, pt->GetUuid(), itemSize, count);
    }
    return t;
}
// ..........................................................
Smp::ISimpleArrayField* Publication::PublishArray(Smp::String8 name, Smp::String8 description, Smp::Int64 count, void* address,
                               Smp::PrimitiveTypeKind type, Smp::ViewKind view, Smp::Bool state, Smp::Bool input,
                               Smp::Bool output) {
    Smp::Publication::IType* t = getArrayType(type, count);
    Smp::Publication::IType* pt = _typeRegistry->GetType(type);
    auto* rf = SimpleArrayField::Create(name, description, count, address, pt, view, t, state, input, output, _pubObj);
    if (rf!=nullptr) {
        addField(rf);
    }
    return rf;
}
// ..........................................................
Smp::IPublication* Publication::PublishArray(Smp::String8 name, Smp::String8 description, Smp::ViewKind view,
                                             Smp::Bool state) {
    LOGE("Publication::PublishArray(...) not implemented yet!")
    return nullptr;
}
// ..........................................................
Smp::IPublication* Publication::PublishStructure(Smp::String8 name, Smp::String8 description, Smp::ViewKind view,
                                                 Smp::Bool state) {
    LOGE("Publication::PublishStructure(...) not implemented yet!")
    return nullptr;
}
// ..........................................................
Smp::Publication::IPublishOperation* Publication::PublishOperation(
                    Smp::String8 name,
                    Smp::String8 description,
                    Smp::ViewKind view) {
    // TODO
    LOGE("Publication::PublishOperation(...) not implemented yet!")
    return nullptr;
}
// ..........................................................
void Publication::PublishOperation(Smp::IOperation* operation) {
    // TODO
    LOGE("Publication::PublishOperation(...) not implemented yet!")
}
// ..........................................................
Smp::IProperty* Publication::PublishProperty(
                        Smp::String8 name,
                        Smp::String8 description,
                        Smp::Uuid typeUuid,
                        Smp::AccessKind accessKind,
                        Smp::ViewKind view) {
    auto dpubObj=dynamic_cast<Smp::IDynamicInvocation*>(_pubObj);
    if (dpubObj==nullptr) {
       throw ExNoDynamicInvocation(_pubObj);
    }
    Smp::Publication::IType* type = _typeRegistry->GetType(typeUuid);
    if (type != nullptr) {
        Property* p = new Property(name, description, _pubObj, type, accessKind, view);
        _pubObj->AddChild(p,dpubObj->GetProperties() );
        return p;
    }
    else {
        throw ExTypeNotRegistered(_pubObj, typeUuid);
    }
    return nullptr;
}
// ..........................................................
void Publication::PublishProperty(Smp::IProperty* property) {
    auto dpubObj=dynamic_cast<Smp::IDynamicInvocation*>(_pubObj);
    if (dpubObj==nullptr) {
        throw ExNoDynamicInvocation(_pubObj);
    }
    _pubObj->AddChild(property, dpubObj->GetProperties() );
}
// ..........................................................
void Publication::Unpublish() {
    for (auto p: _published) {
        _pubObj->RemoveChild(std::get<0>(p), std::get<1>(p));
    }
    _published.clear();
}
// ..........................................................
Smp::IField* Publication::GetField(Smp::String8 fullName) const {
    return _pubObj->GetField(fullName);
}
// ..........................................................
const Smp::FieldCollection* Publication::GetFields() const {
    return _pubObj->GetFields();
}
// ..........................................................
Smp::IProperty* Publication::GetProperty(Smp::String8 name) const {
    auto dpubObj=dynamic_cast<Smp::IDynamicInvocation*>(_pubObj);
    if (dpubObj!=nullptr) {
        return dpubObj->GetProperty(name);
    }
    return nullptr;
}
// ..........................................................
const Smp::PropertyCollection* Publication::GetProperties() const {
    auto dpubObj=dynamic_cast<Smp::IDynamicInvocation*>(_pubObj);
    if (dpubObj!=nullptr) {
        return dpubObj->GetProperties();
    }
    return nullptr;
}
// ..........................................................
Smp::IOperation* Publication::GetOperation(Smp::String8 name) const {
    auto dpubObj=dynamic_cast<Smp::IDynamicInvocation*>(_pubObj);
    if (dpubObj!=nullptr) {
        return dpubObj->GetOperation(name);
    }
    return nullptr;
}
// ..........................................................
const Smp::OperationCollection* Publication::GetOperations() const {
    auto dpubObj=dynamic_cast<Smp::IDynamicInvocation*>(_pubObj);
    if (dpubObj!=nullptr) {
        return dpubObj->GetOperations();
    }
    return nullptr;
}

}  // namespace kern
}  // namespace simph
