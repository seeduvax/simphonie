/*
 * @file Field.h
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simph_smpdk_Field_HPP__
#define __simph_smpdk_Field_HPP__
#include <cstring>
#include <vector>
#include "Smp/IForcibleField.h"
#include "simph/smpdk/Persist.hpp"
#include "simph/smpdk/Type.hpp"

namespace simph {
namespace smpdk {
/**
 *  IPersist
 */
class Field : public Persist, virtual public Smp::IField {
public:
    /**
     * Destructor.
     */
    virtual ~Field();
    // Smp::IField implementation
    Smp::ViewKind GetView() const override;
    Smp::Bool IsState() const override;
    Smp::Bool IsInput() const override;
    Smp::Bool IsOutput() const override;
    const Smp::Publication::IType* GetType() const override;

protected:
    /**
     * Create a field from existing memory space or using its own memory space.
     * This constructor is protected since this Field class is not yet fully
     * concrete but is supposed to be extended.
     * @param name field name
     * @param description field destriction
     * @param viewKind field view kind.
     * @param address to wrap memory space address or nullptr to let the field
     *        allocate itself the memory.
     * @param dataSize to wrap memory space size.
     * @param type data field type
     * @param isState state flag.
     * @param isInput input flag.
     * @param isOutput output flag.
     */
    Field(Smp::String8 name, Smp::String8 description, Smp::ViewKind viewKind, void* address, unsigned int dataSize,
          Smp::Publication::IType* type, Smp::Bool isState, Smp::Bool isInput, Smp::Bool isOutput,
          Smp::IObject* parent);
    inline void setType(Smp::Publication::IType* type) {
        _type = type;
    }
    inline const void* getDataPtr() {
        return _data;
    }

private:
    /** state flag */
    Smp::Bool _stateType;
    /** input flag */
    Smp::Bool _inputType;
    /** output flag */
    Smp::Bool _outputType;
    /** field type */
    const Smp::Publication::IType* _type;
    /** view kind */
    Smp::ViewKind _viewKind;
    /** data address */
    void* _data;
    /** data byte size */
    unsigned int _dataSize;
    /** memory owner flag */
    bool _allocated;
};



class StructureField : public Field {
public:
    StructureField(Smp::String8 name, Smp::String8 description, Smp::ViewKind viewKind, void* address,
                   Smp::Publication::IType* type, Smp::Bool isState, Smp::Bool isInput, Smp::Bool isOutput,
                   Smp::IObject* parent);
    virtual ~StructureField();
    void addField(Smp::IField* f);
    inline const void* getAddress(Smp::Int64 offset = 0) {
        return (const void*)((int64_t)getDataPtr() + offset);
    }
    inline const std::vector<Smp::IField*> getFields() const {
        return _fields;
    }

private:
    std::vector<Smp::IField*> _fields;
};





// --------------------------------------------------------------------
// Primitive type instanciation
// ..........................................................
static Type _char8Type(Smp::Uuids::Uuid_Char8, Smp::PrimitiveTypeKind::PTK_Char8, sizeof(Smp::Char8), "Char8",
                       "One byte char data type");
static Type _boolType(Smp::Uuids::Uuid_Bool, Smp::PrimitiveTypeKind::PTK_Bool, sizeof(Smp::Bool), "Bool",
                      "Bool data type");
static Type _int8Type(Smp::Uuids::Uuid_Int8, Smp::PrimitiveTypeKind::PTK_Int8, sizeof(Smp::Int8), "Int8",
                      "One byte signed int data type");
static Type _int16Type(Smp::Uuids::Uuid_Int16, Smp::PrimitiveTypeKind::PTK_Int16, sizeof(Smp::Int16), "Int16",
                       "Two bytes signed int data type");
static Type _int32Type(Smp::Uuids::Uuid_Int32, Smp::PrimitiveTypeKind::PTK_Int32, sizeof(Smp::Int32), "Int32",
                       "Four bytes signed int data type");
static Type _int64Type(Smp::Uuids::Uuid_Int64, Smp::PrimitiveTypeKind::PTK_Int64, sizeof(Smp::Int64), "Int64",
                       "Eight bytes signed int data type");
static Type _uint8Type(Smp::Uuids::Uuid_UInt8, Smp::PrimitiveTypeKind::PTK_UInt8, sizeof(Smp::UInt8), "UInt8",
                       "One byte unsigned int data type");
static Type _uint16Type(Smp::Uuids::Uuid_UInt16, Smp::PrimitiveTypeKind::PTK_UInt16, sizeof(Smp::UInt16), "UInt16",
                        "Two bytes unsigned int data type");
static Type _uint32Type(Smp::Uuids::Uuid_UInt32, Smp::PrimitiveTypeKind::PTK_UInt32, sizeof(Smp::UInt32), "UInt32",
                        "Four bytes unsigned int data type");
static Type _uint64Type(Smp::Uuids::Uuid_UInt64, Smp::PrimitiveTypeKind::PTK_UInt64, sizeof(Smp::UInt64), "UInt64",
                        "Eight bytes unsigned int data type");
static Type _float32Type(Smp::Uuids::Uuid_Float32, Smp::PrimitiveTypeKind::PTK_Float32, sizeof(Smp::Float32), "Float32",
                         "Four bytes float data type");
static Type _float64Type(Smp::Uuids::Uuid_Float64, Smp::PrimitiveTypeKind::PTK_Float64, sizeof(Smp::Float64), "Float64",
                         "Eight bytes float data type");

}  // namespace smpdk
}  // namespace simph
#endif  // __simph_smpdk_Field_HPP__
