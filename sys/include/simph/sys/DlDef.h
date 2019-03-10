/*
 * @file DlDef.h
 *
 * Copyright 2019 . All rights reserved.
 * Use is subject to license terms.
 *
 * $Id$
 * $Date$
 */
#ifndef __simph_sys_DlDef_HPP__
#define __simph_sys_DlDef_HPP__

#ifdef WIN32
#define SHARED_FUNCTION __declspec (dllexport)
#else
#define SHARED_FUNCTION
#endif

#endif // __simph_sys_DlDef_HPP__
