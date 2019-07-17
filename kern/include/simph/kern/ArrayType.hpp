/*
 * @file ArrayType.h
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simph_kern_ArrayType_HPP__
#define __simph_kern_ArrayType_HPP__
#include "simph/kern/Type.hpp"
#include "Smp/Publication/IArrayType.h"

namespace simph {
	namespace kern {

/**
 *
 */
class ArrayType: public Type,
        virtual public Smp::Publication::IArrayType {
public:
    /**
     * Default constructor.
     */
    ArrayType(Smp::Uuid uuid, Smp::String8 name, Smp::String8 descr, 
            Smp::IObject* parent,
            Smp::UInt64 elemSize, Smp::UInt64 size,Smp::Publication::IType* type);
    /**
     * Destructor.
     */
    virtual ~ArrayType();
    // Smp::Publication::IArrayType implementation
    Smp::UInt64 GetSize() const override;
    const Smp::Publication::IType* GetItemType() const override;
private:
    Smp::UInt64 _size;
    const Smp::Publication::IType* _itemType;
};

}} // namespace simph::kern
#endif // __simph_kern_ArrayType_HPP__
