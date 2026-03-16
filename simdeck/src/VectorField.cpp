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
#include "Smp/IField.h"
#include <sstream>
#include <iostream>

namespace simdeck {

template <typename T>
class TVectorField: public VectorField, public virtual Smp::IArrayField{
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
                           _ptype(ptype)
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
    
        static std::string buildName(std::string parentName, Smp::UInt64 index) {
            std::ostringstream oss;
            oss << 'a' << index << 'b';
            return oss.str();
        }

        void updateFields() const {
            if (_fields.size() != _tData->size()) {
                _fields.clear();
                for (int i=0;i<_tData->size();i++) {
                   std::clog << "update fields" << std::endl;
                    T* fieldAddress =  (*_tData)[i];
                    std::string name = this->GetName();
                    auto f = _ptype->Publish(nullptr, buildName(name, i).c_str(), "", (void*)fieldAddress, GetView(), IsState(), IsInput(), IsOutput());
                    _fields.push_back(f);
                }
            }
        }
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
Smp::IArrayField* VectorField::Create(Smp::String8 name, Smp::String8 description,
                      void* address,
                      Smp::Publication::IType* ptype, Smp::ViewKind viewKind,
                      const Smp::Publication::IType* type,
                      Smp::Bool isState, Smp::Bool isInput, Smp::Bool isOutput,
                      Smp::IObject* parent){

    return new TVectorField<void>(name, description,
                          (std::vector<void*>*)(address),
                          ptype, viewKind, type,
                          isState, isInput, isOutput,
                          parent);
}


} // namespace simdeck