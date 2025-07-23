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
#include "simdeck/Field.hpp"
#include "simphonie/kern/Resolver.hpp"
#include "simphonie/kern/Simulator.hpp"
#include "simphonie/kern/TypeRegistry.hpp"
#include "simphonie/sys/Logger.hpp"
#include "Smp/ISimpleArrayField.h"
#include "Smp/IModel.h"

namespace test {
using namespace simphonie::kern;
using namespace simdeck;


// ----------------------------------------------------------
// test fixture implementation
class TestResolver : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(TestResolver);
    CPPUNIT_TEST(testResolver);
    CPPUNIT_TEST_SUITE_END();

private:
class Model: public simdeck::Component,
            public virtual simdeck::AComposite,
            public virtual Smp::IModel {
public:
    Model(Smp::String8 name, Smp::String8 descr, Smp::IObject* parent):
            simdeck::Component(name, descr, parent) {
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
        simphonie::kern::Simulator simu("simu", "Resovler owner", nullptr);
        auto mdl1 = new Model("to1", "test obj 1", &simu);
        simu.AddModel(mdl1);
        std::unique_ptr<Model> mdl2(new Model("to2", "test obj 2", mdl1));
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
        CPPUNIT_ASSERT_EQUAL((Smp::IObject*)mdl2.get(), resolver.ResolveAbsolute("to1/to2"));
        CPPUNIT_ASSERT_EQUAL((Smp::IObject*)mdl1, resolver.ResolveAbsolute("to1/to2/.."));
        CPPUNIT_ASSERT_EQUAL((Smp::IObject*)mdl2.get(), resolver.ResolveAbsolute("to1/../to1/to2"));
        CPPUNIT_ASSERT_EQUAL((Smp::IObject*)nullptr, resolver.ResolveAbsolute("to1/to2/..."));

        CPPUNIT_ASSERT_EQUAL((Smp::IObject*)nullptr, resolver.ResolveRelative("to1", mdl2.get()));
        CPPUNIT_ASSERT_EQUAL((Smp::IObject*)mdl2.get(), resolver.ResolveRelative("to2", mdl1));
        auto arrField = resolver.ResolveAbsolute("to1.to2.iArray");

        CPPUNIT_ASSERT(dynamic_cast<Smp::ISimpleArrayField*>(arrField) != nullptr);
        CPPUNIT_ASSERT_EQUAL((size_t)3, dynamic_cast<Smp::ISimpleArrayField*>(arrField)->GetSize());

        auto simpleField = resolver.ResolveAbsolute("to1.to2.iArray[2]");
        std::cout << "ptr : " << &simpleField << std::endl;
        CPPUNIT_ASSERT(dynamic_cast<Smp::ISimpleArrayField*>(simpleField) == nullptr);
        CPPUNIT_ASSERT(dynamic_cast<Smp::ISimpleField*>(simpleField) != nullptr);

        auto simpleFieldSimple = dynamic_cast<Smp::ISimpleField*>(simpleField)->GetValue();
        CPPUNIT_ASSERT_EQUAL(Smp::PrimitiveTypeKind::PTK_Int32, simpleFieldSimple.GetType());
        CPPUNIT_ASSERT_EQUAL(42, (int32_t)simpleFieldSimple);
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(TestResolver);
}  // namespace test
