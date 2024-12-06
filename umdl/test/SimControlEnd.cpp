#include "SimControlEnd.hpp"
#include "simph/kern/Scheduler.hpp"
#include "simph/smpdk/EntryPoint.hpp"
#include "simph/sys/Logger.hpp"

namespace test {

void SimControlEnd::ep() {
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

void SimControlEnd::wait() {
    Synchronized(_mutex);
    while (!_completed) {
        MonitorWait(_monitor);
    }
}

SimControlEnd::SimControlEnd(Smp::ISimulator* sim, Smp::Duration stopTime):
            _sim(sim), _stopTime(stopTime) {
    _ep.reset(new simph::smpdk::EntryPoint(this,&SimControlEnd::ep,"endSimuCtrl"));
    _sim->GetEventManager()->Subscribe(
            Smp::Services::IEventManager::SMP_PostSimTimeChangeId,
            _ep.get());
}
}
