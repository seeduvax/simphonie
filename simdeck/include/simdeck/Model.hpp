/*
 * @file Model.hpp
 *
 * Copyright 2025 Sebastien Devaux. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simdeck_Model_HPP__
#define __simdeck_Model_HPP__

#include "simdeck/Component.hpp"
#include "Smp/IModel.h"

namespace simdeck {

/**
 *
 */
class Model: public Component, virtual public Smp::IModel {
    typedef Component Parent;
public:
    /**
     * Default constructor.
     */
    Model(Smp::String8 name, Smp::String8 descr, Smp::IObject* parent);
    /**
     * Destructor.
     */
    virtual ~Model();

private:

};

} // namespace simdeck
#endif // __simdeck_Model_HPP__
