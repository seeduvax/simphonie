/*
 * @file ArrayType.cpp
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simph/kern/ArrayType.hpp"

namespace simph {
	namespace kern {
// --------------------------------------------------------------------
// ..........................................................
ArrayType::ArrayType(Smp::Uuid uuid, Smp::String8 name, Smp::String8 descr, 
        Smp::IObject* parent, Smp::UInt64 elemSize, Smp::UInt64 size,
        Smp::Publication::IType* type):
        Type(uuid,type->GetPrimitiveTypeKind(),elemSize*size,name,descr,parent), 
        _size(size), _itemType(type) {
}
// ..........................................................
ArrayType::~ArrayType() {
}
// --------------------------------------------------------------------
// ..........................................................
Smp::UInt64 ArrayType::GetSize() const {
    return _size;
}
// ..........................................................
const Smp::Publication::IType* ArrayType::GetItemType() const {
    return _itemType;
}
}} // namespace simph::kern
