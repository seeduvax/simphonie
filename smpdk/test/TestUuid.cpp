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
        Smp::Uuid uuid(refUuidStr.c_str());
        std::ostringstream ss;
        ss << uuid;
        CPPUNIT_ASSERT_EQUAL(refUuidStr,ss.str());
    ABS_TEST_CASE_END
ABS_TEST_SUITE_END
} // namespace test
