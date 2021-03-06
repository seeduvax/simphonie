/*
 * @file ExDuplicateUuid.h
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simph_kern_ExDuplicateUuid_HPP__
#define __simph_kern_ExDuplicateUuid_HPP__
#include "Smp/DuplicateUuid.h"
#include "simph/smpdk/Exception.hpp"

namespace simph {
namespace kern {
using namespace simph::smpdk;

/**
 *
 */
class ExDuplicateUuid : public Exception, virtual public Smp::DuplicateUuid {
public:
    /**
     * Default constructor.
     */
    ExDuplicateUuid(Smp::IObject* sender, Smp::String8 oldName, Smp::String8 newName);
    /**
     * Destructor.
     */
    virtual ~ExDuplicateUuid();
    // Smp::DuplicateUuid implementation
    Smp::String8 GetOldName() const noexcept override;
    Smp::String8 GetNewName() const noexcept override;

private:
    Smp::String8 _oldName;
    Smp::String8 _newName;
};

}  // namespace kern
}  // namespace simph
#endif  // __simph_kern_ExDuplicateUuid_HPP__
