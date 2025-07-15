/*
 * @file ArrayType.cpp
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simdeck/ArrayType.hpp"
#include "simdeck/SimpleArrayField.hpp"
#include "Smp/IComponent.h"

namespace simdeck {
// --------------------------------------------------------------------
// ..........................................................
ArrayType::ArrayType(Smp::Uuid uuid, Smp::String8 name, Smp::String8 descr, Smp::IObject* parent,
                     Smp::UInt64 size, Smp::Publication::IType* type)
    : Type(uuid, type->GetPrimitiveTypeKind(), name, descr, parent), _size(size), _itemType(type) {}
// ..........................................................
ArrayType::~ArrayType() {}
// --------------------------------------------------------------------
// ..........................................................
Smp::UInt64 ArrayType::GetSize() const {
    return _size;
}
// ..........................................................
const Smp::Publication::IType* ArrayType::GetItemType() const {
    return _itemType;
}
// --------------------------------------------------------------------
// ..........................................................
Smp::IField* ArrayType::createField(
        Smp::String8 name,
        Smp::String8 description,
        Smp::IComponent* parent,
        void* address,
        Smp::ViewKind view,
        Smp::Bool state,
        Smp::Bool input,
        Smp::Bool output) const {
    return SimpleArrayField::Create(name, description, GetSize(), address,
            GetItemType(), view, this, state, input, output, parent);
}

}  // namespace simdeck
