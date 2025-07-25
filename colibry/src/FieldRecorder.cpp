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

#include <ctime>
#include <iomanip>

#include "Smp/ISimulator.h"
#include "Smp/IStorageReader.h"
#include "Smp/IStorageWriter.h"
#include "Smp/Services/IEventManager.h"
#include "simdeck/StringType.hpp"
#include "simdeck/Type.hpp"

namespace simphonie {
namespace colibry {

class RecorderPortType : public simdeck::Type {
public:
    RecorderPortType(Smp::String8 name, Smp::String8 descr)
        : simdeck::Type(Smp::Uuids::Uuid_Void, Smp::PrimitiveTypeKind::PTK_None, name, descr) {}
    virtual ~RecorderPortType() {}

protected:
    Smp::IField* createField(Smp::String8 name, Smp::String8 description, Smp::IComponent* parent, void* address,
                             Smp::ViewKind view, Smp::Bool state, Smp::Bool input, Smp::Bool output) const override {
        return nullptr;
    }
};

static RecorderPortType _recorderType("RecorderType", "Dummy type for the field recording service as output field.");
// --------------------------------------------------------------------
// ..........................................................
FieldRecorder::FieldRecorder(Smp::String8 name, Smp::String8 description, Smp::IObject* parent, Smp::String8 fileExt)
    : Parent(name, description, parent) {
    _fieldHolder = nullptr;
    _filePath = name;
    {
        auto t = std::time(nullptr);
        auto tm = std::localtime(&t);
        std::ostringstream oss;
        oss << std::put_time(tm, "_%d-%m-%Y_%H-%M-%S");
        _filePath += oss.str();
    }
    _filePath = _filePath + ".";
    if (fileExt!=nullptr) {
        _filePath = _filePath + fileExt;
    }
    else {
        _filePath = _filePath + "frec";
    }
    addEP("step","record fields snapshot", this, &FieldRecorder::step);
    addEP("flush", "Flush record file", this, &FieldRecorder::flush);
}
// ..........................................................
FieldRecorder::~FieldRecorder() {
    // do not delete field holder as soon it is published since it shall
    // be inserted as child field from component inheritence and shall not be
    // deleted twice.
}
// ..........................................................
void FieldRecorder::publish(Smp::IPublication* receiver) {
    _fieldHolder = new FieldHolder("port", "Fields to records connexion port", this);
    receiver->PublishField(_fieldHolder);
    receiver->PublishField("filePath", "", &_filePath, simdeck::StringType::UuidString, Smp::ViewKind::VK_All, false,
                           true, false);
}
// ..........................................................
void FieldRecorder::connect() {
    getSimulator()->GetEventManager()->Subscribe(Smp::Services::IEventManager::SMP_LeaveExecutingId,
                                                 GetEntryPoint("flush"));
}

// --------------------------------------------------------------------
// ..........................................................
FieldRecorder::FieldHolder::FieldHolder(Smp::String8 name, Smp::String8 description, Smp::IObject* parent)
    : FieldHolder::Parent(name, description, parent), _recFields() {}
// ..........................................................
FieldRecorder::FieldHolder::~FieldHolder() {}
// ..........................................................
void FieldRecorder::FieldHolder::Connect(Smp::IField* field) {
    _recFields.push_back(field);
}
// ..........................................................
void FieldRecorder::FieldHolder::Disconnect(Smp::IField* field) {
    _recFields.remove(field);
}
// ..........................................................
const Smp::FieldCollection* FieldRecorder::FieldHolder::GetInputFields() const {
    return &_recFields;
}
// -- fake implementation of other IOutputField services --
void FieldRecorder::FieldHolder::Push() {
    // no rela data to forward.
}
Smp::Bool FieldRecorder::FieldHolder::IsAutomatic() const {
    // do not pretend being automatic since it does nothing unless its step
    // entry point is published.
    return false;
}
Smp::ViewKind FieldRecorder::FieldHolder::GetView() const {
    return Smp::ViewKind::VK_None;
}
Smp::Bool FieldRecorder::FieldHolder::IsState() const {
    return false;
}
Smp::Bool FieldRecorder::FieldHolder::IsInput() const {
    return false;
}
Smp::Bool FieldRecorder::FieldHolder::IsOutput() const {
    // not really an output port, but shall pretend to be to let everything
    // works like it is.
    return true;
}
const Smp::Publication::IType* FieldRecorder::FieldHolder::GetType() const {
    return &_recorderType;
}
void FieldRecorder::FieldHolder::Store(Smp::IStorageWriter* writer) {
    // Make storage writer happy with writing one dummy byte.
    uint8_t b=0;
    writer->Store(&b,1);
}
void FieldRecorder::FieldHolder::Restore(Smp::IStorageReader* reader) {
    // Make storage reader happy with reading one dummy byte. Consistently with
    // Store() implementation
    uint8_t b=0;
    reader->Restore(&b,1);
}
}} // namespace simphonie::colibry
