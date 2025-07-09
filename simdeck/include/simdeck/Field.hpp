/*
 * @file Field.h
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simdeck_Field_HPP__
#define __simdeck_Field_HPP__
#include <cstring>
#include <vector>
#include "simdeck/Persist.hpp"
#include "simdeck/Type.hpp"
#include "Smp/IForcibleField.h"

namespace simdeck {
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
          const Smp::Publication::IType* type, Smp::Bool isState, Smp::Bool isInput, Smp::Bool isOutput,
          Smp::IObject* parent);
    inline const void* getDataPtr() {
        return _data;
    }
    void checkName(Smp::String8 name);

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

}  // namespace simdeck
#endif  // __simdeck_Field_HPP__
