/*
 * @file FixedStringType.hpp
 *
 * Copyright 2025 Sebastien Devaux. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simdeck_FixedStringType_HPP__
#define __simdeck_FixedStringType_HPP__

#include "Smp/Publication/IStringType.h"
#include "simdeck/Type.hpp"

namespace simdeck {

/**
 *
 */
class FixedStringType: public Type, virtual public Smp::Publication::IStringType {
    typedef Type Parent;
public:
    /**
     * Default constructor.
     */
    FixedStringType(Smp::Uuid uuid, Smp::UInt64 maxLength, Smp::String8 name, Smp::String8 descr = "", Smp::IObject* parent = nullptr);
    /**
     * Destructor.
     */
    virtual ~FixedStringType();

    // Smp::Publication::IStringTYpe implementation
    Smp::UInt64 GetMaxLength() const override;
protected:
    virtual Smp::IField* createField(
        Smp::String8 name,
        Smp::String8 description,
        Smp::IComponent* parent,
        void* address,
        Smp::ViewKind view,
        Smp::Bool state,
        Smp::Bool input,
        Smp::Bool output) const override;
    
private:
    Smp::UInt64 _maxLength;
};

} // namespace simdeck
#endif // __simdeck_FixedStringType_HPP__
