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
#include <string>
#include "Smp/ISimpleField.h"
#include "Smp/ISimpleArrayField.h"
#include "Smp/ISimulator.h"
#include "Smp/Services/ITimeKeeper.h"
#include "simphonie/sys/Logger.hpp"

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
void FieldRecorderCsv::init() {
    _file.open(getFilePath(), std::ofstream::out);
    _file.precision(std::numeric_limits<double>::max_digits10 - 1);
    _file << "#time";
    for (auto f : *getInputFields()) {
        auto sf=dynamic_cast<Smp::ISimpleField*>(f);
        std::string absoluteName = f->GetName();
        Smp::IObject* obj = f->GetParent();
        while (obj!=nullptr && obj!=static_cast<Smp::IObject*>(getSimulator())) {
            absoluteName.insert (0,"/");
            absoluteName.insert (0,obj->GetName());
            obj=obj->GetParent();
        }
        if (sf!=nullptr) {
            _file << _delim << absoluteName;
        }
        auto af=dynamic_cast<Smp::ISimpleArrayField*>(f);
        if (af!=nullptr) {
            for(Smp::UInt64 i=0;i<af->GetSize();i++)
            {
                _file << _delim << absoluteName << "["<<i<<"]";
            }
        }
    }
    _file << std::endl;
}
// ..........................................................
void FieldRecorderCsv::disconnect() {
    _file << std::flush;
    _file.close();
}
// ..........................................................
void FieldRecorderCsv::flush() {
    _file << std::flush;
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
        
        auto af=dynamic_cast<Smp::ISimpleArrayField*>(f);
        if (af!=nullptr) {
            for(Smp::UInt64 i=0;i<af->GetSize();i++)
            {
                _file << _delim << af->GetValue(i);
            }
        }
    }
    _file << "\n";
}

}} // namespace simphonie::colibry
