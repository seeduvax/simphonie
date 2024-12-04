#include "SimControlHelper.hpp"
#include "simph/kern/Scheduler.hpp"
#include "simph/sys/Callback.hpp"
#include "simph/smpdk/EntryPoint.hpp"
#include "simph/sys/Logger.hpp"

namespace test {

void SimControlEnd::ep() {
    if (_sim->GetTimeKeeper()->GetSimulationTime()>_stopTime) {
        TRACE("Requesting simulation end");
        _sim->Hold(true);
    }
}

SimControlEnd::SimControlEnd(Smp::ISimulator* sim, Smp::Duration stopTime):
            _sim(sim), _stopTime(stopTime) {
    auto f = simph::sys::Callback::create(&SimControlEnd::ep, this);
    auto epf=std::make_unique<simph::smpdk::EntryPoint>(std::move(f),"endSimuCtrl");
    _sim->GetEventManager()->Subscribe(
            Smp::Services::IEventManager::SMP_PostSimTimeChangeId,
            epf.get());
}
}
