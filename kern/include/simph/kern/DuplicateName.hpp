/*
 * @file DuplicateName.h
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simph_kern_DuplicateName_HPP__
#define __simph_kern_DuplicateName_HPP__
#include "Smp/DuplicateName.h"
#include "simph/kern/Exception.hpp"
#include <string>

namespace simph {
	namespace kern {

/**
 *
 */
class DuplicateName: public Exception,
                    public virtual Smp::DuplicateName {
public:
    /**
     * Default constructor.
     */
    DuplicateName(Smp::IObject* sender,Smp::String8 dName);
    /**
     * Destructor.
     */
    virtual ~DuplicateName();
    // Smp::DuplicateName implementation
    Smp::String8 GetDuplicateName() const noexcept;

private:
    std::string _dName;
};

}} // namespace simph::kern
#endif // __simph_kern_DuplicateName_HPP__
