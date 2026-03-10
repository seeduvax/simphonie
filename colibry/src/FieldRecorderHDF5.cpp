/*
 * @file FieldRecorderHDF5.cpp
 *
 * Copyright 2025 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simphonie/colibry/FieldRecorderHDF5.hpp"
#include "Smp/ISimpleArrayField.h"
#include "Smp/ISimpleField.h"
#include "Smp/ISimulator.h"
#include "Smp/Publication/IType.h"
#include "Smp/Services/ITimeKeeper.h"
#include "simdeck/ExInvalidPrimitiveType.hpp"

namespace simphonie {
namespace colibry {

FieldRecorderHDF5::FieldRecorderHDF5(Smp::String8 name, Smp::String8 description, Smp::IObject* parent)
    : FieldRecorder(name, description, parent, "h5") {
}

FieldRecorderHDF5::~FieldRecorderHDF5() {
    disconnect();
}

H5::PredType FieldRecorderHDF5::_SmpType2H5Type(Smp::PrimitiveTypeKind type) {
    switch (type) {
        case Smp::PrimitiveTypeKind::PTK_Char8:
            return H5::PredType::NATIVE_INT8;
            break;
        case Smp::PrimitiveTypeKind::PTK_Bool:
            return H5::PredType::NATIVE_HBOOL;
            break;
        case Smp::PrimitiveTypeKind::PTK_Int8:
            return H5::PredType::NATIVE_INT8;
            break;
        case Smp::PrimitiveTypeKind::PTK_UInt8:
            return H5::PredType::NATIVE_UINT8;
            break;
        case Smp::PrimitiveTypeKind::PTK_Int16:
            return H5::PredType::NATIVE_INT16;
            break;
        case Smp::PrimitiveTypeKind::PTK_UInt16:
            return H5::PredType::NATIVE_UINT16;
            break;
        case Smp::PrimitiveTypeKind::PTK_Int32:
            return H5::PredType::NATIVE_INT32;
            break;
        case Smp::PrimitiveTypeKind::PTK_UInt32:
            return H5::PredType::NATIVE_UINT32;
            break;
        case Smp::PrimitiveTypeKind::PTK_Int64:
            return H5::PredType::NATIVE_INT64;
            break;
        case Smp::PrimitiveTypeKind::PTK_UInt64:
            return H5::PredType::NATIVE_UINT64;
            break;
        case Smp::PrimitiveTypeKind::PTK_Float32:
            return H5::PredType::NATIVE_FLOAT;
            break;
        case Smp::PrimitiveTypeKind::PTK_Float64:
            return H5::PredType::NATIVE_DOUBLE;
            break;
        case Smp::PrimitiveTypeKind::PTK_Duration:
            return H5::PredType::NATIVE_INT64;
            break;
        case Smp::PrimitiveTypeKind::PTK_DateTime:
            return H5::PredType::NATIVE_INT64;
            break;
        case Smp::PrimitiveTypeKind::PTK_String8:
        case Smp::PrimitiveTypeKind::PTK_None:
        default:
            throw simdeck::ExInvalidPrimitiveType(this, type);
            break;
    }
}

void FieldRecorderHDF5::addField(const char* name, Smp::IField* field) {
    hsize_t size = 1;
    Smp::Bool isArray = false;
    {
        const auto arrayfield = dynamic_cast<Smp::ISimpleArrayField*>(field);
        if (arrayfield) {
            size = arrayfield->GetSize();
            isArray = true;
        }
    }

    Smp::PrimitiveTypeKind smpType = Smp::PrimitiveTypeKind::PTK_Duration;
    if (field) {
        smpType = field->GetType()->GetPrimitiveTypeKind();
    }
    const H5::PredType h5Type = _SmpType2H5Type(smpType);

    const std::vector<hsize_t> dims = {size, 0}; /* to change later if dealing with matrix and so on */
    _fields.push_back(_field(field, isArray, dims, h5Type, smpType));
    std::vector<hsize_t> maxDims(_fields.back().dims);
    maxDims.back() = H5S_UNLIMITED;
    H5::DataSpace dataspace(_fields.back().dims.size(), _fields.back().dims.data(), maxDims.data());
    H5::DSetCreatPropList prop;
    {
        /* chunk ? */
        std::vector<hsize_t> chunkDims(_fields.back().dims);
        chunkDims.back() = 1;
        prop.setChunk(chunkDims.size(), chunkDims.data());
    }
    _fields.back().dataset = _file.createDataSet(name, h5Type, dataspace, prop);
}

void FieldRecorderHDF5::flush() {
    if (_fields.size()>0) {
        // when no fields configured, nothing to flush. init() probably not
        // yet called and _file is even not set.
        _file.flush(H5F_SCOPE_GLOBAL);
    }
}

void FieldRecorderHDF5::init() {
    _file = H5::H5File(getFilePath(), H5F_ACC_TRUNC);

    addField("SimTime");
    for (auto field : *getInputFields()) {
        std::string name = field->GetName();
        auto obj = field->GetParent();
        while (obj!=nullptr && obj != static_cast<Smp::IObject*>(getSimulator())) {
            name.insert(0, std::string(obj->GetName()) + "::");
            obj = obj->GetParent();
        }
        addField(name.c_str(), field);
    }
}


void FieldRecorderHDF5::step() {
    for (auto& field : _fields) {
        {
            field.dims.back()++;
            field.dataset.extend(field.dims.data());
        }
        H5::DataSpace fspace = field.dataset.getSpace();
        H5::DataSpace memspace;
        {
            std::vector<hsize_t> dims(field.dims);
            std::vector<hsize_t> offset(dims.size(), 0);
            dims.back() = 1;
            offset.back() = field.dims.back() - 1;
            fspace.selectHyperslab(H5S_SELECT_SET, dims.data(), offset.data());
            memspace = H5::DataSpace(dims.size(), dims.data());
        }

        /**
         * data vector contains values cast as an int64. Actually, we don't care
         * of the type as H5::DataSet::write expect a void* array.
         * WARNING: change the common storing type if using a larger one (>64 bits)
         **/
        std::vector<Smp::Int64> data;
        if (!field.field) {
            
            /* assuming it is a simulation time field 
             * TODO shall not guess like this. */
            data.push_back(Smp::AnySimple(Smp::PrimitiveTypeKind::PTK_Duration,
                                          getSimulator()->GetTimeKeeper()->GetSimulationTime())
                               .value.int64Value);
        }
        else if (field.isArray) {
            for (Smp::UInt64 i = 0; i < field.simplearrayfield->GetSize(); ++i) {
                data.push_back(field.simplearrayfield->GetValue(i).value.int64Value);
            }
        }
        else {
            data.push_back(field.simplefield->GetValue().value.int64Value);
        }
        field.dataset.write(data.data(), field.h5Type, memspace, fspace);
    }
}

void FieldRecorderHDF5::disconnect() {
    FieldRecorder::disconnect();
    flush();
    _file.close();
    _fields.clear();
}

} /* namespace colibry */
} /* namespace simphonie */
