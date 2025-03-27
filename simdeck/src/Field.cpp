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
             Smp::Publication::IType* type, Smp::Bool isState,
             Smp::Bool isInput, Smp::Bool isOutput,
             Smp::IObject* parent)
    : Persist(name, description, parent),
      _stateType(isState),
      _inputType(isInput),
      _outputType(isOutput),
      _type(type),
      _viewKind(viewKind),
      _data(address == nullptr ? malloc(dataSize) : address),
      _dataSize(dataSize),
      _allocated(address == nullptr) {
    //Moved from Object to Field because ArrayField can have a Collection component
    if ((dynamic_cast<Smp::IArrayField*>(parent)!=nullptr
        || dynamic_cast<Smp::ISimpleArrayField*>(parent)!=nullptr)) {
        // when object is a array member, its name shall be
        // "[i]" with i the 0 based integer index
        if (!std::regex_match(name, std::regex("\\[[0-9][0-9]*\\]"))) {
            throw ExInvalidArrayMemberBadFormat(this, name);
        }
    }
}
// ..........................................................
Field::~Field() {
    if (_allocated) {
        free(_data);
    }
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

// --------------------------------------------------------------------
// StructureField
// ..........................................................
StructureField::StructureField(Smp::String8 name, Smp::String8 description, Smp::ViewKind viewKind, void* address,
                               Smp::Publication::IType* type, Smp::Bool isState, Smp::Bool isInput, Smp::Bool isOutput,
                               Smp::IObject* parent)
    : Field(name, description, viewKind, address, 0, type, isState, isInput, isOutput, parent) {
    auto st = dynamic_cast<StructureType*>(type);
    if (st != nullptr) {
        st->setup(this);
    }
    else {
// TODO        LOGE("Can't setup field " << name << ", its type " << type->GetName() << " is not an expected Structure Type.");
    }
}
// ..........................................................
StructureField::~StructureField() {}
// ..........................................................
/* TODO to be restored with OutputField
void StructureField::Push() {
    for (auto f : _fields) {
        f->Push();
    }
}
// ..........................................................
void StructureField::Connect(Smp::IField* target) {
    auto f = dynamic_cast<StructureField*>(target);
    if (f != nullptr && f->GetType()->GetUuid() == GetType()->GetUuid() && IsOutput() && f->IsInput()
        && _fields.size() == f->_fields.size()) {
        for (int i = 0; i < _fields.size(); i++) {
            _fields[i]->Connect(f->_fields[i]);
        }
    }
    else {
        throw ExInvalidTarget(this, target);
    }
}
*/
// ..........................................................
void StructureField::addField(Smp::IField* f) {
    _fields.push_back(f);
}




}  // namespace simdeck
