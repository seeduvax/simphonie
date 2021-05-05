/*
 * @file Sampler.cpp
 *
 * Copyright 2021 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simph/umdl/Sampler.hpp"
#include <fstream>
#include <iostream>
#include "Smp/ISimulator.h"

namespace simph {
namespace umdl {
// --------------------------------------------------------------------
// ..........................................................
Sampler::Sampler(Smp::String8 name, Smp::String8 descr, Smp::IObject* parent)
    : AStepMdl(name, descr, parent), _fields(), _file() {
    std::string filename = name;
    filename += ".csv";
    _file.open(filename, std::ofstream::out);
}
// ..........................................................
Sampler::~Sampler() {
    _file.close();
}
// ..........................................................
void Sampler::AddField(simph::kern::Field* field) {
    _fields.push_back(field);
}
// ..........................................................
void Sampler::publish(Smp::IPublication* receiver) {
    _file << "SimulationTime";
    for (kern::Field* field : _fields) {
        _file << ";" << field->GetParent()->GetName() << "." << field->GetName();
    }
    _file << std::endl;
}
// ..........................................................
void Sampler::step() {
    _file << getSimulator()->GetTimeKeeper()->GetSimulationTime();
    for (kern::Field* field : _fields) {
        _file << ";" << field->GetValue();
    }
    _file << std::endl;
};

}  // namespace umdl
}  // namespace simph
