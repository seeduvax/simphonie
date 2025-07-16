#ifndef __test_SimpleType_hpp__
#define __test_SimpleType_hpp__

#include "simdeck/Type.hpp"

namespace test {

class SimpleType: public simdeck::Type {
public:
    SimpleType(Smp::Uuid uuid, Smp::PrimitiveTypeKind kind, Smp::String8 name, Smp::String8 desc = "") 
        : simdeck::Type(uuid, kind, name, desc) {}
    ~SimpleType() override = default;
    Smp::IField* createField(
        Smp::String8 name,
        Smp::String8 description,
        Smp::IComponent* parent,
        void* address,
        Smp::ViewKind view,
        Smp::Bool state,
        Smp::Bool input,
        Smp::Bool output) const override { return nullptr; };
};
}  // namespace test
#endif
