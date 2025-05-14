#include "Smp/ISimulator.h"
#include "simdeck/Factory.hpp"
#include "simphonie/sys/DlDef.h"
#include "simphonie/lua/LuaModel.hpp"

namespace simphonie {
namespace lua {

REGISTER_SMP_LIBINIT();
ADD_SMP_FACTORY("LuaModel", LuaModel);

}  // namespace lua
}  // namespace simph
