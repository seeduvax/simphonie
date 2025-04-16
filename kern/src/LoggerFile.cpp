/*
 * @file LoggerFile.cpp
 *
 * Copyright 2025. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simphonie/kern/LoggerFile.hpp"
#include <sstream>
#include "simdeck/StringField.hpp"
#include "simphonie/kern/Logger.hpp"

namespace simphonie {
namespace kern {

LoggerFile::LoggerFile(Smp::String8 name, Smp::String8 descr, Smp::IObject* parent) : Component(name, descr, parent) {
    std::ostringstream s;
    s << getSimulator()->GetName() << ".log";
    _filepath = s.str();
}

void LoggerFile::configure() {
    _file.open(_filepath, std::ofstream::out | std::ofstream::app);
}

LoggerFile::~LoggerFile() {
    if (_file) {
        _file.close();
    }
}

void LoggerFile::publish(Smp::IPublication* receiver) {
    receiver->PublishField(simdeck::StringField::Create("filePath", "", Smp::ViewKind::VK_All, &_filepath, nullptr,
                                                        false, true, false, this));
}

void LoggerFile::log(const LoggerEvent& event) {
    if (_file) {
        _file << event;
        _file.flush();
    }
}

} /* namespace kern */
} /* namespace simphonie */
