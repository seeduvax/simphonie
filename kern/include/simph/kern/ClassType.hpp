/*
 * @file ClassType.h
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simph_kern_ClassType_HPP__
#define __simph_kern_ClassType_HPP__
#include "simph/kern/StructureType.hpp"
#include "Smp/Publication/IClassType.h"

namespace simph {
	namespace kern {
class TypeRegistry;

/**
 *
 */
class ClassType: public StructureType,
            virtual public Smp::Publication::IClassType {
public:
    /**
     * Default constructor.
     */
    ClassType(Smp::Uuid, TypeRegistry* typeReg, Smp::String8 name, 
                Smp::String8 description="", Smp::IObject* parent=nullptr);
    /**
     * Destructor.
     */
    virtual ~ClassType();

private:

};

}} // namespace simph::kern
#endif // __simph_kern_ClassType_HPP__
