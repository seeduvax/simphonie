/*
 * @file ClassType.h
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simdeck_ClassType_HPP__
#define __simdeck_ClassType_HPP__
#include "Smp/Publication/IClassType.h"
#include "simdeck/StructureType.hpp"

namespace simdeck {
class TypeRegistry;

/**
 *
 */
class ClassType : public StructureType, virtual public Smp::Publication::IClassType {
public:
    /**
     * Default constructor.
     */
    ClassType(Smp::Uuid, Smp::Publication::ITypeRegistry* typeReg, Smp::String8 name, Smp::String8 description = "",
              Smp::IObject* parent = nullptr);
    /**
     * Destructor.
     */
    virtual ~ClassType();

private:
};

}  // namespace simdeck
#endif  // __simdeck_ClassType_HPP__
