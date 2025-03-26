/*
 * @file ExCannotDelete.h
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simdeck_ExCannotDelete_HPP__
#define __simdeck_ExCannotDelete_HPP__
#include "simdeck/Exception.hpp"
#include "Smp/CannotDelete.h"
#include "Smp/IContainer.h"

namespace simdeck {

/**
 *
 */
class ExCannotDelete : public Exception, public virtual Smp::CannotDelete {
public:
    /**
     * Default constructor.
     */
    ExCannotDelete(const Smp::IContainer* sender, Smp::IComponent* comp);
    /**
     * Destructor.
     */
    virtual ~ExCannotDelete();

    // Smp::CannotDelete implementation
    Smp::String8 GetContainerName() const noexcept override;
    const Smp::IComponent* GetComponent() const noexcept override;
    Smp::Int64 GetLowerLimit() const noexcept override;

private:
    Smp::Int64 _lower;
    const Smp::IComponent* _comp;
};

}  // namespace simdeck
#endif  // __simdeck_ExCannotDelete_HPP__
