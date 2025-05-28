/*
 * @file StorageReader.cpp
 *
 * Copyright 2025. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simphonie/kern/StorageReader.hpp"
#include <iostream>
#include <sstream>
#include "simdeck/Exception.hpp"

#define SKIP_CHARS      \
    if (_isSkipChar(c)) \
        break;
#define CASE_FIELDS(token, from, to)                                                 \
    case from:                                                                       \
        if (c == token) {                                                            \
            state = to;                                                              \
            break;                                                                   \
        }                                                                            \
        if (_isNameChar(c)) {                                                        \
            parents.back()->components.back().containers.push_back(_Container());    \
            parents.push_back(&parents.back()->components.back().containers.back()); \
            parents.back()->name += c;                                               \
            state = CONT_NAME;                                                       \
            break;                                                                   \
        }
#define CASE_SKIP(token, from, to) \
    case from:                     \
        if (c == token) {          \
            state = to;            \
            break;                 \
        }                          \
        SKIP_CHARS

namespace simphonie {
namespace kern {

StorageReader::StorageReader(Smp::ISimulator* sim, Smp::String8 filePath, Smp::String8 name, Smp::String8 descr,
                             Smp::IObject* parent)
    : simdeck::Object(name, descr, parent), _sim(sim), _in(filePath), _filePath(filePath) {
    _subFilePath = _filePath + ".d";
}

void StorageReader::Restore(Smp::Void* address, Smp::UInt64 size) {
    uint8_t* buf = (uint8_t*)address;
    for (int i = 0; i < size; i++) {
        char highChar, lowChar;
        _in >> highChar >> lowChar;
        const uint8_t high = _getHexPos(highChar);
        const uint8_t low = _getHexPos(lowChar);
        buf[i] = (high << 4) | low;
    }
}

Smp::String8 StorageReader::GetStateVectorFileName() const {
    return _filePath.c_str();
}

Smp::String8 StorageReader::GetStateVectorFilePath() const {
    return _subFilePath.c_str();
}

void StorageReader::connect() {
    _resolver = _sim->GetResolver();
}

std::vector<StorageReader::_Container> StorageReader::_parse() {
    std::vector<_Container> root;
    std::vector<_Container*> parents;
    _State state = ENTRY;

    size_t iLine = 1;

    while (true) {
        char c;
        _in.get(c);

        if (c == '\n') {
            iLine++;
        }

        switch (state) {
            case ENTRY:
                if (_in.eof()) {
                    return root;
                }
                if (_isNameChar(c)) {
                    if (parents.size() == 0) {
                        root.push_back(_Container());
                        parents.push_back(&root.back());
                    }
                    else {
                        parents.back()->components.back().containers.push_back(_Container());
                        parents.push_back(&parents.back()->components.back().containers.back());
                    }
                    parents.back()->name += c;
                    state = CONT_NAME;
                    break;
                }
                SKIP_CHARS
            case CONT_NAME:
                if (_isNameChar(c)) {
                    parents.back()->name += c;
                    break;
                }
                if (_isSkipChar(c)) {
                    state = CONT_EQUAL;
                    break;
                }
                CASE_SKIP('=', CONT_EQUAL, CONT_LBRACE)
                CASE_SKIP('{', CONT_LBRACE, CONT_CONTENT)
            case CONT_CONTENT:
                if (c == '}') {
                    parents.pop_back();
                    if (parents.size() == 0) {
                        state = ENTRY;
                    }
                    else {
                        parents.back()->components.push_back(_Component());
                        state = COMP_NAME;
                    }
                    break;
                }
                if (_isNameChar(c)) {
                    parents.back()->components.push_back(_Component());
                    parents.back()->components.back().name += c;
                    state = COMP_NAME;
                    break;
                }
                SKIP_CHARS
            case COMP_NAME:
                if (_isNameChar(c)) {
                    parents.back()->components.back().name += c;
                    break;
                }
                if (_isSkipChar(c)) {
                    state = COMP_EQUAL;
                    break;
                }
                CASE_SKIP('=', COMP_EQUAL, COMP_LBRACE)
                CASE_SKIP('{', COMP_LBRACE, FIELDS_F)
            case FIELDS_F:
                if (c == '}') {
                    state = CONT_CONTENT;
                    break;
                }
                if (c == 'F') {
                    state = FIELDS_i;
                    break;
                }
                if (_isNameChar(c)) {
                    parents.back()->components.back().containers.push_back(_Container());
                    parents.push_back(&parents.back()->components.back().containers.back());
                    parents.back()->name += c;
                    state = CONT_NAME;
                    break;
                }
                SKIP_CHARS
                CASE_FIELDS('i', FIELDS_i, FIELDS_e)
                CASE_FIELDS('e', FIELDS_e, FIELDS_l)
                CASE_FIELDS('l', FIELDS_l, FIELDS_d)
                CASE_FIELDS('d', FIELDS_d, FIELDS_s)
                CASE_FIELDS('s', FIELDS_s, FIELDS_EQUAL)
                CASE_SKIP('=', FIELDS_EQUAL, FIELDS_LBRACE)
                CASE_SKIP('{', FIELDS_LBRACE, FIELDS_CONTENT)
            case FIELDS_CONTENT:
                if (c == '}') {
                    state = FIELDS_F;
                    break;
                }
                if (_isNameChar(c)) {
                    parents.back()->components.back().fields.push_back(_Component::_Field());
                    parents.back()->components.back().fields.back().name += c;
                    state = FIELD_NAME;
                    break;
                }
                SKIP_CHARS
            case FIELD_NAME:
                if (_isNameChar(c)) {
                    parents.back()->components.back().fields.back().name += c;
                    break;
                }
                if (_isSkipChar(c)) {
                    state = FIELD_EQUAL;
                    break;
                }
                CASE_SKIP('=', FIELD_EQUAL, FIELD_CONTENT)
            case FIELD_CONTENT:
                if (_isValueChar(c)) {
                    parents.back()->components.back().fields.back().value += c;
                    state = FIELD_VALUE;
                    break;
                }
                SKIP_CHARS
            case FIELD_VALUE:
                if (_isValueChar(c)) {
                    parents.back()->components.back().fields.back().value += c;
                    break;
                }
                if (_isSkipChar(c)) {
                    state = FIELDS_CONTENT;
                    break;
                }
            default: {
                std::streampos pos = 0;
                /* look for the beggining of the line */
                if (_in.tellg() > 0) {
                    _in.seekg(_in.tellg() - 1l);
                }
                while (_in.tellg() > 0 && _in.peek() != '\n') {
                    _in.seekg(_in.tellg() - 1l);
                    pos += 1l;
                }
                if (_in.peek() == '\n') {
                    _in.seekg(_in.tellg() + 1l);
                    pos -= 1l;
                }
                std::string line;
                std::getline(_in, line);
                std::ostringstream oss;
                oss << "Unexpected character '" << c << "' (" << GetStateVectorFileName() << ":" << iLine << ":" << pos
                    << "):" << std::endl
                    << line;
                throw simdeck::Exception(this, oss.str().c_str());
            }
        }
    }
}

