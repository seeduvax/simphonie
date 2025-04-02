/*
 * @file FieldRecorder.hpp
 *
 * Copyright 2025 Sebastien Devaux. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simphonie_colibry_FieldRecorder_HPP__
#define __simphonie_colibry_FieldRecorder_HPP__

#include "simdeck/Service.hpp"
#include "simdeck/EntryPointPublisher.hpp"
#include "Smp/IOutputField.h"

namespace simphonie {
namespace colibry {

/**
 * Base abstract class for Fields data recording service.
 * It implements IOutputField just to benefit the related field connection
 * capability. It knows what field to records according the received connection.
 */
class FieldRecorder: public simdeck::Service,
                     virtual simdeck::EntryPointPublisher,
                     virtual Smp::IOutputField {
    typedef simdeck::Service Parent;
public:
    /**
     * Default constructor.
     */
    FieldRecorder(Smp::String8 name, Smp::String8 description,
                                        Smp::IObject* parent,
                                        Smp::String8 fileExt=nullptr);

    /**
     * Destructor.
     */
    virtual ~FieldRecorder();


    // Smp::IOutputField implementation. mostly faked except Connect and
    // Disconnect
    void Connect(Smp::IField* field) override;
    void Disconnect(Smp::IField* field) override;
    const Smp::FieldCollection* GetInputFields() const override;
    void Push() override;
    Smp::Bool IsAutomatic() const override;
    Smp::ViewKind GetView() const override;
    Smp::Bool IsState() const override;
    Smp::Bool IsInput() const override;
    Smp::Bool IsOutput() const override;
    const Smp::Publication::IType* GetType() const override;
    void Restore( Smp::IStorageReader* reader) override;
    void Store( Smp::IStorageWriter* writer) override;

    /**
     * recording sample function.
     * Shall record all connected fields' current value.
     * To be implemented by format specific subclasses. Exposed as SMP entry
     * point to be scheduled when data shall be recorded.
     */ 
    virtual void step() = 0;

    // those two using to make c++ compiler happy whith the Connect/Disconnect
    // from both IComponent and IOutputField.
    using Parent::Connect;
    using Parent::Disconnect;


protected:
    Smp::String8 getFilePath() {
        return _filePath.c_str();
    }

private:
    /** recording output file path */
    std::string _filePath;
    /** list of the fields to be recorded by this recorder */
    simdeck::Collection<Smp::IField> _recFields; 

};

}} // namespace simphonie::colibry
#endif // __simphonie_colibry_FieldRecorder_HPP__
