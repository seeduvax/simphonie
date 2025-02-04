/*
 * @file ExVoidOperation.h
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simph_smpdk_ExVoidOperation_HPP__
#define __simph_smpdk_ExVoidOperation_HPP__
#include "Smp/VoidOperation.h"
#include "simph/smpdk/Exception.hpp"

namespace simph {
namespace smpdk {
using namespace simph::smpdk;

/**
 *
 */
class ExVoidOperation : public Exception, virtual public Smp::VoidOperation {
public:
    /**
     * Default constructor.
     */
    ExVoidOperation(const Smp::IObject* sender, Smp::String8 opName);
    /**
     * Destructor.
     */
    virtual ~ExVoidOperation();
    // Smp::VoidOperation implementation
    Smp::String8 GetOperationName() const noexcept override;

private:
    Smp::String8 _opName;
};

}  // namespace smpdk
}  // namespace simph
#endif  // __simph_smpdk_ExVoidOperation_HPP__
