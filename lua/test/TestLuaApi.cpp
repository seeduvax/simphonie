/*
 * @file TestLuaApi.cpp
 *
 * Copyright 2025 Sebastien Devaux. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "abs/test.h"
#include "simphonie/lua/LuaApi.hpp"
#include "sol/sol.hpp"

namespace test {
using namespace simphonie::lua;

// ----------------------------------------------------------
// test suite implementation
ABS_TEST_SUITE_BEGIN(LuaApi)
// uncomment and complete next line for test suite description
// ABS_TEST_DESCR(test description)

private:
public:
    void setUp() {}

    void tearDown() {}

    ABS_TEST_CASE_BEGIN(LuaApi) {
        ABS_TEST_DESCR(Integration test using a lua script for simulator configuration)
        sol::state lua;
        lua.open_libraries(sol::lib::base, sol::lib::package, sol::lib::string, sol::lib::os, sol::lib::math,
                           sol::lib::table, sol::lib::debug);
        lua.safe_script_file("test/test.lua");
        sol::table g = lua.globals();
        sol::object o = g["sim"]["logger"]["ErrorCounter"]["Value"];
        CPPUNIT_ASSERT_EQUAL(0, o.as<int>());
        o = g["sim"]["logger"]["Counter"]["Value"];
        CPPUNIT_ASSERT(0 < o.as<int>());
    }
    ABS_TEST_CASE_END
    ABS_TEST_SUITE_END
    }  // namespace test
