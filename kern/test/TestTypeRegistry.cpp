/*
 * @file TestTypeRegistry.cpp
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include <cppunit/extensions/HelperMacros.h>
#include "simph/kern/TypeRegistry.hpp"

namespace test {
using namespace simph::kern;

// ----------------------------------------------------------
// test fixture implementation
class TestTypeRegistry: public CppUnit::TestFixture {
CPPUNIT_TEST_SUITE( TestTypeRegistry );
CPPUNIT_TEST( testPublishTypes );
CPPUNIT_TEST_SUITE_END();

private:
    TypeRegistry* _reg;

public:
    void setUp() {
        _reg=new TypeRegistry("TypeRegistry","Type registry service",nullptr);
    }

    void tearDown() {
        delete _reg;
    }

    void testPublishTypes() {
        Smp::Uuid degreUuid("00000000-0000-0000-0000-000000000001");
        Smp::Publication::IType* t=_reg->AddFloatType("degre","angle unit",degreUuid,-360,360,true,true,"'");
        CPPUNIT_ASSERT(t!=nullptr);
        bool exception=false;
        try {
            _reg->AddFloatType("degre","angle unit",degreUuid,-360,360,true,true,"'");
        }
        catch (Smp::Publication::TypeAlreadyRegistered& ex) {
            // this exception is expected here...
            exception=true;
        }
        CPPUNIT_ASSERT(exception);
    }
    void testPublishStruct() {
        Smp::Uuid structUuid("00000000-0000-0000-0000-000000000001");
        Smp::Publication::IStructureType* t=_reg->AddStructureType("struct",
                    "structure test", structUuid);
        CPPUNIT_ASSERT(t!=nullptr);
        t->AddField("f1","Field One integer",Smp::Uuids::Uuid_Int64,0);
        t->AddField("f2","Field Two double",Smp::Uuids::Uuid_Float64,0);
        
    }

};

CPPUNIT_TEST_SUITE_REGISTRATION(TestTypeRegistry);
} // namespace test
