/*
 * @file Type.cpp
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simph/smpdk/Type.hpp"
#include "Smp/IPublication.h"

namespace simph {
namespace smpdk {
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
    return receiver->PublishField(name, description, address, _uuid, view, state, input, output);
}

}  // namespace smpdk
}  // namespace simph
