//
// Copyright (c) 2012-2013 Leon Lee author. All rights reserved.
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __XPR_FILE_SYS_H__
#define __XPR_FILE_SYS_H__
#pragma once

#include "xpr_config.h"

namespace xpr
{
#if defined(XPR_CFG_OS_WINDOWS)
#define XPR_MAX_PATH     MAX_PATH
#define XPR_MAX_FILENAME _MAX_FNAME
#define XPR_MAX_ARG      INFOTIPSIZE
#elif defined(XPR_CFG_OS_LINUX)
#define XPR_MAX_PATH     PATH_MAX
#define XPR_MAX_FILENAME FILENAME_MAX
#define XPR_MAX_ARG      ARG_MAX
#elif defined(XPR_CFG_OS_POSIX)
#define XPR_MAX_PATH     _POSIX_PATH_MAX
#define XPR_MAX_FILENAME _POSIX_FILENAME_MAX
#define XPR_MAX_ARG      ARG_MAX
#else // not XPR_CFG_OS_WINDOWS, not XPR_CFG_OS_LINUX, not XPR_CFG_OS_POSIX
#error Please, support for your OS. <xpr_file_sys.h>
#endif

#if defined(XPR_CFG_OS_WINDOWS)
#define XPR_FILE_SEPARATOR '\\'
#define XPR_PATH_SEPARATOR ';'
#elif defined(XPR_CFG_OS_POSIX)
#define XPR_FILE_SEPARATOR '/'
#define XPR_PATH_SEPARATOR ':'
#else // not XPR_CFG_OS_WINDOWS, not XPR_CFG_OS_POSIX
#error Please, support for your OS. <xpr_file_sys.h>
#endif
} // namespace xpr

#endif // __XPR_FILE_SYS_H__
