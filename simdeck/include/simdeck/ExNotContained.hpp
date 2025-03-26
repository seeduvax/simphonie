/*
 * @file ExNotContained.h
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simdeck_ExNotContained_HPP__
#define __simdeck_ExNotContained_HPP__
#include "simdeck/Exception.hpp"
#include "Smp/IContainer.h"
#include "Smp/NotContained.h"

namespace simdeck {

/**
 *
 */
class ExNotContained : public Exception, public virtual Smp::NotContained {
public:
    /**
     * Default constructor.
     */
    ExNotContained(const Smp::IContainer* sender, Smp::IComponent* comp);
    /**
     * Destructor.
     */
    virtual ~ExNotContained();
    // Smp::NotContained implementation
    Smp::String8 GetContainerName() const noexcept override;
    const Smp::IComponent* GetComponent() const noexcept override;

private:
    const Smp::IComponent* _comp;
};

}  // namespace simdeck
#endif  // __simdeck_ExNotContained_HPP__
