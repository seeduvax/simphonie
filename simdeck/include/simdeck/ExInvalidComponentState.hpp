/*
 * @file ExInvalidComponentState.h
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simdeck_ExInvalidComponentState_HPP__
#define __simdeck_ExInvalidComponentState_HPP__
#include "simdeck/Exception.hpp"
#include "Smp/InvalidComponentState.h"
namespace simdeck {

/**
 *
 */
class ExInvalidComponentState : public Exception, virtual public Smp::InvalidComponentState {
public:
    /**
     * Default constructor.
     */
    ExInvalidComponentState(const Smp::IObject* sender, Smp::ComponentStateKind invalid, Smp::ComponentStateKind expected);
    /**
     * Destructor.
     */
    virtual ~ExInvalidComponentState();

    Smp::ComponentStateKind GetInvalidState() const noexcept;
    Smp::ComponentStateKind GetExpectedState() const noexcept;

private:
    Smp::ComponentStateKind _invalid;
    Smp::ComponentStateKind _expected;
};

}  // namespace simdeck
#endif  // __simdeck_ExInvalidComponentState_HPP__
