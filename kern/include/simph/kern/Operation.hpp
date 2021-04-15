/*
 * @file Operation.hpp
 *
 * Copyright 2020 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simph_kern_Operation_HPP__
#define __simph_kern_Operation_HPP__

#include "Smp/IOperation.h"
#include "simph/smpdk/Collection.hpp"
#include "simph/smpdk/Object.hpp"

namespace simph {
namespace kern {

/**
 *
 */
class Operation : public simph::smpdk::Object, virtual public Smp::IOperation {
public:
    /**
     * Default constructor.
     */
    Operation(Smp::String8 name, Smp::String8 description = "", Smp::IObject* parent = nullptr);
    /**
     * Destructor.
     */
    virtual ~Operation();

    // Smp::IOperation implementation
    const Smp::ParameterCollection* GetParameters() const override;
    Smp::IParameter* GetParameter(Smp::String8 name) const override;
    Smp::IParameter* GetReturnParameter() const override;
    Smp::ViewKind GetView() const override;
    Smp::IRequest* CreateRequest() override;
    void Invoke(Smp::IRequest* request) override;
    void DeleteRequest(Smp::IRequest* request) override;

private:
    simph::smpdk::Collection<Smp::IParameter> _params;
    Smp::IParameter* _retParam;
    Smp::ViewKind _view;
};

}  // namespace kern
}  // namespace simph
#endif  // __simph_kern_Operation_HPP__
