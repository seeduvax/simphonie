/*
 * @file ExInvalidObjectName.h
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simph_smpdk_ExInvalidObjectName_HPP__
#define __simph_smpdk_ExInvalidObjectName_HPP__
#include "Smp/InvalidObjectName.h"
#include "simph/smpdk/Exception.hpp"

namespace simph {
namespace smpdk {

/**
 *
 */
class ExInvalidObjectName : public Exception, virtual public Smp::InvalidObjectName {
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

}  // namespace smpdk
}  // namespace simph
#endif  // __simph_smpdk_ExInvalidObjectName_HPP__
