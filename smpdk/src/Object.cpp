/*
 * @file Object.cpp
 *
 * Copyright 2019 S.Devaux . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simph/smpdk/Object.hpp"
#include <regex>
#include "simph/smpdk/ExInvalidObjectName.hpp"

namespace simph {
namespace smpdk {

// --------------------------------------------------------------------
// ..........................................................
Object::Object(Smp::String8 name, Smp::String8 descr, Smp::IObject* parent)
    : _name(name), _description(descr), _parent(parent) {
    if (!checkName(name)) {
        throw ExInvalidObjectName(this, name);
    }
}
// ..........................................................
Object::~Object() {}
// --------------------------------------------------------------------
// ..........................................................
Smp::String8 Object::GetName() const {
    return _name.c_str();
}
// ..........................................................
Smp::String8 Object::GetDescription() const {
    return _description.c_str();
}
// ..........................................................
Smp::IObject* Object::GetParent() const {
    return _parent;
}
// ..........................................................
void Object::setName(Smp::String8 name) {
    if (!checkName(name)) {
        throw ExInvalidObjectName(this, name);
    }
    _name = name;
}
// ..........................................................
void Object::setDescription(Smp::String8 description) {
    _description = description;
}
// ..........................................................
void Object::setParent(Smp::IObject* parent) {
    _parent = parent;
}
// ..........................................................
bool Object::checkName(Smp::String8 name) {
    std::string n = name;
    // Check for non empty alphanumeric names.
    // '_', '[' and ']' are also valid in names
    if (!std::regex_match(n, std::regex("[a-zA-Z][a-zA-Z0-9_\\[\\]]*"))) {
        return false;
    }
    // ISO/ANSI C++ keywords are not valid (see ECSS SMP 5.2.1.a.1.d).
    // keyword list fetched 2024-12-11 from https://en.cppreference.com/w/cpp/keyword
    static const std::vector<std::string> forbidden = {
        "alignas",
        "alignof",
        "and",
        "and_eq",
        "asm",
        "atomic_cancel",
        "atomic_commit",
        "atomic_noexcept",
        "auto",
        "bitand",
        "bitor",
        "bool",
        "break",
        "case",
        "catch",
        "char",
        "char8_t",
        "char16_t",
        "char32_t",
        "class",
        "compl",
        "concept",
        "const",
        "consteval",
        "constexpr",
        "constinit",
        "const_cast",
        "continue",
        "co_await",
        "co_return",
        "co_yield",
        "decltype",
        "default",
        "delete",
        "do",
        "double",
        "dynamic_cast",
        "else",
        "enum",
        "explicit",
        "export",
        "extern",
        "false",
        "float",
        "for",
        "friend",
        "goto",
        "if",
        "inline",
        "int",
        "long",
        "mutable",
        "namespace",
        "new",
        "noexcept",
        "not",
        "not_eq",
        "nullptr",
        "operator",
        "or",
        "or_eq",
        "private",
        "protected",
        "public",
        "reflexpr",
        "register",
        "reinterpret_cast",
        "requires",
        "return",
        "short",
        "signed",
        "sizeof",
        "static",
        "static_assert",
        "static_cast",
        "struct",
        "switch",
        "synchronized",
        "template",
        "this",
        "thread_local",
        "throw",
        "true",
        "try",
        "typedef",
        "typeid",
        "typename",
        "union",
        "unsigned",
        "using",
        "virtual",
        "void",
        "volatile",
        "wchar_t",
        "while",
        "xor",
        "xor_eq"
    };
    for (auto kw : forbidden) {
        if (kw == n) {
            return false;
        }
    }
    return true;
}

}  // namespace smpdk
}  // namespace simph
