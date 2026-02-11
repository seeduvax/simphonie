/*
 * @file Type.cpp
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simdeck/SimpleType.hpp"
#include "simdeck/Persist.hpp"

namespace simdeck {

class BoundField: public Persist, public virtual Smp::ISimpleField {
public:
    BoundField(Smp::String8 name, Smp::String8 descr, SimpleType* parent,
                    Smp::AnySimple& value):
            Persist(name, descr, parent),
            _value(value),
            _type(parent) {
    }
    virtual ~BoundField() {
    }
    Smp::AnySimple GetValue() const  override {
        return _value;
    }
    void SetValue(Smp::AnySimple value) override {
        // not writeable field, do nothing.
    }
    Smp::ViewKind GetView() const override {
        return Smp::ViewKind::VK_None;
    }
    Smp::Bool IsState() const override {
        return false;
    }
    Smp::Bool IsInput() const override {
        return false;
    }
    Smp::Bool IsOutput() const override {
        return false;
    }
    Smp::PrimitiveTypeKind GetPrimitiveTypeKind() const override{
        return _value.GetType();
    }
    Smp::Publication::IType* GetType() const override {
        return _type;
    }
private:
    Smp::AnySimple& _value;
    SimpleType* _type;
};


// ..........................................................
SimpleType::SimpleType(Smp::Uuid uuid, Smp::PrimitiveTypeKind ptk, Smp::String8 name, Smp::String8 descr, Smp::IObject* parent):
            Type(uuid, ptk, name, descr, parent) {
}
// ..........................................................
SimpleType::~SimpleType() {
    if (_min != nullptr) {
        delete _min;
    }
    if (_max != nullptr) {
        delete _max;
    }
}

// ..........................................................
Smp::IObject* SimpleType::GetChild(Smp::String8 name) const {
    Smp::IObject* ret=0;
    if (strcmp("unit", name)==0) {
        ret=_unit;
    }
    else if (strcmp("min", name)==0) {
        ret=_min;
    }
    else if (strcmp("max", name)==0) {
        ret=_max;
    }
    return ret;
}
// ..........................................................
void SimpleType::setMin(Smp::AnySimple min) {
    if (_min!=nullptr) {
        delete _min;
    }
    _minValue=min;
    _min=new BoundField("min", "minimum value", this, _minValue);
}
// ..........................................................
void SimpleType::setMax(Smp::AnySimple max) {
    if (_max!=nullptr) {
        delete _max;
    }
    _maxValue=max;
    _max=new BoundField("max", "maximum value", this, _maxValue);
}

}  // namespace simdeck
