/*
 * @file TestComposite.cpp
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include <cppunit/extensions/HelperMacros.h>
#include "simph/kern/Composite.hpp"
#include "simph/kern/Container.hpp"
#include "DummyComponent.hpp"


namespace test {
using namespace simph::kern;
class SampleComposite: virtual public Composite {
public:
    SampleComposite(Smp::String8 name, Smp::IObject* parent=nullptr):
                Object(name,"",parent),
                Composite(name,"",parent) {
        addContainer("c1");
        addContainer("c2");
    }
};

// ----------------------------------------------------------
// test fixture implementation
class TestComposite: public CppUnit::TestFixture {
CPPUNIT_TEST_SUITE( TestComposite );
CPPUNIT_TEST( testComposite );
CPPUNIT_TEST_SUITE_END();

private:

public:
    void setUp() {
    }

    void tearDown() {
    }

    void testComposite() {
        SampleComposite composite("composite");
        DummyComponent one("one");
        DummyComponent two("two");
        DummyComponent three("three");
        Container* ct1=dynamic_cast<Container*>(composite.GetContainer("c1"));
        CPPUNIT_ASSERT(ct1!=NULL);
        Container* ct2=dynamic_cast<Container*>(composite.GetContainer("c2"));
        CPPUNIT_ASSERT(ct2!=NULL);
        ct1->AddComponent(&one);        
        ct1->AddComponent(&two);        
        ct2->AddComponent(&three);
        Composite::dump(std::cout,&composite);        
    }

};

CPPUNIT_TEST_SUITE_REGISTRATION(TestComposite);
} // namespace test
