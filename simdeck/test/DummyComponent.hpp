#ifndef __test_DummyComponent_hpp__
#define __test_DummyComponent_hpp__
#include "simdeck/Component.hpp"

namespace test {

class DummyComponent : public virtual simdeck::Component {
public:
    DummyComponent(Smp::String8 name, Smp::String8 descr = "", Smp::IObject* parent = nullptr);
};
}  // namespace test
#endif
