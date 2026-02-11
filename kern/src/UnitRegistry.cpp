/*
 * @file UnitRegistry.cpp
 *
 * Copyright 2026 Sebastien Devaux. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simphonie/kern/UnitRegistry.hpp"
#include "simdeck/StringField.hpp"

namespace simphonie {
namespace kern {

// --------------------------------------------------------------------
// ..........................................................
class Unit: public simdeck::Object, public virtual simdeck::smpext::IUnit {
    typedef simdeck::Object Base;
public:
    Unit(Smp::String8 name, Smp::String8 descr, Smp::IObject* parent, Smp::String8 symbol):
            Base(name, descr, parent),
            _symbol(symbol) {
        _symbolField=simdeck::StringField::Create(
                                "Symbol",
                                "Unit short name",
                                Smp::ViewKind::VK_All,
                                &_symbol,
                                nullptr, 
                                false,
                                false,
                                true,
                                this);
    }
    virtual ~Unit() {
        delete _symbolField;
    }
    // IUnit implementation
    Smp::String8 GetSymbol() const {
        return _symbol.c_str();
    }
    // Smp::IObject specialization
    Smp::IObject* GetChild(Smp::String8 name) const override {
        if (strcmp("Symbol",name) == 0) {
            return _symbolField;
        }
        return nullptr;
    }
private:
    std::string _symbol;    
    Smp::IField* _symbolField;
};


// --------------------------------------------------------------------
// ..........................................................
UnitRegistry::UnitRegistry(Smp::String8 name, Smp::String8 description,
                            Smp::IObject* parent): Base(name, description, parent) {
}
// ..........................................................
UnitRegistry::~UnitRegistry() {
}
// --------------------------------------------------------------------
// ..........................................................
const simdeck::smpext::UnitCollection* UnitRegistry::GetUnits() const {
    return &_units;
}
// ..........................................................
simdeck::smpext::IUnit* UnitRegistry::GetUnit(Smp::String8 name) const {
    for (auto unit: _units) {
        if (strcmp(name,unit->GetSymbol()) == 0) {
            return unit;
        }
    }
    return nullptr;
}
// ..........................................................
void UnitRegistry::AddUnit(Smp::String8 name, Smp::String8 description, Smp::String8 symbol) {
    if (_units.at(name) == nullptr) {
        _units.push_back(new Unit(name, description, this, symbol));
    }
}

}} // namespace simphonie::kern
