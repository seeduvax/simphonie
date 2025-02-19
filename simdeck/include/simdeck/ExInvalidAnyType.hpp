/*
 * @file ExInvalidAnyType.h
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simdeck_ExInvalidAnyType_HPP__
#define __simdeck_ExInvalidAnyType_HPP__
#include "Smp/InvalidAnyType.h"
#include "simdeck/Exception.hpp"

namespace simdeck {
using namespace simdeck;

/**
 *
 */
class ExInvalidAnyType : public Exception, virtual public Smp::InvalidAnyType {
public:
    /**
     * Default constructor.
     */
    ExInvalidAnyType(const Smp::IObject* sender, Smp::AnySimple invalidValue, Smp::PrimitiveTypeKind expected);
    /**
     * Destructor.
     */
    virtual ~ExInvalidAnyType();
    // Smp::InvalidAnyType implementation
    Smp::AnySimple GetInvalidValue() const noexcept override;
    Smp::PrimitiveTypeKind GetExpectedType() const noexcept override;

private:
    Smp::AnySimple _invalidValue;
    Smp::PrimitiveTypeKind _expected;
};

}  // namespace simdeck
#endif  // __simdeck_ExInvalidAnyType_HPP__
