/*
 * @file Field.cpp
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simph/kern/Field.hpp"
#include <string.h>

namespace simph {
	namespace kern {
// --------------------------------------------------------------------
// ..........................................................
Field::Field(Smp::String8 name, Smp::String8 description,
        Smp::ViewKind viewKind, 
        void* address, unsigned int dataSize,
        Smp::Bool isState,
        Smp::Bool isInput,
        Smp::Bool isOutput) {
    setName(name);
    setDescription(description);
    _viewKind=viewKind;
    _data=address;
    _dataSize=dataSize;
    _inputType=isInput;
    _outputType=isOutput;
    _stateType=isState;
    _src=nullptr;
    _forced=false;
}
// ..........................................................
Field::~Field() {
}
// --------------------------------------------------------------------
// ..........................................................
Smp::ViewKind Field::GetView() const {
    return _viewKind;
}
// ..........................................................
Smp::Bool Field::IsState() const {
    return _stateType;
}
// ..........................................................
Smp::Bool Field::IsInput() const {
    return _inputType;
}
// ..........................................................
Smp::Bool Field::IsOutput() const {
    return _outputType;
}
// ..........................................................
const Smp::Publication::IType* Field::GetType() const {
    return _type;
}
// ..........................................................
void Field::connect(Field* src) {
    if (src->_dataSize==_dataSize) {
        _src=src;
    }
}
// ..........................................................
void Field::update() {
    memcpy(_src->_data,_data,_dataSize);
}
// --------------------------------------------------------------------
// Smp::IForcibleField implementation
// ..........................................................
void Field::Force(Smp::AnySimple value) {
    _forcedValue=value;
    _forced=true;
}
// ..........................................................
void Field::Unforce() {
    _forced=false;
}
// ..........................................................
Smp::Bool Field::IsForced() {
    return _forcedValue;
}
// ..........................................................
void Field::Freeze() {
    Force(GetValue());
}

}} // namespace simph::kern
