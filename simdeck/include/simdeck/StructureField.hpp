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

#include <tuple>
#include "Smp/IOutputField.h"
#include "Smp/IStructureField.h"
#include "simdeck/Collection.hpp"
#include "simdeck/Field.hpp"
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

// ..........................................................
class StructureOutputField : public StructureField, virtual public Smp::IOutputField {
public:
    StructureOutputField(Smp::String8 name, Smp::String8 description, Smp::ViewKind viewKind, void* address,
                         const Smp::Publication::IType* type, Smp::Bool isState, Smp::Bool isInput, Smp::Bool isOutput,
                         Smp::IObject* parent)
        : StructureField(name, description, viewKind, address, type, isState, isInput, isOutput, parent) {}
    virtual ~StructureOutputField() {}
    // Smp::IOutputField implementation
    void Push() override;
    void Connect(Smp::IField* target) override;
    void Disconnect(Smp::IField* target) override;
    const Smp::FieldCollection* GetInputFields() const override {
        return &_targets;
    }
    Smp::Bool IsAutomatic() const override {
        return false;
    }

private:
    std::vector<std::tuple<Smp::IOutputField*, int> > _outputFields;
    Collection<Smp::IField> _targets;
};

} // namespace simdeck
#endif // __simdeck_StructureField_HPP__
