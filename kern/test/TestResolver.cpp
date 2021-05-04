/*
 * @file TestResolver.cpp
 *
 * Copyright 2020 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include <cppunit/extensions/HelperMacros.h>
#include "simph/kern/Field.hpp"
#include "simph/kern/Resolver.hpp"

#include "simph/sys/Logger.hpp"

namespace test {
using namespace simph::kern;

// ----------------------------------------------------------
// test fixture implementation
class TestResolver : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(TestResolver);
    CPPUNIT_TEST(testResolver);
    CPPUNIT_TEST_SUITE_END();

private:
public:
    void setUp() {}

    void tearDown() {}

    void testResolver() {
        simph::smpdk::Component simu("simu", "Resovler owner", nullptr);
        Resolver resolver("resolver", "test resolver", &simu);
        Smp::IComponent* obj1 = new simph::smpdk::Component("to1", "test obj 1", &simu);
        Smp::IComponent* obj2 = new simph::smpdk::Component("to2", "test obj 2", obj1);
        // auto field1 = new simph::kern::SimpleArrayField("tf1", "test field 1", obj2, 3);
        Smp::IPublication* p = resolver.publish(obj1);
        CPPUNIT_ASSERT(p != nullptr);
        Smp::IPublication* q = resolver.publish(obj2);
        CPPUNIT_ASSERT(q != nullptr);
        // simph::kern::SimpleArrayField* r = resolver.publish(field1);
        // r->SetValue(0,3);
        // r->SetValue(1,2);
        // r->SetValue(2,3);
        // CPPUNIT_ASSERT(r != nullptr);
        resolver.dump();
        CPPUNIT_ASSERT(p != q);
        CPPUNIT_ASSERT_EQUAL(p, resolver.publish(obj1));
        CPPUNIT_ASSERT_EQUAL((Smp::IObject*)nullptr, resolver.ResolveAbsolute("plop"));
        CPPUNIT_ASSERT_EQUAL((Smp::IObject*)obj1, resolver.ResolveAbsolute("to1"));
        CPPUNIT_ASSERT_EQUAL((Smp::IObject*)nullptr, resolver.ResolveAbsolute("to2"));
        CPPUNIT_ASSERT_EQUAL((Smp::IObject*)obj2, resolver.ResolveAbsolute("to1/to2"));
        CPPUNIT_ASSERT_EQUAL((Smp::IObject*)nullptr, resolver.ResolveRelative("to1", obj2));
        CPPUNIT_ASSERT_EQUAL((Smp::IObject*)obj2, resolver.ResolveRelative("to2", obj1));
        // std::cout << resolver.ResolveRelative("to1/to2[0]", obj1) << std::endl;
        // CPPUNIT_ASSERT_EQUAL(3, resolver.ResolveRelative("to1/to2[0]", obj1));
        delete obj1;
        delete obj2;
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(TestResolver);
}  // namespace test
