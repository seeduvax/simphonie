/*
 * @file DlDef.h
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simphonie_sys_DlDef_HPP__
#define __simphonie_sys_DlDef_HPP__

#ifdef WIN32
#define SHARED_FUNCTION __declspec (dllexport)
#else
#define SHARED_FUNCTION
#endif

#endif // __simphonie_sys_DlDef_HPP__
