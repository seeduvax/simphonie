/*
 * @file FieldWrap.cpp
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simph/kern/FieldWrap.hpp"
#include <string.h>

namespace simph {
	namespace kern {
// --------------------------------------------------------------------
// ..........................................................
FieldWrap::FieldWrap(Smp::String8 name, Smp::String8 description,
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
}
// ..........................................................
FieldWrap::~FieldWrap() {
}
// ..........................................................
Smp::ViewKind FieldWrap::GetView() const {
    return _viewKind;
}
// ..........................................................
Smp::Bool FieldWrap::IsState() const {
    return _stateType;
}
// ..........................................................
Smp::Bool FieldWrap::IsInput() const {
    return _inputType;
}
// ..........................................................
Smp::Bool FieldWrap::IsOutput() const {
    return _outputType;
}
// ..........................................................
const Smp::Publication::IType* FieldWrap::GetType() const {
    return _type;
}
// ..........................................................
void FieldWrap::connect(FieldWrap* src) {
    if (src->_dataSize==_dataSize) {
        _src=src;
    }
}
// ..........................................................
void FieldWrap::update() {
    memcpy(src->_data,_data,_dataSize);
}


}} // namespace simph::kern
