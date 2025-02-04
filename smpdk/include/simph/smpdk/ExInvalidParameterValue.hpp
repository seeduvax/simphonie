/*
 * @file ExInvalidParameterValue.h
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simph_smpdk_ExInvalidParameterValue_HPP__
#define __simph_smpdk_ExInvalidParameterValue_HPP__
#include "Smp/InvalidParameterValue.h"
#include "simph/smpdk/ExInvalidAnyType.hpp"

namespace simph {
namespace smpdk {

/**
 *
 */
class ExInvalidParameterValue : public ExInvalidAnyType, virtual public Smp::InvalidParameterValue {
public:
    /**
     * Default constructor.
     */
    ExInvalidParameterValue(
                const Smp::IObject* sender,
                Smp::String8 opName,
                Smp::String8 paramName,
                Smp::AnySimple invalidValue,
                Smp::PrimitiveTypeKind expected);
    /**
     * Destructor.
     */
    virtual ~ExInvalidParameterValue();
    // Smp::InvalidParameterValue implementation
    Smp::String8 GetOperationName() const noexcept override;
    Smp::String8 GetParameterName() const noexcept override;
private:
    Smp::String8 _opName;
    Smp::String8 _paramName;
};

}  // namespace smpdk
}  // namespace simph
#endif  // __simph_smpdk_ExInvalidParameterValue_HPP__
