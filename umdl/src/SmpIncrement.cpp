/*
 * @file SmpIncrement.cpp
 *
 * Copyright 2021 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simphonie/umdl/SmpIncrement.hpp"
#include "Smp/ISimulator.h"

namespace simphonie {
namespace umdl {
// --------------------------------------------------------------------
// ..........................................................
SmpIncrement::SmpIncrement(Smp::String8 name, Smp::String8 descr, Smp::IObject* parent)
    : AStepMdl(name, descr, parent), _input(0), _output(0) {}
// ..........................................................
SmpIncrement::~SmpIncrement() {}
// ..........................................................
void SmpIncrement::step() {
    getSimulator()->GetLogger()->Log(this,"calling SmpIncrement::step", Smp::Services::ILogger::LMK_Information);
    _output = _input + 1;
};
// ..........................................................
void SmpIncrement::publish(Smp::IPublication* receiver) {
    receiver->PublishField("output", "Output", &_output, Smp::ViewKind::VK_All, false, false, true);
    receiver->PublishField("input", "Output", &_input, Smp::ViewKind::VK_All, false, true, false);
}

}  // namespace umdl
}  // namespace simph
