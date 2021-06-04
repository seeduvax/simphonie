/*
 * @file Sampler.cpp
 *
 * Copyright 2021 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simph/kern/Sampler.hpp"
#include <fstream>
#include <iostream>
#include "Smp/ISimulator.h"
#include "simph/sys/Callback.hpp"
#include "simph/sys/Logger.hpp"

namespace simph {
namespace kern {
// --------------------------------------------------------------------
// ..........................................................
Sampler::Sampler(Smp::String8 name, Smp::String8 descr, Smp::IObject* parent)
    : AStepMdl(name, descr, parent), _fields(), _file(), _fileName(std::string(GetName()) + ".csv") {
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
    addEP("initColumn", "Init Entry point that Initialize column headers", &Sampler::initColumn, this);
    getSimulator()->AddInitEntryPoint(this->GetEntryPoint("initColumn"));

    addEP("debugPrintFile", "", [=]() {
        TRACE("Sampler::debugPrintFile: " << _fileName << "\n" << std::ifstream(_fileName).rdbuf());
    });
}
// ..........................................................
Sampler::~Sampler() {
    _file.close();
}
// ..........................................................
void Sampler::AddField(simph::kern::Field* field) {
    if (dynamic_cast<Smp::IArrayField*>(field) != nullptr) {
        Smp::IArrayField* fieldArray = dynamic_cast<Smp::IArrayField*>(field);
        for (int idx = 0; idx < fieldArray->GetSize(); idx++) {
            _fields.push_back(dynamic_cast<simph::kern::Field*>(fieldArray->GetItem(idx)));
        }
    }
    else {
        _fields.push_back(field);
    }
    /*
    {comment %id=SDE-003
    OK for now, will need rework when starting to think about multi-threaded
    simulation. Only fields from the same scheduling context (written by the same
    thread than the one calling this sampler's step entry point) shall be allowed
    here.
    }
    {comment %id=AA-001
    No SDE, it should remain simple.
    Sampler shouldn't read fields that are refreshed by another sequencer than the one that calls its step entrypoint.
    Consider using several samplers, one for each sequencer (single threaded context), and may merge results at reset.
    }
     */
}

// TODO : Sauvergarde en fichier Binaire
// ..........................................................
void Sampler::initColumn() {
    _file.open(_fileName, std::ofstream::out);
    _file << "#SimulationTime";
    for (kern::Field* field : _fields) {
        Smp::IObject* parent = field->GetParent();
        std::string path = field->GetName();
        // TODO  (AA to Mael) please consider developping an helper to compute iobject's fullpath
        // Simulator isn't added to the path
        while (parent->GetParent() != nullptr) {
            path = parent->GetName() + ("." + path);
            parent = parent->GetParent();
        }
        _file << ";" << path;
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

}  // namespace kern
}  // namespace simph
