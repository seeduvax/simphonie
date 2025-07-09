/*
 * @file ExNoDynamicInvocation.hpp
 *
 * Copyright 2025 Sebastien Devaux. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simphonie_kern_ExNoDynamicInvocation_HPP__
#define __simphonie_kern_ExNoDynamicInvocation_HPP__

#include "Smp/NoDynamicInvocation.h"
#include "simdeck/Exception.hpp"

namespace simphonie {
namespace kern {

/**
 *
 */
class ExNoDynamicInvocation: public simdeck::Exception, virtual public Smp::NoDynamicInvocation {
public:
    /**
     * Default constructor.
     */
    ExNoDynamicInvocation(const Smp::IComponent* sender);
    /**
     * Destructor.
     */
    virtual ~ExNoDynamicInvocation();

    // Smp::NoDynamicInvocation implementation
    const Smp::IComponent* GetComponent() const noexcept override;

private:
    const Smp::IComponent* _component;

};

}} // namespace simphonie::kern
#endif // __simphonie_kern_ExNoDynamicInvocation_HPP__
