/*
 * @file ArrayField.cpp
 *
 * Copyright 2025 Sebastien Devaux. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simdeck/ArrayField.hpp"
#include "simdeck/SimpleArrayField.hpp"
#include "simdeck/SimpleField.hpp"

namespace simdeck {
/*
// --------------------------------------------------------------------
// ..........................................................
ArrayField::ArrayField() {
}
// ..........................................................
ArrayField::~ArrayField() {
}
*/
// --------------------------------------------------------------------
// toprint operator
// ..........................................................
// toprint Smp::IArrayField
std::ostream& toprint(std::ostream& os, const Smp::IArrayField& obj) {
    os << "[ ";
    for (Smp::UInt64 i = 0; i < obj.GetSize(); i++) {
        if (dynamic_cast<Smp::IArrayField*>(obj.GetItem(i)) != nullptr) {
            toprint(os, *(dynamic_cast<Smp::IArrayField*>(obj.GetItem(i))));
        }
        else if (dynamic_cast<Smp::ISimpleArrayField*>(obj.GetItem(i)) != nullptr) {
            toprint(os, *(dynamic_cast<Smp::ISimpleArrayField*>(obj.GetItem(i))));
        }
        else {
            auto field = dynamic_cast<Smp::ISimpleField*>(obj.GetItem(i));
            toprint(os, *field);
        }

        if (i < obj.GetSize() - 1) {
            os << " ; ";
        }
        else {
            os << " ]";
        }
    }
    return os;
}

} // namespace simdeck
