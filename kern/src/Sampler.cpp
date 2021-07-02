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
#include "simph/kern/Field.hpp"
#include "simph/kern/TypeRegistry.hpp"
#include "simph/sys/Callback.hpp"
#include "simph/sys/Logger.hpp"

namespace simph {
namespace kern {
// --------------------------------------------------------------------
// ..........................................................
Sampler::Sampler(Smp::String8 name, Smp::String8 descr, Smp::IObject* parent)
    : AStepMdl(name, descr, parent), _fields(), _fileName(std::string(GetName())), _modeValue(false) {
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
    auto field =
        new simph::kern::TField<bool>("mode", "true : sampler generate binary files, false : sampler generate csv file",
                                      Smp::ViewKind::VK_All, &_modeValue, false, true, false, this);
    this->addField(field);
}

// ..........................................................
Sampler::~Sampler() {
    _file.close();
}

// ..........................................................
void Sampler::recordField(simph::kern::Field* field) {
    if (dynamic_cast<Smp::IArrayField*>(field) != nullptr) {
        Smp::IArrayField* fieldArray = dynamic_cast<Smp::IArrayField*>(field);
        for (int idx = 0; idx < fieldArray->GetSize(); idx++) {
            recordField(dynamic_cast<simph::kern::Field*>(fieldArray->GetItem(idx)));
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
        for (kern::Field* field : _fields) {
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
        for (kern::Field* field : _fields) {
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
        for (kern::Field* field : _fields) {
            auto valType = field->GetPrimitiveTypeKind();
            std::string valTypeName = kern::TypeRegistry::getPrimitiveTypeName(valType);
            _file.write(reinterpret_cast<const char*>(valTypeName.c_str()), valTypeName.size());
            _file << '\x00';
        }
        _file << '\x01';
    }
}

// ..........................................................
void Sampler::step() {
    if (_modeValue) {
        _file << getSimulator()->GetTimeKeeper()->GetSimulationTime();
        for (kern::Field* field : _fields) {
            _file << ";" << field->GetValue();
        }
        _file << std::endl;
    }
    else {
        Smp::Duration simTime = getSimulator()->GetTimeKeeper()->GetSimulationTime();
        const char* bufferTimeValue = reinterpret_cast<const char*>(&(simTime));
        _file.write(reinterpret_cast<const char*>(bufferTimeValue), sizeof(simTime));
        for (kern::Field* field : _fields) {
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
        _file << '\x01';
    }
};

}  // namespace kern
}  // namespace simph