void StorageReader::_restoreComponent(Smp::IContainer* parent, const _Component& comp) {
    std::cout << "COMP " << comp.name << std::endl;
    auto component = parent->GetComponent(comp.name.c_str());
    // for (auto& fld : comp.fields) {
    //     auto field = dynamic_cast<Smp::IPersist*>(_resolver
    //         ->ResolveRelative(fld.name.c_str(), component));
    //     if (!field) {
    //         std::ostringstream oss;
    //         oss << "Field '" << fld.name << "' of component '" << comp.name
    //             << "' is not valid" << std::endl;
    //         throw simdeck::Exception(this, oss.str().c_str());
    //     }
    //     field->Restore(this);
    // }
    for (auto& cont : comp.containers) {
        auto container = _sim->GetContainer(cont.name.c_str());
        _restoreContainer(container, cont);
    }
}

void StorageReader::_restoreContainer(Smp::IContainer* parent, const _Container& cont) {
    std::cout << "CONT " << cont.name << std::endl;
    for (auto& comp : cont.components) {
        _restoreComponent(parent, comp);
    }
}

void StorageReader::restore() {
    const std::vector<_Container> containers = _parse();

    for (auto& cont : containers) {
        auto container = _sim->GetContainer(cont.name.c_str());
        _restoreContainer(container, cont);
    }
}

} /* namespace kern */
} /* namespace simphonie */
