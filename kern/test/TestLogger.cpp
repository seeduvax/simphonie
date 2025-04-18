/*
 * @file TestLogger.cpp
 *
 * Copyright 2025. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include <unordered_map>
#include "Smp/ISimpleField.h"
#include "Smp/Int32.h"
#include "abs/test.h"
#include "simphonie/kern/Logger.hpp"
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

Simulator* _sim;
Logger* _logger;

public:
void setUp() {
    _sim = new Simulator("simulator", "", nullptr);
    _logger = new Logger("logger", "", _sim);
}

void tearDown() {
    delete _logger;
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
    _sim->AddService(_logger);
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
            std::cout << s.str() << std::endl;
            auto out = dynamic_cast<Smp::ISimpleField*>(_sim->GetResolver()->ResolveRelative(s.str().c_str(), _logger))
                           ->GetValue();
            CPPUNIT_ASSERT_EQUAL(lmk.second.nLogs, out.value.int32Value);
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

ABS_TEST_SUITE_END
} /* namespace test */
