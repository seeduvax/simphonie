#include "Smp/ISimulator.h"

namespace test {
class SimControlEnd {
public:
    SimControlEnd(Smp::ISimulator* sim, Smp::Duration stopTime);
private:
    void ep();
    Smp::ISimulator* _sim;
    Smp::Duration _stopTime;
};
}
