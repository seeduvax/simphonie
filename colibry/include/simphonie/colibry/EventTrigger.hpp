/*
 * @file EventTrigger.hpp
 *
 * Copyright 2026 Sebastien Devaux. All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simphonie_colibry_EventTrigger_HPP__
#define __simphonie_colibry_EventTrigger_HPP__

#include "simphonie/colibry/Tracker.hpp"
#include "Smp/Services/IEventManager.h"

namespace simphonie {
namespace colibry {

/**
 * Emit an event on condition defined by an expression.
 * The expression capabilities are inherited from the `Tracker` service. See
 * `Tracker` documentation for specific expression handling instruction.
 *
 * For most criteria (all but on changed), the expression is expected to be a
 * boolean expression, then result is considered to be true when computed value
 * is not 0. The `EventTrigger` service provides some string input fields to
 * define the name of events to trigger on various conditions related to the
 * computed expression value:
 *  - `OnTrueEvent`: name of the event to trig each time the expression is 
 *    evaluated as true (different to 0).
 *  - 'OnBecomeTrueEvent': event to trig each time the expression result becomes
 *    true. the event is not triggered when the expression is evaluated again
 *    and the result remains true.
 *  - 'OnBecomeFalseEvent': event to trig each time the expression result 
 *     switched from true to false.  
 *  - 'OnChangedEvent": event to trig each time the expression result changed 
 *     what ever the change is and not considering the result is zero/not zero
 *     boolean criteria. The event is triggered also when the result switch from
 *     a non zero value to another non zero value.   
 *
 * The registration to the event manager is performed on component Connect, 
 * meaning the events name shall be provided during the simulator building 
 * phase after the publication making the fields available and before the 
 * connect when the fields value are consumed.
 *
 * The default value for each input field is an empty string, meaning no event
 * to trig for the specific related condition.   
 */
class EventTrigger: public Tracker {
    typedef Tracker Parent;
public:
    /**
     * Default constructor.
     */
    EventTrigger(Smp::String8 name, Smp::String8 descr, Smp::IObject* parent);
    /**
     * Destructor.
     */
    virtual ~EventTrigger();

protected:
    // SMP component component specialization
    // publish specific fields.
    void publish(Smp::IPublication* receiver) override;
    // SMP connect component specialization
    // bind the events names to the event manager.
    void connect() override;
    /**
     * on evaluate callback specialization.
     * Trigger the events when the expected expression result and changes are 
     * met. See class description for more details on what event are triggered
     * on what condition
     */ 
    void onEvaluate() override;

private:
    /** on true event name */
    std::string _onTrueEventName="";
    /** on true event id */
    Smp::Services::EventId _onTrueEventId=-1;
    /** on become true event name */
    std::string _onBecomeTrueEventName="";
    /** on become true event id */
    Smp::Services::EventId _onBecomeTrueEventId=-1;
    /** on become false event name */
    std::string _onBecomeFalseEventName="";
    /** on become false event id */
    Smp::Services::EventId _onBecomeFalseEventId=-1;
    /** on changed event name */
    std::string _onChangedEventName="";
    /** on changed event id */
    Smp::Services::EventId _onChangedEventId=-1;
    /** Reference to the host simulator's event manager */
    Smp::Services::IEventManager* _eventManager=nullptr;

};

}} // namespace simphonie::colibry
#endif // __simphonie_colibry_EventTrigger_HPP__
