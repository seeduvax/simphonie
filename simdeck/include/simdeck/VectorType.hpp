/*
 * @file VectorType.hpp
 *
 * Copyright 2026 Sebastien Devaux. All rights reserved.
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

using namespace simdeck;

/**
 *
 */
class VectorType : public Type{
        typedef Type Parent;
public:
    /**
     * Default constructor.
     */
    VectorType(Smp::Uuid uuid, Smp::Uuid puuid, Smp::PrimitiveTypeKind primitiveTypeKind, Smp::String8 name, Smp::String8 descr, Smp::IObject* parent);
    /**
     * Destructor.
     */
    virtual ~VectorType();

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

private: 
        Smp::Uuid _puuid;





};

} // namespace simdeck
#endif // __simdeck_VectorType_HPP__
