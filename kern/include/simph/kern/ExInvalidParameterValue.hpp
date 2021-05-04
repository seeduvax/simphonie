/*
 * @file ExInvalidParameterValue.h
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simph_kern_ExInvalidParameterValue_HPP__
#define __simph_kern_ExInvalidParameterValue_HPP__
#include "Smp/InvalidParameterValue.h"
#include "simph/smpdk/Exception.hpp"

namespace simph {
namespace kern {
using namespace simph::smpdk;

/**
 *
 */
class ExInvalidParameterValue : public Exception, virtual public Smp::InvalidParameterValue {
public:
    /**
     * Default constructor.
     */
    ExInvalidParameterValue(Smp::IObject* sender, Smp::String8 opName, Smp::AnySimple value);
    /**
     * Destructor.
     */
    virtual ~ExInvalidParameterValue();
    // Smp::InvalidParameterValue implementation
    Smp::String8 GetParameterName() const noexcept override;
    Smp::AnySimple GetValue() const noexcept override;
private:
    const Smp::String8 _pName;
    Smp::AnySimple _value;
};

}  // namespace kern
}  // namespace simph
#endif  // __simph_kern_ExInvalidParameterValue_HPP__
