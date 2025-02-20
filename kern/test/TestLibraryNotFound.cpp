/*
 * @file TestSmpIncrement.cpp
 *
 * Copyright 2021 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include <cppunit/extensions/HelperMacros.h>
#include "simphonie/kern/ExFileNotFound.hpp"
#include "simphonie/kern/Simulator.hpp"

namespace test {
using namespace simphonie::kern;

// ----------------------------------------------------------
// test fixture implementation
class TestFileNotFound : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(TestFileNotFound);
    CPPUNIT_TEST(testLoadableLibrary);
    CPPUNIT_TEST(testNotLoadableLibrary);
    CPPUNIT_TEST_SUITE_END();

private:
    Simulator _sim;

public:
    void setUp() {}

    void tearDown() {}

    void testLoadableLibrary() {
        std::vector<std::string> lib;
        lib.push_back("simphonie_sys");
        lib.push_back("simdeck");
        CPPUNIT_ASSERT(testLoadLibraries(lib) == true);
    }

    void testNotLoadableLibrary() {
        std::vector<std::string> lib;
        lib.push_back("simph_NotExistingLibrary");
        testLoadLibraries(lib);
        CPPUNIT_ASSERT(testLoadLibraries(lib) == false);
    }

    bool testLoadLibraries(std::vector<std::string> libraries) {
        try {
            for (auto itr : libraries) {
                _sim.LoadLibrary(itr.c_str());
            }
        }
        catch (simphonie::kern::ExFileNotFound ex) {
            return false;
        }
        return true;
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(TestFileNotFound);

}  // namespace test
