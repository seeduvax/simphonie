/*
 * @file SmpIncrement.cpp
 *
 * Copyright 2021 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simph/umdl/SmpIncrement.hpp"

namespace simph {
    namespace umdl {
// --------------------------------------------------------------------
// ..........................................................
SmpIncrement::SmpIncrement(Smp::String8 name, Smp::String8 descr, Smp::IObject* parent):
    AStepMdl(name,descr,parent),
    _input(0),
    _output(0) {
}
// ..........................................................
SmpIncrement::~SmpIncrement() {
}
// ..........................................................
void SmpIncrement::step() {
        _output = _input + 1;
};
// ..........................................................
void SmpIncrement::publish(Smp::IPublication* receiver) {
    receiver->PublishField("output","Output",
                                &_output,Smp::ViewKind::VK_All,
                                false,false,true);
    receiver->PublishField("input","Output",
                                &_input,Smp::ViewKind::VK_All,
                                false,true,false);
}

}} // namespace simph::umdl
