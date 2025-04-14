/*
 * @file StorageWriter.hpp
 *
 * Copyright 2025 Sebastien Devaux. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simphonie_kern_StorageWriter_HPP__
#define __simphonie_kern_StorageWriter_HPP__

#include <fstream>
#include "Smp/ISimulator.h"
#include "Smp/IStorageWriter.h"

namespace simphonie {
namespace kern {

/**
 *
 */
class StorageWriter : public Smp::IStorageWriter {
public:
    /**
     * Default constructor.
     */
    StorageWriter(Smp::ISimulator* sim, Smp::String8 filePath);
    /**
     * Destructor.
     */
    virtual ~StorageWriter();

    void store();

    // Smp::IStorageWriter implementation
    void Store(const Smp::Void* address, Smp::UInt64 size) override;
    Smp::String8 GetStateVectorFileName() const override;
    Smp::String8 GetStateVectorFilePath() const override;

private:
    Smp::ISimulator* _sim;
    int _indent = 0;
    std::ofstream _out;
    std::string _filePath;
    std::string _subFilePath;

    void indent();
    void storeContainer(Smp::IContainer* container);
    void storeComponent(Smp::IComponent* component);
};

}  // namespace kern
}  // namespace simphonie
#endif  // __simphonie_kern_StorageWriter_HPP__
