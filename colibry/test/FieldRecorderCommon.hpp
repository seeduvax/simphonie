#ifndef __simphonie_colibry_test_FieldRecordCommon_hpp__
#define __simphonie_colibry_test_FieldRecordCommon_hpp__
#include <filesystem>
#include "simdeck/SimpleField.hpp"
#include "simdeck/SimpleType.hpp"
#include "simphonie/kern/Simulator.hpp"

namespace test {
template <typename T>
void recordSimpleFieldsCase(const char * fileExt) {
    // output file in test build dir
    std::string fPath=getenv("TTARGETDIR");
    fPath=fPath + "simpleFieldsRec" + fileExt; 
    // delete file if exist for no side effect from test case to test case
    std::filesystem::remove(fPath);

    simphonie::kern::Simulator sim("TestSim");
    T* recorder = new T("testfrec", "", &sim);
    sim.AddService(recorder);
    sim.Publish();
    // set output file name
    auto ff=dynamic_cast<Smp::ISimpleField*>(recorder->GetField("filePath"));
    CPPUNIT_ASSERT(ff!=nullptr);
    ff->SetValue({Smp::PrimitiveTypeKind::PTK_String8, fPath.c_str()});
    
    sim.Configure();

    simdeck::SimpleType _float64Type(Smp::Uuids::Uuid_Int64, Smp::PrimitiveTypeKind::PTK_Float64, "Float64",
                                     "Eight bytes signed float data type");
    simdeck::SimpleType _int32Type(Smp::Uuids::Uuid_Int32, Smp::PrimitiveTypeKind::PTK_Int32, "Int32",
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
    sim.Connect();

    recorder->step();
    dblV-=1.1;
    intV++;
    recorder->step();
}
} // namespace test
#endif // __simphonie_colibry_test_FieldRecordCommon_hpp__
