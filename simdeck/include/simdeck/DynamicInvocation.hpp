/*
 * @file DynamicInvocation.hpp
 *
 * Copyright 2025 Sebastien Devaux. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simdeck_DynamicInvocation_HPP__
#define __simdeck_DynamicInvocation_HPP__

#include "Smp/IDynamicInvocation.h"
#include "simdeck/Component.hpp"

namespace simdeck {

/**
 * Componant able to have dynamically callable operation.
 * 
 * Notice: name of this class is chosen to match the SMP interface it
 * implements. The Smp::IDynamicInvocation name was not so well choosed since it
 * denotes more an intended use than what it really is.
 */
class DynamicInvocation: public Component, virtual public Smp::IDynamicInvocation {
    typedef Component Parent;
public:
    /**
     * Default constructor.
     */
    DynamicInvocation(Smp::String8 name, Smp::String8 descr, Smp::IObject* parent);
    /**
     * Destructor.
     */
    virtual ~DynamicInvocation();

    // Smp::IDynamicInvocation implementation.
    void Invoke(Smp::IRequest* request) override;
    inline Smp::IProperty* GetProperty(Smp::String8 name) const override {
        return _properties.at(name);
    }
    inline const Smp::PropertyCollection* GetProperties() const override {
        return &_properties;
    }
    inline Smp::IOperation* GetOperation(Smp::String8 name) const override {
        return _operations.at(name);
    }
    inline const Smp::OperationCollection* GetOperations() const override {
        return &_operations;
    }
private:
    OwnedCollection<Smp::IProperty> _properties;
    OwnedCollection<Smp::IOperation> _operations;    
};

} // namespace simdeck
#endif // __simdeck_DynamicInvocation_HPP__
