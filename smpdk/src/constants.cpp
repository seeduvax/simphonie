#include "Smp/ISimulator.h"
#include "Smp/Services/IEventManager.h"
#include "Smp/Services/ILogger.h"
#include "Smp/Services/ITimeKeeper.h"
#include "Smp/Uuid.h"

namespace Smp {
// --------------------------------------------------------------------
// ..........................................................
constexpr Uuid Smp::Uuids::Uuid_Char8;
constexpr Uuid Smp::Uuids::Uuid_Bool;
constexpr Uuid Smp::Uuids::Uuid_Int8;
constexpr Uuid Smp::Uuids::Uuid_Int16;
constexpr Uuid Smp::Uuids::Uuid_Int32;
constexpr Uuid Smp::Uuids::Uuid_Int64;
constexpr Uuid Smp::Uuids::Uuid_UInt8;
constexpr Uuid Smp::Uuids::Uuid_UInt16;
constexpr Uuid Smp::Uuids::Uuid_UInt32;
constexpr Uuid Smp::Uuids::Uuid_UInt64;
constexpr Uuid Smp::Uuids::Uuid_Float32;
constexpr Uuid Smp::Uuids::Uuid_Float64;

// --------------------------------------------------------------------
// ..........................................................
constexpr Smp::Char8 Smp::ISimulator::SMP_SimulatorServices[];
constexpr Smp::Char8 Smp::ISimulator::SMP_SimulatorModels[];

// ..........................................................
std::ostream& operator<<(std::ostream& os, const Smp::SimulatorStateKind& obj) {
    os << (Smp::Int32)obj;
    switch (obj) {
        case Smp::SimulatorStateKind::SSK_Building:
            os << "(SSK_Building)";
            break;
        case Smp::SimulatorStateKind::SSK_Connecting:
            os << "(SSK_Connecting)";
            break;
        case Smp::SimulatorStateKind::SSK_Initialising:
            os << "(SSK_Initialising)";
            break;
        case Smp::SimulatorStateKind::SSK_Standby:
            os << "(SSK_Standby)";
            break;
        case Smp::SimulatorStateKind::SSK_Executing:
            os << "(SSK_Executing)";
            break;
        case Smp::SimulatorStateKind::SSK_Storing:
            os << "(SSK_Storing)";
            break;
        case Smp::SimulatorStateKind::SSK_Restoring:
            os << "(SSK_Restoring)";
            break;
        case Smp::SimulatorStateKind::SSK_Reconnecting:
            os << "(SSK_Reconnecting)";
            break;
        case Smp::SimulatorStateKind::SSK_Exiting:
            os << "(SSK_Exiting)";
            break;
        case Smp::SimulatorStateKind::SSK_Aborting:
            os << "(SSK_Aborting)";
            break;
        default:
            os << "()";
            break;
    }
    return os;
}

// --------------------------------------------------------------------
// ..........................................................
namespace Services {
// ..........................................................
constexpr Smp::Services::EventId IEventManager::SMP_LeaveConnectingId;
constexpr Smp::Services::EventId IEventManager::SMP_EnterInitialisingId;
constexpr Smp::Services::EventId IEventManager::SMP_LeaveInitialisingId;
constexpr Smp::Services::EventId IEventManager::SMP_EnterStandbyId;
constexpr Smp::Services::EventId IEventManager::SMP_LeaveStandbyId;
constexpr Smp::Services::EventId IEventManager::SMP_EnterExecutingId;
constexpr Smp::Services::EventId IEventManager::SMP_LeaveExecutingId;
constexpr Smp::Services::EventId IEventManager::SMP_EnterStoringId;
constexpr Smp::Services::EventId IEventManager::SMP_LeaveStoringId;
constexpr Smp::Services::EventId IEventManager::SMP_EnterRestoringId;
constexpr Smp::Services::EventId IEventManager::SMP_LeaveRestoringId;
constexpr Smp::Services::EventId IEventManager::SMP_EnterExitingId;
constexpr Smp::Services::EventId IEventManager::SMP_EnterAbortingId;
constexpr Smp::Services::EventId IEventManager::SMP_EpochTimeChangedId;
constexpr Smp::Services::EventId IEventManager::SMP_MissionTimeChangedId;
constexpr Smp::Services::EventId IEventManager::SMP_EnterReconnectingId;
constexpr Smp::Services::EventId IEventManager::SMP_LeaveReconnectingId;
constexpr Smp::Services::EventId IEventManager::SMP_PreSimTimeChangeId;
constexpr Smp::Services::EventId IEventManager::SMP_PostSimTimeChangeId;
constexpr Smp::Char8 IEventManager::SMP_EventManager[];
constexpr Smp::Char8 IEventManager::SMP_LeaveConnecting[];
constexpr Smp::Char8 IEventManager::SMP_EnterInitialising[];
constexpr Smp::Char8 IEventManager::SMP_LeaveInitialising[];
constexpr Smp::Char8 IEventManager::SMP_EnterStandby[];
constexpr Smp::Char8 IEventManager::SMP_LeaveStandby[];
constexpr Smp::Char8 IEventManager::SMP_EnterExecuting[];
constexpr Smp::Char8 IEventManager::SMP_LeaveExecuting[];
constexpr Smp::Char8 IEventManager::SMP_EnterStoring[];
constexpr Smp::Char8 IEventManager::SMP_LeaveStoring[];
constexpr Smp::Char8 IEventManager::SMP_EnterRestoring[];
constexpr Smp::Char8 IEventManager::SMP_LeaveRestoring[];
constexpr Smp::Char8 IEventManager::SMP_EnterExiting[];
constexpr Smp::Char8 IEventManager::SMP_EnterAborting[];
constexpr Smp::Char8 IEventManager::SMP_EpochTimeChanged[];
constexpr Smp::Char8 IEventManager::SMP_MissionTimeChanged[];
constexpr Smp::Char8 IEventManager::SMP_EnterReconnecting[];
constexpr Smp::Char8 IEventManager::SMP_LeaveReconnecting[];
constexpr Smp::Char8 IEventManager::SMP_PreSimTimeChange[];
constexpr Smp::Char8 IEventManager::SMP_PostSimTimeChange[];

// ..........................................................
constexpr LogMessageKind ILogger::LMK_Debug;
constexpr LogMessageKind ILogger::LMK_Error;
constexpr LogMessageKind ILogger::LMK_Event;
constexpr LogMessageKind ILogger::LMK_Information;
constexpr LogMessageKind ILogger::LMK_Warning;
constexpr Smp::Char8 ILogger::LMK_DebugName[];
constexpr Smp::Char8 ILogger::LMK_ErrorName[];
constexpr Smp::Char8 ILogger::LMK_EventName[];
constexpr Smp::Char8 ILogger::LMK_InformationName[];
constexpr Smp::Char8 ILogger::LMK_WarningName[];

// ..........................................................
constexpr Smp::Char8 ILinkRegistry::SMP_LinkRegistry[];

// ..........................................................
constexpr Smp::Char8 ITimeKeeper::SMP_TimeKeeper[];
}  // namespace Services

}  // namespace Smp
