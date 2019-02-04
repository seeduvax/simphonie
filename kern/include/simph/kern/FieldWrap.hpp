/*
 * @file FieldWrap.h
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simph_kern_FieldWrap_HPP__
#define __simph_kern_FieldWrap_HPP__
#include "Smp/IField.h"
#include "simph/kern/Persist.hpp"

namespace simph {
	namespace kern {

/**
 *
 */
class FieldWrap: virtual public Persist, virtual public Smp::IField {
public:
    /**
     * Default constructor.
     */
    FieldWrap(Smp::String8 name, Smp::String8 description,
            Smp::ViewKind viewKind, void* address,
            unsigned int dataSize,
            Smp::Bool isState,
            Smp::Bool isInput,
            Smp::Bool isOutput
            );
    /**
     * Destructor.
     */
    virtual ~FieldWrap();
    // Smp::IFieldWrap implementation
    Smp::ViewKind GetView() const;
    Smp::Bool IsState() const;
    Smp::Bool IsInput() const;
    Smp::Bool IsOutput() const;
    const Smp::Publication::IType* GetType() const;
    void connect(FieldWrap* src);
    void update();
private:
    Smp::Bool _stateType;
    Smp::Bool _inputType;
    Smp::Bool _outputType;
    Smp::Publication::IType* _type;
    Smp::ViewKind _viewKind;
    void* _data;
    unsigned int _dataSize;
    FieldWrap* _src;
};

template <typename T>
class TFieldWrap: public FieldWrap {
public:
    TFieldWrap(Smp::String8 name, Smp::String8 description,
            Smp::ViewKind viewKind, T* address,
            Smp::Bool isState,
            Smp::Bool isInput,
            Smp::Bool isOutput
            ): FieldWrap(name,description,viewKind,(void*)address,sizeof(T),
                        isState,isInput,isOutput) {
    }
    virtual ~TFieldWrap() {
    }

private:
    T* _tData;
};

}} // namespace simph::kern
#endif // __simph_kern_FieldWrap_HPP__
