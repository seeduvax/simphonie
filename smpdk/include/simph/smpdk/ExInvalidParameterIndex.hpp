/*
 * @file ExInvalidParameterIndex.h
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simph_smpdk_ExInvalidParameterIndex_HPP__
#define __simph_smpdk_ExInvalidParameterIndex_HPP__
#include "Smp/InvalidParameterIndex.h"
#include "simph/smpdk/Exception.hpp"

namespace simph {
namespace smpdk {
using namespace simph::smpdk;

/**
 *
 */
class ExInvalidParameterIndex : public Exception, virtual public Smp::InvalidParameterIndex {
public:
    /**
     * Default constructor.
     */
    ExInvalidParameterIndex(const Smp::IObject* sender, Smp::String8 opName, Smp::Int32 index, Smp::Int32 count);
    /**
     * Destructor.
     */
    virtual ~ExInvalidParameterIndex();
    // Smp::InvalidParameterIndex implementation
    Smp::String8 GetOperationName() const noexcept override;
    Smp::Int32 GetParameterIndex() const noexcept override;
    Smp::Int32 GetParameterCount() const noexcept override;

private:
    const Smp::String8 _opName;
    Smp::Int32 _index;
    Smp::Int32 _count;
};

}  // namespace smpdk
}  // namespace simph
#endif  // __simph_smpdk_ExInvalidParameterIndex_HPP__
