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
#include <iostream>


namespace simdeck {

// --------------------------------------------------------------------
// ..........................................................
Type::Type(Smp::Uuid uuid, Smp::PrimitiveTypeKind kind, Smp::String8 name, Smp::String8 descr,
           Smp::IObject* parent)
    : Object(name, descr, parent), _uuid(uuid), _kind(kind) {}
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

        Smp::IObject* parent = f->GetParent();
        auto parentComponent=dynamic_cast<Smp::IComponent*>(parent);
        if (parentComponent!=nullptr) {
            // and remove the dummy temp field once no more useful.
            parentComponent->RemoveChild(f, parentComponent->GetFields());
            parent = parentComponent;
        }

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
    // TODO error management and related exception throwing
    return nullptr;
}

}  // namespace simdeck
