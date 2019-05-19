/*
 * @file StructureType.cpp
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simph/kern/StructureType.hpp"

namespace simph {
	namespace kern {
// --------------------------------------------------------------------
// ..........................................................
StructureType::StructureType(Smp::Uuid uuid, Smp::String8 name, 
            Smp::String8 description, Smp::IObject* parent):
        Type(uuid,Smp::PrimitiveTypeKind::PTK_None,name,description,parent) {
}
// ..........................................................
StructureType::~StructureType() {
}
// --------------------------------------------------------------------
// ..........................................................
void StructureType::AddField(
            Smp::String8 name, Smp::String8 description, Smp::Uuid uuid,
            Smp::Int64 offset, Smp::ViewKind view,
            Smp::Bool state, Smp::Bool input,
            Smp::Bool output) {
    struct StructureType::FieldDescr fd;
    fd.name=name;
    fd.description=description;
    fd.uuid=uuid;
    fd.offset=offset;
    fd.view=view;
    fd.state=state;
    fd.input=input;
    fd.output=output;
    _fields.push_back(fd);
}

}} // namespace simph::kern
