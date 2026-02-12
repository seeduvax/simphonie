/*
 * @file FieldRecorderHDF5.hpp
 *
 * Copyright 2025 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simphonie_colibry_FieldRecorderHDF5_HPP__
#define __simphonie_colibry_FieldRecorderHDF5_HPP__

#include <vector>
#include "H5Cpp.h"
#include "Smp/Bool.h"
#include "Smp/ISimpleArrayField.h"
#include "Smp/ISimpleField.h"
#include "Smp/UInt64.h"
#include "simphonie/colibry/FieldRecorder.hpp"

namespace simphonie {
namespace colibry {

class FieldRecorderHDF5 : public FieldRecorder {
public:
    FieldRecorderHDF5(Smp::String8 name, Smp::String8 description = "", Smp::IObject* parent = nullptr);
    ~FieldRecorderHDF5();
    void step() override;
    void init() override;

protected:
    void flush() override;
    void disconnect() override;

private:
    struct _field {
        inline _field(const Smp::IField* f, const Smp::Bool arr, std::vector<hsize_t> ds, const H5::PredType h5ty,
                      const Smp::PrimitiveTypeKind smptype)
            : field(f), isArray(arr), dims(ds), h5Type(h5ty), smpType(smptype) {}

        union {
            const Smp::IField* field;
            const Smp::ISimpleField* simplefield;
            const Smp::ISimpleArrayField* simplearrayfield;
        };
        const Smp::Bool isArray;
        std::vector<hsize_t> dims;
        const H5::PredType h5Type;
        const Smp::PrimitiveTypeKind smpType;
        H5::DataSet dataset;
    };

    H5::PredType _SmpType2H5Type(Smp::PrimitiveTypeKind type);
    void addField(const char* name, Smp::IField* field = nullptr);
    H5::H5File _file;
    std::vector<_field> _fields;
};

} /* namespace colibry */
} /* namespace simphonie */
#endif /* __simphonie_colibry_FieldRecorderHDF5_HPP__ */
