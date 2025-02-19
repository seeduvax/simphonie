/*
 * @file CompositeComponent.hpp
 *
 * Copyright 2025 Sebastien Devaux. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simdeck_CompositeComponent_HPP__
#define __simdeck_CompositeComponent_HPP__

#include "simdeck/Component.hpp"
#include "simdeck/Composite.hpp"

namespace simdeck {

/**
 *
 */
class CompositeComponent: public Component, virtual public Composite {
    typedef Component Parent;
public:
    /**
     * Default constructor.
     */
    CompositeComponent(Smp::String8 name, Smp::String8 descr, Smp::IObject* parent);
    /**
     * Destructor.
     */
    virtual ~CompositeComponent();

private:

};

} // namespace simdeck
#endif // __simdeck_CompositeComponent_HPP__
