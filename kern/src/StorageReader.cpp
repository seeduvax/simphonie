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
#define FIELDS(token, to)  \
    s += c;                \
    if (c == token) {      \
        state = to;        \
        break;             \
    }                      \
    if (_isNameChar(c)) {  \
        state = CONT_NAME; \
        break;             \
    }
#define WAIT_TOKEN(token, to) \
    if (c == token) {         \
        state = to;           \
        break;                \
    }                         \
    SKIP_CHARS
#define LOOP_NAME         \
    if (_isNameChar(c)) { \
        s += c;           \
        break;            \
    }

namespace simphonie {
namespace kern {

StorageReader::StorageReader(Smp::ISimulator* sim, Smp::String8 filePath, Smp::String8 name, Smp::String8 descr,
                             Smp::IObject* parent)
    : simdeck::Object(name, descr, parent), _sim(sim), _in(filePath), _filePath(filePath) {
    _subFilePath = _filePath + ".d";
}

void StorageReader::Restore(Smp::Void* address, Smp::UInt64 size) {
    uint8_t* buf = (uint8_t*)address;
    __asm("int $3");
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

std::vector<StorageReader::_Container> StorageReader::_parse() {
    _Component root;
    std::vector<_Component*> parents = {&root};
    _State state = NEXT_IS_CONT;
    std::string s = "";

    size_t iLine = 1;

    while (true) {
        char c;
        _in.get(c);

        if (c == '\n') {
            iLine++;
        }

        switch (state) {
            case NEXT_IS_CONT:
                if (_in.eof()) {
                    return root.containers;
                }
                if (_isNameChar(c)) {
                    s += c;
                    state = CONT_NAME;
                    break;
                }
                if (c == '}') {
                    parents.pop_back();
                    state = NEXT_IS_COMP;
                    break;
                }
                SKIP_CHARS
            case CONT_NAME:
                LOOP_NAME
                if (_isSkipChar(c)) {
                    parents.back()->containers.push_back(_Container());
                    parents.back()->containers.back().name = s;
                    s = "";
                    state = CONT_EQUAL;
                    break;
                }
            case CONT_EQUAL:
                WAIT_TOKEN('=', CONT_LBRACE)
            case CONT_LBRACE:
                WAIT_TOKEN('{', CONT_CONTENT)
            case CONT_CONTENT:
                if (c == '}') {
                    s = "";
                    state = NEXT_IS_CONT;
                    break;
                }
                if (_isNameChar(c)) {
                    s += c;
                    state = COMP_NAME;
                    break;
                }
                SKIP_CHARS
            case NEXT_IS_COMP:
                if (_isNameChar(c)) {
                    s += c;
                    state = COMP_NAME;
                    break;
                }
                if (c == '}') {
                    state = NEXT_IS_CONT;
                    break;
                }
                SKIP_CHARS
            case COMP_NAME:
                LOOP_NAME
                if (_isSkipChar(c)) {
                    parents.back()->containers.back().components.push_back(_Component());
                    parents.push_back(&parents.back()->containers.back().components.back());
                    parents.back()->name = s;
                    s = "";
                    state = COMP_EQUAL;
                    break;
                }
            case COMP_EQUAL:
                WAIT_TOKEN('=', COMP_LBRACE)
            case COMP_LBRACE:
                WAIT_TOKEN('{', FIELDS_F)
            case FIELDS_F:
                if (c == '}') {
                    s = "";
                    parents.pop_back();
                    state = CONT_CONTENT;
                    break;
                }
                if (c == 'F') {
                    state = FIELDS_i;
                    break;
                }
                if (_isNameChar(c)) {
                    s += c;
                    state = CONT_NAME;
                    break;
                }
                SKIP_CHARS
            case FIELDS_i:
                FIELDS('i', FIELDS_e)
            case FIELDS_e:
                FIELDS('e', FIELDS_l)
            case FIELDS_l:
                FIELDS('l', FIELDS_d)
            case FIELDS_d:
                FIELDS('d', FIELDS_s)
            case FIELDS_s:
                FIELDS('s', FIELDS_EQUAL)
            case FIELDS_EQUAL:
                if (c == '=') {
                    s = "";
                    state = FIELDS_LBRACE;
                    break;
                }
                SKIP_CHARS
            case FIELDS_LBRACE:
                WAIT_TOKEN('{', FIELDS_CONTENT)
            case FIELDS_CONTENT:
                if (c == '}') {
                    state = FIELDS_F;
                    break;
                }
                if (_isNameChar(c)) {
                    s += c;
                    state = FIELD_NAME;
                    break;
                }
                SKIP_CHARS
            case FIELD_NAME:
                LOOP_NAME
                if (_isSkipChar(c)) {
                    parents.back()->fields.push_back(_Component::_Field());
                    parents.back()->fields.back().name = s;
                    s = "";
                    state = FIELD_EQUAL;
                    break;
                }
            case FIELD_EQUAL:
                WAIT_TOKEN('=', FIELD_CONTENT)
            case FIELD_CONTENT:
                if (_isValueChar(c)) {
                    s += c;
                    state = FIELD_VALUE;
                    break;
                }
                SKIP_CHARS
            case FIELD_VALUE:
                LOOP_NAME
                if (_isSkipChar(c)) {
                    parents.back()->fields.back().value = s;
                    s = "";
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
    const auto component = parent->GetComponent(comp.name.c_str());
    for (auto& fld : comp.fields) {
        auto field = dynamic_cast<Smp::IPersist*>(_sim->GetResolver()->ResolveRelative(fld.name.c_str(), component));
        if (!field) {
            std::ostringstream oss;
            oss << "Field '" << fld.name << "' of component '" << comp.name << "' is not valid" << std::endl;
            throw simdeck::Exception(this, oss.str().c_str());
        }
        field->Restore(this);
    }
    for (auto& cont : comp.containers) {
        const auto composite = dynamic_cast<Smp::IComposite*>(component);
        _restoreContainer(composite, cont);
    }
}

void StorageReader::_restoreContainer(Smp::IComposite* parent, const _Container& cont) {
    const auto container = parent->GetContainer(cont.name.c_str());
    for (auto& comp : cont.components) {
        _restoreComponent(container, comp);
    }
}

void StorageReader::restore() {
    const auto containers = _parse();
    const auto composite = dynamic_cast<Smp::IComposite*>(_sim);
    for (const auto cont : containers) {
        _restoreContainer(composite, cont);
    }
}

} /* namespace kern */
} /* namespace simphonie */
