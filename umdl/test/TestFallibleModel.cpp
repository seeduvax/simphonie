/*
 * @file TestComposite.cpp
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include <cppunit/extensions/HelperMacros.h>
#include "Smp/IFailure.h"
#include "simph/kern/Failure.hpp"
#include "simph/smpdk/Collection.hpp"
#include "simph/umdl/FallibleModel.hpp"

namespace test {

// ----------------------------------------------------------
// test fixture implementation
class TestFallibleModel : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(TestFallibleModel);
    CPPUNIT_TEST(testFallibleModel);
    CPPUNIT_TEST_SUITE_END();

private:
public:
    void setUp() {}

    void tearDown() {}

    void testFallibleModel() {
        // auto failureList = simph::smpdk::Collection("failureList");

        // Collection<Failure*> failureList;
        // simph::kern::Failure f1("failure1");
        // failureList.push_back(&f1);
        // simph::kern::Failure f2("failure2");
        // failureList.push_back(&f2);
        // failureList.push_back(failure);

        // auto fallibleModel = new FallibleModel("fallibleModel", "description", nullptr,
        // dynamic_cast<Smp::FailureCollection>(failureList)); CPPUNIT_ASSERT(fallibleModel != nullptr);
        // CPPUNIT_ASSERT(fallibleModel->IsFailed() == false);
        // failure->Fail();
        // CPPUNIT_ASSERT(fallibleModel->IsFailed() == true);
        // failure->Unfail();
        // CPPUNIT_ASSERT(fallibleModel->IsFailed() == false);
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(TestFallibleModel);
}  // namespace test
