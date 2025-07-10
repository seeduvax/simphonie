/*
 * @file StructureField.cpp
 *
 * Copyright 2025 Sebastien Devaux. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simdeck/StructureField.hpp"
#include "simdeck/SimpleField.hpp"
#include "simdeck/SimpleArrayField.hpp"
#include "Smp/Publication/IArrayType.h"

namespace simdeck {
// ..........................................................
StructureField::StructureField(Smp::String8 name, Smp::String8 description, Smp::ViewKind viewKind, void* address,
                               const Smp::Publication::IType* type, Smp::Bool isState, Smp::Bool isInput, Smp::Bool isOutput,
                               Smp::IObject* parent,
                               std::vector<StructureType::FieldDescr>& fieldsSpec)
    : Field(name, description, viewKind, address, 0, type, isState, isInput, isOutput, parent) {
    auto st = dynamic_cast<const StructureType*>(type);
    for (auto fd: fieldsSpec) {
        Smp::IField* f=nullptr;
        auto st=dynamic_cast<Smp::Publication::IArrayType*>(fd.type);
        if (st!=nullptr) {
            f=SimpleArrayField::Create(fd.name, fd.description, 
                        st->GetSize(),
                        (void*)((uint8_t*)address + fd.offset),
                        st->GetItemType(),
                        fd.view,
                        fd.type, 
                        isState, isInput, isOutput, this); 
        }
        else if (type->GetPrimitiveTypeKind()!=Smp::PrimitiveTypeKind::PTK_None) {
            f=SimpleField::Create(fd.name, fd.description, fd.view, fd.type,
                        (void*)((uint8_t*)address + fd.offset),
                        isState, isInput, isOutput, this);
        }
        if (f!=nullptr) {
            _fields.push_back(f);
        }
    }
}
// ..........................................................
StructureField::~StructureField() {}
// ..........................................................
/* TODO to be restored with OutputField
void StructureField::Push() {
    for (auto f : _fields) {
        f->Push();
    }
}
// ..........................................................
void StructureField::Connect(Smp::IField* target) {
    auto f = dynamic_cast<StructureField*>(target);
    if (f != nullptr && f->GetType()->GetUuid() == GetType()->GetUuid() && IsOutput() && f->IsInput()
        && _fields.size() == f->_fields.size()) {
        for (int i = 0; i < _fields.size(); i++) {
            _fields[i]->Connect(f->_fields[i]);
        }
    }
    else {
        throw ExInvalidTarget(this, target);
    }
}
*/

// ..........................................................
void StructureField::Store(Smp::IStorageWriter* writer ) {
    for (auto f: _fields) {
        f->Store(writer);
    }
}
// ..........................................................
void StructureField::Restore(Smp::IStorageReader* reader) {
    for (auto f: _fields) {
        f->Restore(reader);
    }
}
} // namespace simdeck
