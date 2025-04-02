/*
 * @file FieldRecorder.cpp
 *
 * Copyright 2025 Sebastien Devaux. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simphonie/colibry/FieldRecorder.hpp"
#include "simdeck/Type.hpp"

namespace simphonie {
namespace colibry {

static simdeck::Type _recorderType(Smp::Uuids::Uuid_Void, Smp::PrimitiveTypeKind::PTK_None, 0, "RecorderType",
                       "Dummy type for the field recording service as output field.");
// --------------------------------------------------------------------
// ..........................................................
FieldRecorder::FieldRecorder(Smp::String8 name, Smp::String8 description,
                    Smp::IObject* parent, Smp::String8 fileExt):
                     Parent(name, description, parent),
                    _recFields("recFields", "List of fields to record", this) {
    _filePath = name;
    // TODO consider adding date and time to the default file name to avoid
    // overwriting.
    _filePath = _filePath + ".";
    if (fileExt!=nullptr) {
        _filePath = _filePath + fileExt;
    }
    else {
        _filePath = _filePath + "frec";
    }
    addEP("step","record fields snapshot", this, &FieldRecorder::step);
    
}
// ..........................................................
FieldRecorder::~FieldRecorder() {
}
// --------------------------------------------------------------------
// IOutput field implementation
// ..........................................................
void FieldRecorder::Connect(Smp::IField* field) {
std::cout<<"!!!!!!!!!!!!!!!!!!!!! "<<field->GetName()<<std::endl; 
    _recFields.push_back(field);
}
// ..........................................................
void FieldRecorder::Disconnect(Smp::IField* field) {
    _recFields.remove(field);
}
// ..........................................................
const Smp::FieldCollection* FieldRecorder::GetInputFields() const {
    return &_recFields;
}
// -- fake implementation of other IOutputField services --
void FieldRecorder::Push() {
    // no rela data to forward.
}
Smp::Bool FieldRecorder::IsAutomatic() const {
    // do not pretend bneing automatic since it does nothing unless its step
    // entry point is published.
    return false;
}
Smp::ViewKind FieldRecorder::GetView() const {
    return Smp::ViewKind::VK_None;
}
Smp::Bool FieldRecorder::IsState() const {
    return false;
}
Smp::Bool FieldRecorder::IsInput() const {
    return false;
}
Smp::Bool FieldRecorder::IsOutput() const {
    return true;
}
const Smp::Publication::IType* FieldRecorder::GetType() const {
    return &_recorderType;
} 
void FieldRecorder::Restore(Smp::IStorageReader* reader) {
}
void FieldRecorder::Store(Smp::IStorageWriter* writer) {
}



}} // namespace simphonie::colibry
