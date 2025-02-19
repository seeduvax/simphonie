/*
 * @file DuplicateName.h
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simdeck_DuplicateName_HPP__
#define __simdeck_DuplicateName_HPP__
#include <string>
#include "Smp/DuplicateName.h"
#include "simdeck/Exception.hpp"

namespace simdeck {

/**
 *
 */
class ExDuplicateName : public Exception, public virtual Smp::DuplicateName {
public:
    /**
     * Default constructor.
     */
    ExDuplicateName(const Smp::IObject* sender, Smp::String8 dName);
    /**
     * Destructor.
     */
    virtual ~ExDuplicateName();
    // Smp::DuplicateName implementation
    Smp::String8 GetDuplicateName() const noexcept override;

private:
    std::string _dName;
};

}  // namespace simdeck
#endif  // __simdeck_DuplicateName_HPP__
