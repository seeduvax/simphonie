#ifndef __test_SimControl_hpp
#define __test_SimControl_hpp
#include "Smp/ISimulator.h"
#include "simdeck/EntryPointPublisher.hpp"
#include "simdeck/Service.hpp"

namespace simphonie {
namespace colibry {

/**
 * Service to control spontaneous stop of simulator.
 * At the moment the only stop criteria is when simulation time has reached
 * the stop time.
 */
class SimControl : public simdeck::Service, virtual public simdeck::EntryPointPublisher {
public:
    SimControl(Smp::String8 name, Smp::String8 descr = "", Smp::IObject* parent = nullptr);
    virtual ~SimControl();
    // Service specialization.
    void publish(Smp::IPublication* receiver) override;
    void connect() override;

private:
    void checkStopCondition();
    /** Host simulator */
    Smp::ISimulator* _sim = nullptr;
    /** Max simulation time. */
    Smp::Duration _stopTime;
};
}  // namespace colibry
}  // namespace simphonie
#endif
