/*
 * @file TestSimpleVectorField.cpp
 *
 * Copyright 2026 Sebastien Devaux. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
 #include "abs/test.h"
 #include "simdeck/SimpleVectorField.hpp"
 
 namespace test {
 using namespace simdeck;
 
// ----------------------------------------------------------
// test suite implementation
ABS_TEST_SUITE_BEGIN( SimpleVectorField )
// uncomment and complete next line for test suite description
// ABS_TEST_DESCR(test description)

private:

public:
    void setUp() {
    }

    void tearDown() {
    }

    ABS_TEST_CASE_BEGIN(VectorWrapping) {
        std::vector<Smp::Int32> vint={1,2,4,8,16,32};
        auto vf=SimpleVectorField::Create("vint","",&vint,nullptr,
                Smp::ViewKind::VK_All, nullptr,
                false, true, false, nullptr);
        CPPUNIT_ASSERT_EQUAL(vint.size(), vf->GetSize());
        auto v=vf->GetValue(2);
        CPPUNIT_ASSERT_EQUAL(vint[2],(Smp::Int32)v);
        vint.push_back(42);
        CPPUNIT_ASSERT_EQUAL(vint.size(), vf->GetSize());
        v=vf->GetValue(6);
        CPPUNIT_ASSERT_EQUAL(vint[6],(Smp::Int32)v);
    }
    ABS_TEST_CASE_END
ABS_TEST_SUITE_END
} // namespace test
