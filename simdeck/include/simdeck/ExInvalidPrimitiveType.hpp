/*
 * @file ExInvalidPrimitiveType.h
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simdeck_ExInvalidPrimitiveType_HPP__
#define __simdeck_ExInvalidPrimitiveType_HPP__
#include "Smp/Publication/InvalidPrimitiveType.h"
#include "simdeck/Exception.hpp"

namespace simdeck {
using namespace simdeck;

/**
 *
 */
class ExInvalidPrimitiveType : public Exception, virtual public Smp::Publication::InvalidPrimitiveType {
public:
    /**
     * Default constructor.
     */
    ExInvalidPrimitiveType(const Smp::IObject* sender, Smp::PrimitiveTypeKind type);
    /**
     * Destructor.
     */
    virtual ~ExInvalidPrimitiveType();
    // Smp::Publication::InvalidPrimitiveType implementation
    Smp::String8 GetTypeName() const noexcept override;
    Smp::PrimitiveTypeKind GetType() const noexcept override;

private:
    Smp::PrimitiveTypeKind _type;
    std::string _typeName;
};

}  // namespace simdeck
#endif  // __simdeck_ExInvalidPrimitiveType_HPP__
