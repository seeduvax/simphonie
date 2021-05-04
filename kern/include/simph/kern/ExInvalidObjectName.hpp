/*
 * @file ExInvalidObjectName.hpp
 *
 * Copyright 2021 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simph_kern_ExInvalidObjectName_HPP__
#define __simph_kern_ExInvalidObjectName_HPP__
#include "Smp/InvalidObjectName.h"
#include "simph/smpdk/Exception.hpp"

namespace simph {
namespace kern {

/**
 *
 */
class ExInvalidObjectName : public simph::smpdk::Exception, public virtual Smp::Exception {
public:
    /**
     * Default constructor.
     */
    ExInvalidObjectName(Smp::IObject* sender, Smp::String8 objname);
    /**
     * Destructor.
     */
    virtual ~ExInvalidObjectName();

    Smp::String8 GetInvalidName() const noexcept;

private:
    Smp::String8 _objname;
};

}  // namespace kern
}  // namespace simph
#endif  // __simph_kern_ExInvalidObjectName_HPP__
