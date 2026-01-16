/*
 * @file VectorType.hpp
 *
 * Copyright 2025 Sebastien Devaux. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simdeck_VectorType_HPP__
#define __simdeck_VectorType_HPP__

#include "simdeck/ArrayType.hpp"

namespace simdeck {

class VectorType: public ArrayType {
    typedef ArrayType Parent;
    
public:
    static const Smp::Uuid UuidVector;

public:
    VectorType(Smp::String8 name, Smp::String8 descr, Smp::IObject* parent,
               Smp::UInt64 allocationSize, Smp::Publication::IType* type);

    virtual ~VectorType();
};

} // namespace simdeck
#endif // __simdeck_VectorType_HPP__
