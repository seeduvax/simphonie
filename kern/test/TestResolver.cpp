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
#include "Smp/IModel.h"

namespace test {
using namespace simph::kern;


// ----------------------------------------------------------
// test fixture implementation
class TestResolver : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(TestResolver);
    CPPUNIT_TEST(testResolver);
    CPPUNIT_TEST_SUITE_END();

private:
class Model: public simph::smpdk::Component,
            public virtual simph::smpdk::Composite,
            public virtual Smp::IModel {
public:
    Model(Smp::String8 name, Smp::String8 descr, Smp::IObject* parent):
            simph::smpdk::Component(name, descr, parent) {
        addContainer("Children");
        auto mp=dynamic_cast<Model*>(parent);
        if (mp!=nullptr) {
            auto c=mp->GetContainer("Children");
            if (c!=nullptr) {
                c->AddComponent(this);
            }
        }
    }
    virtual ~Model() {
    }
    Smp::Int32* _i3DVect=nullptr;
    Smp::String8 GetName() const override {
        return Component::GetName();
    }
    Smp::String8 GetDescription() const override {
        return Component::GetDescription();
    }
    Smp::IObject* GetParent() const override {
        return Component::GetParent();
    }
    Smp::IObject* GetChild(Smp::String8 name) const override {
        return Component::GetChild(name);
    }
protected:
    void publish(Smp::IPublication* receiver) {
        if (_i3DVect!=nullptr) {
            receiver->PublishArray("iArray", "test int array", 3, _i3DVect,
                    Smp::PrimitiveTypeKind::PTK_Int32);
        }
    }

};

public:
    void setUp() {}

    void tearDown() {}

    void testResolver() {
        simph::kern::Simulator simu("simu", "Resovler owner", nullptr);
        auto mdl1 = new Model("to1", "test obj 1", &simu);
        simu.AddModel(mdl1);
        auto mdl2 = new Model("to2", "test obj 2", mdl1);
        Smp::Int32 iArray[] = {12, 17, 42};
        mdl2->_i3DVect = iArray;

        simu.Publish();
        simu.Configure();
        simu.Connect();

        Resolver& resolver = dynamic_cast<Resolver&>(*(simu.GetResolver()));

        TRACE("==>")
        resolver.GetEntryPoint("dump")->Execute();

        CPPUNIT_ASSERT_EQUAL((Smp::IObject*)nullptr, resolver.ResolveAbsolute("plop"));
        CPPUNIT_ASSERT_EQUAL((Smp::IObject*)mdl1, resolver.ResolveAbsolute("to1"));
        CPPUNIT_ASSERT_EQUAL((Smp::IObject*)nullptr, resolver.ResolveAbsolute("to2"));
        CPPUNIT_ASSERT_EQUAL((Smp::IObject*)mdl2, resolver.ResolveAbsolute("to1/to2"));
        CPPUNIT_ASSERT_EQUAL((Smp::IObject*)mdl1, resolver.ResolveAbsolute("to1/to2/.."));
        CPPUNIT_ASSERT_EQUAL((Smp::IObject*)mdl2, resolver.ResolveAbsolute("to1/../to1/to2"));
        CPPUNIT_ASSERT_EQUAL((Smp::IObject*)nullptr, resolver.ResolveAbsolute("to1/to2/..."));

        CPPUNIT_ASSERT_EQUAL((Smp::IObject*)nullptr, resolver.ResolveRelative("to1", mdl2));
        CPPUNIT_ASSERT_EQUAL((Smp::IObject*)mdl2, resolver.ResolveRelative("to2", mdl1));
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
