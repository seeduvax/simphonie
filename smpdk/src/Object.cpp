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
#include "Smp/IComponent.h"
#include "Smp/IEntryPointPublisher.h"
#include "Smp/IComposite.h"
#include "Smp/IContainer.h"
#include "Smp/IDynamicInvocation.h"

#include <iostream>

namespace simph {
namespace smpdk {

// --------------------------------------------------------------------
// ..........................................................
Object::Object(Smp::String8 name, Smp::String8 descr, Smp::IObject* parent)
    : _name(name), _description(descr), _parent(parent) {
    checkName(name);
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
Smp::IObject* Object::resolveChild(Smp::String8 path, const Smp::IObject* from) {
    // See path parsing state diagram in definition file (ref 1e2889e) 
    // subsection "Path parsing" somewhere in "Design" section.
    // (fig resolver_path_parse_state.png)
    const Smp::IObject* obj=from;
    enum State {
        Init,
        Sep,
        Self,
        Parent,
        Name,
        Index,
        EndIndex,
        DotSep,
        End 
    };
    State state=State::Init;
    int i=0;
    std::string name="";
    while (state!=State::End) {
        char c=path[i];
        switch (state) {
            case State::Init: {
                    switch (c) {
                        case '/':
                            state=State::Sep;
                            break;
                        case '.':
                            state=State::Self;
                            break;
                        default:
                            // don't really care char validity, since this
                            // should lead to not found child and finally 
                            // returning nullptr.
                            // This will be the same for many case of switching
                            // to Name state and won't be repeated further
                            name.append(1,c);
                            state=State::Name;
                            break;
                    }
                }
                break;
            case State::Sep: {
                    switch (c) {
                        case '/':
                            // Ignore succesive /
                            break;
                        case '.':
                            state=State::Self;
                            break;
                        default:
                            name.append(1,c);
                            state=State::Name;
                            break;
                    }
                }
                break;
            case State::Self: {
                    switch (c) {
                        case '/':
                            state=State::Sep;
                            break;
                        case '.':
                            state=State::Parent;
                            break;
                        default:
                            name.append(1,c);
                            state=State::Name;
                    }
                }
                break;
            case State::Parent: {
                    switch(c) {
                        case '/':
                            obj=obj->GetParent();
                            state=State::Sep;
                            break;
                        case '\0':
                            obj=obj->GetParent();
                            state=State::End;
                            break;
                        default:
                            obj=nullptr;
                            state=State::End;
                    }
                }
                break;
            case State::Name: {
                    switch (c) {
                        case '/':
                            obj=obj->GetChild(name.c_str());
                            name="";
                            state=State::Sep;
                            break;
                        case '[':
                            obj=obj->GetChild(name.c_str());
                            name="[";
                            state=State::Index;
                            break;
                        case '.':
                            obj=obj->GetChild(name.c_str());
                            name="";
                            state=State::DotSep;
                            break;
                        case '\0':
                            obj=obj->GetChild(name.c_str());
                            state=State::End;
                            break;
                        default:
                            name.append(1,c);
                            break;
                    }
                }
                break;
            case State::Index: {
                    switch (c) {
                        case ']':
                            name.append(1,c);
                            obj=obj->GetChild(name.c_str());
                            name="";
                            state=State::EndIndex;
                            break;
                        default:
                            name.append(1,c);
                            break;
                    }
                }
                break;
            case State::EndIndex: {
                    switch (c) {
                        case '/':
                            state=State::Sep;
                            break;
                        case '.':
                            state=State::DotSep;
                            break;
                        case '[':
                            state=State::Index;
                            break;
                        case '\0':
                            state=State::End;
                            break;
                        default:
                            // parse error
                            obj=nullptr;
                            break;
                    }
                }
                break;
            case State::DotSep: {
                    switch (c) {
                        case '.':
                        case '/':
                            // parse error
                            obj=nullptr;
                            break;
                        default:
                            name.append(1,c);
                            state=State::Name;
                    }
                }
                break;
        }
        if (c=='\0' || obj==nullptr) {
            // force final state if end of string is reached or at some level
            // any of GetParent() or GetChild() returned nullptr.
            state=State::End;
        }
        else {
            // jump to next char.
            i++;
        }
    }
    // even travelling the object hierchy does not need anything else than
    // const object. Requester of object may seek for non const object.
    // Then cast to non const is needed.
    return (Smp::IObject*)obj;
}
// ..........................................................
Smp::IObject* Object::GetChild(Smp::String8 name) const {
    // quite strange to play with dynamic cast like that, but
    // the only way to deal properly with possible virtual multiple
    // inheritences.
    Smp::IObject* res=nullptr;
    {
        auto o=dynamic_cast<const Smp::IComponent*>(this);
        if (o!=nullptr) {
            res=o->GetFields()->at(name);
        }
    }
    if (res==nullptr) {
        auto o=dynamic_cast<const Smp::IEntryPointPublisher*>(this);
        if (o!=nullptr) {
            res=o->GetEntryPoint(name);
        }
    }
    if (res==nullptr) {
        auto o=dynamic_cast<const Smp::IComposite*>(this);
        if (o!=nullptr) {
            for (auto ctnr: *(o->GetContainers())) {
                if (res==nullptr) {
                    res=ctnr->GetComponent(name);
                }
            }
        }
    }
    if (res==nullptr) {
        auto o=dynamic_cast<const Smp::IContainer*>(this);
        if (o!=nullptr) {
            res=o->GetComponent(name);
        }
    }
    if (res==nullptr) {
        auto o=dynamic_cast<const Smp::IDynamicInvocation*>(this);
        if (o!=nullptr) {
            res=o->GetProperty(name);
            if (res==nullptr) {
                res=o->GetOperation(name);
            }
        }
    }
    return res;
}

// --------------------------------------------------------------------
// ..........................................................
class ExInvalidNameBadFormat: public ExInvalidObjectName {
public:
    ExInvalidNameBadFormat(Smp::IObject* sender, Smp::String8 invalidName):
            ExInvalidObjectName(sender, invalidName) {
        std::ostringstream d;
        d << "'" << invalidName << "' invalid name format, bad char or does not start with a letter.";
        setDescription(d.str().c_str());
        setMessage();
    }
    virtual ~ExInvalidNameBadFormat() {
    }
};
class ExInvalidNameCKeyword: public ExInvalidObjectName {
public:
    ExInvalidNameCKeyword(Smp::IObject* sender, Smp::String8 invalidName):
            ExInvalidObjectName(sender, invalidName) {
        std::ostringstream d;
        d << "'" << invalidName << "' is a C++/ANSI keyword that can't be used as a SMP object name.";
        setDescription(d.str().c_str());
        setMessage();
    }
    virtual ~ExInvalidNameCKeyword() {
    }
};
// ..........................................................
void Object::checkName(Smp::String8 name) {
    std::string n=name;
    // Check for non empty alphanumeric names.
    // '_', '[' and ']' are also valid in names
    if (!std::regex_match(n, std::regex("[a-zA-Z][a-zA-Z0-9_\\[\\]]*"))) {
        throw ExInvalidNameBadFormat(this, name);
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
            throw ExInvalidNameCKeyword(this, name);
        }
    }
}

}  // namespace smpdk
}  // namespace simph
