#include "Smp/Uuid.h"

namespace Smp {
// --------------------------------------------------------------------
// ..........................................................
Uuid::Uuid(const char * value) {
// TODO
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
// ..........................................................
// ..........................................................
// ..........................................................
// ..........................................................
// ..........................................................
// ..........................................................
// ..........................................................
// ..........................................................
// ..........................................................
}
