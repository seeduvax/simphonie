/*
 * @file CWrapper.h
 *
 * Copyright 2025. All rights reserved.
 * Use is subject to license terms.
 * 
 * @brief POC for C layer for interacting with C++ code.
 *
 * $Id$
 * $Date$
 */
#ifndef __CWrapper_H__
#define __CWrapper_H__

#include <stdlib.h>
#include <stdbool.h>

/* Parameters */
#define CW_PREFIX simphonie
// #define CW_POSTFIX post
#define CW_SEPARATOR _

/* Utils */
#define CW_CONCAT3(a, b, c) a ## b ## c
#define CW_CC3EX(a, b, c) CW_CONCAT3(a, b, c)
#define CW_BUILD_SPACE_RELATIVE(a, b) CW_CC3EX(a, CW_SEPARATOR, b)
#define CW_BUILD_SPACE_ABSOLUTE(b) b
#define CW_SWITCH_BUILD_SPACE(_1, _2, name, ...) name
#define CW_BUILD_SPACE(...) CW_SWITCH_BUILD_SPACE(__VA_ARGS__, CW_BUILD_SPACE_RELATIVE, CW_BUILD_SPACE_ABSOLUTE)(__VA_ARGS__)

/* Name Retrieving */
#if defined(CW_PREFIX) && defined(CW_POSTFIX)
    #define CW_NAME_SPACE() CW_CC3EX(CW_PREFIX, CW_SEPARATOR, CW_CC3EX(CW_SPACE, CW_SEPARATOR, CW_POSTFIX))
    #define CW_NAME_AT_SPACE(name) CW_CC3EX(CW_PREFIX, CW_SEPARATOR, CW_CC3EX(CW_SPACE, CW_SEPARATOR, CW_CC3EX(name, CW_SEPARATOR, CW_POSTFIX)))
    #define CW_NAME_WITH_SPACE(space, name) CW_CC3EX(CW_PREFIX, CW_SEPARATOR, CW_CC3EX(space, CW_SEPARATOR, CW_CC3EX(name, CW_SEPARATOR, CW_POSTFIX)))
#elif defined(CW_PREFIX)
    #define CW_NAME_SPACE() CW_CC3EX(CW_PREFIX, CW_SEPARATOR, CW_SPACE)
    #define CW_NAME_AT_SPACE(name) CW_CC3EX(CW_PREFIX, CW_SEPARATOR, CW_CC3EX(CW_SPACE, CW_SEPARATOR, name))
    #define CW_NAME_WITH_SPACE(space, name) CW_CC3EX(CW_PREFIX, CW_SEPARATOR, CW_CC3EX(space, CW_SEPARATOR, name))
#elif defined(CW_POSTFIX)
    #define CW_NAME_SPACE() CW_CC3EX(CW_SPACE, CW_SEPARATOR, CW_POSTFIX)
    #define CW_NAME_AT_SPACE(name) CW_CC3EX(CW_SPACE, CW_SEPARATOR, CW_CC3EX(name, CW_SEPARATOR, CW_POSTFIX))
    #define CW_NAME_WITH_SPACE(space, name) CW_CC3EX(space, CW_SEPARATOR, CW_CC3EX(name, CW_SEPARATOR, CW_POSTFIX))
#else
    #define CW_NAME_SPACE() CW_SPACE
    #define CW_NAME_AT_SPACE(name) CW_CC3EX(CW_SPACE, CW_SEPARATOR, name)
    #define CW_NAME_WITH_SPACE(space, name) CW_CC3EX(space, CW_SEPARATOR, name)
#endif
#define CW_SWITCH_NAME(_1, _2, name, ...) name
#define CW(...) CW_SWITCH_NAME(__VA_OPT__(__VA_ARGS__,) CW_NAME_WITH_SPACE, CW_NAME_AT_SPACE, CW_NAME_SPACE)(__VA_ARGS__)

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ---------------------------------- Smp ----------------------------------- */
#define CW_Smp CW_BUILD_SPACE(Smp)
#define CW_SPACE CW_Smp

typedef int32_t CW(Int32);
typedef int64_t CW(Int64);
typedef bool CW(Bool);
typedef const char* CW(String8);
typedef void CW(IObject);

#undef CW_SPACE


/* ----------------------------- Smp::Services ------------------------------ */
#define CW_Smp_Services CW_BUILD_SPACE(CW_Smp, Services)
#define CW_SPACE CW_Smp_Services

typedef CW(CW_Smp, Int32) CW(LogMessageKind);
typedef CW(CW_Smp, Int64) CW(EventId);
typedef void CW(ILogger);
typedef void CW(IEventManager);

#undef CW_SPACE


/* ------------------------- Smp::Services::ILogger ------------------------- */
#define CW_Smp_Services_ILogger CW_BUILD_SPACE(CW_Smp_Services, ILogger)
#undef CW_SPACE
#define CW_SPACE CW_Smp_Services_ILogger

CW(CW_Smp_Services, LogMessageKind) CW(QueryLogMessageKind) (CW()* self, CW(CW_Smp, String8) messageKindName);

void CW(Log) (CW()* self, const CW(CW_Smp, IObject)* sender, CW(CW_Smp, String8) message, CW(CW_Smp_Services, LogMessageKind) kind);

#undef CW_SPACE


/* ---------------------- Smp::Services::IEventManager ---------------------- */
#define CW_Smp_Services_IEventManager CW_BUILD_SPACE(CW_Smp_Services, IEventManager)
#undef CW_SPACE
#define CW_SPACE CW_Smp_Services_IEventManager

CW(CW_Smp_Services, EventId) CW(QueryEventId) (CW()* self, CW(CW_Smp, String8) eventName);

void CW(Emit) (CW()* self, CW(CW_Smp_Services, EventId) event, CW(CW_Smp, Bool) synchronous);

#undef CW_SPACE


#undef CW_Smp_Services_IEventManager
#undef CW_Smp_Services_ILogger
#undef CW_Smp_Services
#undef CW_Smp


#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* __CWrapper_H__ */
