/*
 * @file ExInvalidAnyType.h
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simph_kern_ExInvalidAnyType_HPP__
#define __simph_kern_ExInvalidAnyType_HPP__
#include "Smp/InvalidAnyType.h"
#include "simph/smpdk/Exception.hpp"

namespace simph {
namespace kern {
using namespace simph::smpdk;

/**
 *
 */
class ExInvalidAnyType : public Exception, virtual public Smp::InvalidAnyType {
public:
    /**
     * Default constructor.
     */
    ExInvalidAnyType(Smp::IObject* sender, Smp::AnySimple invalidValue, Smp::PrimitiveTypeKind expected);
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

}  // namespace kern
}  // namespace simph
#endif  // __simph_kern_ExInvalidAnyType_HPP__
