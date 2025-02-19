/*
 * @file EPPModel.hpp
 *
 * Copyright 2025 Sebastien Devaux. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simdeck_EPPModel_HPP__
#define __simdeck_EPPModel_HPP__

#include "simdeck/Model.hpp"
#include "simdeck/EntryPointPublisher.hpp"

namespace simdeck {

/**
 *
 */
class EPPModel: public Model, virtual public EntryPointPublisher {
    typedef Model Parent;
public:
    /**
     * Default constructor.
     */
    EPPModel(Smp::String8 name, Smp::String8 descr, Smp::IObject* parent);
    /**
     * Destructor.
     */
    virtual ~EPPModel();

private:

};

} // namespace simdeck
#endif // __simdeck_EPPModel_HPP__
