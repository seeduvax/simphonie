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
#include "Smp/IForcibleField.h"
#include "Smp/ISimpleField.h"
#include "simph/kern/Persist.hpp"

namespace simph {
	namespace kern {

/**
 *
 */
class Field: virtual public Persist, 
                    virtual public Smp::IForcibleField {
public:
    /**
     * Default constructor.
     */
    Field(Smp::String8 name, Smp::String8 description,
            Smp::ViewKind viewKind, void* address,
            unsigned int dataSize,
            Smp::Bool isState,
            Smp::Bool isInput,
            Smp::Bool isOutput,
            Smp::IObject* parent
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
    void connect(Field* src);
    void update();
    // Smp::IForcibleField implementation
    void Force(Smp::AnySimple value);
    void Unforce();
    Smp::Bool IsForced();
    void Freeze();

private:
    Smp::Bool _stateType;
    Smp::Bool _inputType;
    Smp::Bool _outputType;
    Smp::Publication::IType* _type;
    Smp::ViewKind _viewKind;
    void* _data;
    unsigned int _dataSize;
    Field* _src;
    Smp::Bool _forced;
    Smp::AnySimple _forcedValue;
};

template <typename T>
class TField: public Field {
public:
    TField(Smp::String8 name, Smp::String8 description,
            Smp::ViewKind viewKind, T* address,
            Smp::Bool isState,
            Smp::Bool isInput,
            Smp::Bool isOutput,
            Smp::IObject* parent
            ): Field(name,description,viewKind,(void*)address,sizeof(T),
                        isState,isInput,isOutput,parent) {
        _tData=address;
    }
    virtual ~TField() {
    }
    // Smp::ISimpleField implementation
    Smp::AnySimple GetValue() const;
    void SetValue(Smp::AnySimple value) {
        *_tData=value;
    }

private:
    T* _tData;
};

}} // namespace simph::kern
#endif // __simph_kern_Field_HPP__
