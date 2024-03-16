/*
 * @file EntryPoint.hpp
 *
 * Copyright 2024 Sebastien Devaux. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simph_smpdk_EntryPoint_HPP__
#define __simph_smpdk_EntryPoint_HPP__

#include <functional>
#include <memory>
#include "simph/smpdk/Object.hpp"
#include "Smp/IEntryPoint.h"

namespace simph {
    namespace smpdk {

/**
 *
 */
template <typename Class, typename Func>
class EntryPoint: public Object, virtual public Smp::IEntryPoint {
public:
    /**
     * Default constructor.
     */
    EntryPoint(Class c,Func f, Smp::String8 name, Smp::String8 descr="",
               Smp::IObject* parent=nullptr) 
            : Object(name,descr,parent),
              _class(c), _func(f) {
    }
    /**
     * Destructor.
     */
    virtual ~EntryPoint() {
    }
    
    // Smp::IEntryPoint implementation
    void Execute() const override {
        (_class->*_func)();
    }
private:
    Class _class;
    Func _func;
};

}} // namespace simph::smpdk
#endif // __simph_smpdk_EntryPoint_HPP__
