/*
 * @file TestObjectRegistry.cpp
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include <cppunit/extensions/HelperMacros.h>
#include "simph/kern/ObjectsRegistry.hpp"
#include "simph/kern/Simulator.hpp"
#include "simph/sys/Logger.hpp"

namespace test {
using namespace simph::kern;

// ----------------------------------------------------------
// test fixture implementation
class TestObjectsRegistry: public CppUnit::TestFixture {
CPPUNIT_TEST_SUITE( TestObjectsRegistry );
CPPUNIT_TEST( testRegistry );
CPPUNIT_TEST_SUITE_END();

private:

public:
    void setUp() {
    }

    void tearDown() {
    }

    void testRegistry() {
        Simulator* sim=new Simulator();
        ObjectsRegistry* registry=
                dynamic_cast<ObjectsRegistry*>(sim->GetResolver());
        CPPUNIT_ASSERT(registry!=nullptr);
        registry->dump();
        sim->Publish();
        TRACE("---------------------------");
        registry->dump();
    }

};

CPPUNIT_TEST_SUITE_REGISTRATION(TestObjectsRegistry);
} // namespace test
