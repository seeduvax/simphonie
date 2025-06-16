/*
 * @file SimSyncDataShare.cpp
 *
 * Copyright 2025. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simphonie/mt/SimSyncDataShare.hpp"
#include "simdeck/ExInvalidFieldName.hpp"
#include "simdeck/Type.hpp"

namespace simphonie {
namespace mt {

static simdeck::Type _dataShareType(Smp::Uuids::Uuid_Void, Smp::PrimitiveTypeKind::PTK_None, 0, "DataShareType",
                                    "Dummy type for the simulation synchronization service as output field.");

class SimSyncDataShare::FieldHolder : public simdeck::Object, virtual public Smp::IOutputField {
public:
    inline FieldHolder(Smp::String8 name, Smp::String8 description, Smp::IObject* parent)
        : simdeck::Object(name, description, parent), _fields("fields", "collection of fields to be held", this) {}
    ~FieldHolder() = default;
    /* Smp::IOutputField implementation. mostly faked except Connect and Disconnect */
    inline void Connect(Smp::IField* field) override {
        _fields.push_back(field);
    }
    inline void Disconnect(Smp::IField* field) override {
        _fields.remove(field);
    }
    inline const Smp::FieldCollection* GetInputFields() const override {
        return &_fields;
    }
    inline void Push() override {}
    inline Smp::Bool IsAutomatic() const override {
        return false;
    }
    inline Smp::ViewKind GetView() const override {
        return Smp::ViewKind::VK_None;
    }
    inline Smp::Bool IsState() const override {
        return false;
    }
    inline Smp::Bool IsInput() const override {
        return false;
    }
    inline Smp::Bool IsOutput() const override {
        return true;
    } /* actually not an usual output */
    inline const Smp::Publication::IType* GetType() const override {
        return &_dataShareType;
    }
    inline void Restore(Smp::IStorageReader* reader) override {
        /* useless but to prevent compiler warnings */
        uint8_t b = 0;
        reader->Restore(&b, 1);
    }
    inline void Store(Smp::IStorageWriter* writer) override {
        /* useless but to prevent compiler warnings */
        uint8_t b = 0;
        writer->Store(&b, 1);
    }

private:
    simdeck::Collection<Smp::IField> _fields;
};

SimSyncDataShare::SimSyncDataShare() {}

SimSyncDataShare::DataType SimSyncDataShare::retrieveData() {
    if (_outputs == nullptr) {
        return {};
    }
    _buf.clear();
    for (const auto field : *(_outputs->GetInputFields())) {
        field->Store(this);
    }
    return _buf;
}

void SimSyncDataShare::loadData(const DataType& data) {
    if (_inputs == nullptr) {
        return;
    }
    _buf.clear();
    _buf = data;
    for (const auto field : *(_inputs->GetInputFields())) {
        field->Restore(this);
    }
}

void SimSyncDataShare::publish(Smp::IObject* parent, Smp::IPublication* receiver) {
    _inputs = new FieldHolder("inputs", "Input fields that will receive data from the slave.", parent);
    receiver->PublishField(_inputs);
    _outputs = new FieldHolder("outputs", "Output fields that will send data to the slave.", parent);
    receiver->PublishField(_outputs);
}

void SimSyncDataShare::Store(const Smp::Void* address, Smp::UInt64 size) {
    auto addr = reinterpret_cast<const uint8_t*>(address);
    _buf.insert(_buf.end(), addr, addr + size);
}

void SimSyncDataShare::Restore(Smp::Void* address, Smp::UInt64 size) {
    std::memcpy(address, _buf.data(), size);
    _buf.erase(_buf.begin(), _buf.begin() + size);
}

}  // namespace mt
}  // namespace simphonie
