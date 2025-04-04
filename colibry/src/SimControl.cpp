#include "simphonie/colibry/SimControl.hpp"

#include <Smp/IPublication.h>
#include <Smp/Services/IEventManager.h>
#include <Smp/Services/ITimeKeeper.h>

#include <climits>

#include "simdeck/EntryPoint.hpp"
#include "simphonie/sys/Logger.hpp"

#define CHECK_EP_NAME "checkStop"

namespace simphonie {
namespace colibry {
// --------------------------------------------------------------------
// ..........................................................
SimControl::SimControl(Smp::String8 name, Smp::String8 descr, Smp::IObject* parent)
    : simdeck::Service(name, descr, parent) {
    _stopTime = LONG_LONG_MAX;
    addEP(CHECK_EP_NAME, "Check stop condition and request simulation hold when condition is met.", this,
          &SimControl::checkStopCondition);
}
// ..........................................................
SimControl::~SimControl() {}
// --------------------------------------------------------------------
// ..........................................................
void SimControl::checkStopCondition() {
    if (_sim->GetTimeKeeper()->GetSimulationTime() >= _stopTime) {
        TRACE("Requesting simulation end");
        _sim->Hold(true);
    }
}
// ..........................................................
void SimControl::publish(Smp::IPublication* receiver) {
    receiver->PublishField("stopTime",
                           "Max simulation time. Service shall stop simulation when simulation time reaches this value",
                           &_stopTime, Smp::ViewKind::VK_All, false, true, false);
}
// ..........................................................
void SimControl::connect() {
    _sim = getSimulator();
    _sim->GetEventManager()->Subscribe(Smp::Services::IEventManager::SMP_PostSimTimeChangeId,
                                       GetEntryPoint(CHECK_EP_NAME));
}

}  // namespace colibry
}  // namespace simphonie
