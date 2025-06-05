/*
 * @file Logger.cpp
 *
 * Copyright 2025. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simphonie/kern/Logger.hpp"
#include <cstring>
#include <iomanip>
#include "Smp/ISimulator.h"
#include "Smp/Services/ITimeKeeper.h"
#include "simphonie/kern/LoggerOStream.hpp"
#include "simphonie/kern/Publication.hpp"

#define CONTAINER_NAME "Backends"

namespace simphonie {
namespace kern {

std::unordered_map<Smp::Services::LogMessageKind, Logger::_LMK> Logger::_LMKMap = {
    {Smp::Services::ILogger::LMK_Event, {Smp::Services::ILogger::LMK_EventName, Smp::Services::ILogger::LMK_Event, 0}},
    {Smp::Services::ILogger::LMK_Information,
     {Smp::Services::ILogger::LMK_InformationName, Smp::Services::ILogger::LMK_Information, 0}},
    {Smp::Services::ILogger::LMK_Warning,
     {Smp::Services::ILogger::LMK_WarningName, Smp::Services::ILogger::LMK_Warning, 0}},
    {Smp::Services::ILogger::LMK_Error, {Smp::Services::ILogger::LMK_ErrorName, Smp::Services::ILogger::LMK_Error, 0}},
    {Smp::Services::ILogger::LMK_Debug, {Smp::Services::ILogger::LMK_DebugName, Smp::Services::ILogger::LMK_Debug, 0}}};

Smp::Int32 Logger::_logCounter = 0;
std::mutex Logger::_countersMutex;

Logger::Logger(Smp::String8 name, Smp::String8 descr, Smp::IObject* parent) : Component(name, descr, parent) {
    {
        std::unique_ptr<LoggerOStream> defaultLogger(new LoggerOStream("LoggerOStream", "Logger to stdout/stderr/stdlog", this));
        addContainer(CONTAINER_NAME, "Logger's backends")->AddComponent(defaultLogger.get());
        _backends.push_back(defaultLogger.get());
        _defaultLogger = std::move(defaultLogger);
    }

    addEP("resetCounters", "Reset events' counters", this, &Logger::resetCounters);
}

Logger::~Logger() {}

void Logger::publish(Smp::IPublication* receiver) {
    receiver->PublishField("Counter", "Counter of logs", &Logger::_logCounter, Smp::ViewKind::VK_All, true, false,
                           true);

    for (auto& lmk : _LMKMap) {
        std::ostringstream s;
        s << lmk.second.name << "Counter";
        receiver->PublishField(s.str().c_str(), "Logs' counter for the specific level", &lmk.second.counter,
                               Smp::ViewKind::VK_All, true, false, true);
    }
}

void Logger::configure() {
    Smp::IContainer* container = GetContainer(CONTAINER_NAME);
    for (auto component : *(container->GetComponents())) {
        if (component == _defaultLogger.get())
            continue;
        ILoggerBackend* backend = dynamic_cast<ILoggerBackend*>(component);
        if (backend != nullptr) {
            _backends.push_back(backend);
        }
    }

    if (_backends.size() > 1) {
        container->DeleteComponent(_defaultLogger.release());
        _backends.erase(_backends.begin());
    }
}

Smp::Services::LogMessageKind Logger::QueryLogMessageKind(Smp::String8 messageKindName) {
    for (const auto& lmk : _LMKMap) {
        if (strcmp(messageKindName, lmk.second.name) == 0) {
            return lmk.second.kind;
        }
    }
    return Smp::Services::ILogger::LMK_Debug;
}

void Logger::Log(const Smp::IObject* sender, Smp::String8 message, Smp::Services::LogMessageKind kind) {
    const Smp::Services::ITimeKeeper* time = getSimulator()->GetTimeKeeper();
    const LoggerEvent event(time->GetZuluTime(), time->GetSimulationTime(), sender, message, kind);

    {
        const std::lock_guard<std::mutex> lock(_logMutex);
        for (ILoggerBackend* logger : _backends) {
            logger->log(event);
        }
    }

    {
        const std::lock_guard<std::mutex> lock(Logger::_countersMutex);
        _LMKMap.at(kind).counter++;
        _logCounter++;
    }
}

void Logger::resetCounters() {
    const std::lock_guard<std::mutex> lock(Logger::_countersMutex);

    for (auto& lmk : _LMKMap) {
        lmk.second.counter = 0;
    }

    Logger::_logCounter = 0;
}

} /* namespace kern */
} /* namespace simphonie */
