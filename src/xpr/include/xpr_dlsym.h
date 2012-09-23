//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __XPR_DLSYM_H__
#define __XPR_DLSYM_H__
#pragma once

namespace xpr
{
#if defined(XPR_CFG_OS_WINDOWS) && !defined(XPR_CFG_DL_STATIC)
#define XPR_DL_EXPORT __declspec(dllexport)
#define XPR_DL_IMPORT __declspec(dllimport)
#if defined(XPR_CFG_DL_EXPORT)
#define XPR_DL_API XPR_DL_EXPORT
#else // not XPR_CFG_DL_EXPORT
#define XPR_DL_API XPR_DL_IMPORT
#endif
#else // not XPR_CFG_OS_WINDOWS || XPR_CFG_DL_STATIC
#define XPR_DL_EXPORT
#define XPR_DL_IMPORT
#define XPR_DL_API
#endif

#if defined(XPR_CFG_OS_WINDOWS)
#define XPR_DL_FILE_EXT ".dll"
#elif defined(XPR_CFG_OS_LINUX) || defined(XPR_CFG_OS_SOLARIS) || defined(XPR_CFG_OS_TRU64)
#define XPR_DL_FILE_EXT ".so"
#elif defined(XPR_CFG_OS_MACOSX)
#define XPR_DL_FILE_EXT ".dylib"
#elif defined(XPR_CFG_OS_HPUX)
#define XPR_DL_FILE_EXT ".sl"
#else // not XPR_CFG_OS_WINDOWS, XPR_CFG_OS_LINUX, XPR_CFG_OS_SOLARIS, XPR_CFG_OS_TRU64, XPR_CFG_OS_MACOSX, XPR_CFG_OS_HPUX
#error Please, support for your OS. <xpr_dlsym.h>
#endif
} // namespace xpr

#endif // __XPR_DLSYM_H__
