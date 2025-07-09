/*
 * @file TestPublication.cpp
 *
 * Copyright 2020 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include <string.h>

#include "Smp/ISimpleArrayField.h"
#include "Smp/ISimpleField.h"
#include "abs/test.h"
#include "simdeck/Component.hpp"
#include "simdeck/StringField.hpp"
#include "simdeck/StringType.hpp"
#include "simphonie/kern/Publication.hpp"
#include "simphonie/kern/TypeRegistry.hpp"
#include "simphonie/sys/Logger.hpp"

namespace test {
using namespace simphonie::kern;
using namespace simdeck;

// ----------------------------------------------------------
// test fixture implementation
ABS_TEST_SUITE_BEGIN(Publication)

private:
public:
    void setUp() {}

    void tearDown() {}

    ABS_TEST_CASE_BEGIN(PublishField) {
        std::unique_ptr<simdeck::Component> component(new simdeck::Component("testObj", "dummy object for testing", nullptr));
        TypeRegistry treg("TypeRegistry", "", nullptr);
        Publication pub(component.get(), &treg);

        Smp::Char8 testChar = 'A';
        pub.PublishField("Char", "char8 test pub", &testChar);
        Smp::ISimpleField* f = dynamic_cast<Smp::ISimpleField*>(pub.GetField("Char"));
        CPPUNIT_ASSERT(f != nullptr);
        CPPUNIT_ASSERT_EQUAL(f, dynamic_cast<Smp::ISimpleField*>(component->GetField("Char")));
        CPPUNIT_ASSERT(strcmp(f->GetName(), "Char") == 0);
        CPPUNIT_ASSERT_EQUAL('A', (char)f->GetValue());

        Smp::Int32 testInt32 = -17042;
        pub.PublishField("int32", "int32 test pub", &testInt32);
        f = dynamic_cast<Smp::ISimpleField*>(pub.GetField("int32"));
        CPPUNIT_ASSERT(f != nullptr);
        CPPUNIT_ASSERT_EQUAL(f, dynamic_cast<Smp::ISimpleField*>(component->GetField("int32")));
        CPPUNIT_ASSERT(strcmp(f->GetName(), "int32") == 0);
        CPPUNIT_ASSERT_EQUAL(-17042, (int32_t)f->GetValue());

        Smp::Float64 testDouble = 42.042;
        pub.PublishField("Double", "float 64 test pub", &testDouble);
        f = dynamic_cast<Smp::ISimpleField*>(pub.GetField("Double"));
        CPPUNIT_ASSERT_EQUAL(f, dynamic_cast<Smp::ISimpleField*>(component->GetChild("Double")));
        CPPUNIT_ASSERT(f != nullptr);
        CPPUNIT_ASSERT(strcmp(f->GetName(), "Double") == 0);
        CPPUNIT_ASSERT_EQUAL(42.042, (double)f->GetValue());

        const Smp::FieldCollection* fc = pub.GetFields();
        CPPUNIT_ASSERT(fc != nullptr);
        CPPUNIT_ASSERT_EQUAL((size_t)3, fc->size());
        CPPUNIT_ASSERT(fc->at("Char") != nullptr);
        CPPUNIT_ASSERT(fc->at("int32") != nullptr);
        CPPUNIT_ASSERT(fc->at("Double") != nullptr);
        CPPUNIT_ASSERT(fc->at("double64") == nullptr);
        CPPUNIT_ASSERT_EQUAL(fc->at("Char"), pub.GetField("Char"));
        CPPUNIT_ASSERT_EQUAL(fc->at("int32"), pub.GetField("int32"));
        CPPUNIT_ASSERT_EQUAL(fc->at("Double"), pub.GetField("Double"));
    }
    ABS_TEST_CASE_END
    ABS_TEST_CASE_BEGIN(PublishArrayField) {
        TypeRegistry tReg("TypesRegistry", "test types registry", nullptr);
        std::unique_ptr<simdeck::Component> component(new simdeck::Component("testObj", "dummy object for testing", nullptr));
        Publication pub(component.get(), &tReg);

        Smp::Int32 iArray[] = {12, 17, 42};
        pub.PublishArray("iArray", "int array test pub", 3, iArray, Smp::PrimitiveTypeKind::PTK_Int32);

        Smp::ISimpleArrayField* f = dynamic_cast<Smp::ISimpleArrayField*>(pub.GetField("iArray"));
        CPPUNIT_ASSERT(f != nullptr);
        CPPUNIT_ASSERT(strcmp(f->GetName(), "iArray") == 0);
        CPPUNIT_ASSERT_EQUAL(12, (int32_t)f->GetValue(0));
        CPPUNIT_ASSERT_EQUAL(17, (int32_t)f->GetValue(1));
        CPPUNIT_ASSERT_EQUAL(42, (int32_t)f->GetValue(2));
    }
    ABS_TEST_CASE_END

    ABS_TEST_CASE_BEGIN(PublishStringField) {
        TypeRegistry tReg("TypesRegistry", "test types registry", nullptr);
        std::unique_ptr<simdeck::Component> component(
            new simdeck::Component("testObj", "dummy object for testing", nullptr));
        Publication pub(component.get(), &tReg);

        std::string stringField = "String Field";
        auto f = pub.PublishField("strField", "std::string test pub", &stringField, StringType::UuidString);
        CPPUNIT_ASSERT(f != nullptr);
        auto child = component->GetField("strField");
        CPPUNIT_ASSERT(f == child);
        auto sf = dynamic_cast<StringField*>(f);
        CPPUNIT_ASSERT(sf != nullptr);
        std::string retrieve = (const char*)sf->GetValue();
        CPPUNIT_ASSERT(retrieve == "String Field");
    }
    ABS_TEST_CASE_END
    ABS_TEST_SUITE_END
}  // namespace test
