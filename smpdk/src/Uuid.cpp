#include "Smp/Uuid.h"
#include <cstring>
#include <iomanip>
#include <sstream>
#include "Smp/PrimitiveTypes.h"

#include "simph/smpdk/endian.h"
#include "simph/smpdk/Exception.hpp"

namespace Smp {
// --------------------------------------------------------------------
// ..........................................................
Uuid::Uuid(const char* value) {
    Data1 = 0;
    Data2.fill(0);
    Data3.fill(0);

    size_t bSize = sizeof(Uuid);
    uint8_t* buf = reinterpret_cast<uint8_t*>(this);
    const char* ptr = value;
    int i = 0;
    while (*ptr != '\0' && i < 2 * bSize) {
        int v = -1;
        if (*ptr >= 'A' && *ptr <= 'F') {
            v = *ptr - 'A' + 0x0A;
        }
        else if (*ptr >= 'a' && *ptr <= 'f') {
            v = *ptr - 'a' + 0x0A;
        }
        else if (*ptr >= '0' && *ptr <= '9') {
            v = *ptr - '0';
        }
        ptr++;
        if (v >= 0) {
            int j = i >> 1;
            if (i % 2 != 0) {
                buf[j] = buf[j] << 4;
            }
            buf[j] += v;
            i++;
        }
    }
    if (i != 2 * bSize) {
        std::string msg =  "Cannot parse UUID " + std::string(value);
        throw simph::smpdk::Exception(nullptr,  msg.c_str());
    }

    Data1 = be32toh(Data1);
    Data2[0] = be16toh(Data2[0]);
    Data2[1] = be16toh(Data2[1]);
    Data2[2] = be16toh(Data2[2]);
}
// ..........................................................
bool Uuid::operator==(const Smp::Uuid& other) const {
    return Data1 == other.Data1 && Data2 == other.Data2 && Data3 == other.Data3;
}
// ..........................................................
bool Uuid::operator!=(const Smp::Uuid& other) const {
    return !operator==(other);
}
// ..........................................................
bool Uuid::operator<(const Smp::Uuid& other) const {
    return Data1 < other.Data1
           || (Data1 == other.Data1
               && (Data2[0] < other.Data2[0]
                   || (Data2[0] == other.Data2[0]
                       && (Data2[1] < other.Data2[1]
                           || (Data2[1] == other.Data2[1]
                               && (Data2[2] < other.Data2[2]
                                   || (Data2[2] == other.Data2[2]
                                       && (Data3[0] < other.Data3[0]
                                           || (Data3[0] == other.Data3[0]
                                               && (Data3[1] < other.Data3[1]
                                                   || (Data3[1] == other.Data3[1]
                                                       && (Data3[2] < other.Data3[3]
                                                           || (Data3[2] == other.Data3[2]
                                                               && (Data3[3] < other.Data3[3]
                                                                   || (Data3[3] == other.Data3[3]
                                                                       && (Data3[4] < other.Data3[4]
                                                                           || (Data3[4] == other.Data3[4]
                                                                               && (Data3[5]
                                                                                   < other.Data3[5]))))))))))))))))));
}
// ..........................................................
std::ostream& operator<<(std::ostream& os, const Smp::Uuid& uuid) {
    os << std::hex << std::setfill('0') << std::setw(8) << uuid.Data1 
       << "-" << std::setw(4) << uuid.Data2[0] 
       << "-" << uuid.Data2[1] 
       << "-" << uuid.Data2[2] 
       << "-" << std::setw(2) 
              << (int)uuid.Data3[0] << (int)uuid.Data3[1] << (int)uuid.Data3[2]
              << (int)uuid.Data3[3] << (int)uuid.Data3[4] << (int)uuid.Data3[5]
              << std::dec ;
    return os;
}
}  // namespace Smp
