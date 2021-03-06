/*
 * @file ExVoidOperation.h
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simph_kern_ExVoidOperation_HPP__
#define __simph_kern_ExVoidOperation_HPP__
#include "Smp/VoidOperation.h"
#include "simph/smpdk/Exception.hpp"

namespace simph {
namespace kern {
using namespace simph::smpdk;

/**
 *
 */
class ExVoidOperation : public Exception, virtual public Smp::VoidOperation {
public:
    /**
     * Default constructor.
     */
    ExVoidOperation(Smp::IObject* sender, Smp::String8 opName);
    /**
     * Destructor.
     */
    virtual ~ExVoidOperation();
    // Smp::VoidOperation implementation
    Smp::String8 GetOperationName() const noexcept override;

private:
    Smp::String8 _opName;
};

}  // namespace kern
}  // namespace simph
#endif  // __simph_kern_ExVoidOperation_HPP__
