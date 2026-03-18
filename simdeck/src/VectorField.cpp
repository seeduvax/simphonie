/*
 * @file VectorField.cpp
 *
 * Copyright 2026 Sebastien Devaux. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simdeck/VectorField.hpp"
#include "Smp/IStorageReader.h"
#include "Smp/IStorageWriter.h"
#include "Smp/Publication/ITypeRegistry.h"
#include "Smp/IField.h"
#include "Smp/ISimpleField.h"
#include "Smp/ISimpleArrayField.h"
#include "Smp/IArrayField.h"
#include "Smp/IOutputField.h"
#include "simdeck/Collection.hpp"
#include "simdeck/SimpleVectorType.hpp"
#include "simdeck/SimpleArrayField.hpp"
#include "simdeck/FieldPublisher.hpp"
#include <sstream>
#include <iostream>

namespace simdeck {

template <typename T>
class TVectorField: public VectorField, public virtual Smp::IArrayField {
    public:
        TVectorField(Smp::String8 name, Smp::String8 description, std::vector<T*>* address,
                      Smp::Publication::IType* ptype, Smp::ViewKind viewKind, const Smp::Publication::IType* type,
                      Smp::Bool isState, Smp::Bool isInput, Smp::Bool isOutput,
                      Smp::IObject* parent):
                       VectorField(name, description, address,
                          viewKind, type,
                          isState, isInput, isOutput,
                          parent),
                           _tData((std::vector<T*>*)address),
                           _ptype(ptype), 
                           _pub(new FieldPublisher(this)) // must be initialize to null
                      {
                        updateFields();
                      }

        /*
        * Destructor.
        */
        virtual ~TVectorField() {}

        Smp::IObject* GetChild(Smp::String8 fullName) const override {
            updateFields();
            for (auto& f: _fields) {
                if (fullName==f->GetName()) {
                    return f;
                }
            }
            return nullptr;
        }
                      
        Smp::UInt64 GetSize() const override {
            updateFields();
            return (Smp::UInt64)_fields.size();
        }

        Smp::IField* GetItem(Smp::UInt64 index) const override{
            updateFields();
            return _fields[index];
        }

        void Restore(Smp::IStorageReader* reader) override {
            Smp::UInt64 size = GetSize();
            reader->Restore(&size, sizeof(size));
            std::vector<T*> temp;
            for (int i=0; i<size; i++) {
                T* fieldAddress =  (*_tData)[i];
                Smp::UInt64 size = sizeof(fieldAddress);
                T* item;
                reader->Restore(item, size);
                temp.push_back(item);
            }
            _tData->clear();
            for(int i = 0; i< size;i++){
                _tData->push_back(temp[i]);
            }
            updateFields();
        }

        void Store(Smp::IStorageWriter* writer) override {
           Smp::UInt64 size=GetSize();
            writer->Store(&size, sizeof(size));
            for (Smp::IField* item: _fields) {
                writer->Store(item, sizeof(item));
            }
        }

        


    private:
        std::vector<T*>* _tData;
        mutable std::vector<Smp::IField*> _fields;
        Smp::Publication::IType* _ptype;
        FieldPublisher* _pub;
    
        static std::string buildName(std::string parentName, Smp::UInt64 index) {
            std::ostringstream oss;
            oss << "[" << index << "]";
            return oss.str();
        }

        void updateFields() const {
            if (_fields.size() != _tData->size()) {
                _fields.clear();
                for (int i=0;i<_tData->size();i++) {
                    T* fieldAddress =  (*_tData)[i];
                    std::string name = this->GetName();
                    // _pub is a dummy publisher and is only usefull to provide a parent to the created field.
                    std::string fName = buildName(name, i);
                    auto f = _ptype->Publish(_pub, fName.c_str(), "", (void*)fieldAddress, GetView(), IsState(), IsInput(), IsOutput());
                    _fields.push_back(f);
                }
            }
        }
};

// ..........................................................
template <typename T>
class TVectorOutputField : public TVectorField<T>, public virtual Smp::IOutputField {
public:

    TVectorOutputField(Smp::String8 name, Smp::String8 description, std::vector<T*>* address,
                      Smp::Publication::IType* ptype, Smp::ViewKind viewKind, const Smp::Publication::IType* type,
                      Smp::Bool isState, Smp::Bool isInput,
                      Smp::IObject* parent)
                      :TVectorField<T>(name, description, address, ptype, 
                      viewKind, type, isState, isInput, true, parent)
                      {                        
                      }

    virtual ~TVectorOutputField() {}

