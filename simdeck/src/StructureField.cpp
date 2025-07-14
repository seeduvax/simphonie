/*
 * @file StructureField.cpp
 *
 * Copyright 2025 Sebastien Devaux. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include <tuple>
#include "simdeck/StructureField.hpp"
#include "simdeck/SimpleField.hpp"
#include "simdeck/SimpleArrayField.hpp"
#include "simdeck/ExInvalidTarget.hpp"
#include "Smp/Publication/IArrayType.h"
#include "Smp/IOutputField.h"

namespace simdeck {
// ..........................................................
StructureField::StructureField(Smp::String8 name, Smp::String8 description, Smp::ViewKind viewKind, void* address,
                               const Smp::Publication::IType* type, Smp::Bool isState, Smp::Bool isInput, Smp::Bool isOutput,
                               Smp::IObject* parent)
    : Field(name, description, viewKind, address, 0, type, isState, isInput, isOutput, parent) {
}
// ..........................................................
StructureField::~StructureField() {}

// ..........................................................
void StructureField::Store(Smp::IStorageWriter* writer ) {
    for (auto f: _fields) {
        f->Store(writer);
    }
}
// ..........................................................
void StructureField::Restore(Smp::IStorageReader* reader) {
    for (auto f: _fields) {
        f->Restore(reader);
    }
}
// ..........................................................
// ..........................................................
class StructureOutputField: public StructureField, virtual public Smp::IOutputField {
public:
    StructureOutputField(Smp::String8 name, Smp::String8 description, Smp::ViewKind viewKind, void* address,
                               const Smp::Publication::IType* type, Smp::Bool isState, Smp::Bool isInput, Smp::Bool isOutput,
                               Smp::IObject* parent):
            StructureField(name, description, viewKind, address, type, isState, isInput, isOutput, parent) {
    }
    virtual ~StructureOutputField() {
    }
    // Smp::IOutputField implementation
    void Push() override {
        for (auto f : _outputFields) {
            std::get<0>(f)->Push();
        }
    }
    void Connect(Smp::IField* target) override {
        if (_outputFields.empty()) {
            // probably 1st connect, iterated structure fields to retrieve them
            // as output fields.
            int i=0;
            for (auto f: *(this->GetFields())) {

                auto o=dynamic_cast<Smp::IOutputField*>(f);
                if (o!=nullptr) {
                    _outputFields.push_back({o, i});
                }
                i++;
            }
        }
        auto f = dynamic_cast<Smp::IStructureField*>(target);
        if (f != nullptr && f->GetType()->GetUuid() == this->GetType()->GetUuid() && f->IsInput()
            && this->GetFields()->size() == f->GetFields()->size()) {
            auto targetFields=f->GetFields();
            for (auto of: _outputFields) {
                // TODO check index is OK in target collection and throw invalid target exception otherwise.
                std::get<0>(of)->Connect(targetFields->at(std::get<1>(of)));
            }
        }
        else {
            throw ExInvalidTarget(this, target);
        }
        _targets.push_back(target);
    }
    void Disconnect(Smp::IField* target) override {
        auto tsf=dynamic_cast<Smp::IStructureField*>(target);
        if (tsf!=nullptr && _targets.contain(target)) {
            _targets.remove(target);
            auto targetFields=tsf->GetFields();
            for (auto f : _outputFields) {
                std::get<0>(f)->Disconnect(targetFields->at(std::get<1>(f)));
            }
        }
    }
    const Smp::FieldCollection* GetInputFields() const override {
        return &_targets;
    } 
    Smp::Bool IsAutomatic() const override {
        return false;
    }
private:
    std::vector<std::tuple<Smp::IOutputField*,int> > _outputFields;
    Collection<Smp::IField> _targets;
};

// ..........................................................
// ..........................................................
Smp::IField* StructureField::Create(Smp::String8 name, Smp::String8 description, Smp::ViewKind viewKind, void* address,
                               const Smp::Publication::IType* type, Smp::Bool isState, Smp::Bool isInput, Smp::Bool isOutput,
                               Smp::IObject* parent,
                               const std::vector<StructureType::FieldDescr>& fieldsSpec) {
    StructureField* sf=nullptr;
    if (isOutput) {
        sf=new StructureOutputField(name, description, viewKind, address, type, isState, isInput, isOutput, parent);
    }
    else {
        sf=new StructureField(name, description, viewKind, address, type, isState, isInput, isOutput, parent);
    }
    if (sf!=nullptr) {
        for (auto fd: fieldsSpec) {
            Smp::IField* f=nullptr;
            auto st=dynamic_cast<Smp::Publication::IArrayType*>(fd.type);

            if (st!=nullptr) {
                f=SimpleArrayField::Create(fd.name, fd.description, 
                            st->GetSize(),
                            (void*)((uint8_t*)address + fd.offset),
                            st->GetItemType(),
                            fd.view,
                            fd.type, 
                            isState, isInput, isOutput, sf); 
            }
            else if (fd.type->GetPrimitiveTypeKind()!=Smp::PrimitiveTypeKind::PTK_None) {
                f=SimpleField::Create(fd.name, fd.description, fd.view, fd.type,
                            (void*)((uint8_t*)address + fd.offset),
                            isState, isInput, isOutput, sf);
            }
            if (f!=nullptr) {
                sf->addField(f);
            }
        }
    }
    return sf;
}
} // namespace simdeck
