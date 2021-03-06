/*
 * @file ExDuplicateLiteral.h
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simph_kern_ExDuplicateLiteral_HPP__
#define __simph_kern_ExDuplicateLiteral_HPP__
#include "Smp/Publication/DuplicateLiteral.h"
#include "simph/smpdk/Exception.hpp"

namespace simph {
namespace kern {
using namespace simph::smpdk;

/**
 *
 */
class ExDuplicateLiteral : public Exception, virtual public Smp::Publication::DuplicateLiteral {
public:
    /**
     * Default constructor.
     */
    ExDuplicateLiteral(Smp::IObject* sender, Smp::String8 name, Smp::Int32 value);
    /**
     * Destructor.
     */
    virtual ~ExDuplicateLiteral();

    // Smp::Publication::DuplicateLiteral implementation
    Smp::String8 GetLiteralName() const noexcept override;
    Smp::Int32 GetLiteralValue() const noexcept override;

private:
    Smp::String8 _lName;
    Smp::Int32 _lValue;
};

}  // namespace kern
}  // namespace simph
#endif  // __simph_kern_ExDuplicateLiteral_HPP__