    void Connect(Smp::IField* target) override{
        auto t = dynamic_cast<Smp::IArrayField*>(target);
        bool isSameType = target->GetType()->GetUuid() == this->GetType()->GetUuid();
        if (t!=nullptr 
            && !_targets.contain(t) 
            && t->GetSize() >= this->GetSize()
            && isSameType ){
            _targets.push_back(t);
        }
    }

    void Disconnect(Smp::IField* target) override {
        auto t = dynamic_cast<Smp::IArrayField*>(target);
        if (t!=nullptr && _targets.contain(t)) {
            _targets.remove(t);
        }
    }

    void Push() override {
       for (auto target: _targets) {
            auto t=dynamic_cast<Smp::IArrayField*>(target);
            auto max=this->GetSize()<t->GetSize()?this->GetSize():t->GetSize();
            // TODO define a better copy implementation when target is the exact same type
            
            for (Smp::UInt64 i=0;i<max;i++) {
                recursivePush(t->GetItem(i),this->GetItem(i));
            }
        }
    }

    void recursivePush(Smp::IField* targetItemfield, Smp::IField* itemfield){
        Smp::IArrayField* tArrayField = dynamic_cast<Smp::IArrayField*>(targetItemfield);
        if(tArrayField != nullptr){
            auto tItemField = dynamic_cast<Smp::IArrayField*>(itemfield);
            for(int i = 0; i< tArrayField->GetSize(); i++){
                recursivePush(tArrayField->GetItem(i), tItemField->GetItem(i));
            }
        }

        Smp::ISimpleArrayField* targetSimpleArrayField = dynamic_cast<Smp::ISimpleArrayField*>(targetItemfield);
        if(targetSimpleArrayField != nullptr){
            SimpleArrayField* itemSimpleArrayField = dynamic_cast<SimpleArrayField*>(itemfield);
            auto max=itemSimpleArrayField->GetSize()<targetSimpleArrayField->GetSize()?itemSimpleArrayField->GetSize():targetSimpleArrayField->GetSize();
            // TODO define a better copy implementation when target is the exact same type
            for (Smp::UInt64 i=0;i<max;i++) {
                targetSimpleArrayField->SetValue(i,itemSimpleArrayField->GetValue(i));
            }
            return;
        }
        
        Smp::ISimpleField* targetSimpleField = dynamic_cast<Smp::ISimpleField*>(targetItemfield);
        if (targetSimpleField!= nullptr) {
            Smp::ISimpleField* itemSimpleField = dynamic_cast<Smp::ISimpleField*>(itemfield);
            targetSimpleField->SetValue(itemSimpleField->GetValue());            
        }
    }

    const Smp::FieldCollection* GetInputFields() const override {
        return &_targets;
    }
    Smp::Bool IsAutomatic() const override {
        return false;
    }

private:
    Collection<Smp::IField> _targets;
};


// ..........................................................
VectorField::VectorField(
                      Smp::String8 name, Smp::String8 description,
                      void* address,
                      Smp::ViewKind viewKind,
                      const Smp::Publication::IType* type,
                      Smp::Bool isState, Smp::Bool isInput, Smp::Bool isOutput,
                      Smp::IObject* parent)
                    : Field(name, description, viewKind, address, 
                            0, type, isState, isInput,
                            isOutput, parent) {
}

// ..........................................................
VectorField::~VectorField() {
}

// ..........................................................
template <typename T>
Smp::IArrayField* VectorField::Create(
                            Smp::String8 name,
                            Smp::String8 description,
                            std::vector<T*>* address,
                            Smp::Publication::IType* ptype,
                            Smp::ViewKind viewKind,
                            const Smp::Publication::IType* type,
                            Smp::Bool isState,
                            Smp::Bool isInput,
                            Smp::Bool isOutput,
                            Smp::IObject* parent) {
    if (isOutput) {
        return new TVectorOutputField<T>(
                        name, description, 
                        static_cast<std::vector<T*>*>(address), ptype, viewKind, type,
                        isState, isInput, parent);
    }
    else {
        return new TVectorField<T>(
                        name, description, 
                        static_cast<std::vector<T*>*>(address), ptype, viewKind, type,
                        isState, isInput, false, parent);
    }
}



// ..........................................................
Smp::IArrayField* VectorField::Create(Smp::String8 name, Smp::String8 description,
                      void* address,
                      Smp::Publication::IType* ptype, Smp::ViewKind viewKind,
                      const Smp::Publication::IType* type,
                      Smp::Bool isState, Smp::Bool isInput, Smp::Bool isOutput,
                      Smp::IObject* parent){

    return  VectorField::Create<void>(name, description,
                          (std::vector<void*>*)(address),
                          ptype, viewKind, type,
                          isState, isInput, isOutput,
                          parent);
}


} // namespace simdeck