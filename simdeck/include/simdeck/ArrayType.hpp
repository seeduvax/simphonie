/*
 * @file ArrayType.h
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simdeck_ArrayType_HPP__
#define __simdeck_ArrayType_HPP__
#include "Smp/Publication/IArrayType.h"
#include "simdeck/Type.hpp"

namespace simdeck {

/**
 *
 */
class ArrayType : public Type, virtual public Smp::Publication::IArrayType {
public:
    /**
     * Default constructor.
     */
    ArrayType(Smp::Uuid uuid, Smp::String8 name, Smp::String8 descr, Smp::IObject* parent, Smp::UInt64 elemSize,
              Smp::UInt64 size, Smp::Publication::IType* type);
    /**
     * Destructor.
     */
    virtual ~ArrayType();
    // Smp::Publication::IArrayType implementation
    Smp::UInt64 GetSize() const override;
    const Smp::Publication::IType* GetItemType() const override;

private:
    Smp::UInt64 _size;
    const Smp::Publication::IType* _itemType;
};

}  // namespace simdeck
#endif  // __simdeck_ArrayType_HPP__
