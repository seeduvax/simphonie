/*
 * @file ExCannotDelete.h
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simph_smpdk_ExCannotDelete_HPP__
#define __simph_smpdk_ExCannotDelete_HPP__
#include "Smp/CannotDelete.h"
#include "Smp/IContainer.h"
#include "simph/smpdk/Exception.hpp"

namespace simph {
namespace smpdk {

/**
 *
 */
class ExCannotDelete : public Exception, public virtual Smp::CannotDelete {
public:
    /**
     * Default constructor.
     */
    ExCannotDelete(Smp::IContainer* sender, Smp::IComponent* comp);
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

}  // namespace smpdk
}  // namespace simph
#endif  // __simph_smpdk_ExCannotDelete_HPP__
