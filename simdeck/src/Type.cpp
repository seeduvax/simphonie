/*
 * @file Type.cpp
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simdeck/Type.hpp"
#include "Smp/IPublication.h"
#include "Smp/IComponent.h"
#include "simdeck/SimpleArrayField.hpp"

namespace simdeck {

Type _byteType(Smp::Uuids::Uuid_UInt8, Smp::PrimitiveTypeKind::PTK_UInt8, sizeof(Smp::UInt8), "UInt8",
                              "One byte unsigned int data type");

// --------------------------------------------------------------------
// ..........................................................
Type::Type(Smp::Uuid uuid, Smp::PrimitiveTypeKind kind, Smp::UInt64 typeSize, Smp::String8 name, Smp::String8 descr,
           Smp::IObject* parent)
    : Object(name, descr, parent), _uuid(uuid), _kind(kind), _size(typeSize) {}
// ..........................................................
Type::~Type() {}
// --------------------------------------------------------------------
// ..........................................................
Smp::PrimitiveTypeKind Type::GetPrimitiveTypeKind() const {
    return _kind;
}
// ..........................................................
Smp::Uuid Type::GetUuid() const {
    return _uuid;
}
// ..........................................................
Smp::IField* Type::Publish(Smp::Publication::IPublishField* receiver,
                   Smp::String8 name,
                   Smp::String8 description,
                   void* address,
                   Smp::ViewKind view,
                   Smp::Bool state,
                   Smp::Bool input,
                   Smp::Bool output) {
    // temporary publish a dummy field to retrieve the parent.
    auto f=receiver->PublishField(name, description, (Smp::UInt8*)address,view, state, input, output);
    if (f!=nullptr) {
        auto parent=dynamic_cast<Smp::IComponent*>(f->GetParent());
        if (parent!=nullptr) {
            // and remove the dummy temp field once no more useful.
            parent->RemoveChild(f, parent->GetFields());
            auto field = this->createField(
                                name,
                                description,
                                parent,
                                address,
                                view,
                                state,
                                input,
                                output);
            receiver->PublishField(field);
            return field;
        }
    }
    // TODO error management and related exception throwing
    return nullptr;
}
// ..........................................................
// default field creation is an array of bytes.
Smp::IField* Type::createField( Smp::String8 name,
                                Smp::String8 descr,
                                Smp::IComponent* parent,
                                void* address,
                                Smp::ViewKind view,
                                Smp::Bool state,
                                Smp::Bool input,
                                Smp::Bool output) const {
    return SimpleArrayField::Create(name, descr, _size, (Smp::UInt8*)address,
                                &_byteType,
                                view, this, state, input, output, parent); 
    
}

}  // namespace simdeck
