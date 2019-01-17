/*
 * @file InvalidComponentState.h
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simph_kern_InvalidComponentState_HPP__
#define __simph_kern_InvalidComponentState_HPP__
#include "Smp/InvalidComponentState.h"
#include "simph/kern/Exception.hpp"
namespace simph {
	namespace kern {

/**
 *
 */
class InvalidComponentState: virtual public Exception, virtual public Smp::InvalidComponentState {
public:
    /**
     * Default constructor.
     */
    InvalidComponentState(Smp::IObject* sender,
		        Smp::ComponentStateKind invalid,
                        Smp::ComponentStateKind expected);
    /**
     * Destructor.
     */
    virtual ~InvalidComponentState();
    
    Smp::ComponentStateKind GetInvalidState() const noexcept;
    Smp::ComponentStateKind GetExpectedState() const noexcept;

private:
    Smp::ComponentStateKind _invalid;
    Smp::ComponentStateKind _expected;

};

}} // namespace simph::kern
#endif // __simph_kern_InvalidComponentState_HPP__
