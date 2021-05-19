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
#include "simph/kern/Simulator.hpp"
#include "simph/kern/TypeRegistry.hpp"

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
        simph::kern::Simulator simu("simu", "Resovler owner", nullptr);
        Resolver& resolver = dynamic_cast<Resolver&>(*(simu.GetResolver()));
        simph::smpdk::Component obj1("to1", "test obj 1", &simu);
        simph::smpdk::Component obj2("to2", "test obj 2", &obj1);

        Smp::IPublication* p = resolver.publish(&obj1);
        CPPUNIT_ASSERT(p != nullptr);
        Smp::IPublication* q = resolver.publish(&obj2);
        CPPUNIT_ASSERT(q != nullptr);
        // FIXME publish fields should be accessible by the component itself   (not nullptr)
        // CPPUNIT_ASSERT(obj1.GetField("to2") != nullptr);

        // publish an array field called 'iArray', size  3
        // that belongs to component 'to2'
        Smp::Int32 iArray[] = {12, 17, 42};
        q->PublishArray("iArray", "test int array", 3, iArray, Smp::PrimitiveTypeKind::PTK_Int32);

        TRACE("==>")
        resolver.GetEntryPoint("dump")->Execute();

        CPPUNIT_ASSERT(p != q);
        CPPUNIT_ASSERT_EQUAL((Smp::IObject*)nullptr, resolver.ResolveAbsolute("plop"));
        CPPUNIT_ASSERT_EQUAL((Smp::IObject*)&obj1, resolver.ResolveAbsolute("to1"));
        CPPUNIT_ASSERT_EQUAL((Smp::IObject*)nullptr, resolver.ResolveAbsolute("to2"));
        CPPUNIT_ASSERT_EQUAL((Smp::IObject*)&obj1, resolver.ResolveAbsolute("to1/to2/.."));
        CPPUNIT_ASSERT_EQUAL((Smp::IObject*)&obj2, resolver.ResolveAbsolute("to1/../to1/to2"));
        CPPUNIT_ASSERT_EQUAL((Smp::IObject*)&simu, resolver.ResolveAbsolute("to1/to2/..."));

        CPPUNIT_ASSERT_EQUAL((Smp::IObject*)&obj2, resolver.ResolveAbsolute("to1/to2"));
        CPPUNIT_ASSERT_EQUAL((Smp::IObject*)nullptr, resolver.ResolveRelative("to1", &obj2));
        CPPUNIT_ASSERT_EQUAL((Smp::IObject*)&obj2, resolver.ResolveRelative("to2", &obj1));
        auto arrField = resolver.ResolveAbsolute("to1.to2.iArray");

        // TODO: question why our array fields are a simpleField or forcible field
        // (that exposes 'Force' and 'Setvalue' methods which take an AnySimple as argument )
        // (A. Astyl: shouldn't we consider that only leaves fields which are basically a simple fields
        // are the only ones to be considered fallible/forcible ?
        // CPPUNIT_ASSERT(dynamic_cast<Smp::ISimpleField*>(arrField)==nullptr);

        CPPUNIT_ASSERT(dynamic_cast<Smp::IArrayField*>(arrField) != nullptr);
        CPPUNIT_ASSERT_EQUAL((size_t)3, dynamic_cast<Smp::IArrayField*>(arrField)->GetSize());

        auto simpleField = resolver.ResolveAbsolute("to1.to2.iArray[2]");
        std::cout << "ptr : " << &simpleField << std::endl;
        CPPUNIT_ASSERT(dynamic_cast<Smp::IArrayField*>(simpleField) == nullptr);
        CPPUNIT_ASSERT(dynamic_cast<Smp::ISimpleField*>(simpleField) != nullptr);

        auto simpleFieldSimple = dynamic_cast<Smp::ISimpleField*>(simpleField)->GetValue();
        CPPUNIT_ASSERT_EQUAL(Smp::PrimitiveTypeKind::PTK_Int32, simpleFieldSimple.GetType());
        CPPUNIT_ASSERT_EQUAL(42, (int32_t)simpleFieldSimple);
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(TestResolver);
}  // namespace test
