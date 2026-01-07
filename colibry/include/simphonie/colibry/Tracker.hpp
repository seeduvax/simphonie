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
 * Simulation status tracker based on S-Expression on fields value.
 *
 * This service is composed of a S-Expression engine working on Fields value to
 * detect specific simulation states and acts when a specific configuration is
 * met. This service by itself does nothing on expression returned value. It is
 * expected to be enhanced to build more specific service triggering actions
 * according the expression result.
 *
 * The expression shall be defined either:
 *   - setting the string value of the `Expression` input field.
 *   - programmatically using the `setExpression` method.
 * The expression engine is configured by the `Init` entry point, meaning that
 * the expression defined (with `Expression` input field or `setExpression` 
 * method) remains without effect until the `Init` entry point call. Expression
 * may include references to:
 *   - fields: using the absolute path (from the SMP resolver point of view) and
 *     starting with `/`
 *   - events: SMP event manager's global event by name. The resulting value is
 *     the count of emit to the event manager for this event at the instant of 
 *     the evaluation.
 *
 * Examples :
 *   - `(> /mdlA/out (+ /mdlB/out /mdlA/in))` returns 1 when the output of 
 *     model A  is greater than the sum of the output of the model B and the
 *     input of the model A.
 *   - `(+ TheEventA TheEventB)' returns the sum of TheEventA and TheEventB
 *     events emit count.
 *
 * On component Configure step the `Init` entry point is registered to the 
 * host simulator with its `AddInitEntryPoint` method, then it is called at the
 * end of the simulator building process. This means that on standard use case,
 * the simulator integrator shall only have to define the expression sometime
 * after the service publication and before the end of the simulator 
 * configuration.
 *
 * The expression may be update at any time during the simulation, but the 
 * `Init` entry point shall be explicitly executed or scheduled to make the
 * expression change fully applied.  
 *
 * The expression result computation is handled by the `Evaluate` entry point.
 * Simulator integrators (and/or subclasses implementers) shall define the
 * evaluation activation policy by scheduling this entry point and/or
 * registering it to some specific event(s) of the SMP event manager.
 *
 * Additional `Tracker` service's output fields:
 * - `Out`: Smp::Float64, result of the last computed expression evaluation.
 * - `Changed`: Smp::Bool, true when last expression result is different from 
 *    the previous one.
 * - `ChangedCount`: Smp::UInt64, count of the evaluation resulting to a value
 *    change (how many times `Changed` output was true after `Evaluate` each
 *    entry point execution).
 *
 * References:
 *  - S-Expression: see https://en.wikipedia.org/wiki/S-expression
 *  - S-Expression implementation provided by Titouan Abadie:
 *    https://github.com/abadiet/SXEval
 */
class Tracker: public simdeck::Service,
            virtual public simdeck::AComposite,
            virtual public simdeck::EntryPointPublisher {
    typedef simdeck::Service Parent;
public:
    /**
     * Main constructor.
     * @param name instance name.
     */
    Tracker(Smp::String8 name, Smp::String8 descr="", Smp::IObject* parent=nullptr);
    /**
     * Destructor.
     */
    virtual ~Tracker();

    /**
     * Set expression to be evaluated.
     * Convenience method to define the expression programmatically.
     * Simulator integrator may define the expression by setting the 
     * `Expression` input field value. Notice: this only change the expression
     * value and does nothing else. The `Init` entry point shall be called after
     * that to run the expression parsing and expression engine reconfiguration.
     */  
    inline void setExpression(Smp::String8 expression) {
        _expression = expression;
    }
    /** 
     * Expression evaluation entry point. 
     * Computes the expression results.
     */
    void epEvaluate();

protected:
    // Component specialization
    void publish(Smp::IPublication* receiver) override;
    void connect() override;

    /** 
     * additional actions to be done on evaluation.
     * Intended to be overridden by subclasses to trigger specific action
     * after expression evaluation.
     */
    virtual void onEvaluate();

    /** 
     * Expression engine `Init` entry point.
     * Launch the expression parsing and the expression engine is built for
     * further fast evaluation of the expression.
     * The expression shall be define prior to this entry point either by
     * setting the `Expression` field value or call the `setExpression` method.
     */ 
    void epInit();

    // quick access to fields for the subclasses.
    inline Smp::Float64 getOut() {
        return _out;
    }
    inline Smp::UInt64 getChangedCount() {
        return _changedCount;
    }
    inline Smp::Bool getChanged() {
        return _changed;
    }

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
    Smp::UInt64 _changedCount=0;
    /** 
     * true when evaluation results is different than the previous evaluation 
     */
    Smp::Bool _changed=false;
    /** the expression to evaluate */
    std::string _expression="";
    /** the expression interpreter */
    sxeval::SXEval<Smp::Float64> _evaluator;
};

}} // namespace simphonie::colibry
#endif // __simphonie_colibry_Tracker_HPP__
