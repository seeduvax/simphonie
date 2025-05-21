/*
 * @file TestFieldRecorderCsv.cpp
 *
 * Copyright 2025 Sebastien Devaux. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "abs/test.h"
#include "simphonie/colibry/FieldRecorderCsv.hpp"
#include "simphonie/kern/Simulator.hpp"

#include "simdeck/SimpleField.hpp"

namespace test {
using namespace simphonie::colibry;

// ----------------------------------------------------------
// test suite implementation
ABS_TEST_SUITE_BEGIN( FieldRecorderCsv )
// uncomment and complete next line for test suite description
// ABS_TEST_DESCR(test description)

private:

public:
    void setUp() {
    }

    void tearDown() {
    }

    ABS_TEST_CASE_BEGIN(RecordSimpleFields) {
        ABS_TEST_DESCR(Chack FieldRecorderCsv can record few simple fields )
        simphonie::kern::Simulator sim("TestSim");
        auto recorder=new FieldRecorderCsv("testfrec", "", &sim);
        sim.AddService(recorder);
        sim.Publish();
        sim.Configure();
        sim.Connect();

        simdeck::Type _float64Type(Smp::Uuids::Uuid_Int64, Smp::PrimitiveTypeKind::PTK_Float64, sizeof(Smp::Float64), "Float64",
                       "Eight bytes signed float data type");
        simdeck::Type _int32Type(Smp::Uuids::Uuid_Int32, Smp::PrimitiveTypeKind::PTK_Int32, sizeof(Smp::Int32), "Int32",
                       "Four bytes signed int data type");

        Smp::Float64 dblV=42.0;
        Smp::Int32 intV=42;
        auto dblF=simdeck::SimpleField::Create("dbl","",Smp::ViewKind::VK_None, 
                                          &_float64Type, &dblV, false, false, false, nullptr);
        auto intF=simdeck::SimpleField::Create("int32","",Smp::ViewKind::VK_None, 
                                          &_int32Type, &intV, false, false, false, nullptr);
        auto port=dynamic_cast<Smp::IOutputField*>(recorder->GetChild("port"));
        CPPUNIT_ASSERT(port!=nullptr);
        port->Connect(dblF);
        port->Connect(intF);
    
        recorder->step();
        dblV-=1.1;
        intV++;
        recorder->step();
    }
    ABS_TEST_CASE_END
ABS_TEST_SUITE_END
} // namespace test
