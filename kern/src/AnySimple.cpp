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
    return *this;
}
AnySimple& AnySimple::operator=(AnySimple&& other) {
    type=std::move(other.type);
    value=std::move(other.value);
    return *this;
}
AnySimple::~AnySimple() noexcept {
}
void AnySimple::SetValue(Smp::PrimitiveTypeKind kind,Smp::Bool v) {
    type=kind;
    value.boolValue=v;
}
void AnySimple::SetValue(Smp::PrimitiveTypeKind kind,Smp::Char8 v) {
    type=kind;
    value.char8Value=v;
}
void AnySimple::SetValue(Smp::PrimitiveTypeKind kind,Smp::String8 v) {
    type=kind;
    value.string8Value=v;
}
void AnySimple::SetValue(Smp::PrimitiveTypeKind kind,Smp::UInt8 v) {
    type=kind;
    value.uInt8Value=v;
}
void AnySimple::SetValue(Smp::PrimitiveTypeKind kind,Smp::UInt16 v) {
    type=kind;
    value.uInt16Value=v;
}
void AnySimple::SetValue(Smp::PrimitiveTypeKind kind,Smp::UInt32 v) {
    type=kind;
    value.uInt32Value=v;
}
void AnySimple::SetValue(Smp::PrimitiveTypeKind kind,Smp::UInt64 v) {
    type=kind;
    value.uInt64Value=v;
}
void AnySimple::SetValue(Smp::PrimitiveTypeKind kind,Smp::Int8 v) {
    type=kind;
    value.int8Value=v;
}
void AnySimple::SetValue(Smp::PrimitiveTypeKind kind,Smp::Int16 v) {
    type=kind;
    value.int16Value=v;
}
void AnySimple::SetValue(Smp::PrimitiveTypeKind kind,Smp::Int32 v) {
    type=kind;
    value.int32Value=v;
}
void AnySimple::SetValue(Smp::PrimitiveTypeKind kind,Smp::Int64 v) {
    type=kind;
    value.int64Value=v;
}
void AnySimple::SetValue(Smp::PrimitiveTypeKind kind,Smp::Float32 v) {
    type=kind;
    value.float32Value=v;
}
void AnySimple::SetValue(Smp::PrimitiveTypeKind kind,Smp::Float64 v) {
    type=kind;
    value.float64Value=v;
}
AnySimple::operator Smp::Bool() const {
    return value.boolValue;
}
AnySimple::operator Smp::Char8() const {
    return value.char8Value;
}
AnySimple::operator Smp::String8() const {
    return value.string8Value;
}
AnySimple::operator Smp::UInt8() const {
    return value.uInt8Value;
}
AnySimple::operator Smp::UInt16() const {
    return value.uInt16Value;
}
AnySimple::operator Smp::UInt32() const {
    return value.uInt32Value;
}
AnySimple::operator Smp::UInt64() const {
    return value.uInt64Value;
}
AnySimple::operator Smp::Int8() const {
    return value.int8Value;
}
AnySimple::operator Smp::Int16() const {
    return value.int16Value;
}
AnySimple::operator Smp::Int32() const {
    return value.int32Value;
}
AnySimple::operator Smp::Int64() const {
    return value.int64Value;
}
AnySimple::operator Smp::Float32() const {
    return value.float32Value;
}
AnySimple::operator Smp::Float64() const {
    return value.float64Value;
}
Smp::String8 AnySimple::MoveString() {
    return std::move(value.string8Value);
}
Smp::PrimitiveTypeKind AnySimple::GetType() const noexcept {
    return type;
}
bool AnySimple::operator==(const Smp::AnySimple& other) const {
    if (type==other.type) {
        switch (type) {
            case Smp::PrimitiveTypeKind::PTK_Char8:
                return value.char8Value==other.value.char8Value;
            case Smp::PrimitiveTypeKind::PTK_Bool:
                return value.boolValue==other.value.boolValue;
            case Smp::PrimitiveTypeKind::PTK_String8:
                return strcmp(value.string8Value,other.value.string8Value)==0;
            case Smp::PrimitiveTypeKind::PTK_UInt8:
                return value.uInt8Value==other.value.uInt8Value;
            case Smp::PrimitiveTypeKind::PTK_UInt16:
                return value.uInt16Value==other.value.uInt16Value;
            case Smp::PrimitiveTypeKind::PTK_UInt32:
                return value.uInt32Value==other.value.uInt32Value;
            case Smp::PrimitiveTypeKind::PTK_UInt64:
                return value.uInt64Value==other.value.uInt64Value;
            case Smp::PrimitiveTypeKind::PTK_Int8:
                return value.int8Value==other.value.int8Value;
            case Smp::PrimitiveTypeKind::PTK_Int16:
                return value.int16Value==other.value.int16Value;
            case Smp::PrimitiveTypeKind::PTK_Int32:
                return value.int32Value==other.value.int32Value;
            case Smp::PrimitiveTypeKind::PTK_Int64:
                return value.int64Value==other.value.int64Value;
            case Smp::PrimitiveTypeKind::PTK_Float32:
                return value.float32Value==other.value.float32Value;
            case Smp::PrimitiveTypeKind::PTK_Float64:
                return value.float64Value==other.value.float64Value;
            default:
                return false;
        }
    }
    return false;
}
bool AnySimple::operator!=(const Smp::AnySimple& other) const {
    return !(*this==other);
}
}
