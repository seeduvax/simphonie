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

        uuid.Data1 = 42;
        for (uint32_t idx = 0; idx < uuid.Data2.size(); idx++) {
            uuid.Data2[idx] = idx + 5;
        }
        for (uint32_t idx = 0; idx < uuid.Data3.size(); idx++) {
            uuid.Data3[idx] = idx + 5;
        }
        std::ostringstream ss2;
        ss2 << uuid;
        CPPUNIT_ASSERT_EQUAL(std::string("0000002A-0005-0006-0007-05060708090A"), ss2.str());
    ABS_TEST_CASE_END
    
    ABS_TEST_CASE_BEGIN(CompareUuid)
        Smp::Uuid uuid1;
        Smp::Uuid uuid2;
        CPPUNIT_ASSERT(!(uuid1 < uuid2));

        uuid1.Data1 = 42;
        uuid2.Data1 = 49;
        CPPUNIT_ASSERT(uuid1 < uuid2);
        uuid2.Data1 = 42;
        CPPUNIT_ASSERT(!(uuid1 < uuid2));
        
        uuid2.Data2[1] = 55;
        CPPUNIT_ASSERT(uuid1 < uuid2);
        
        uuid2.Data2[2] = 45;
        CPPUNIT_ASSERT(uuid1 < uuid2);
        
        uuid1.Data2[0] = 20;
        CPPUNIT_ASSERT(uuid2 < uuid1);
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
