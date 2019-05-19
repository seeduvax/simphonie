/*
 * @file StructureType.h
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simph_kern_StructureType_HPP__
#define __simph_kern_StructureType_HPP__
#include "simph/kern/Type.hpp"
#include "Smp/Publication/IStructureType.h"
#include <vector>

namespace simph {
	namespace kern {

/**
 *
 */
class StructureType: public Type, 
                virtual public Smp::Publication::IStructureType {
public:
    /**
     * Default constructor.
     */
    StructureType(Smp::Uuid uuid, Smp::String8 name, 
                Smp::String8 description="", Smp::IObject* parent=nullptr);
    /**
     * Destructor.
     */
    virtual ~StructureType();
    // Smp::Publication::IStructureType implementation
    void AddField(
        Smp::String8 name, Smp::String8 description, Smp::Uuid uuid,
        Smp::Int64 offset, Smp::ViewKind view = Smp::ViewKind::VK_All,
        Smp::Bool state = true, Smp::Bool input = false,
        Smp::Bool output = false) override;

private:
    struct FieldDescr {
        Smp::String8 name;
        Smp::String8 description;
        Smp::Uuid uuid;
        Smp::Int64 offset;
        Smp::ViewKind view;
        Smp::Bool state;
        Smp::Bool input;
        Smp::Bool output;
    };
    std::vector<struct FieldDescr> _fields;
};

}} // namespace simph::kern
#endif // __simph_kern_StructureType_HPP__