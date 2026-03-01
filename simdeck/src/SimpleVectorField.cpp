/*
 * @file SimpleVectorField.cpp
 *
 * Copyright 2026 Sebastien Devaux. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */

#include <sstream>
#include "simdeck/SimpleVectorField.hpp"
#include "Smp/ISimpleArrayField.h"
#include "Smp/IStorageReader.h"
#include "Smp/IStorageWriter.h"
#include "Smp/IOutputField.h"
#include "simdeck/Field.hpp"
#include "simdeck/Collection.hpp"

namespace simdeck {

/**
 *
 */
template <typename T>
// TODO constratin if possible T to be one of the primitive type kind.
class SimpleVectorField: public Field, public virtual Smp::ISimpleArrayField {
public:
    /**
     * Default constructor.
     */
    SimpleVectorField();
    /**
     * Destructor.
     */
    virtual ~SimpleVectorField() {
        for (auto f: _fields) {
            delete f;
        }
    }

    // Smp::ISimpleArrayField implementation
    Smp::UInt64 GetSize() {
        updateFields();
        return _data.size();
    }
    Smp::IObject* GetChild(Smp::String8 fullName) const override {
        updateFields();
        for (auto f: _fields) {
            if (fullName==f->GetName()) {
                return f;
            }
        }
        return nullptr;
    }
    Smp::AnySimple GetValue(Smp::UInt64 index) override {
        return (*_data)[index];
    }
    void SetValue(Smp::UInt64 index, Smp::AnySimple value) override {
        (*_data)[index]=value;
    }
    void GetValues( Smp::UInt64 length,
                    Smp::AnySimple* values,
                    Smp::UInt64 startIndex = 0) const override {
        int j=0;
        for (int i=startIndex; i<_data->size() && j<length; i++) {
            values[j]=GetValue(i);
            j++;
        } 
    }
    void SetValues( Smp::UInt64 length,
                    Smp::AnySimpleArray values,
                    Smp::UInt64 startIndex = 0) override {
        int j=0;
        for (int i=startIndex; i < _data->size(); i++) {
            (*_data)[i]=values[j];
            j++;
        }
    }
    void Restore(Smp::IStorageReader* reader) override {
        Smp::UInt64 size;
        reader->Restore(&size, sizeof(size));
        _data->clear();
        for (int i=0; i<size; i++) {
            T item;
            reader->Restore(&item, sizeof(T));
            _data->push_back(item);
        }
    }
    void Store(Smp::IStorageWriter* writer) override {
        Smp::UInt64 size=_data->size();
        writer->Store(&size, sizeof(size));
        for (T item: *_data) {
            writer->Store(&item, sizeof(T));
        }
    }

private:

    class SimpleItemField: public Field, public virtual Smp::ISimpleField {
    public:
        SimpleItemField(Smp::UInt64 index, SimpleVectorField<T>* parent):
            Field(buildName(index).c_str(),"",parent),
            _parent(parent),
            _index(index) {
        }
        Smp::AnySimple GetValue() const override {
            return _parent->GetValue(_index);
        }
        void SetValue(Smp::AnySimple value) override {
            _parent->SetValue(_index, value);
        }

    protected:
        static std::string buildName(Smp::UInt64 index) {
            std::ostringstream oss;
            oss << '[' << index << ']';
            return oss.str();
        }
        
    private:
        SimpleVectorField<T>* _parent;
        Smp::UInt64 _index;
    };

    void updateFields() {
        if (_fields.size() < _data->size()) {
            for (int i=_fields.size(); i<_data->size(); i++) {
                _fields.push_back(new SimpleItemField(i,this));
            }
        }
        else {
            for (int i=_data->size(); i<_fields.size(); i++) {
                delete _fields[i];
            }
            _fields.resize(_data->size());
        }
    }
    std::vector<T>* _data;
    std::vector<SimpleItemField*> _fields;
};

template <typename T>
// TODO constratin if possible T to be one of the primitive type kind.
class SimpleVectorOutoutField: public SimpleVectorField<T>, public virtual Smp::IOutputField {
public:
    // Smp::IOutputField implementation
    void Connect(Smp::IField* target) override {
        auto sf=dynamic_cast<Smp::ISimpleArrayField*>(target);
        if (sf!=nullptr 
                && !_targets.contain(sf) 
                && sf->GetSize()>=this->GetSize()) {
            _targets.push_back(sf);
        }
    }
    void Disconnect(Smp::IField* target) override {
        auto sf=dynamic_cast<Smp::ISimpleArrayField*>(target);
        if (sf!=nullptr && _targets.contain(sf)) {
            _targets.remove(sf);
        }
    }
    void Push() override {
        for (auto target: _targets) {
            auto t=dynamic_cast<Smp::ISimpleArrayField*>(target);
            auto max=this->GetSize()<t->GetSize()?this->GetSize():t->GetSize();
            for (Smp::UInt64 i=0;i<max;i++) {
                t->SetValue(i,this->GetValue(i));
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
    Collection<Smp::IField> _targets;
};
} // namespace simdeck
