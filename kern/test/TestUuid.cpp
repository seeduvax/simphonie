/*
 * @file TestUuid.cpp
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include <cppunit/extensions/HelperMacros.h>
#include "Smp/Uuid.h"
#include "simph/sys/Logger.hpp"
#include <iomanip>

namespace test {

// ----------------------------------------------------------
// test fixture implementation
class TestUuid: public CppUnit::TestFixture {
CPPUNIT_TEST_SUITE( TestUuid );
CPPUNIT_TEST( testUuid );
CPPUNIT_TEST_SUITE_END();

private:

public:
    void setUp() {
    }

    void tearDown() {
    }

    void testUuid() {
        #define TUUID "90854492-4254-11e9-94be-7b7bf5d77665"
        TRACE(""<<TUUID);
        Smp::Uuid uuid(TUUID);
        TRACE(""<<std::hex<<uuid.Data1);
        TRACE(""<<std::hex<<uuid.Data2[0]);
        TRACE(""<<std::hex<<uuid.Data2[1]);
        TRACE(""<<std::hex<<uuid.Data2[2]);
        TRACE(""<<std::hex<<(int)uuid.Data3[0]);
        TRACE(""<<std::hex<<(int)uuid.Data3[1]);
        TRACE(""<<std::hex<<(int)uuid.Data3[2]);
        TRACE(""<<std::hex<<(int)uuid.Data3[3]);
        TRACE(""<<std::hex<<(int)uuid.Data3[4]);
        TRACE(""<<std::hex<<(int)uuid.Data3[5]);
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(TestUuid);
} // namespace test
