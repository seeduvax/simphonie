/*
 * @file ExTypeAlreadyRegistered.h
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simph_kern_ExTypeAlreadyRegistered_HPP__
#define __simph_kern_ExTypeAlreadyRegistered_HPP__
#include "simph/kern/Exception.hpp"
#include "Smp/Publication/TypeAlreadyRegistered.h"

namespace simph {
	namespace kern {

/**
 *
 */
class ExTypeAlreadyRegistered: public Exception,
        virtual public Smp::Publication::TypeAlreadyRegistered {
public:
    /**
     * Default constructor.
     */
    ExTypeAlreadyRegistered(Smp::IObject* sender, Smp::String8 name,
                        Smp::Publication::IType* type);
    /**
     * Destructor.
     */
    virtual ~ExTypeAlreadyRegistered();
    // Smp::Publication::TypeAlreadyRegistered implementation
    Smp::String8 GetTypeName() const noexcept override;
    Smp::Publication::IType* GetType() const noexcept override;
private:
    Smp::String8 _name;
    Smp::Publication::IType* _type;
};

}} // namespace simph::kern
#endif // __simph_kern_ExTypeAlreadyRegistered_HPP__
