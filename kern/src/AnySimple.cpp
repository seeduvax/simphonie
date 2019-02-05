#include "Smp/AnySimple.h"

namespace Smp {

AnySimple::AnySimple(): 
    type(Smp::PrimitiveTypeKind::PTK_None) {
}
AnySimple::AnySimple(Smp::PrimitiveTypeKind kind): 
    type(kind) {
}
AnySimple::AnySimple(const AnySimple& other):
    type(other.type), 
    value(other.value) {
}
AnySimple::AnySimple(AnySimple&& other):
    type(std::move(other.type)), 
    value(std::move(other.value)) {
}
AnySimple& AnySimple::operator=(const AnySimple& other) {
    type=other.type;
    value=other.value;
}
AnySimple& AnySimple::operator=(AnySimple&& other) {
    type=std::move(other.type);
    value=std::move(other.value);
}
AnySimple::~AnySimple() {
}
}
