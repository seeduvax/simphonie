/*
 * @file SimpleVectorType.hpp
 *
 * Copyright 2025 Sebastien Devaux. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simdeck_VectorType_HPP__
#define __simdeck_VectorType_HPP__

#include "simdeck/Type.hpp"
#include "Smp/IField.h"

namespace simdeck {

class SimpleVectorType: public Type {
    typedef Type Parent;
    
public:
    static const Smp::Uuid UuidVector;

public:
    SimpleVectorType(Smp::PrimitiveTypeKind primitiveTypeKind, Smp::String8 name, Smp::String8 descr, Smp::IObject* parent);

    virtual ~SimpleVectorType();

protected:
    Smp::IField* createField(
        Smp::String8 name,
        Smp::String8 description,
        Smp::IComponent* parent,
        void* address,
        Smp::ViewKind view,
        Smp::Bool state,
        Smp::Bool input,
        Smp::Bool output) const override;
};

} // namespace simdeck
#endif // __simdeck_VectorType_HPP__
