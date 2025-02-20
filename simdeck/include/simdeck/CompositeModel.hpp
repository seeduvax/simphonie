/*
 * @file CompositeModel.hpp
 *
 * Copyright 2025 Sebastien Devaux. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simdeck_CompositeModel_HPP__
#define __simdeck_CompositeModel_HPP__

#include "simdeck/Model.hpp"
#include "simdeck/Composite.hpp"

namespace simdeck {

/**
 *
 */
class CompositeModel: public Model, virtual public AComposite {
    typedef Model Parent;
public:
    /**
     * Default constructor.
     */
    CompositeModel(Smp::String8 name, Smp::String8 descr, Smp::IObject* parent);
    /**
     * Destructor.
     */
    virtual ~CompositeModel();

private:

};

} // namespace simdeck
#endif // __simdeck_CompositeModel_HPP__
