/*
 * @file FieldRecorderCsv.hpp
 *
 * Copyright 2025 Sebastien Devaux. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simphonie_colibry_FieldRecorderCsv_HPP__
#define __simphonie_colibry_FieldRecorderCsv_HPP__

#include <fstream>
#include "simphonie/colibry/FieldRecorder.hpp"

namespace simphonie {
namespace colibry {

/**
 *
 */
class FieldRecorderCsv: public FieldRecorder {
    typedef FieldRecorder Parent;
public:
    /**
     * Default constructor.
     */
    FieldRecorderCsv(Smp::String8 name, Smp::String8 description, Smp::IObject* parent);
    /**
     * Destructor.
     */
    virtual ~FieldRecorderCsv();

    // FieldRecorder step specialization. Output fields' value as text line into
    // the file.
    void step() override;

protected:
    // Component specialization to open/close file on simulator connect/disconnect
    void connect() override;
    void disconnect() override;

private:
    std::ofstream _file;
    char _delim=',';

};

}} // namespace simphonie::colibry
#endif // __simphonie_colibry_FieldRecorderCsv_HPP__
