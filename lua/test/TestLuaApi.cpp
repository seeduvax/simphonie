/*
 * @file TestLuaApi.cpp
 *
 * Copyright 2021 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include <cppunit/extensions/HelperMacros.h>
#include "simph/lua/LuaApi.hpp"

namespace test {
using namespace simph::lua;

// ----------------------------------------------------------
// test fixture implementation
class TestLuaApi: public CppUnit::TestFixture {
CPPUNIT_TEST_SUITE( TestLuaApi );
// TODO for each test method:
// CPPUNIT_TEST( test...);
CPPUNIT_TEST_SUITE_END();

private:

public:
    void setUp() {
    }

    void tearDown() {
    }

};

CPPUNIT_TEST_SUITE_REGISTRATION(TestLuaApi);
} // namespace test
