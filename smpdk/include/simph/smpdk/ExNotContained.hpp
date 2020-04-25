/*
 * @file ExNotContained.h
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simph_smpdk_ExNotContained_HPP__
#define __simph_smpdk_ExNotContained_HPP__
#include "Smp/NotContained.h"
#include "Smp/IContainer.h"
#include "simph/smpdk/Exception.hpp"

namespace simph {
	namespace smpdk {

/**
 *
 */
class ExNotContained: public Exception,
        public virtual Smp::NotContained {
public:
    /**
     * Default constructor.
     */
    ExNotContained(Smp::IContainer* sender, Smp::IComponent* comp);
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

}} // namespace simph::smpdk
#endif // __simph_smpdk_ExNotContained_HPP__
