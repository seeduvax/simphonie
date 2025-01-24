/*
 * @file Sampler.cpp
 *
 * Copyright 2021 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simph/esmp/Sampler.hpp"
#include <fstream>
#include <iostream>
#include "Smp/ISimulator.h"
#include "Smp/Services/ITimeKeeper.h"
#include "simph/smpdk/Field.hpp"
#include "simph/kern/TypeRegistry.hpp"
#include "simph/sys/Callback.hpp"
#include "simph/sys/Logger.hpp"
#include "Smp/IPublication.h"

// TODO: stop using directly kern classes, rely on SMP and smpdk only.
// => may need to relocate some part of kern to smpdk.

namespace simph {
namespace esmp {
// --------------------------------------------------------------------
// ..........................................................
Sampler::Sampler(Smp::String8 name, Smp::String8 descr, Smp::IObject* parent)
    : Parent(name, descr, parent), _fields(), _fileName(std::string(GetName())), _modeValue(false) {
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

    addEP("step", "Recording step: save the current values of all linked field.", this, &Sampler::step);
    addEP("initColumn", "Init Entry point that Initialize column headers", this, &Sampler::initColumn);
}

// ..........................................................
Sampler::~Sampler() {
    _file.close();
}

// --------------------------------------------------------------------
// ..........................................................
void Sampler::Publish(Smp::IPublication* receiver) {
    receiver->PublishField("mode", "true : sampler generate binary files, false : sampler generate csv file", &_modeValue, Smp::ViewKind::VK_All, false, true, false);
}


// ..........................................................
void Sampler::Connect(Smp::ISimulator* sim) {
    getSimulator()->AddInitEntryPoint(this->GetEntryPoint("initColumn"));
}

// ..........................................................
void Sampler::recordField(Smp::IField* field) {
    if (dynamic_cast<Smp::IArrayField*>(field) != nullptr) {
        Smp::IArrayField* fieldArray = dynamic_cast<Smp::IArrayField*>(field);
        for (int idx = 0; idx < fieldArray->GetSize(); idx++) {
            recordField(dynamic_cast<simph::smpdk::Field*>(fieldArray->GetItem(idx)));
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

// ..........................................................
void Sampler::initColumn() {
    if (_modeValue) {
        _file.open(_fileName + ".csv", std::ofstream::out);
        _file << "#SimulationTime";
        std::string path;
        for (Smp::IField* field : _fields) {
            Smp::IObject* parent = field->GetParent();
            path = field->GetName();
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
    else {
        _file.open(_fileName + ".res", std::ofstream::out | std::ios::binary);
        std::string path;
        _file << '\x01';
        _file << "SimulationTime" << '\x00';
        for (Smp::IField* field : _fields) {
            Smp::IObject* parent = field->GetParent();
            path = field->GetName();
            while (parent->GetParent() != nullptr) {
                path = parent->GetName() + ("." + path);
                parent = parent->GetParent();
            }
            _file.write(reinterpret_cast<const char*>(path.c_str()), path.size());
            _file << '\x00';
        }
        _file << '\x01';
        _file << "PTK_Int64" << '\x00';
        for (Smp::IField* f : _fields) {
            auto field=dynamic_cast<Smp::ISimpleField*>(f);
            if (field!=nullptr) {
                auto valType = field->GetPrimitiveTypeKind();
                std::string valTypeName = kern::TypeRegistry::getPrimitiveTypeName(valType);
                _file.write(reinterpret_cast<const char*>(valTypeName.c_str()), valTypeName.size());
                _file << '\x00';
            }
        }
        _file << '\x01';
    }
}

// ..........................................................
void Sampler::step() {
    if (_modeValue) {
        _file << getSimulator()->GetTimeKeeper()->GetSimulationTime();
        for (Smp::IField* f : _fields) {
            auto field=dynamic_cast<Smp::ISimpleField*>(f);
            if (field!=nullptr) {
                _file << ";" << field->GetValue();
            }
        }
        _file << std::endl;
    }
    else {
        Smp::Duration simTime = getSimulator()->GetTimeKeeper()->GetSimulationTime();
        const char* bufferTimeValue = reinterpret_cast<const char*>(&(simTime));
        _file.write(reinterpret_cast<const char*>(bufferTimeValue), sizeof(simTime));
        for (Smp::IField* f : _fields) {
            auto field=dynamic_cast<Smp::ISimpleField*>(f);
            if (field!=nullptr) {
                _file << '\x00';
                const char* val = nullptr;
                auto anyValue = field->GetValue();
                switch (anyValue.GetType()) {
                    case Smp::PrimitiveTypeKind::PTK_Char8:
                        val = (const char*)&anyValue.value.char8Value;
                        break;
                    case Smp::PrimitiveTypeKind::PTK_Bool:
                        val = (const char*)&anyValue.value.boolValue;
                        break;
                    case Smp::PrimitiveTypeKind::PTK_Int8:
                        val = (const char*)&anyValue.value.int8Value;
                        break;
                    case Smp::PrimitiveTypeKind::PTK_UInt8:
                        val = (const char*)&anyValue.value.uInt8Value;
                        break;
                    case Smp::PrimitiveTypeKind::PTK_Int16:
                        val = (const char*)&anyValue.value.int16Value;
                        break;
                    case Smp::PrimitiveTypeKind::PTK_UInt16:
                        val = (const char*)&anyValue.value.uInt16Value;
                        break;
                    case Smp::PrimitiveTypeKind::PTK_Int32:
                        val = (const char*)&anyValue.value.int32Value;
                        break;
                    case Smp::PrimitiveTypeKind::PTK_UInt32:
                        val = (const char*)&anyValue.value.uInt32Value;
                        break;
                    case Smp::PrimitiveTypeKind::PTK_Int64:
                        val = (const char*)&anyValue.value.int64Value;
                        break;
                    case Smp::PrimitiveTypeKind::PTK_UInt64:
                        val = (const char*)&anyValue.value.uInt64Value;
                        break;
                    case Smp::PrimitiveTypeKind::PTK_Float32:
                        val = (const char*)&anyValue.value.float32Value;
                        break;
                    case Smp::PrimitiveTypeKind::PTK_Float64:
                        val = (const char*)&anyValue.value.float64Value;
                        break;
                    case Smp::PrimitiveTypeKind::PTK_Duration:
                        val = (const char*)&anyValue.value.durationValue;
                        break;
                    case Smp::PrimitiveTypeKind::PTK_DateTime:
                        val = (const char*)&anyValue.value.dateTimeValue;
                        break;
                }
                // récupérer adress mémoire anysimple
                auto sizeValue = kern::TypeRegistry::getPrimitiveTypeSize(anyValue.GetType());
                _file.write(reinterpret_cast<const char*>(val), sizeValue);
            }
        }
        _file << '\x01';
    }
};

}  // namespace esmp
}  // namespace simph
