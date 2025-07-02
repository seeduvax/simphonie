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

#include "simdeck/Object.hpp"
#include "Smp/Publication/IType.h"

namespace simdeck {

/**
 *
 */
class StringType: public Object, virtual public Smp::Publication::IType {
    typedef Object Parent;
    
public:
    static const Smp::Uuid UuidString;

public:
    /**
     * Default constructor.
     */
    StringType(Smp::IObject* parent);
    /**
     * Destructor.
     */
    virtual ~StringType();

    // Type specialization
    Smp::PrimitiveTypeKind GetPrimitiveTypeKind() const override;
    Smp::Uuid GetUuid() const override;
    Smp::IField* Publish(
        Smp::Publication::IPublishField* receiver,
        Smp::String8 name,
        Smp::String8 description,
        Smp::Void* address,
        Smp::ViewKind view = Smp::ViewKind::VK_All,
        Smp::Bool state = true,
        Smp::Bool input = false,
        Smp::Bool output = false) override;
};

} // namespace simdeck
#endif // __simdeck_StringType_HPP__
