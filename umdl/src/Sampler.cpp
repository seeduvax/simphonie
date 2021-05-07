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
    /*
    {comment %id=SDE-001
    Creating the file at that moment on model construction with such naming
    convention without knowing what is the current directory can be an issue. I
    suggest adding a String8 input to this model to receive the file path to
    use. Then the file creation and 1st line output shall be delayed in
    connect method, that is at a time when the simulator (and its configuration)
    shall have set the value for this input.
    }
     */
}
// ..........................................................
Sampler::~Sampler() {
    _file.close();
}
// ..........................................................
void Sampler::AddField(simph::kern::Field* field) {
    _fields.push_back(field);
    /*
    {comment %id=SDE-003
    OK for now, will need rework when starting to think about multi-threaded
    simulation. Only fields from the same scheduling context (written by the same
    thread than the one calling this sampler's step entry point) shall be allowed
    here.
    }
     */
}
// ..........................................................
void Sampler::configure() {
    /*
    {comment %id=SDE-002
    Too early, write the first line in connect() or in a init entry point.
    }
     */
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
