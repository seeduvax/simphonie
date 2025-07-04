/*
 * @file StringType.hpp
 *
 * Copyright 2025 Sebastien Devaux. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simdeck_StringType_HPP__
#define __simdeck_StringType_HPP__

#include "simdeck/Type.hpp"

namespace simdeck {

/**
 *
 */
class StringType: public Type {
    typedef Type Parent;
    
public:
    static const Smp::Uuid UuidString;

public:
    /**
     * Default constructor.
     */
    StringType(Smp::String8 name, Smp::String8 descr, Smp::IObject* parent);
    /**
     * Destructor.
     */
    virtual ~StringType();

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
#endif // __simdeck_StringType_HPP__
