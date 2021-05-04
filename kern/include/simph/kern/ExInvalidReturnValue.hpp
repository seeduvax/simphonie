/*
 * @file ExInvalidReturnValue.hpp
 *
 * Copyright 2021 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simph_kern_ExInvalidReturnValue_HPP__
#define __simph_kern_ExInvalidReturnValue_HPP__
#include "Smp/InvalidReturnValue.h"
#include "simph/smpdk/Exception.hpp"

namespace simph {
namespace kern {

/**
 *
 */
class ExInvalidReturnValue : public simph::smpdk::Exception, virtual public Smp::InvalidReturnValue {
public:
    /**
     * Default constructor.
     */
    ExInvalidReturnValue(Smp::IObject* sender, Smp::String8 opname, Smp::AnySimple value);
    /**
     * Destructor.
     */
    virtual ~ExInvalidReturnValue();

    Smp::String8 GetOperationName() const noexcept override;

    Smp::AnySimple GetValue() const noexcept override;

private:
    Smp::String8 _opname;
    Smp::AnySimple _value;
};

}  // namespace kern
}  // namespace simph
#endif  // __simph_kern_ExInvalidReturnValue_HPP__
