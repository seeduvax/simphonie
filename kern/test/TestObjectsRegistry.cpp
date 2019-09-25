/*
 * @file TestObjectRegistry.cpp
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include <cppunit/extensions/HelperMacros.h>
#include "simph/kern/ObjectsRegistry.hpp"
#include "simph/kern/Simulator.hpp"
#include "simph/sys/Logger.hpp"
#include "Smp/IDataflowField.h"

namespace test {
using namespace simph::kern;

// ----------------------------------------------------------
// test fixture implementation
class TestObjectsRegistry: public CppUnit::TestFixture {
CPPUNIT_TEST_SUITE( TestObjectsRegistry );
CPPUNIT_TEST( testRegistry );
CPPUNIT_TEST( testPublishArray );
CPPUNIT_TEST_SUITE_END();

private:

public:
    void setUp() {
    }

    void tearDown() {
    }

    void testRegistry() {
        Simulator* sim=new Simulator();
        ObjectsRegistry* registry=
                dynamic_cast<ObjectsRegistry*>(sim->GetResolver());
        CPPUNIT_ASSERT(registry!=nullptr);
        registry->dump();
        sim->Publish();
        TRACE("---------------------------");
        registry->dump();
        delete sim;
    }

    
    void testPublishArray() {
        int32_t iaOut[]={0,1,2,3,4};
        int32_t iaIn[]={0,0,0,0,0};

        auto treg=new TypeRegistry("TypeRegistry","Type registry service",nullptr);
        ObjectsRegistry* oreg=new ObjectsRegistry("oreg","",nullptr,treg);

        oreg->PublishArray("iaOut","",5,iaOut,
                    Smp::PrimitiveTypeKind::PTK_Int32,
                    Smp::ViewKind::VK_All, false, false, true);
        oreg->PublishArray("iaIn","",5,iaIn,
                    Smp::PrimitiveTypeKind::PTK_Int32,
                    Smp::ViewKind::VK_All, false, true, false);
        TRACE("---------------------------");
        oreg->dump();
        auto obj=oreg->ResolveAbsolute("iaOut");
        CPPUNIT_ASSERT(obj!=nullptr);
        auto src=dynamic_cast<Smp::IDataflowField*>(oreg->ResolveAbsolute("iaOut"));
        auto tgt=dynamic_cast<Smp::IDataflowField*>(oreg->ResolveAbsolute("iaIn"));
        CPPUNIT_ASSERT(src!=nullptr);
        CPPUNIT_ASSERT(tgt!=nullptr);
        src->Connect(tgt);
        src->Push();
        for (int i=0;i<5;i++) {
            LOGD("Checking entry #"<<i);
            CPPUNIT_ASSERT_EQUAL(iaOut[i],iaIn[i]);
        }

        delete oreg;
        delete treg;
    }

};

CPPUNIT_TEST_SUITE_REGISTRATION(TestObjectsRegistry);
} // namespace test
