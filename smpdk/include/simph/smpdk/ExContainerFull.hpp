/*
 * @file ExContainerFull.h
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simph_smpdk_ExContainerFull_HPP__
#define __simph_smpdk_ExContainerFull_HPP__
#include "Smp/ContainerFull.h"
#include "Smp/IContainer.h"
#include "simph/smpdk/Exception.hpp"

namespace simph {
namespace smpdk {

/**
 *
 */
class ExContainerFull : public Exception, public virtual Smp::ContainerFull {
public:
    /**
     * Default constructor.
     */
    ExContainerFull(Smp::IContainer* sender);
    /**
     * Destructor.
     */
    virtual ~ExContainerFull();
    // Smp::ContainerFull implementation
    Smp::String8 GetContainerName() const noexcept override;
    Smp::Int64 GetContainerSize() const noexcept override;

private:
    Smp::Int64 _size;
    ;
};

}  // namespace smpdk
}  // namespace simph
#endif  // __simph_smpdk_ExContainerFull_HPP__
