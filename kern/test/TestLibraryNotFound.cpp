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
#include "simph/kern/ExLibraryNotFound.hpp"
#include "simph/kern/Simulator.hpp"

namespace test {
using namespace simph::kern;

// ----------------------------------------------------------
// test fixture implementation
class TestLibraryNotFound : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(TestLibraryNotFound);
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
        lib.push_back("libsimph_sys.so");
        lib.push_back("libsimph_smpdk.so");
        CPPUNIT_ASSERT(testLoadLibraries(lib) == true);
    }

    void testNotLoadableLibrary() {
        std::vector<std::string> lib;
        lib.push_back("libsimph_NotExistingLibrary.so");
        testLoadLibraries(lib);
        CPPUNIT_ASSERT(testLoadLibraries(lib) == false);
    }

    bool testLoadLibraries(std::vector<std::string> libraries) {
        try {
            for (auto itr : libraries) {
                _sim.LoadLibrary(itr.c_str());
            }
        }
        catch (simph::kern::ExLibraryNotFound ex) {
            return false;
        }
        return true;
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(TestLibraryNotFound);

}  // namespace test
