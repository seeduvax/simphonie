/*
 * @file ExInvalidObjectType.h
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simph_smpdk_ExInvalidObjectType_HPP__
#define __simph_smpdk_ExInvalidObjectType_HPP__
#include "simph/smpdk/Exception.hpp"
#include "Smp/InvalidObjectType.h"

namespace simph {
	namespace smpdk {

/**
 *
 */
class ExInvalidObjectType: public Exception,
                virtual public Smp::InvalidObjectType {
public:
    /**
     * Default constructor.
     */
    ExInvalidObjectType(Smp::IObject* sender, Smp::IObject* invalid);
    /**
     * Destructor.
     */
    virtual ~ExInvalidObjectType();
    // Smp::InvalidObjectType exception
    Smp::IObject* GetInvalidObject() const noexcept;

private:
    Smp::IObject* _invalidObject;
};

}} // namespace simph::smpdk
#endif // __simph_smpdk_ExInvalidObjectType_HPP__
