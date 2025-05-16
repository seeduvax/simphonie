/*
 * @file SimControl.hpp
 *
 * Copyright 2025 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simphonie_colibry_SimControl_HPP__
#define __simphonie_colibry_SimControl_HPP__

#include <memory>
#include "Smp/ISimulator.h"
#include "Smp/Services/EventId.h"
#include "Smp/String8.h"
#include "simdeck/Component.hpp"
#include "simdeck/Composite.hpp"
#include "simdeck/EntryPointPublisher.hpp"
#include "simdeck/Service.hpp"
#include "sxeval/SXEval.hpp"

namespace simphonie {
namespace colibry {

class SimControl : public simdeck::Service,
                   virtual public simdeck::EntryPointPublisher,
                   virtual public simdeck::AComposite {
public:
    SimControl(Smp::String8 name, Smp::String8 descr = "", Smp::IObject* parent = nullptr);
    ~SimControl();

protected:
    void publish(Smp::IPublication* receiver) override;
    void connect() override;

private:
    typedef double T;

    class _EventHandler : public simdeck::Component, virtual public simdeck::EntryPointPublisher {
    public:
        _EventHandler(Smp::String8 name, Smp::String8 descr, Smp::IObject* parent, Smp::Services::EventId eventId);
        inline void handle() {
            _counter++;
        }
        inline T& get() {
            return _counter;
        }

    private:
        T _counter;
    };

    /**
     * TODO
     * Not thread safe! Should not be called during a check.
     */
    void _applyCondition();
    void _checkStopCondition();

    std::string _condition;
    std::unique_ptr<sxeval::SXEval<T> > _evaluator;
    std::vector<std::unique_ptr<_EventHandler> > _eventHandlers;
};

} /* namespace colibry */
} /* namespace simphonie */
#endif /* __simphonie_colibry_SimControl_HPP__ */
