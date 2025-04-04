/*
 * @file FieldRecorderCsv.cpp
 *
 * Copyright 2025 Sebastien Devaux. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simphonie/colibry/FieldRecorderCsv.hpp"
#include "Smp/ISimpleField.h"
#include "Smp/ISimulator.h"
#include "Smp/Services/ITimeKeeper.h"

namespace simphonie {
namespace colibry {
// --------------------------------------------------------------------
// ..........................................................
FieldRecorderCsv::FieldRecorderCsv(Smp::String8 name, Smp::String8 description,
            Smp::IObject* parent): Parent(name, description, parent, "csv") {
}
// ..........................................................
FieldRecorderCsv::~FieldRecorderCsv() {
}
// --------------------------------------------------------------------
// ..........................................................
void FieldRecorderCsv::connect() {
    _file.open(getFilePath(), std::ofstream::out);
}
// ..........................................................
void FieldRecorderCsv::disconnect() {
    _file.close();
}
// --------------------------------------------------------------------
// ..........................................................
void FieldRecorderCsv::step() {
    _file << getSimulator()->GetTimeKeeper()->GetSimulationTime();
    for (auto f : *getInputFields()) {
        auto sf=dynamic_cast<Smp::ISimpleField*>(f);
        if (sf!=nullptr) {
            _file << _delim << sf->GetValue();
        }
    }
    _file << std::endl;
}

}} // namespace simphonie::colibry
