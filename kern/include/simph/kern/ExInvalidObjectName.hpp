/*
 * @file ExInvalidObjectName.h
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simph_kern_ExInvalidObjectName_HPP__
#define __simph_kern_ExInvalidObjectName_HPP__
#include "simph/kern/Exception.hpp"
#include "Smp/InvalidObjectName.h"

namespace simph {
	namespace kern {

/**
 *
 */
class ExInvalidObjectName: public Exception,
                        virtual public Smp::InvalidObjectName {
public:
    /**
     * Default constructor.
     */
    ExInvalidObjectName(Smp::IObject* sender, Smp::String8 invalidName);
    /**
     * Destructor.
     */
    virtual ~ExInvalidObjectName();
    // Smp::InvalidObjectName implementation
    Smp::String8 GetInvalidName() const noexcept override;
private:
    Smp::String8 _invalidName;
};

}} // namespace simph::kern
#endif // __simph_kern_ExInvalidObjectName_HPP__
