/*
 * @file StorageReader.hpp
 *
 * Copyright 2025. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simphonie_kern_StorageReader_HPP__
#define __simphonie_kern_StorageReader_HPP__

#include <fstream>
#include <vector>
#include "Smp/ISimulator.h"
#include "Smp/IStorageReader.h"
#include "Smp/Services/IResolver.h"
#include "simdeck/Object.hpp"

namespace simphonie {
namespace kern {

class StorageReader : public simdeck::Object, virtual public Smp::IStorageReader {
public:
    StorageReader(Smp::ISimulator* sim, Smp::String8 filePath, Smp::String8 name, Smp::String8 descr = "",
                  Smp::IObject* parent = nullptr);
    ~StorageReader() = default;

    void restore();

    void Restore(Smp::Void* address, Smp::UInt64 size) override;
    Smp::String8 GetStateVectorFileName() const override;
    Smp::String8 GetStateVectorFilePath() const override;

private:
    enum _State {
        NEXT_IS_CONT,

        CONT_NAME,
        CONT_EQUAL,
        CONT_LBRACE,
        CONT_CONTENT,

        NEXT_IS_COMP,

        COMP_NAME,
        COMP_EQUAL,
        COMP_LBRACE,

        FIELDS_F,
        FIELDS_i,
        FIELDS_e,
        FIELDS_l,
        FIELDS_d,
        FIELDS_s,
        FIELDS_EQUAL,
        FIELDS_LBRACE,
        FIELDS_CONTENT,

        FIELD_NAME,
        FIELD_EQUAL,
        FIELD_CONTENT,
        FIELD_VALUE
    };

    struct _Container;
    struct _Component {
        struct _Field {
            std::string name, value;
        };
        std::string name;
        std::vector<_Field> fields;
        std::vector<_Container> containers;
    };

    struct _Container {
        std::string name;
        std::vector<_Component> components;
    };

    static inline uint8_t _getHexPos(char c) {
        return (c <= '9') ? c - '0' : c - 'a' + 10;
    }
    static inline bool _isSkipChar(char c) {
        return c == ' ' || c == '\n' || c == '\t' || c == '\r';
    }
    static inline bool _isNameChar(char c) {
        return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
    }
    static bool _isValueChar(char c) {
        return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f');
    }
    std::vector<_Container> _parse();
    void _restoreComponent(Smp::IContainer* parent, const _Component& comp);
    void _restoreContainer(Smp::IComposite* parent, const _Container& cont);

    Smp::ISimulator* _sim;
    std::ifstream _in;
    std::string _filePath;
    std::string _subFilePath;
};

} /* namespace kern */
} /* namespace simphonie */
#endif /* __simphonie_kern_StorageReader_HPP__ */
