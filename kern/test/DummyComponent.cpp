#include "DummyComponent.hpp"

namespace test {
DummyComponent::DummyComponent(Smp::String8 name,Smp::String8 descr,
                        Smp::IObject* parent):
                    Component(name,descr,parent) {
}
}
