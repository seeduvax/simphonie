/*
 * @file TestTimeKeeper.cpp
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include <cppunit/extensions/HelperMacros.h>
#include "simph/kern/TimeKeeper.hpp"
#include "simph/sys/Logger.hpp"
#include "simph/kern/Scheduler.hpp"
#include "simph/kern/Simulator.hpp"

namespace test {
using namespace simph::kern;

// ----------------------------------------------------------
// test fixture implementation
class TestTimeKeeper : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(TestTimeKeeper);
    CPPUNIT_TEST(testTimeKeeper);
    CPPUNIT_TEST_SUITE_END();

    // Fake scheduler returning specific next event simulation time.
    class TSched: public Scheduler {
    public:
        TSched(): Scheduler("TSched","",nullptr) {
        }
        virtual ~TSched() {
        }
        Smp::Duration GetNextScheduledEventTime() const override {
            return 10000;
        }
    };
    // Fake simulator using the faks scheduler
    class TSim: public Simulator {
    public:
        TSim(): Simulator("TSim"),
            _sched(new TSched()) {
        }
        virtual ~TSim() {
            delete _sched;
        }
        Smp::Services::IScheduler* GetScheduler() const override {
            return _sched;
        }

    private:
        Smp::Services::IScheduler* _sched;
    };

private:
public:
    void setUp() {}

    void tearDown() {}
    void testTimeKeeper() {
        TSim sim;
        sim.Publish();
        sim.Configure();
        sim.Connect();
        auto tk=sim.GetTimeKeeper();
        Smp::DateTime zulu = tk->GetZuluTime();
        TRACE("zulu=" << (int64_t)zulu);
        CPPUNIT_ASSERT_EQUAL((Smp::Duration)0, tk->GetSimulationTime());
        CPPUNIT_ASSERT_EQUAL((Smp::DateTime)0, tk->GetMissionTime());
        CPPUNIT_ASSERT(zulu < tk->GetZuluTime());
        tk->SetMissionTime(100);

        // TODO register some entry point to events emiited by timekeeper to 
        // check event are well fired.

        // send scheduler event to trigger simulation time update.
        // since fake scheduler return constant for next event time, this
        // will force the time keeper current simulation date.
        auto evMgr=sim.GetEventManager();
        evMgr->Emit(evMgr->QueryEventId("Scheduler_PreEventExecute"));

        // check reached times values.
        TRACE("epoch=" << (int64_t)tk->GetEpochTime());
        TRACE("mission=" << (int64_t)tk->GetMissionTime());
        CPPUNIT_ASSERT_EQUAL((Smp::Duration)10000, tk->GetSimulationTime());
        CPPUNIT_ASSERT_EQUAL((Smp::DateTime)10100, tk->GetMissionTime());
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(TestTimeKeeper);
}  // namespace test
