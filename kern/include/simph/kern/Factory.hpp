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
#include "simph/kern/Object.hpp"
#include "Smp/IFactory.h"

namespace simph {
	namespace kern {
/**
 *
 */
template <typename T>
class Factory: public Object, virtual public Smp::IFactory {
public:
    /**
     * Default constructor.
     */
    Factory(Smp::String8 name, Smp::String8 descr, Smp::IObject* parent,
                Smp::Uuid uuid):
                Object(name,descr,parent),
                _uuid(uuid) {
    }
    /**
     * Destructor.
     */
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
        // TODO retrieve demangled type name
        return typeid(T).name();
    }
private:
    Smp::Uuid _uuid;
};

}} // namespace simph::kern
#endif // __simph_kern_Factory_HPP__
