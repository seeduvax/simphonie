/*
 * @file Logger1D.h
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simph_umdl_Logger1D_HPP__
#define __simph_umdl_Logger1D_HPP__
#include "simph/kern/AStepMdl.hpp"

namespace simph {
namespace umdl {
/**
 *
 */
class Logger1D : public virtual simph::kern::AStepMdl {
public:
    /**
     * Default constructor.
     */
    Logger1D(Smp::String8 name, Smp::String8 descr, Smp::IObject* parent);
    /**
     * Destructor.
     */
    virtual ~Logger1D();
    // AStepMdl implementation
    void step();

public:
    void publish(Smp::IPublication* receiver);

private:
    Smp::Float64 _in;
};

}  // namespace umdl
}  // namespace simph
#endif  // __simph_umdl_Logger1D_HPP__
