/*
 * @file ExInvalidPrimitiveType.h
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simph_kern_ExInvalidPrimitiveType_HPP__
#define __simph_kern_ExInvalidPrimitiveType_HPP__
#include "simph/kern/Exception.hpp"
#include "Smp/Publication/InvalidPrimitiveType.h"

namespace simph {
	namespace kern {

/**
 *
 */
class ExInvalidPrimitiveType: public Exception,
        virtual public Smp::Publication::InvalidPrimitiveType {
public:
    /**
     * Default constructor.
     */
    ExInvalidPrimitiveType(Smp::IObject* sender, Smp::PrimitiveTypeKind type);
    /**
     * Destructor.
     */
    virtual ~ExInvalidPrimitiveType();
    // Smp::Publication::InvalidPrimitiveType implementation
    Smp::String8 GetTypeName() const noexcept override;
    Smp::PrimitiveTypeKind GetType() const noexcept override;
private:
    Smp::PrimitiveTypeKind _type;
};

}} // namespace simph::kern
#endif // __simph_kern_ExInvalidPrimitiveType_HPP__
