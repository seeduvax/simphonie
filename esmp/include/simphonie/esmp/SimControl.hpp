#ifndef __test_SimControl_hpp
#define __test_SimControl_hpp
#include "Smp/ISimulator.h"
#include "simdeck/Service.hpp"
#include "simphonie/sys/Synchro.hpp"

namespace simphonie {
namespace esmp {
class SimControl: public simdeck::Service {
public:
    SimControl(Smp::String8 name, Smp::String8 descr = "",
              Smp::IObject* parent = nullptr);
    virtual ~SimControl();
    void wait();
    void publish(Smp::IPublication* receiver) override;
private:
    void ep();
    Smp::ISimulator* _sim;
    Smp::Duration _stopTime;
    std::unique_ptr<Smp::IEntryPoint> _ep;
    std::mutex _mutex;
    std::condition_variable _monitor;
    bool _completed=false;
};
}  // namespace esmp
}  // namespace simph
#endif
