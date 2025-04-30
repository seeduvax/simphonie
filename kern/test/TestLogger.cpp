/*
 * @file TestLogger.cpp
 *
 * Copyright 2025. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include <fstream>
#include <iostream>
#include <string>
#include <thread>
#include <unordered_map>
#include "Smp/ISimpleField.h"
#include "Smp/Int32.h"
#include "abs/test.h"
#include "simphonie/kern/Logger.hpp"
#include "simphonie/kern/LoggerAsync.hpp"
#include "simphonie/kern/LoggerFile.hpp"
#include "simphonie/kern/LoggerOStream.hpp"
#include "simphonie/kern/Resolver.hpp"
#include "simphonie/kern/Simulator.hpp"

namespace test {
using namespace simphonie::kern;

ABS_TEST_SUITE_BEGIN(Logger)

private:
    struct _LMK {
        Smp::String8 name;
        Smp::Services::LogMessageKind kind;
        Smp::Int32 nLogs;
    };

    std::unique_ptr<Simulator> _sim;
    std::unique_ptr<Logger> _ownedLogger;
    Logger* _logger;

public:
void setUp() {
    _sim.reset(new Simulator("simulator", "", nullptr));
    _ownedLogger.reset(new Logger("logger", "", _sim.get()));
    _logger = _ownedLogger.get();
}

void tearDown() {
    _ownedLogger.reset();
    _sim.reset();
}

ABS_TEST_CASE_BEGIN(TestLoggerCounters) {
    ABS_TEST_DESCR(Test the counters of logs as well as the reset entry point)
    ABS_TEST_CASE_REQ(simph.log.fld .1)
    ABS_TEST_CASE_REQ(simph.log.fld .2)
    ABS_TEST_CASE_REQ(simph.log.fld .3)
    ABS_TEST_CASE_REQ(simph.log.fld .4)
    ABS_TEST_CASE_REQ(simph.log.fld .5)
    ABS_TEST_CASE_REQ(simph.log.fld .6)
    ABS_TEST_CASE_REQ(simph.log.fld .7)

    const std::unordered_map<Smp::Services::LogMessageKind, _LMK> lmkMap = {
        {Smp::Services::ILogger::LMK_Event,
         {Smp::Services::ILogger::LMK_EventName, Smp::Services::ILogger::LMK_Event, 2}},
        {Smp::Services::ILogger::LMK_Information,
         {Smp::Services::ILogger::LMK_InformationName, Smp::Services::ILogger::LMK_Information, 3}},
        {Smp::Services::ILogger::LMK_Warning,
         {Smp::Services::ILogger::LMK_WarningName, Smp::Services::ILogger::LMK_Warning, 6}},
        {Smp::Services::ILogger::LMK_Error,
         {Smp::Services::ILogger::LMK_ErrorName, Smp::Services::ILogger::LMK_Error, 4}},
        {Smp::Services::ILogger::LMK_Debug,
         {Smp::Services::ILogger::LMK_DebugName, Smp::Services::ILogger::LMK_Debug, 5}}};

    _sim->Initialise();
    _sim->AddService(_ownedLogger.release());
    _sim->Publish();

    _logger->GetEntryPoint("resetCounters")->Execute();

    {
        for (const auto lmk : lmkMap) {
            for (int i = 0; i < lmk.second.nLogs; i++) {
                _logger->Log(_logger, "TestLoggerCounters", lmk.second.kind);
            }
        }
    }

    {
        for (const auto lmk : lmkMap) {
            std::ostringstream s;
            s << lmk.second.name << "Counter";
            auto out = dynamic_cast<Smp::ISimpleField*>(_sim->GetResolver()->ResolveRelative(s.str().c_str(), _logger));
            CPPUNIT_ASSERT(nullptr != out);
            CPPUNIT_ASSERT_EQUAL(lmk.second.nLogs, out->GetValue().value.int32Value);
        }
    }

    _logger->GetEntryPoint("resetCounters")->Execute();

    {
        for (const auto lmk : lmkMap) {
            std::ostringstream s;
            s << lmk.second.name << "Counter";
            auto out = dynamic_cast<Smp::ISimpleField*>(_sim->GetResolver()->ResolveRelative(s.str().c_str(), _logger))
                           ->GetValue();
            CPPUNIT_ASSERT_EQUAL(0, out.value.int32Value);
        }
    }
}
ABS_TEST_CASE_END

ABS_TEST_CASE_BEGIN(TestLoggerFileAndContent) {
    ABS_TEST_CASE_REQ(simph.log.cfg .1)
    ABS_TEST_CASE_REQ(simph.log.cfg .2)
    ABS_TEST_CASE_REQ(simph.log.ev .3)
    ABS_TEST_CASE_REQ(simph.log.ev .4)
    ABS_TEST_CASE_REQ(simph.log.ev .5)
    ABS_TEST_CASE_REQ(simph.log.ev .6)

    std::unique_ptr<LoggerFile> loggerFile(new LoggerFile("loggerFile", "", _logger));
    _logger->GetContainer("Backends")->AddComponent(loggerFile.get());
    _sim->Initialise();
    _sim->AddService(_ownedLogger.release());
    _sim->Publish();

    auto filepath = dynamic_cast<Smp::ISimpleField*>(_sim->GetResolver()->ResolveRelative("filePath", loggerFile.get()));
    CPPUNIT_ASSERT(nullptr != filepath);
    {
        std::ostringstream s;
        s << _sim->GetName() << ".log";
        CPPUNIT_ASSERT(0 == std::strcmp(s.str().c_str(), filepath->GetValue().value.string8Value));
    }

    _sim->Configure();

    {
        // log to create at least the file if not created.
        _logger->Log(_sim.get(), "TestCreation", Smp::Services::ILogger::LMK_Information);

        std::ifstream file(filepath->GetValue().value.string8Value, std::ios::ate);
        CPPUNIT_ASSERT(file.good());

        _logger->Log(_sim.get(), "TestLoggerFile", Smp::Services::ILogger::LMK_Debug);

        std::string line;
        for (std::streamoff i = 1; i <= file.tellg(); i++) {
            file.seekg(-i, std::ios::end);
            char ch;
            file.get(ch);
            if (ch == '\n' && i != 1) {
                break;
            }
            line.insert(line.begin(), ch);
        }
        CPPUNIT_ASSERT(std::string::npos != line.find(Smp::Services::ILogger::LMK_DebugName));
        CPPUNIT_ASSERT(std::string::npos != line.find("TestLoggerFile"));
        CPPUNIT_ASSERT(std::string::npos != line.find(_sim->GetName()));
        std::ostringstream s;
        s << std::this_thread::get_id();
        CPPUNIT_ASSERT(std::string::npos != line.find(s.str().c_str()));
    }
}
ABS_TEST_CASE_END

ABS_TEST_CASE_BEGIN(TestLoggerAsync) {
    ABS_TEST_CASE_REQ(simph.log.cfg .5)
    ABS_TEST_CASE_REQ(simph.log.unsync .1)

    std::unique_ptr<LoggerAsync> loggerAsync(new LoggerAsync("loggerAsync", "", _logger));
    std::unique_ptr<LoggerOStream> loggerOStream(new LoggerOStream("loggerOStream", "", _logger));
    _logger->GetContainer("Backends")->AddComponent(loggerAsync.get());
    _logger->GetContainer("Backends")->AddComponent(loggerOStream.get());
    _sim->Initialise();
    _sim->AddService(_ownedLogger.release());
    _sim->Publish();
    _sim->Configure();

    auto bufferSize = dynamic_cast<Smp::ISimpleField*>(_sim->GetResolver()->ResolveRelative("bufferSize", loggerAsync.get()));
    CPPUNIT_ASSERT(nullptr != bufferSize);

    const auto threadId = std::this_thread::get_id();

    while (loggerAsync->count() > 0) {
        loggerAsync->consumeEvent();
    }

    std::thread thrd([&] {
        const LoggerEvent& event = loggerAsync->getEvent();
        CPPUNIT_ASSERT(_sim.get() == event.getSender());
        CPPUNIT_ASSERT(0 == std::strcmp("TestLoggerAsync", event.getMessage().c_str()));
        CPPUNIT_ASSERT_EQUAL(Smp::Services::ILogger::LMK_Debug, event.getKind());
        CPPUNIT_ASSERT(0 == event.getSimulationTime());
        CPPUNIT_ASSERT(threadId == event.getThreadId());
        loggerAsync->consumeEvent();
    });

    _logger->Log(_sim.get(), "TestLoggerAsync", Smp::Services::ILogger::LMK_Debug);

    thrd.join();
}
ABS_TEST_CASE_END

ABS_TEST_SUITE_END
} /* namespace test */
