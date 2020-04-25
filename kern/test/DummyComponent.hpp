#ifndef __test_DummyComponent_hpp__
#define __test_DummyComponent_hpp__
#include "simph/smpdk/Component.hpp"

namespace test {

class DummyComponent: public virtual simph::smpdk::Component {
public:
    DummyComponent(Smp::String8 name, Smp::String8 descr="",
                        Smp::IObject* parent=nullptr);
};
}
#endif
