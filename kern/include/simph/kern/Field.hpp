/*
 * @file Field.h
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simph_kern_Field_HPP__
#define __simph_kern_Field_HPP__
#include "Smp/IField.h"
#include "simph/kern/Persist.hpp"

namespace simph {
	namespace kern {

/**
 *
 */
class Field: virtual public Persist, virtual public Smp::IField {
public:
    /**
     * Default constructor.
     */
    Field(Smp::String8 name, Smp::String8 description,
            Smp::ViewKind viewKind, void* address,
            unsigned int dataSize,
            Smp::Bool isState,
            Smp::Bool isInput,
            Smp::Bool isOutput
            );
    /**
     * Destructor.
     */
    virtual ~Field();
    // Smp::IField implementation
    Smp::ViewKind GetView() const;
    Smp::Bool IsState() const;
    Smp::Bool IsInput() const;
    Smp::Bool IsOutput() const;
    const Smp::Publication::IType* GetType() const;
    void copyFrom(Field* src);
private:
    Smp::Bool _stateType;
    Smp::Bool _inputType;
    Smp::Bool _outputType;
    Smp::Publication::IType* _type;
    Smp::ViewKind _viewKind;
    void* _data;
    unsigned int _dataSize;
};

template <typename T>
class TField: public Field {
public:
    TField(Smp::String8 name, Smp::String8 description,
            Smp::ViewKind viewKind, T* address,
            Smp::Bool isState,
            Smp::Bool isInput,
            Smp::Bool isOutput
            ): Field(name,description,viewKind,(void*)address,sizeof(T),
                        isState,isInput,isOutput) {
    }
    virtual ~TField() {
    }

private:
    T* _tData;
};

}} // namespace simph::kern
#endif // __simph_kern_Field_HPP__
