/*
 * @file ClassType.h
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simph_smpdk_ClassType_HPP__
#define __simph_smpdk_ClassType_HPP__
#include "Smp/Publication/IClassType.h"
#include "simph/smpdk/StructureType.hpp"

namespace simph {
namespace smpdk {
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

}  // namespace smpdk
}  // namespace simph
#endif  // __simph_smpdk_ClassType_HPP__
