/*
 * @file AStepMdl.cpp
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#include "simph/umdl/AStepMdl.hpp"

namespace simph {
	namespace umdl {

class StepEP: virtual public simph::kern::Object,
            virtual public Smp::IEntryPoint {
public:
    StepEP(Smp::String8 name,Smp::String8 descr, Smp::IObject* parent): 
                        simph::kern::Object(name,descr,parent) {
        _target=dynamic_cast<AStepMdl*>(parent);
    }
    virtual ~StepEP() {
    }
    void Execute() const {
        if (_target!=nullptr) {
            _target->step();
        }
    }
private:
    AStepMdl* _target;    
};
// --------------------------------------------------------------------
// ..........................................................
AStepMdl::AStepMdl(Smp::String8 name, Smp::String8 descr,
                        Smp::IObject* parent):
                    simph::kern::Component(name,descr,parent), 
                    _epList("epList","Entry points",this) {
    _epList.push_back(new StepEP("step","Main model entry point",this));
}
// ..........................................................
AStepMdl::~AStepMdl() {
    for (auto ep: _epList) {
        delete ep;
    }
}
// --------------------------------------------------------------------
// ..........................................................
const Smp::EntryPointCollection* AStepMdl::GetEntryPoints() const {
    return &_epList;
}
// ..........................................................
Smp::IEntryPoint* AStepMdl::GetEntryPoint(Smp::String8 name) const {
    return _epList.at(name);
}
}} // namespace simph::umdl
