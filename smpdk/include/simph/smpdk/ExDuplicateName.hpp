/*
 * @file DuplicateName.h
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simph_smpdk_DuplicateName_HPP__
#define __simph_smpdk_DuplicateName_HPP__
#include <string>
#include "Smp/DuplicateName.h"
#include "simph/smpdk/Exception.hpp"

namespace simph {
namespace smpdk {

/**
 *
 */
class ExDuplicateName : public Exception, public virtual Smp::DuplicateName {
public:
    /**
     * Default constructor.
     */
    ExDuplicateName(Smp::IObject* sender, Smp::String8 dName);
    /**
     * Destructor.
     */
    virtual ~ExDuplicateName();
    // Smp::DuplicateName implementation
    Smp::String8 GetDuplicateName() const noexcept override;

private:
    std::string _dName;
};

}  // namespace smpdk
}  // namespace simph
#endif  // __simph_smpdk_DuplicateName_HPP__
