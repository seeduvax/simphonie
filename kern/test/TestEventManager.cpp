/*
 * @file TestEventManager.cpp
 *
 * Copyright 2022 Sebastien Devaux. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include <cppunit/extensions/HelperMacros.h>

#include "simph/kern/EventManager.hpp"
#include "simph/sys/Logger.hpp"

namespace test {
using namespace simph::kern;

class Cpt {
public:
    int _value;
    void inc() {
        _value++;
    }
    Cpt() : _value(0) {}
};
class EPCpt : public simph::smpdk::Object, public Smp::IEntryPoint {
public:
    Cpt& _cpt;
    EPCpt(Smp::String8 name, Cpt& cpt) : simph::smpdk::Object(name, "", nullptr), _cpt(cpt) {}
    virtual ~EPCpt() {}
    void Execute() const override {
        _cpt.inc();
    }
};

// ----------------------------------------------------------
// test fixture implementation
class TestEventManager : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(TestEventManager);
    // TODO for each test method:
    CPPUNIT_TEST(testEventManager);
    CPPUNIT_TEST_SUITE_END();

private:
public:
    void setUp() {}

    void tearDown() {}

    void testEventManager() {
        EventManager evMgr("", "", nullptr);
        Smp::Services::EventId evA = evMgr.QueryEventId("A");
        Smp::Services::EventId evB = evMgr.QueryEventId("B");
        Smp::Services::EventId evAbis = evMgr.QueryEventId("A");
        CPPUNIT_ASSERT(evA > Smp::Services::IEventManager::SMP_PostSimTimeChangeId);
        CPPUNIT_ASSERT(evB > Smp::Services::IEventManager::SMP_PostSimTimeChangeId);
        CPPUNIT_ASSERT(evB != evA);
        CPPUNIT_ASSERT(evA == evAbis);
        Cpt cA;
        Cpt cB;
        Cpt cC;
        EPCpt epA("A", cA);
        EPCpt epB("B", cB);
        EPCpt epC("C", cC);
        evMgr.Subscribe(evA, &epA);
        evMgr.Subscribe(evB, &epB);
        evMgr.Subscribe(evA, &epC);
        evMgr.Subscribe(evB, &epC);
        CPPUNIT_ASSERT_EQUAL(0, cA._value);
        CPPUNIT_ASSERT_EQUAL(0, cB._value);
        CPPUNIT_ASSERT_EQUAL(0, cC._value);
        evMgr.Emit(evA);
        CPPUNIT_ASSERT_EQUAL(1, cA._value);
        CPPUNIT_ASSERT_EQUAL(0, cB._value);
        CPPUNIT_ASSERT_EQUAL(1, cC._value);
        evMgr.Emit(evB);
        CPPUNIT_ASSERT_EQUAL(1, cA._value);
        CPPUNIT_ASSERT_EQUAL(1, cB._value);
        CPPUNIT_ASSERT_EQUAL(2, cC._value);
        evMgr.Unsubscribe(evB, &epC);
        evMgr.Emit(evB);
        CPPUNIT_ASSERT_EQUAL(1, cA._value);
        CPPUNIT_ASSERT_EQUAL(2, cB._value);
        CPPUNIT_ASSERT_EQUAL(2, cC._value);
        evMgr.Emit(evA);
        CPPUNIT_ASSERT_EQUAL(2, cA._value);
        CPPUNIT_ASSERT_EQUAL(2, cB._value);
        CPPUNIT_ASSERT_EQUAL(3, cC._value);
        bool exThrown = false;
        try {
            evMgr.Emit(Smp::Services::IEventManager::SMP_PostSimTimeChangeId + 1);
        }
        catch (Smp::Services::InvalidEventId& ex) {
            exThrown = true;
        }
        CPPUNIT_ASSERT(exThrown);
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(TestEventManager);
}  // namespace test
