/*
 * @file StructureType.h
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simdeck_StructureType_HPP__
#define __simdeck_StructureType_HPP__
#include <vector>
#include "Smp/Publication/IStructureType.h"
#include "simdeck/Type.hpp"
#include "Smp/Publication/ITypeRegistry.h"

namespace simdeck {
class StructureField;
/**
 *
 */
class StructureType : public Type, virtual public Smp::Publication::IStructureType {
public:
    /**
     * Default constructor.
     */
    StructureType(Smp::Uuid uuid, Smp::Publication::ITypeRegistry* typeReg, Smp::String8 name, Smp::String8 description = "",
                  Smp::IObject* parent = nullptr);
    /**
     * Destructor.
     */
    virtual ~StructureType();
    // Smp::Publication::IStructureType implementation
    void AddField(Smp::String8 name,
                  Smp::String8 description,
                  Smp::Uuid uuid,
                  Smp::UInt64 offset,
                  Smp::ViewKind view = Smp::ViewKind::VK_All,
                  Smp::Bool state = true,
                  Smp::Bool input = false,
                  Smp::Bool output = false) override;
    /**
     * Setup structure field according to this structure type definition.
     * @param sf StructureField to setup
     */
    void setup(StructureField* sf);

private:
    struct FieldDescr {
        Smp::String8 name;
        Smp::String8 description;
        Smp::Uuid uuid;
        Smp::Int64 offset;
        Smp::UInt64 size;
        Smp::ViewKind view;
        Smp::Bool state;
        Smp::Bool input;
        Smp::Bool output;
    };
    std::vector<struct FieldDescr> _fields;
    Smp::Publication::ITypeRegistry* _typeRegistry;
};

}  // namespace simdeck
#endif  // __simdeck_StructureType_HPP__
