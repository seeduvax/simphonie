/*
 * @file SmpIncrement.cpp
 *
 * Copyright 2021 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simph/T_kern/SmpIncrement.hpp"

namespace simph {
	namespace T_kern {
// ..........................................................
SmpIncrement::SmpIncrement(Smp::String8 name, Smp::String8 description, Smp::IComposite* parent)
    : simba::ASmpModel(name, description, parent), _input(0), _output(0) {
}
// ..........................................................
SmpIncrement::~SmpIncrement() {
}
// ..........................................................
void SmpIncrement::publishModel() {
    addEntryPoint("step", "step", &SmpIncrement::step);
    PublishField("input", _input, simba::PUB_INPUT);
    PublishField("output", _output, simba::PUB_OUTPUT);
}
// ..........................................................

}} // namespace simph::T_kern
