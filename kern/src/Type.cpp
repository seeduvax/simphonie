/*
 * @file Type.cpp
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simph/kern/Type.hpp"
#include "Smp/IPublication.h"

namespace simph {
	namespace kern {
// --------------------------------------------------------------------
// ..........................................................
Type::Type(Smp::Uuid uuid, Smp::PrimitiveTypeKind kind, Smp::UInt64 typeSize,
            Smp::String8 name, Smp::String8 descr, Smp::IObject* parent):
                    Object(name,descr,parent),
                    _uuid(uuid),
                    _kind(kind),
                    _size(typeSize) {
}
// ..........................................................
Type::~Type() {
}
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
void Type::Publish(Smp::IPublication* receiver,
             Smp::String8 name,
             Smp::String8 description,
             void* address,
             Smp::ViewKind view,
             Smp::Bool state,
             Smp::Bool input,
             Smp::Bool output) {
    receiver->PublishField(name,description,address,
                          _uuid,view,state,input,output);
}

}} // namespace simph::kern
