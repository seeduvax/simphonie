/*
 * @file StructureField.hpp
 *
 * Copyright 2025 Sebastien Devaux. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simdeck_StructureField_HPP__
#define __simdeck_StructureField_HPP__

#include "Smp/IStructureField.h"
#include "simdeck/Field.hpp"
#include "simdeck/Collection.hpp"
#include "simdeck/StructureType.hpp"

namespace simdeck {

class StructureField : public Field, virtual public Smp::IStructureField {
protected:
    StructureField(Smp::String8 name, Smp::String8 description, Smp::ViewKind viewKind, void* address,
                   const Smp::Publication::IType* type, Smp::Bool isState, Smp::Bool isInput, Smp::Bool isOutput,
                   Smp::IObject* parent);
public:
    virtual ~StructureField();
    static Smp::IField* Create(Smp::String8 name, Smp::String8 description, Smp::ViewKind viewKind, void* address,
                   const Smp::Publication::IType* type, Smp::Bool isState, Smp::Bool isInput, Smp::Bool isOutput,
                   Smp::IObject* parent, const std::vector<StructureType::FieldDescr>& fieldsDescr);
    inline const Smp::FieldCollection* GetFields() const override {
        return &_fields;
    }
    inline Smp::IField* GetField(Smp::String8 name) const override {
        return _fields.at(name);
    }
    void Store(Smp::IStorageWriter* writer) override;
    void Restore(Smp::IStorageReader* reader) override;
protected:
    inline void addField(Smp::IField* field) {
        _fields.push_back(field);
    }

private:
    OwnedCollection<Smp::IField> _fields;
};

} // namespace simdeck
#endif // __simdeck_StructureField_HPP__
