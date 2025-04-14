/*
 * @file Logger.cpp
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simphonie/kern/Logger.hpp"
#include <cstring>
#include "Smp/ISimulator.h"
#include "Smp/Services/ITimeKeeper.h"
#include "simphonie/kern/Publication.hpp"

namespace simphonie {
namespace kern {

Logger::Backend::Backend() {}
Logger::Backend::~Backend() {}

static Smp::String8 _LMK_NamesTable[] = {Smp::Services::ILogger::LMK_InformationName,
                                         Smp::Services::ILogger::LMK_EventName, Smp::Services::ILogger::LMK_WarningName,
                                         Smp::Services::ILogger::LMK_ErrorName, Smp::Services::ILogger::LMK_DebugName};

Logger::Logger(Smp::String8 name, Smp::String8 descr, Smp::IObject* parent) : Parent(name, descr, parent) {
    _simulator = dynamic_cast<Smp::ISimulator*>(parent); /* TODO assuming the simulator is the parent */
    addContainer("Backends", "Logger backends");
}

Smp::Services::LogMessageKind Logger::QueryLogMessageKind(Smp::String8 messageKindName) {
    for (int i = 0; i <= Smp::Services::ILogger::LMK_Debug; i++) {
        if (strcmp(messageKindName, _LMK_NamesTable[i]) == 0) {
            return i;
        }
    }
    return Smp::Services::ILogger::LMK_Debug;
}

std::string Logger::buildLogString(LoggerEvent& event) {
    std::ostringstream s;
    s << event._zuluTime << "\t" << event._simulationTime << "\t" << _LMK_NamesTable[event._kind] << "\t";
    s << event._threadId << "\t" << event._senderName << "\t" << event._msg << std::endl;
    return s.str();
}

void Logger::configure() {
    auto be = GetContainer("Backends");
    for (auto comp : *(be->GetComponents())) {
        auto backend = dynamic_cast<Backend*>(comp);
        if (backend != nullptr) {
            _backends.push_back(backend);
        }
        else {
            // TODO throw exception or log something?
        }
    }
}

void Logger::Log(const Smp::IObject* sender, Smp::String8 message, Smp::Services::LogMessageKind kind) {
    LoggerEvent event;

    const Smp::Services::ITimeKeeper* time = _simulator->GetTimeKeeper();
    event._zuluTime = time->GetZuluTime();
    event._simulationTime = time->GetSimulationTime();
    event._senderName = sender->GetName();
    event._msg = message;
    event._kind = kind;
    event._threadId = std::this_thread::get_id();
    if (!_backends.empty()) {
        for (auto backend : _backends) {
            (dynamic_cast<Backend*>(backend))->Log(event);
        }
    }
    else {
        std::clog << buildLogString(event);
    }
}

} /* namespace kern */
}  // namespace simphonie
