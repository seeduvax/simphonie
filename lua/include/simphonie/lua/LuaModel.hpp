/*
 * @file LuaModel.hpp
 *
 * Copyright 2025 Sebastien Devaux. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simphonie_lua_LuaModel_HPP__
#define __simphonie_lua_LuaModel_HPP__

#include <map>

#include "Smp/IEntryPointPublisher.h"
#include "simdeck/Model.hpp"
#include "sol/sol.hpp"

namespace simphonie {
namespace lua {

/**
 *
 */
class LuaModel: public simdeck::Model, virtual public Smp::IEntryPointPublisher {
    typedef simdeck::Model Parent;
public:
    /**
     * Default constructor.
     */
    LuaModel(Smp::String8 name, Smp::String8 description, Smp::IObject* parent);
    /**
     * Destructor.
     */
    virtual ~LuaModel();

    // Smp::IEntryPointPublisher implementation
    const Smp::EntryPointCollection* GetEntryPoints() const override;
    Smp::IEntryPoint* GetEntryPoint(Smp::String8 name) const override;

    void addEntryPoint(Smp::String8, Smp::String8 name, sol::protected_function func);

    void publishInt(Smp::String8 name) {
        _datas[name] = new int;
    }
    int getValue(Smp::String8 name) {
        return *(_datas[name]);
    }
    void setValue(Smp::String8 name, int value) {
        *(_datas[name]) = value;
    }

protected:
    void publish(Smp::IPublication* receiver) override;
    void configure() override;
    void connect() override;

private:
    sol::table _luaModel;
    sol::state _lua;
    std::string _scriptPath;

    std::string buildName(Smp::String8 spec);
    void call(const char* name);
    simdeck::OwnedCollection<Smp::IEntryPoint> _epList;

    Smp::IPublication* _pub;
    std::map<std::string, int*> _datas;

    class EntryPoint;
};

}} // namespace simphonie::lua
#endif // __simphonie_lua_LuaModel_HPP__
