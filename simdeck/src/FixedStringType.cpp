/*
 * @file FixedStringType.cpp
 *
 * Copyright 2025 Sebastien Devaux. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simdeck/FixedStringType.hpp"

namespace simdeck {
// --------------------------------------------------------------------
// ..........................................................
FixedStringType::FixedStringType(Smp::Uuid uuid, Smp::UInt64 maxLength, Smp::String8 name, Smp::String8 descr, Smp::IObject* parent): 
            Parent(uuid, Smp::PrimitiveTypeKind::PTK_String8, name, descr, parent),
            _maxLength(maxLength) {
}
// ..........................................................
FixedStringType::~FixedStringType() {
}

// --------------------------------------------------------------------
// ..........................................................
Smp::UInt64 FixedStringType::GetMaxLength() const {
    return _maxLength;
}
// ..........................................................
Smp::IField* FixedStringType::createField(
        Smp::String8 name,
        Smp::String8 description,
        Smp::IComponent* parent,
        void* address,
        Smp::ViewKind view,
        Smp::Bool state,
        Smp::Bool input,
        Smp::Bool output) const {
//    return FixedStringField::Create();
return nullptr;
}

} // namespace simdeck
