#ifndef __test_SimControlEnd_hpp
#define __test_SimControlEnd_hpp
#include "Smp/ISimulator.h"
#include "simph/sys/Synchro.hpp"

namespace test {
class SimControlEnd {
public:
    SimControlEnd(Smp::ISimulator* sim, Smp::Duration stopTime);
    void wait();
private:
    void ep();
    Smp::ISimulator* _sim;
    Smp::Duration _stopTime;
    std::unique_ptr<Smp::IEntryPoint> _ep;
    std::mutex _mutex;
    std::condition_variable _monitor;
    bool _completed=false;
};
}
#endif
