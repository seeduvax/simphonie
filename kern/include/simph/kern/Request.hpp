/*
 * @file Request.h
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simph_kern_Request_HPP__
#define __simph_kern_Request_HPP__

#include "Smp/IRequest.h"
#include "simph/smpdk/Collection.hpp"

namespace simph {
namespace kern {
using namespace simph::smpdk;

class Request : public Smp::IRequest {
public:
    struct Parameter {
        std::string name;
        Smp::AnySimple value;
    };

    Request(Smp::String8 operationName, const std::vector<Parameter>& parameters,
            Smp::PrimitiveTypeKind returnType = Smp::PrimitiveTypeKind::PTK_None);

    virtual ~Request();

public:
    inline Smp::String8 GetOperationName() const override {
        return _operationName.c_str();
    }

    inline Smp::Int32 GetParameterCount() const override {
        return (Smp::Int32)_parameters.size();
    }

    Smp::Int32 GetParameterIndex(Smp::String8 parameterName) const override;

    void SetParameterValue(Smp::Int32 index, Smp::AnySimple value) override;

    Smp::AnySimple GetParameterValue(Smp::Int32 index) const override;

    void SetReturnValue(Smp::AnySimple value) override;

    Smp::AnySimple GetReturnValue() const override;

private:
    std::string _operationName;
    Smp::AnySimple _returnValue;
    std::vector<Parameter> _parameters;
};

}  // namespace kern
}  // namespace simph
#endif  // __simph_kern_Request_HPP__
