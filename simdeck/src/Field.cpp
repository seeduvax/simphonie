/*
 * @file Field.cpp
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simdeck/Field.hpp"
#include "simdeck/ExInvalidTarget.hpp"
#include "simdeck/Object.hpp"
#include "simdeck/StructureType.hpp"
#include "simdeck/Type.hpp"

#include "simdeck/ExInvalidObjectName.hpp"
#include "Smp/IArrayField.h"
#include "Smp/ISimpleArrayField.h"
#include <sstream>
#include <regex>

namespace simdeck {
// --------------------------------------------------------------------
// ..........................................................
class ExInvalidArrayMemberBadFormat: public ExInvalidObjectName {
public:
    ExInvalidArrayMemberBadFormat(Smp::IObject* sender, Smp::String8 invalidName):
            ExInvalidObjectName(sender, invalidName) {
        std::ostringstream d;
        d << "'" << invalidName << "' invalid name format, bad char or does not start with a letter.";
        setDescription(d.str().c_str());
        setMessage();
    }
    virtual ~ExInvalidArrayMemberBadFormat() {
    }
};
Field::Field(Smp::String8 name, Smp::String8 description,
             Smp::ViewKind viewKind, void* address, unsigned int dataSize,
             const Smp::Publication::IType* type, Smp::Bool isState,
             Smp::Bool isInput, Smp::Bool isOutput,
             Smp::IObject* parent)
    : Persist(name, description, parent,true),
      _stateType(isState),
      _inputType(isInput),
      _outputType(isOutput),
      _type(type),
      _viewKind(viewKind),
      _data(address == nullptr ? malloc(dataSize) : address),
      _dataSize(dataSize),
      _allocated(address == nullptr) {
    checkName(name);
}
// ..........................................................
Field::~Field() {
    if (_allocated) {
        free(_data);
    }
}
// --------------------------------------------------------------------
// ..........................................................

void Field::checkName(Smp::String8 name) {
    if ((dynamic_cast<Smp::IArrayField*>(GetParent())!=nullptr
        || dynamic_cast<Smp::ISimpleArrayField*>(GetParent())!=nullptr)) {
        // when object is a array member, its name shall be
        // "[i]" with i the 0 based integer index
        if (!std::regex_match(name, std::regex("\\[[0-9][0-9]*\\]"))) {
            throw ExInvalidArrayMemberBadFormat(this, name);
        }
    }
    else{
        Object::checkName(name);
    }
}

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

}  // namespace simdeck
