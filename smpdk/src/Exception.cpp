/*
 * @file Exception.cpp
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simph/smpdk/Exception.hpp"
#include <sstream>

namespace simph {
namespace smpdk {
// --------------------------------------------------------------------
// ..........................................................
Exception::Exception() : _name(), _description(), _sender(nullptr), _msg() {}
// ..........................................................
Exception::~Exception() {}
// --------------------------------------------------------------------
// ..........................................................
Smp::String8 Exception::what() const noexcept {
    return _msg.c_str();
}
// ..........................................................
Smp::String8 Exception::GetName() const noexcept {
    return _name.c_str();
}
// ..........................................................
Smp::String8 Exception::GetDescription() const noexcept {
    return _description.c_str();
}
// ..........................................................
void Exception::setName(Smp::String8 name) {
    _name = name;
}
// ..........................................................
void Exception::setDescription(Smp::String8 description) {
    _description = description;
}
// ..........................................................
void Exception::setSender(const Smp::IObject* sender) {
    _sender = sender;
}
// ..........................................................
void Exception::setMessage(Smp::String8 msg) {
    if (msg == nullptr && _sender != nullptr) {
        std::ostringstream s;
        s << "Exception " << _name << " from " << _sender->GetName() << ": " << _description << std::ends;
        _msg = s.str();
    }
    else {
        _msg = msg;
    }
}
// ..........................................................
const Smp::IObject* Exception::GetSender() const noexcept {
    return _sender;
}
// ..........................................................
Smp::String8 Exception::GetMessage() const noexcept {
    return _msg.c_str();
}

}  // namespace smpdk
}  // namespace simph
