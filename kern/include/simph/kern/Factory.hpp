/*
 * @file Factory.h
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simph_kern_Factory_HPP__
#define __simph_kern_Factory_HPP__
#include "simph/sys/RttiUtil.hpp"
#include "simph/smpdk/Object.hpp"
#include "Smp/IFactory.h"
#include "simph/sys/DlDef.h"
#include <memory>
#include <functional>
#include <iostream>

namespace simph {
	namespace kern {
using namespace simph::smpdk;
/**
 *
 */
template <typename T>
class Factory: public Object, virtual public Smp::IFactory {
public:
    Factory(Smp::String8 name, Smp::String8 descr, Smp::IObject* parent, Smp::Uuid uuid):
        Object(name,descr,parent),
        _uuid(uuid),
        _type(simph::sys::RttiUtil::demangle(typeid(T).name())) {
    }
    virtual ~Factory() {
    }
    // Smp::IFactory implementation
    Smp::Uuid GetUuid() const {
        return _uuid;
    }
    Smp::IComponent* CreateInstance(Smp::String8 name, Smp::String8 descr,
                                    Smp::IComposite* parent) override {
        // TODO check name and throw Smp::InvalidObjectName if needed
        return new T(name,descr,parent);
    }
    void DeleteInstance(Smp::IComponent* instance) override {
        // TODO is it needed to check type or anything else before delete ?
        // If no, what is the purpose of DeleteInstance ? why not use delete
        // directly ?
        delete instance;
    }
    Smp::String8 GetTypeName() const override {
        return _type.c_str();
    }
private:
    Smp::Uuid _uuid;
    std::string _type;
};

/**
 * Macro to register model factories:
 * Ex:
 *      using namespace a::b;    
 *      REGISTER_SMP_LIBINIT();
 *      ADD_SMP_FACTORY(ModelA);
 *      ADD_SMP_FACTORY(ModelB);
 *      ADD_SMP_FACTORY(ModelC);
 *      ...
 */
#define REGISTER_SMP_LIBINIT()\
static std::vector< std::unique_ptr<Smp::IFactory> > _factories;\
static std::vector< std::function<Smp::IFactory*(Smp::ISimulator*)> > _createFactoryFns;\
extern "C" { \
    SHARED_FUNCTION bool Initialise(Smp::ISimulator* simulator,\
                    Smp::Publication::ITypeRegistry* reg) {\
        if (simulator) { \
            for (auto& createFactoryFn : _createFactoryFns) {\
                auto f = createFactoryFn(simulator);\
                simulator->RegisterFactory(f);\
                _factories.push_back(std::unique_ptr<Smp::IFactory>(f));\
            }\
            return true;\
        }\
        else {\
            return false;\
        }\
    }\
    SHARED_FUNCTION bool Finalise() {\
        _factories.clear();\
        return true;\
    }\
}

#define ADD_SMP_FACTORY(factoryType) \
static int static_add_##factoryType { [&]{\
    _createFactoryFns.push_back([&](Smp::ISimulator* simulator){\
        return new simph::kern::Factory<factoryType>(\
            "" #factoryType "_factory","",simulator,Smp::Uuid(#factoryType));\
    });\
    return 1;\
}()};


}} // namespace simph::kern
#endif // __simph_kern_Factory_HPP__
