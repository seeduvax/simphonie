/*
 * @file ExInvalidParent.hpp
 *
 * Copyright 2025 Sebastien Devaux. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simdeck_ExInvalidParent_HPP__
#define __simdeck_ExInvalidParent_HPP__

#include "simdeck/Exception.hpp"
#include "Smp/InvalidParent.h"

namespace simdeck {

/**
 *
 */
class ExInvalidParent: public Exception, virtual public Smp::InvalidParent {
public:
    /**
     * Default constructor.
     */
    ExInvalidParent(const Smp::IObject* sender, const Smp::IObject* found, Smp::IObject* expected);
    /**
     * Destructor.
     */
    virtual ~ExInvalidParent();
    // Smp::InvalidException implementation
    const Smp::IObject* GetParentFound() const noexcept override;
    Smp::IObject* GetParentExpected() noexcept override;

private:
    const Smp::IObject* _found;
    Smp::IObject* _expected;

};

} // namespace simdeck
#endif // __simdeck_ExInvalidParent_HPP__
