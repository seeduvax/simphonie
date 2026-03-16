/*
 * @file TestVectorField.cpp
 *
 * Copyright 2026 Sebastien Devaux. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
 #include "abs/test.h"
 #include "simdeck/VectorField.hpp"
 #include "simdeck/SimpleVectorField.hpp"
 #include "simdeck/SimpleArrayField.hpp"
 #include "simdeck/SimpleVectorType.hpp"
 #include "simdeck/ArrayType.hpp"
 #include "Smp/Publication/ITypeRegistry.h"
 #include "Smp/IOutputField.h"
 #include "Smp/IArrayField.h"
 #include "Smp/ISimpleArrayField.h"
 #include "simdeck/Object.hpp"
 #include "simdeck/VectorType.hpp"
 #include "simdeck/SimpleType.hpp"
 #include <map>
 
namespace test {
using namespace simdeck;

class DummyTypeRegistry: public Object, public Smp::Publication::ITypeRegistry{

public:
        DummyTypeRegistry( Smp::String8 name, Smp::String8 descr = "",
         Smp::IObject* parent = nullptr):Object(name, descr, parent){}
        ~DummyTypeRegistry(){}
        Smp::Publication::IType* GetType(
                    Smp::PrimitiveTypeKind type) const override{
                    return _type;
         }
         Smp::Publication::IType* GetType(
                Smp::Uuid typeUuid) const override{
                return _typeByUuid.at(typeUuid);
        }

        inline  void Register(Smp::Publication::IType* type){
            _typeByUuid.insert({type->GetUuid(), type});
        }

        Smp::Publication::IType* AddFloatType(
                Smp::String8 name,
                Smp::String8 description,
                Smp::Uuid typeUuid,
                Smp::Float64 minimum,
                Smp::Float64 maximum,
                Smp::Bool minInclusive,
                Smp::Bool maxInclusive,
                Smp::String8 unit,
                Smp::PrimitiveTypeKind type = Smp::PrimitiveTypeKind::PTK_Float64) override{ return nullptr;}

                Smp::Publication::IType* AddIntegerType(
                Smp::String8 name,
                Smp::String8 description,
                Smp::Uuid typeUuid,
                Smp::Int64 minimum,
                Smp::Int64 maximum,
                Smp::String8 unit,
                Smp::PrimitiveTypeKind type = Smp::PrimitiveTypeKind::PTK_Int32)  override{ return nullptr;}

                Smp::Publication::IEnumerationType* AddEnumerationType(
                Smp::String8 name,
                Smp::String8 description,
                Smp::Uuid typeUuid)   override{ return nullptr;}

                Smp::Publication::IArrayType* AddArrayType(
                Smp::String8 name,
                Smp::String8 description,
                Smp::Uuid typeUuid,
                Smp::Uuid itemTypeUuid,
                Smp::UInt64 itemSize,
                Smp::UInt64 arrayCount,
                Smp::Bool simpleArray = false)  override{ return nullptr;}

                Smp::Publication::IStringType* AddStringType(
                Smp::String8 name,
                Smp::String8 description,
                Smp::Uuid typeUuid,
                Smp::UInt64 length)  override{ return nullptr;}

                Smp::Publication::IStructureType* AddStructureType(
                Smp::String8 name,
                Smp::String8 description,
                Smp::Uuid typeUuid) override{ return nullptr;}

                Smp::Publication::IClassType* AddClassType(
                Smp::String8 name,
                Smp::String8 description,
                Smp::Uuid typeUuid,
                Smp::Uuid baseClassUuid) override{ return nullptr;}
private:
    SimpleVectorType* _type;
    std::map<Smp::Uuid, Smp::Publication::IType*> _typeByUuid;
};

class TestSimpleVectorType: public simdeck::SimpleVectorType {
typedef simdeck::SimpleVectorType Base;
    public:
    TestSimpleVectorType(Smp::Uuid uuid, Smp::PrimitiveTypeKind kind, Smp::String8 name, Smp::String8 descr, Smp::IObject* parent):
    	_uuid(uuid),
        Base(kind, name, descr, parent) {
    }
    virtual ~TestSimpleVectorType() {
    }
    Smp::Uuid GetUuid() const override {
        return _uuid;

    }
private:
    Smp::Uuid _uuid;
};
class TestSimpleArrayType: public simdeck::ArrayType {
typedef simdeck::ArrayType Base;
    public:
    TestSimpleArrayType(Smp::Uuid uuid, Smp::String8 name, Smp::String8 descr, Smp::IObject* parent,
              Smp::UInt64 size, Smp::Publication::IType* type):
    	_uuid(uuid),
        Base(uuid, name, descr, parent, size, type) {
    }
    virtual ~TestSimpleArrayType() {
    }
    Smp::Uuid GetUuid() const override {
        return _uuid;

    }
private:
    Smp::Uuid _uuid;
};


 
 // ----------------------------------------------------------
 // test suite implementation
 ABS_TEST_SUITE_BEGIN( VectorField )
 // uncomment and complete next line for test suite description
 // ABS_TEST_DESCR(test description)

 private:
    DummyTypeRegistry* _typeRegistry;
    TestSimpleVectorType* _simplevectorTypeInt32;
    VectorType* _vectorTypeInt;
    VectorType* _vectorTypeArrayDouble;
    const Smp::Uuid Uuid_Vector_Vector_Integer = { 0, 0, 0, { 'V','e','c','V','e','c','t','I'} };
    const Smp::Uuid Uuid_Vector_Integer        = { 0, 0, 0, { 'V','e','c','t','o','r','I','n'} };

    TestSimpleArrayType* _simplearrayTypeDouble;
    VectorType* _vectorTypeDouble;
    SimpleType* _float64Type;
    const Smp::Uuid Uuid_Vector_Array_Double    = { 0, 0, 0, { 'V','e','c','A','r','r','a','D'} };
    const Smp::Uuid Uuid_Vector_Double          = { 0, 0, 0, { 'V','e','c','t','o','r','D','o'} };

    const Smp::Uuid Uuid_Vector_Vector_Array_Double    = { 0, 0, 0, { 'V','e','c','V','e','c','a','D'} };

 
 public:
 void setUp() {
    _typeRegistry = new DummyTypeRegistry("zz", "", nullptr);
    _vectorTypeInt = new VectorType(Uuid_Vector_Vector_Integer, Uuid_Vector_Integer, Smp::PrimitiveTypeKind::PTK_Int32, "Int32",
                    "Eight bytes signed int data type", dynamic_cast<Smp::IObject*>(_typeRegistry));
    _simplevectorTypeInt32 = new TestSimpleVectorType(Uuid_Vector_Integer, Smp::PrimitiveTypeKind::PTK_Int32, "Int32",
                    "Eight bytes signed int data type", dynamic_cast<Smp::IObject*>(_typeRegistry));

    _vectorTypeDouble = new VectorType(Uuid_Vector_Array_Double, Uuid_Vector_Double, Smp::PrimitiveTypeKind::PTK_Float64, "float64",
                    "Eight bytes signed dounle data type", dynamic_cast<Smp::IObject*>(_typeRegistry));
    _float64Type = new SimpleType(Smp::Uuids::Uuid_Float64, Smp::PrimitiveTypeKind::PTK_Float64, "float64",
                    "Eight bytes signed double data type");
    _simplearrayTypeDouble = new TestSimpleArrayType(Uuid_Vector_Double, "float64", "Eight bytes signed double data type", 
    dynamic_cast<Smp::IObject*>(_typeRegistry), 2, _float64Type);
    
    _vectorTypeArrayDouble = new VectorType(Uuid_Vector_Vector_Array_Double, Uuid_Vector_Array_Double, Smp::PrimitiveTypeKind::PTK_Float64, "float64",
                    "Eight bytes signed dounle data type", dynamic_cast<Smp::IObject*>(_typeRegistry));
    _typeRegistry->Register(_simplevectorTypeInt32);
    _typeRegistry->Register(_simplearrayTypeDouble);
    _typeRegistry->Register(_vectorTypeDouble);
 }
 
 void tearDown() {
 }
 
 ABS_TEST_CASE_BEGIN(VectorVectorInt) {
        std::vector<Smp::Int32> v1 = {1,2,4,8,16,32};
        std::vector<Smp::Int32> v2 = {64,128,256,512,1028,2048};
        std::vector<Smp::Int32> v3 = {1,3,5,7};
        std::vector<std::vector<Smp::Int32>*> vvint={&v1,&v2};
        auto vf=VectorField::Create("vvint","", (void*)(&vvint), _simplevectorTypeInt32,
                Smp::ViewKind::VK_All, _vectorTypeInt,
                false, true, false, nullptr);
        CPPUNIT_ASSERT_EQUAL(vvint.size(), vf->GetSize()); 
        auto field1 = vf->GetItem(0);
        auto vfield1 = dynamic_cast<SimpleVectorField*>(field1);
        CPPUNIT_ASSERT_EQUAL(v1.size(), vfield1->GetSize());
        v1.push_back(5);
        CPPUNIT_ASSERT_EQUAL(v1.size(), vfield1->GetSize());
        vvint.push_back(&v3);
        CPPUNIT_ASSERT_EQUAL(vvint.size(), vf->GetSize());
    }
    ABS_TEST_CASE_END

    ABS_TEST_CASE_BEGIN(VectorArrayDouble) {
        std::array<Smp::Float64, 2> v1 = {1.0325,2.0325};
        std::array<Smp::Float64, 2> v2 = {64.0325,128.0325};
        std::array<Smp::Float64, 2> v3 = {1.0325,3.0325};
        std::vector<std::array<Smp::Float64, 2>*> vadouble={&v1,&v2};
        auto vf=VectorField::Create("vadouble","",(void*)(&vadouble),_simplearrayTypeDouble,
                Smp::ViewKind::VK_All, _vectorTypeDouble,
                false, true, false, nullptr);
        CPPUNIT_ASSERT_EQUAL(vadouble.size(), vf->GetSize()); 
        auto field1 = vf->GetItem(0);
        auto vfield1 = dynamic_cast<SimpleArrayField*>(field1);        
        CPPUNIT_ASSERT_EQUAL(v1.size(), vfield1->GetSize());
        auto value = vfield1->GetValue(1);
        std::clog << "value: " << value << std::endl;
        CPPUNIT_ASSERT_EQUAL((Smp::Float64)v1[1], (Smp::Float64)value);
        vadouble.push_back(&v3);
        CPPUNIT_ASSERT_EQUAL(vadouble.size(), vf->GetSize());
        vadouble.pop_back();
        CPPUNIT_ASSERT_EQUAL(2, (int)vf->GetSize());
    }
    ABS_TEST_CASE_END


    ABS_TEST_CASE_BEGIN(VectorVectorArrayDoubleOutputField) {
        std::array<Smp::Float64, 2> v1 = {1.0325,2.0325};
        std::array<Smp::Float64, 2> v2 = {64.0325,128.0325};
        std::array<Smp::Float64, 2> v11 = {1.2,2.3};
        std::array<Smp::Float64, 2> v22 = {1.2,2.3};
        std::vector<std::array<Smp::Float64, 2>*> firstVAdouble={&v1,&v2};
        
        std::array<Smp::Float64, 2> v3 = {3.0325, 4.0325};
        std::array<Smp::Float64, 2> v4 = {256.0325, 512.0325};
        std::array<Smp::Float64, 2> v33 = {1.2,2.3};
        std::array<Smp::Float64, 2> v44 = {1.2,2.3};
        std::vector<std::array<Smp::Float64, 2>*> secondVAdouble={&v3,&v4};        
        std::vector<std::vector<std::array<Smp::Float64, 2>*>*> nestedVector = {&firstVAdouble, &secondVAdouble};
        std::vector<std::array<Smp::Float64, 2>*> firstVAdoubleOutput={&v11,&v22};
        std::vector<std::array<Smp::Float64, 2>*> secondVAdoubleOutput={&v33,&v44};
        std::vector<std::vector<std::array<Smp::Float64, 2>*>*> outputVector = {&firstVAdoubleOutput,&secondVAdoubleOutput};
        
       auto vf=VectorField::Create("nestedVAdouble","",(void*)(&nestedVector),_vectorTypeDouble,
                Smp::ViewKind::VK_All, _vectorTypeArrayDouble,
                false, true, false, nullptr);

       auto vfOutput=VectorField::Create("nestedVAdoubleOutput","", (void*)(&outputVector),_vectorTypeDouble,
                Smp::ViewKind::VK_All,_vectorTypeArrayDouble ,
                false, false, true, nullptr);

        CPPUNIT_ASSERT(vfOutput->IsOutput());
        Smp::IOutputField* outputField = dynamic_cast<Smp::IOutputField*>(vfOutput);
        CPPUNIT_ASSERT(outputField != nullptr);
        outputField->Connect(vf);
        outputField->Push();
        
        for(int i =0; i < 2; i++){
            auto expectedField = dynamic_cast<Smp::IArrayField*>(vf->GetItem(i));
            auto outputInnerField = dynamic_cast<Smp::IArrayField*>(vfOutput->GetItem(i));
            for(int j= 0; j < expectedField->GetSize(); j++){
                auto expectedArrayField =dynamic_cast<Smp::ISimpleArrayField*>(expectedField->GetItem(j));
                auto outputArrayField = dynamic_cast<Smp::ISimpleArrayField*>(outputInnerField->GetItem(j));
                for(int k = 0; k < expectedArrayField->GetSize(); k++){
                    CPPUNIT_ASSERT_DOUBLES_EQUAL((Smp::Float64)expectedArrayField->GetValue(k), (Smp::Float64)outputArrayField->GetValue(k), 1e-15);
                }
            }
        }
    }
    ABS_TEST_CASE_END
    

 ABS_TEST_SUITE_END


 } // namespace test

