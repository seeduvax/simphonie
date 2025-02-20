/*
 * @file TestUuid.cpp
 *
 * Copyright 2024 Sebastien Devaux. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "abs/test.h"
#include "Smp/Uuid.h"
#include "Smp/Exception.h"
#include <sstream>

namespace test {
// ----------------------------------------------------------
// test suite implementation
ABS_TEST_SUITE_BEGIN( Uuid )
// uncomment and cmplete next line for test suite description
// ABS_TEST_DESCR(test description)

private:

public:
    void setUp() {
    }

    void tearDown() {
    }

    ABS_TEST_CASE_BEGIN(Serialisation)
        ABS_TEST_DESCR(Check deserialization and serialization of Uuid preserve value)
        std::string refUuidStr="01234567-abcd-1234-fedc-ba9876543210";
        std::string expectedUuidStr="01234567-ABCD-1234-FEDC-BA9876543210";
        Smp::Uuid uuid(refUuidStr.c_str());
        std::ostringstream ss;
        ss << uuid;
        CPPUNIT_ASSERT_EQUAL(expectedUuidStr,ss.str());

        Smp::Uuid tmp;
        tmp.Data1 = 0x01234567;
        tmp.Data2[0] = 0xabcd;
        tmp.Data2[1] = 0x1234;
        tmp.Data2[2] = 0xfedc;
        tmp.Data3[0] = 0xba;
        tmp.Data3[1] = 0x98;
        tmp.Data3[2] = 0x76;
        tmp.Data3[3] = 0x54;
        tmp.Data3[4] = 0x32;
        tmp.Data3[5] = 0x10;
        CPPUNIT_ASSERT_EQUAL(tmp, uuid);
        CPPUNIT_ASSERT(uuid != Smp::Uuid());
    ABS_TEST_CASE_END
    
    ABS_TEST_CASE_BEGIN(UuidErrorParsing)
        ABS_TEST_DESCR(Check Error parsing and exception throwing)
        std::string refUuidStr="This is for an error";
        try {
            Smp::Uuid uuid(refUuidStr.c_str());
            CPPUNIT_FAIL("Exception not thrown");
        }
        catch (const Smp::Exception& ex) {
        }
    ABS_TEST_CASE_END
ABS_TEST_SUITE_END
} // namespace test
