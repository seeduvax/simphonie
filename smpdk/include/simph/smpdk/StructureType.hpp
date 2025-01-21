/*
 * @file StructureType.h
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simph_smpdk_StructureType_HPP__
#define __simph_smpdk_StructureType_HPP__
#include <vector>
#include "Smp/Publication/IStructureType.h"
#include "simph/smpdk/Type.hpp"
#include "Smp/Publication/ITypeRegistry.h"

namespace simph {
namespace smpdk {
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

}  // namespace smpdk
}  // namespace simph
#endif  // __simph_smpdk_StructureType_HPP__
