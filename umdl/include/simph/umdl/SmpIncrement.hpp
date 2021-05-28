/*
 * @file SmpIncrement.hpp
 *
 * Copyright 2021 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simph_umdl_SmpIncrement_HPP__
#define __simph_umdl_SmpIncrement_HPP__

#include <Smp/IPublication.h>
#include "simph/kern/AStepMdl.hpp"

namespace simph {
namespace umdl {

class SmpIncrement : public simph::kern::AStepMdl {
public:
    SmpIncrement(Smp::String8 name, Smp::String8 descr, Smp::IObject* parent);
    virtual ~SmpIncrement();

    void step() override;

private:
    void publish(Smp::IPublication* receiver);

    Smp::Float64 _input;
    Smp::Float64 _output;
};

}  // namespace umdl
}  // namespace simph
#endif  // __simph_umdl_SmpIncrement_HPP__
