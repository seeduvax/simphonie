#include "Smp/Uuid.h"
#include "Smp/PrimitiveTypes.h"
#include <cstring>

namespace Smp {
constexpr Uuid Smp::Uuids::Uuid_Char8;
constexpr Uuid Smp::Uuids::Uuid_Bool;
constexpr Uuid Smp::Uuids::Uuid_Int8;
constexpr Uuid Smp::Uuids::Uuid_Int16;
constexpr Uuid Smp::Uuids::Uuid_Int32;
constexpr Uuid Smp::Uuids::Uuid_Int64;
constexpr Uuid Smp::Uuids::Uuid_UInt8;
constexpr Uuid Smp::Uuids::Uuid_UInt16;
constexpr Uuid Smp::Uuids::Uuid_UInt32;
constexpr Uuid Smp::Uuids::Uuid_UInt64;
constexpr Uuid Smp::Uuids::Uuid_Float32;
constexpr Uuid Smp::Uuids::Uuid_Float64;
// --------------------------------------------------------------------
// ..........................................................
Uuid::Uuid(const char * value) {
    // TODO check if this naive way of doing does not
    // introduce endianness issue.
    int bSize=sizeof(Data1)+sizeof(Data2)+sizeof(Data3);
    uint8_t buf[bSize];
    std::memset(buf,0,bSize);
    const char* ptr=value;
    int i=0;
    while (*ptr!='\0' && i<2*bSize) {
        int v=-1;
        if (*ptr>='A' && *ptr<='F') {
            v=*ptr - 'A' + 0x0A;
        }
        else if (*ptr>='a' && *ptr<='f') {
            v=*ptr - 'a'+ 0x0A;
        }
        else if (*ptr>='0' && *ptr<='9') {
            v=*ptr - '0';
        }
        ptr++;
        if (v>=0) {
            int j=i/2;
            if (i%2!=0) {
                buf[j]=buf[j]<<4;
            }
            buf[j]+=v;
            i++;
        }
    }
    uint32_t* bd1=(uint32_t*)buf;
    Data1=*bd1;
    std::array<uint16_t,3>* bd2=(std::array<uint16_t,3>*)(buf+sizeof(Data1));
    Data2=*bd2;
    std::array<uint8_t,6>* bd3=(std::array<uint8_t,6>*)(buf+sizeof(Data1)+sizeof(Data2));
    Data3=*bd3;
}
// ..........................................................
bool Uuid::operator==(const Smp::Uuid& other) const {
    return Data1==other.Data1
            && Data2[0]==other.Data2[0]
            && Data2[1]==other.Data2[1]
            && Data2[2]==other.Data2[2]
            && Data3[0]==other.Data3[0]
            && Data3[1]==other.Data3[1]
            && Data3[2]==other.Data3[2]
            && Data3[3]==other.Data3[3]
            && Data3[4]==other.Data3[4]
            && Data3[5]==other.Data3[5];
}
// ..........................................................
bool Uuid::operator!=(const Smp::Uuid& other) const {
    return !operator==(other);
}
// ..........................................................
bool Uuid::operator<(const Smp::Uuid& other) const {
    // TODO check endianness effect of uint16 && uint32 for such
    // comparaison...
    return Data1<other.Data1 ||
        (Data1==other.Data1 && (Data2[0]<other.Data2[0] ||
         (Data2[0]==other.Data2[0] && (Data2[1]<other.Data2[1] ||
          (Data2[1]==other.Data2[1] && (Data2[2]<other.Data2[2] ||
           (Data2[2]==other.Data2[2] && (Data3[0]<other.Data3[0] ||
            (Data3[0]==other.Data3[0] && (Data3[1]<other.Data3[1] ||
             (Data3[1]==other.Data3[1] && (Data3[2]<other.Data3[3] ||
              (Data3[2]==other.Data3[2] && (Data3[3]<other.Data3[3] ||
               (Data3[3]==other.Data3[3] && (Data3[4]<other.Data3[4] ||
                (Data3[4]==other.Data3[4] && (Data3[5]<other.Data3[5]
             ))))))))))))))))));
}
// ..........................................................
}
