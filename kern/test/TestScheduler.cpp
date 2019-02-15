/*
 * @file TestScheduler.cpp
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include <cppunit/extensions/HelperMacros.h>
#include "simph/kern/Scheduler.hpp"
#include "simph/kern/Simulator.hpp"
#include "simph/sys/Logger.hpp"

namespace test {
using namespace simph::kern;

// ----------------------------------------------------------
// test fixture implementation
class TestScheduler: public CppUnit::TestFixture, virtual public Smp::IEntryPoint {
CPPUNIT_TEST_SUITE( TestScheduler );
CPPUNIT_TEST( testSchedule );
CPPUNIT_TEST_SUITE_END();

private:
    Simulator* _sim;
public:
    void setUp() {
    }

    void tearDown() {
    }
    
    void testSchedule() {
        _sim=new Simulator();
        Scheduler* scheduler=dynamic_cast<Scheduler*>(_sim->GetScheduler());
        _sim->Publish();
        _sim->Configure();
        _sim->Connect();
        scheduler->AddSimulationTimeEvent(this,10);
        scheduler->AddSimulationTimeEvent(this,30);
        scheduler->AddSimulationTimeEvent(this,20);
        scheduler->AddSimulationTimeEvent(this,20);
        scheduler->step();
        scheduler->step();
        scheduler->step();
        scheduler->step();
        delete _sim;
    }

    // Smp::IEntryPoint implementation
    Smp::String8 GetName() const {
        return "testEP";
    }
    Smp::String8 GetDescription() const {
        return "";
    }
    Smp::IObject* GetParent() const {
        return nullptr;
    }
    void Execute() const {
        TRACE(""<<_sim->GetTimeKeeper()->GetSimulationTime());
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(TestScheduler);
} // namespace test
