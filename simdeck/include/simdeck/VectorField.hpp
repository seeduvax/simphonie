/*
 * @file VectorField.hpp
 *
 * Copyright 2026 Sebastien Devaux. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simdeck_VectorField_HPP__
#define __simdeck_VectorField_HPP__

#include "Smp/IArrayField.h"
#include "simdeck/Field.hpp"
#include "simdeck/VectorType.hpp"

namespace simdeck {

template<typename T>
std::vector<Smp::IField*> createItemFieldsFrom(T& data);

template<typename T>
uint32_t getCurrentVectorSize(T& data);


template<typename T>
class VectorField: public Field, virtual public Smp::IArrayField {
    typedef Field Parent;
public:
    VectorField(
            Smp::String8 name, Smp::String8 description,
            Smp::ViewKind viewKind, T* address,
            const VectorType* vectorType,
            Smp::Bool isState, Smp::Bool isInput, Smp::Bool isOutput,
            Smp::IObject* parent
    ): Parent(name, description, viewKind, address, 0, vectorType,
            isState, isInput, isOutput, parent), _pObjT(address) {
        updateFields();
    }

    virtual ~VectorField() {
        for (auto f : _fields) {
            delete f;
        }
    }

    // IArrayField implementation
    Smp::UInt64 GetSize() override {
        return _fields.size();
    }

    Smp::IField* GetItem(Smp::UInt64 index) const override {
        return (index < _fields.size()) ?_fields[index] : nullptr;
    }

    void updateFields() {
        if (getCurrentVectorSize(*_pObjT) == _fields.size()) {
            // do nothing since size matches
            return;
        }
        for (auto f: createItemFieldsFrom(*_pObjT)) {
            _fields.push_back(f);
        }
    }

    // to be used by IOutputField ?
    void copyFrom(const VectorField<T>& other) {
        *_pObjT = other._pObjT;
        updateFields();
    }

private:
    T* _pObjT;
    std::vector< Smp::IField* > _fields;
};

} // namespace simdeck
#endif // __simdeck_VectorField_HPP__
