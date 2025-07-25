/*
 * @file LuaModel.cpp
 *
 * Copyright 2025 Sebastien Devaux. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simphonie/lua/LuaModel.hpp"

#include "Smp/IPublication.h"
#include "Smp/ISimulator.h"
#include "simdeck/EntryPoint.hpp"

namespace simphonie {
namespace lua {

class LuaModel::EntryPoint: public simdeck::EntryPoint {
    typedef simdeck::EntryPoint Parent;
public:
    EntryPoint(Smp::String8 name, Smp::String8 description, 
                sol::protected_function luaEP,
                sol::object self, LuaModel* parent):
            Parent(name, description, parent), 
            _luaEP(luaEP),
            _self(self),
            _owner(parent) {
    }
    ~EntryPoint() {
    }

    void Execute() const override {
        auto res=_luaEP(_self);
        if (!res.valid()) {
            sol::error err = res;
            _owner->getSimulator()->GetLogger()->Log(this, err.what(), Smp::Services::ILogger::LMK_Error);
        }
    }


private:
    sol::protected_function _luaEP;
    sol::object _self;
    LuaModel* _owner;
};

// --------------------------------------------------------------------
// ..........................................................
LuaModel::LuaModel(Smp::String8 name, Smp::String8 description, Smp::IObject* parent):
        Parent(buildName(name).c_str(), description, parent),
        _epList() {
    std::string s=name;
    auto loc=s.find('=');
    if ( loc != std::string::npos ) {
        _scriptPath=s.substr(loc+1);
    }

    _lua.open_libraries(sol::lib::base, sol::lib::package, sol::lib::string, sol::lib::os, sol::lib::math,
                       sol::lib::table, sol::lib::debug);
    _lua.safe_script_file(_scriptPath.c_str());
    _lua.safe_script("Smp=require 'simphonie_lua'");
    sol::table g=_lua.globals();
    _luaModel=g["model"];
}
// ..........................................................
LuaModel::~LuaModel() {
    for (const auto& data : _publishedIntData) {
        delete[] data;
    }
    for (const auto& data : _publishedDoubleData) {
        delete[] data;
    }
    // TODO SegFault table's destructor: may be related to github.com/ThePhd/sol2/issues/335
}

// --------------------------------------------------------------------
// ..........................................................
void LuaModel::call(const char* name) {
    sol::protected_function pf = _luaModel[name];
    if (pf != sol::nil) {
        auto res = pf(sol::object(_lua, sol::in_place, this));
        if (!res.valid()) {
            sol::error err = res;
            getSimulator()->GetLogger()->Log(this, err.what(), Smp::Services::ILogger::LMK_Error);
        }
    }
}
// ..........................................................
void LuaModel::publish(Smp::IPublication* receiver) {
    _pub = receiver;
    call("publish");
}
// ..........................................................
void LuaModel::configure() {
    call("configure");
}
// ..........................................................
void LuaModel::connect() {
    call("connect");
}

// --------------------------------------------------------------------
// ..........................................................
const Smp::EntryPointCollection* LuaModel::GetEntryPoints() const {
    return &_epList;
}
// ..........................................................
Smp::IEntryPoint* LuaModel::GetEntryPoint(Smp::String8 name) const {
    return _epList.at(name);
}
// ..........................................................
void LuaModel::addEntryPoint(Smp::String8 name, Smp::String8 description, sol::protected_function func) {
     _epList.push_back(new LuaModel::EntryPoint(name, description, 
                func,
                sol::object(_lua, sol::in_place, this), this));
}

// --------------------------------------------------------------------
// ..........................................................
std::string LuaModel::buildName(Smp::String8 spec) {
    std::string s=spec;
    auto loc=s.find('=');
    if ( loc != std::string::npos ) {
        return s.substr(0,loc);
    }
    return "LuaModel";
}
// --------------------------------------------------------------------
// ..........................................................
void LuaModel::publishFieldsImpl(sol::table fields, bool isInput, bool isOutput, bool isState) {
    for (auto f : fields) {
        std::string name = f.first.as<std::string>();
        sol::object elem;
        sol::table v = sol::nil;
        size_t vsize = 1;
        if (f.second.is<sol::table>()) {
            v = f.second;
            elem = v[1];
            vsize = v.size();
        }
        else {
            elem = f.second;
        }
        if (elem.is<int64_t>()) {
            int64_t* data = new int64_t[vsize];
            _publishedIntData.push_back(data);
            if (vsize == 1) {
                data[0] = elem.as<int64_t>();
                _pub->PublishField(name.c_str(), "", data, Smp::ViewKind::VK_All, isState, isInput, isOutput);
            }
            else {
                for (int i = 0; i < vsize; i++) {
                    data[i] = v[i];
                }
                _pub->PublishArray(name.c_str(), "", vsize, data, Smp::PrimitiveTypeKind::PTK_Int64,
                                   Smp::ViewKind::VK_All, isState, isInput, isOutput);
            }
        }
        else if (elem.is<double>()) {
            double* data = new double[vsize];
            _publishedDoubleData.push_back(data);
            if (vsize == 1) {
                data[0] = elem.as<double>();
                _pub->PublishField(name.c_str(), "", data, Smp::ViewKind::VK_All, isState, isInput, isOutput);
            }
            else {
                for (int i = 0; i < vsize; i++) {
                    data[i] = v[i + 1];
                }
                _pub->PublishArray(name.c_str(), "", vsize, (void*)data, Smp::PrimitiveTypeKind::PTK_Float64,
                                   Smp::ViewKind::VK_All, isState, isInput, isOutput);
            }
        }
        else {}
    }
}
// ..........................................................
void LuaModel::publishFields(sol::table fields) {
    sol::object t = fields["input"];
    if (t != sol::nil) {
        publishFieldsImpl(t, true, false, false);
    }
    t = fields["output"];
    if (t != sol::nil) {
        publishFieldsImpl(t, false, true, false);
    }
    t = fields["state"];
    if (t != sol::nil) {
        publishFieldsImpl(t, false, false, true);
    }
}
// ..........................................................
sol::lua_value LuaModel::getValue(Smp::String8 name, sol::this_state L) {
    // TODO quite bad to do this way shall register "casters" when publishing data
    // and reuse it here.
    sol::lua_value res = sol::lua_value(L, 0);
    auto simpleValue = this->GetSimpleValue(name);
    switch (simpleValue.GetType()) {
        case Smp::PrimitiveTypeKind::PTK_Bool:
            res = sol::lua_value(L, (Smp::Bool)simpleValue);
            break;
        case Smp::PrimitiveTypeKind::PTK_Int8:
            res = sol::lua_value(L, (Smp::Int8)simpleValue);
            break;
        case Smp::PrimitiveTypeKind::PTK_Int16:
            res = sol::lua_value(L, (Smp::Int16)simpleValue);
            break;
        case Smp::PrimitiveTypeKind::PTK_Int32:
            res = sol::lua_value(L, (Smp::Int32)simpleValue);
            break;
        case Smp::PrimitiveTypeKind::PTK_Int64:
            res = sol::lua_value(L, (Smp::Int64)simpleValue);
            break;
        case Smp::PrimitiveTypeKind::PTK_UInt8:
            res = sol::lua_value(L, (Smp::UInt8)simpleValue);
            break;
        case Smp::PrimitiveTypeKind::PTK_UInt16:
            res = sol::lua_value(L, (Smp::UInt16)simpleValue);
            break;
        case Smp::PrimitiveTypeKind::PTK_UInt32:
            res = sol::lua_value(L, (Smp::UInt32)simpleValue);
            break;
        case Smp::PrimitiveTypeKind::PTK_UInt64:
            res = sol::lua_value(L, (Smp::UInt64)simpleValue);
            break;
        case Smp::PrimitiveTypeKind::PTK_Float32:
            res = sol::lua_value(L, (Smp::Float32)simpleValue);
            break;
        case Smp::PrimitiveTypeKind::PTK_Float64:
            res = sol::lua_value(L, (Smp::Float64)simpleValue);
            break;
    }
    return res;
}
// ..........................................................
void LuaModel::setValue(Smp::String8 name, sol::object value) {
    // TODO quite bad to do this way shall register "casters" when publishing data
    // and reuse it here.
    auto simpleValue = this->GetSimpleValue(name);
    switch (simpleValue.GetType()) {
        case Smp::PrimitiveTypeKind::PTK_Bool:
            simpleValue.SetValue(simpleValue.GetType(), value.as<Smp::Bool>());
            break;
        case Smp::PrimitiveTypeKind::PTK_Int8:
            simpleValue.SetValue(simpleValue.GetType(), value.as<Smp::Int8>());
            break;
        case Smp::PrimitiveTypeKind::PTK_Int16:
            simpleValue.SetValue(simpleValue.GetType(), value.as<Smp::Int16>());
            break;
        case Smp::PrimitiveTypeKind::PTK_Int32:
            simpleValue.SetValue(simpleValue.GetType(), value.as<Smp::Int32>());
            break;
        case Smp::PrimitiveTypeKind::PTK_Int64:
            simpleValue.SetValue(simpleValue.GetType(), value.as<Smp::Int64>());
            break;
        case Smp::PrimitiveTypeKind::PTK_UInt8:
            simpleValue.SetValue(simpleValue.GetType(), value.as<Smp::UInt8>());
            break;
        case Smp::PrimitiveTypeKind::PTK_UInt16:
            simpleValue.SetValue(simpleValue.GetType(), value.as<Smp::UInt16>());
            break;
        case Smp::PrimitiveTypeKind::PTK_UInt32:
            simpleValue.SetValue(simpleValue.GetType(), value.as<Smp::UInt32>());
            break;
        case Smp::PrimitiveTypeKind::PTK_UInt64:
            simpleValue.SetValue(simpleValue.GetType(), value.as<Smp::UInt64>());
            break;
        case Smp::PrimitiveTypeKind::PTK_Float32:
            simpleValue.SetValue(simpleValue.GetType(), value.as<Smp::Float32>());
            break;
        case Smp::PrimitiveTypeKind::PTK_Float64:
            simpleValue.SetValue(simpleValue.GetType(), value.as<Smp::Float64>());
            break;
    }
    this->SetSimpleValue(name, simpleValue);
}
}} // namespace simphonie::lua
