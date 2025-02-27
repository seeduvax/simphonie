#include "Smp/Uuid.h"
#include <cstring>
#include <iomanip>
#include <sstream>
#include "Smp/PrimitiveTypes.h"

#include "simdeck/endian.h"
#include "simdeck/Exception.hpp"

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
        throw simdeck::Exception(nullptr,  msg.c_str());
    }

    // ntoh32 : network to host, equivalent to be32toh
    Data1 = ntoh32(Data1);
    Data2[0] = ntoh16(Data2[0]);
    Data2[1] = ntoh16(Data2[1]);
    Data2[2] = ntoh16(Data2[2]);
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
    return Data1 < other.Data1 || (Data1 == other.Data1 && (Data2 < other.Data2 || (Data2 == other.Data2 && Data3 < other.Data3)));
}
// ..........................................................
std::ostream& operator<<(std::ostream& os, const Smp::Uuid& uuid) {
    os << std::hex << std::uppercase << std::setfill('0') << std::setw(8) << uuid.Data1 
       << "-" << std::setw(4) << uuid.Data2[0] 
       << "-" << std::setw(4) << uuid.Data2[1] 
       << "-" << std::setw(4) << uuid.Data2[2] 
       << "-" << std::setw(2) << uint16_t(uuid.Data3[0])
              << std::setw(2) << uint16_t(uuid.Data3[1])
              << std::setw(2) << uint16_t(uuid.Data3[2])
              << std::setw(2) << uint16_t(uuid.Data3[3]) 
              << std::setw(2) << uint16_t(uuid.Data3[4]) 
              << std::setw(2) << uint16_t(uuid.Data3[5])
              << std::nouppercase << std::dec;
    return os;
}
}  // namespace Smp
