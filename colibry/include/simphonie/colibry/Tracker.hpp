/*
 * @file Tracker.hpp
 *
 * Copyright 2025 Sebastien Devaux. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simphonie_colibry_Tracker_HPP__
#define __simphonie_colibry_Tracker_HPP__

#include "simdeck/Composite.hpp"
#include "simdeck/EntryPointPublisher.hpp"
#include "simdeck/Service.hpp"
#include "sxeval/SXEval.hpp"
#include "Smp/Services/EventId.h"

namespace simphonie {
namespace colibry {

#define EVALUATE_EP_NAME "Evaluate"
/**
 *
 */
class Tracker: public simdeck::Service,
            virtual public simdeck::AComposite,
            virtual public simdeck::EntryPointPublisher {
    typedef simdeck::Service Parent;
public:
    /**
     * Default constructor.
     */
    Tracker(Smp::String8 name, Smp::String8 descr="", Smp::IObject* parent=nullptr);
    /**
     * Destructor.
     */
    virtual ~Tracker();

    /**
     * set expression to be evaluated
     */  
    inline void setExpression(Smp::String8 expression) {
        _expression = expression;
    }
    /** Expression evaluation entry point */
    void epEvaluate();

protected:
    // Component specialization
    void publish(Smp::IPublication* receiver) override;
    void connect() override;

    /** additional actions to be done on evaluation result change */
    virtual void onChange(Smp::Float64 value);

    /** Expression engine init entry point */
    void epInit();

private:

    class EventCounter: public simdeck::Component, virtual public simdeck::EntryPointPublisher {
    public:
        EventCounter(Smp::String8 name, Smp::String8 descr, Smp::IObject* parent, Smp::Services::EventId eventId);
        inline void handle() {
            _counter+=1.0;
        }
        inline Smp::Float64& get() {
            return _counter;
        }

    private:
        Smp::Float64 _counter=0;
    };
    /** expressions result */
    Smp::Float64 _out=0;
    /** Change count */
    Smp::UInt64 _changeCount=0;
    /** true when evalation results is different than the previous evaluation */
    Smp::Bool _changed=false;
    /** the expression to evaluate */
    std::string _expression="";
    /** the expression interpreter */
    sxeval::SXEval<Smp::Float64> _evaluator;
};

}} // namespace simphonie::colibry
#endif // __simphonie_colibry_Tracker_HPP__
