/*
 * @file SimSyncDataShare.hpp
 *
 * Copyright 2025. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simphonie_mt_SimSyncDataShare_HPP__
#define __simphonie_mt_SimSyncDataShare_HPP__

#include <sstream>
#include <string>
#include <vector>
#include "Smp/IField.h"
#include "Smp/IOutputField.h"
#include "Smp/IPublication.h"
#include "Smp/IStorageReader.h"
#include "Smp/IStorageWriter.h"
#include "Smp/Services/IResolver.h"
#include "Smp/String8.h"
#include "simdeck/Collection.hpp"
#include "simdeck/Object.hpp"

namespace simphonie {
namespace mt {

class SimSyncDataShare : virtual public Smp::IStorageWriter, virtual public Smp::IStorageReader {
public:
    typedef std::vector<uint8_t> DataType;

    SimSyncDataShare();
    ~SimSyncDataShare() = default;

    DataType retrieveData();
    void loadData(const DataType& data);
    void publish(Smp::IObject* parent, Smp::IPublication* receiver);

    void Store(const Smp::Void* address, Smp::UInt64 size) override;
    void Restore(Smp::Void* address, Smp::UInt64 size) override;
    Smp::String8 GetStateVectorFileName() const override {
        return nullptr;
    };
    Smp::String8 GetStateVectorFilePath() const override {
        return nullptr;
    };

private:
    /**
     * Wrapper of a field collection as Smp::IOutput field to benefit of
     * the Connect/Disconnect API.
     */
    class FieldHolder;

    FieldHolder* _inputs;
    FieldHolder* _outputs;
    DataType _buf;
};

} /* namespace mt */
} /* namespace simphonie */
#endif /* __simphonie_mt_SimSyncDataShare_HPP__ */
