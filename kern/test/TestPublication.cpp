/*
 * @file TestPublication.cpp
 *
 * Copyright 2020 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include <cppunit/extensions/HelperMacros.h>
#include <string.h>
#include "Smp/ISimpleArrayField.h"
#include "Smp/ISimpleField.h"
#include "simph/kern/Publication.hpp"
#include "simph/kern/TypeRegistry.hpp"
#include "simph/smpdk/Object.hpp"

namespace test {
using namespace simph::kern;

// ----------------------------------------------------------
// test fixture implementation
class TestPublication : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(TestPublication);
    // TODO for each test method:
    CPPUNIT_TEST(testPublishField);
    CPPUNIT_TEST(testPublishArrayField);
    CPPUNIT_TEST_SUITE_END();

private:
public:
    void setUp() {}

    void tearDown() {}

    void testPublishField() {
        Publication pub(new simph::smpdk::Object("testObj", "dummy object for testing", nullptr), nullptr);

        Smp::Char8 testChar = 'A';
        pub.PublishField("char", "char8 test pub", &testChar);
        Smp::ISimpleField* f = dynamic_cast<Smp::ISimpleField*>(pub.getChild("char"));
        CPPUNIT_ASSERT(f != nullptr);
        CPPUNIT_ASSERT_EQUAL(f, dynamic_cast<Smp::ISimpleField*>(pub.GetField("char")));
        CPPUNIT_ASSERT(strcmp(f->GetName(), "char") == 0);
        CPPUNIT_ASSERT_EQUAL('A', (char)f->GetValue());

        Smp::Int32 testInt32 = -17042;
        pub.PublishField("int32", "int32 test pub", &testInt32);
        f = dynamic_cast<Smp::ISimpleField*>(pub.getChild("int32"));
        CPPUNIT_ASSERT(f != nullptr);
        CPPUNIT_ASSERT_EQUAL(f, dynamic_cast<Smp::ISimpleField*>(pub.GetField("int32")));
        CPPUNIT_ASSERT(strcmp(f->GetName(), "int32") == 0);
        CPPUNIT_ASSERT_EQUAL(-17042, (int32_t)f->GetValue());

        Smp::Float64 testDouble = 42.042;
        pub.PublishField("double", "float 64 test pub", &testDouble);
        f = dynamic_cast<Smp::ISimpleField*>(pub.getChild("double"));
        CPPUNIT_ASSERT_EQUAL(f, dynamic_cast<Smp::ISimpleField*>(pub.GetField("double")));
        CPPUNIT_ASSERT(f != nullptr);
        CPPUNIT_ASSERT(strcmp(f->GetName(), "double") == 0);
        CPPUNIT_ASSERT_EQUAL(42.042, (double)f->GetValue());

        const Smp::FieldCollection* fc = pub.GetFields();
        CPPUNIT_ASSERT(fc != nullptr);
        CPPUNIT_ASSERT_EQUAL((size_t)3, fc->size());
        CPPUNIT_ASSERT(fc->at("char") != nullptr);
        CPPUNIT_ASSERT(fc->at("int32") != nullptr);
        CPPUNIT_ASSERT(fc->at("double") != nullptr);
        CPPUNIT_ASSERT(fc->at("double64") == nullptr);
        CPPUNIT_ASSERT_EQUAL(fc->at("char"), pub.GetField("char"));
        CPPUNIT_ASSERT_EQUAL(fc->at("int32"), pub.GetField("int32"));
        CPPUNIT_ASSERT_EQUAL(fc->at("double"), pub.GetField("double"));
    }

    void testPublishArrayField() {
        TypeRegistry tReg("TypesRegistry", "test types registry", nullptr);
        Publication pub(new simph::smpdk::Object("testObj", "dummy object for testing", nullptr), &tReg);

        Smp::Int32 iArray[] = {12, 17, 42};
        pub.PublishArray("iArray", "int array test pub", 3, iArray, Smp::PrimitiveTypeKind::PTK_Int32);

        Smp::ISimpleArrayField* f = dynamic_cast<Smp::ISimpleArrayField*>(pub.getChild("iArray"));
        CPPUNIT_ASSERT(f != nullptr);
        CPPUNIT_ASSERT(strcmp(f->GetName(), "iArray") == 0);
        CPPUNIT_ASSERT_EQUAL(12, (int32_t)f->GetValue(0));
        CPPUNIT_ASSERT_EQUAL(17, (int32_t)f->GetValue(1));
        CPPUNIT_ASSERT_EQUAL(42, (int32_t)f->GetValue(2));
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(TestPublication);
}  // namespace test
