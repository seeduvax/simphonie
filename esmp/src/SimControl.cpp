#include "simphonie/esmp/SimControl.hpp"

#include <Smp/Services/IEventManager.h>
#include <Smp/Services/ITimeKeeper.h>
#include <Smp/IPublication.h>
#include <climits>

#include "simdeck/EntryPoint.hpp"
#include "simphonie/sys/Logger.hpp"

namespace simphonie {
namespace esmp {
void SimControl::ep() {
    TRACE(_stopTime-_sim->GetTimeKeeper()->GetSimulationTime());
    if (_sim->GetTimeKeeper()->GetSimulationTime()>_stopTime) {
        TRACE("Requesting simulation end");
        _sim->Hold(true);
        {
            Synchronized(_mutex);
            _completed=true;
        }
        _monitor.notify_all();
    }
}
void SimControl::publish(Smp::IPublication* receiver) {
    receiver->PublishField("stop", "Input: where to stop", &_stopTime, Smp::ViewKind::VK_All, false, true, false);
}
void SimControl::wait() {
    Synchronized(_mutex);
    while (!_completed) {
        MonitorWait(_monitor);
    }
}

SimControl::SimControl(Smp::String8 name, Smp::String8 descr,
              Smp::IObject* parent):
            simdeck::Service(name,descr,parent) {
    _sim = getSimulator();
    _stopTime = LONG_LONG_MAX;
    _ep.reset(simdeck::EntryPoint::Create("endSimuCtrl","",this,&SimControl::ep));
    _sim->GetEventManager()->Subscribe(
            Smp::Services::IEventManager::SMP_PostSimTimeChangeId,
            _ep.get());
}
SimControl::~SimControl() {
}
}  // namespace esmp
}  // namespace simph
