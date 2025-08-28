/*
 * @file Logger.hpp
 *
 * Copyright 2025. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simphonie_kern_Logger_HPP__
#define __simphonie_kern_Logger_HPP__

#include <mutex>
#include <thread>
#include <unordered_map>
#include "Smp/IPublication.h"
#include "Smp/ISimulator.h"
#include "Smp/Services/ILogger.h"
#include "simdeck/Component.hpp"
#include "simdeck/Container.hpp"
#include "simdeck/EntryPointPublisher.hpp"
#include "simdeck/Factory.hpp"
#include "simphonie/kern/ILoggerBackend.hpp"
#include "simphonie/kern/LoggerEvent.hpp"
#include "simphonie/kern/Simulator.hpp"
#include "simphonie/sys/DlDef.h"

namespace simphonie {
namespace kern {

class Logger : public Component,
               virtual public Smp::Services::ILogger,
               virtual public AComposite,
               virtual public simdeck::EntryPointPublisher {
public:
    Logger(Smp::String8 name, Smp::String8 descr = "", Smp::IObject* parent = nullptr);
    ~Logger() override;

    void addBackend(ILoggerBackend* obj);

    Smp::Services::LogMessageKind QueryLogMessageKind(Smp::String8 messageKindName) override;
    void Log(const Smp::IObject* sender, Smp::String8 message,
             Smp::Services::LogMessageKind kind = Smp::Services::ILogger::LMK_Debug) override;

protected:
    struct _LMK {
        const Smp::String8 name;
        const Smp::Services::LogMessageKind kind;
        Smp::Int32 counter;
    };
    static std::unordered_map<Smp::Services::LogMessageKind, _LMK> _LMKMap;

    void publish(Smp::IPublication* receiver);
    void configure() override;

private:
    static Smp::Int32 _logCounter;
    static std::mutex _countersMutex;
    std::mutex _logMutex;
    std::unique_ptr<Component> _defaultLogger;
    std::vector<ILoggerBackend*> _backends;

    void resetCounters();

    friend LoggerEvent;
};

} /* namespace kern */
} /* namespace simphonie */

#endif /* __simphonie_kern_Logger_HPP__ */
