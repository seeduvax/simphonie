/*
 * @file CWrapper.c
 *
 * Copyright 2025. All rights reserved.
 * Use is subject to license terms.
 *
 * @brief POC for C layer for interacting with C++ code.
 * 
 * $Id$
 * $Date$
 */
#include "CWrapper.h"
#include "Smp/Services/ILogger.h"
#include "Smp/Services/IEventManager.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ---------------------------------- Smp ---------------------------------- */
#define CW_Smp CW_BUILD_SPACE(Smp)
#define CW_SPACE CW_Smp

#undef CW_SPACE


/* ----------------------------- Smp::Services ----------------------------- */
#define CW_Smp_Services CW_BUILD_SPACE(CW_Smp, Services)
#define CW_SPACE CW_Smp_Services

#undef CW_SPACE


/* ------------------------- Smp::Services::ILogger ------------------------- */
#define CW_Smp_Services_ILogger CW_BUILD_SPACE(CW_Smp_Services, ILogger)
#define CW_SPACE CW_Smp_Services_ILogger

CW(CW_Smp_Services, LogMessageKind) CW(QueryLogMessageKind) (CW()* self, CW(CW_Smp, String8) messageKindName) {
    return static_cast<Smp::Services::ILogger*>(self)->QueryLogMessageKind(messageKindName);
}

void CW(Log) (CW()* self, const CW(CW_Smp, IObject)* sender, CW(CW_Smp, String8) message, CW(CW_Smp_Services, LogMessageKind) kind) {
    static_cast<Smp::Services::ILogger*>(self)->Log(static_cast<const Smp::IObject*>(sender), message, kind);
}

#undef CW_SPACE


/* ---------------------- Smp::Services::IEventManager ---------------------- */
#define CW_Smp_Services_IEventManager CW_BUILD_SPACE(CW_Smp_Services, IEventManager)
#undef CW_SPACE
#define CW_SPACE CW_Smp_Services_IEventManager

CW(CW_Smp_Services, EventId) CW(QueryEventId) (CW()* self, CW(CW_Smp, String8) eventName) {
    return static_cast<Smp::Services::IEventManager*>(self)->QueryEventId(eventName);
}

void CW(Emit) (CW()* self, CW(CW_Smp_Services, EventId) event, CW(CW_Smp, Bool) synchronous) {
    static_cast<Smp::Services::IEventManager*>(self)->Emit(event, synchronous);
}

#undef CW_SPACE


#undef CW_Smp_Services_IEventManager
#undef CW_Smp_Services_ILogger
#undef CW_Smp_Services
#undef CW_Smp


#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */
