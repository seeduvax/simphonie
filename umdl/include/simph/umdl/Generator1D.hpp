/*
 * @file Generator1D.h
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simph_umdl_Generator1D_HPP__
#define __simph_umdl_Generator1D_HPP__
#include "Smp/IPublication.h"
#include "simph/kern/AStepMdl.hpp"

namespace simph {
namespace umdl {

/**
 *
 */
class Generator1D : public simph::kern::AStepMdl {
public:
    /**
     * Default constructor.
     */
    Generator1D(Smp::String8 name, Smp::String8 descr, Smp::IObject* parent);
    /**
     * Destructor.
     */
    virtual ~Generator1D();
    // AStepMdl implementation
    void step();

protected:
    void publish(Smp::IPublication* receiver);
    virtual Smp::Float64 compute() = 0;

private:
    Smp::Float64 _out;
};

}  // namespace umdl
}  // namespace simph
#endif  // __simph_umdl_Generator1D_HPP__
