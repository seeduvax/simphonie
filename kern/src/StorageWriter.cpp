/*
 * @file StorageWriter.cpp
 *
 * Copyright 2025 Sebastien Devaux. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simphonie/kern/StorageWriter.hpp"
#include "Smp/IComponent.h"

namespace simphonie {
namespace kern {

const char* _hexDigit = "0123456789abcdef";
// --------------------------------------------------------------------
// ..........................................................
StorageWriter::StorageWriter(Smp::ISimulator* sim, Smp::String8 filePath)
    : _sim(sim), _out(filePath), _filePath(filePath) {
    _subFilePath = _filePath + ".d";
}
// ..........................................................
StorageWriter::~StorageWriter() {}

// --------------------------------------------------------------------
// ..........................................................
void StorageWriter::Store(const Smp::Void* address, Smp::UInt64 size) {
    uint8_t* buf = (uint8_t*)address;
    for (int i = 0; i < size; i++) {
        _out << _hexDigit[(buf[i] & 0xF0) >> 4] << _hexDigit[buf[i] & 0x0F];
    }
}
// ..........................................................
Smp::String8 StorageWriter::GetStateVectorFileName() const {
    return _filePath.c_str();
}
// ..........................................................
Smp::String8 StorageWriter::GetStateVectorFilePath() const {
    return _subFilePath.c_str();
}

// --------------------------------------------------------------------
// ..........................................................
void StorageWriter::indent() {
    for (int i = 0; i < _indent; i++) {
        _out << "    ";
    }
}
// ..........................................................
void StorageWriter::storeComponent(Smp::IComponent* component) {
    indent();
    _out << component->GetName() << " = {" << std::endl;
    _indent++;
    indent();
    _out << "Fields = {" << std::endl;
    _indent++;
    for (auto field : *(component->GetFields())) {
        if (field->IsState()) {
            indent();
            _out << field->GetName() << " = ";
            field->Store(this);
            _out << std::endl;
        }
    }
    _indent--;
    indent();
    _out << "}" << std::endl;
    auto composite = dynamic_cast<Smp::IComposite*>(component);
    if (composite != nullptr) {
        for (auto container : *(composite->GetContainers())) {
            storeContainer(container);
        }
    }
    _indent--;
    indent();
    _out << "}" << std::endl;
}
// ..........................................................
void StorageWriter::storeContainer(Smp::IContainer* container) {
    indent();
    _out << container->GetName() << " = {" << std::endl;

    _indent++;
    for (auto component : *(container->GetComponents())) {
        storeComponent(component);
    }
    _indent--;

    indent();
    _out << "}" << std::endl;
}
// ..........................................................
void StorageWriter::store() {
    for (auto container : *(_sim->GetContainers())) {
        storeContainer(container);
    }
}

}  // namespace kern
}  // namespace simphonie
