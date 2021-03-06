/*
 * @file ExInvalidComponentState.h
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simph_smpdk_ExInvalidComponentState_HPP__
#define __simph_smpdk_ExInvalidComponentState_HPP__
#include "Smp/InvalidComponentState.h"
#include "simph/smpdk/Exception.hpp"
namespace simph {
namespace smpdk {

/**
 *
 */
class ExInvalidComponentState : public Exception, virtual public Smp::InvalidComponentState {
public:
    /**
     * Default constructor.
     */
    ExInvalidComponentState(Smp::IObject* sender, Smp::ComponentStateKind invalid, Smp::ComponentStateKind expected);
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

}  // namespace smpdk
}  // namespace simph
#endif  // __simph_smpdk_ExInvalidComponentState_HPP__
