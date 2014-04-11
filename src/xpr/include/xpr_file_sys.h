//
// Copyright (c) 2012-2013 Leon Lee author. All rights reserved.
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __XPR_FILE_SYS_H__
#define __XPR_FILE_SYS_H__ 1
#pragma once

#include "xpr_config.h"
#include "xpr_types.h"
#include "xpr_dlsym.h"
#include "xpr_string.h"

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
#define XPR_FILE_SEPARATOR                  '\\'
#define XPR_FILE_SEPARATOR_MBCS_STRING      "\\"
#define XPR_FILE_SEPARATOR_WIDE_STRING     L"\\"
#define XPR_PATH_SEPARATOR                  ';'
#define XPR_PATH_SEPARATOR_MBCS_STRING      ";"
#define XPR_PATH_SEPARATOR_WIDE_STRING     L";"
#define XPR_DRIVE_SEPARATOR                 ':'
#define XPR_DRIVE_SEPARATOR_MBCS_STRING     ":"
#define XPR_DRIVE_SEPARATOR_WIDE_STRING    L":"
#elif defined(XPR_CFG_OS_POSIX)
#define XPR_FILE_SEPARATOR                  '/'
#define XPR_FILE_SEPARATOR_MBCS_STRING      "/"
#define XPR_FILE_SEPARATOR_WIDE_STRING     L"/"
#define XPR_PATH_SEPARATOR                  ':'
#define XPR_PATH_SEPARATOR_MBCS_STRING      ":"
#define XPR_PATH_SEPARATOR_WIDE_STRING     L":"
#else // not XPR_CFG_OS_WINDOWS, not XPR_CFG_OS_POSIX
#error Please, support for your OS. <xpr_file_sys.h>
#endif
#define XPR_FILE_EXT_SEPARATOR              '.'
#define XPR_FILE_EXT_SEPARATOR_MBCS_STRING  "."
#define XPR_FILE_EXT_SEPARATOR_WIDE_STRING L"."

#if defined(XPR_CFG_UNICODE)
#define XPR_FILE_SEPARATOR_STRING     XPR_FILE_SEPARATOR_WIDE_STRING
#define XPR_FILE_EXT_SEPARATOR_STRING XPR_FILE_EXT_SEPARATOR_WIDE_STRING
#define XPR_PATH_SEPARATOR_STRING     XPR_PATH_SEPARATOR_WIDE_STRING
#if defined(XPR_CFG_OS_WINDOWS)
#define XPR_DRIVE_SEPARATOR_STRING    XPR_DRIVE_SEPARATOR_WIDE_STRING
#endif
#else
#define XPR_FILE_SEPARATOR_STRING     XPR_FILE_SEPARATOR_MBCS_STRING
#define XPR_FILE_EXT_SEPARATOR_STRING XPR_FILE_EXT_SEPARATOR_MBCS_STRING
#define XPR_PATH_SEPARATOR_STRING     XPR_PATH_SEPARATOR_MBCS_STRING
#if defined(XPR_CFG_OS_WINDOWS)
#define XPR_DRIVE_SEPARATOR_STRING    XPR_DRIVE_SEPARATOR_MBCS_STRING
#endif
#endif

#define XPR_MBCS_FILE_PATH __FILE__
#define XPR_WIDE_FILE_PATH XPR_WIDEN(__FILE__)
#define XPR_MBCS_FILE_NAME (strrchr(XPR_MBCS_FILE_PATH, XPR_FILE_SEPARATOR)+1)
#define XPR_WIDE_FILE_NAME (wcsrchr(XPR_WIDE_FILE_PATH, XPR_FILE_SEPARATOR)+1)
#if defined(XPR_CFG_UNICODE)
#define XPR_FILE_PATH XPR_WIDE_FILE_PATH
#define XPR_FILE_NAME XPR_WIDE_FILE_NAME
#else // not XPR_CFG_UNICODE
#define XPR_FILE_PATH XPR_MBCS_FILE_PATH
#define XPR_FILE_NAME XPR_MBCS_FILE_NAME
#endif

class XPR_DL_API FileSys
{
public:
    static xpr_bool_t  exist(const xpr::string &aFilePath);
    static xpr_rcode_t mkdir(const xpr::string &aDir);
    static xpr_rcode_t mkdir_recursive(const xpr::string &aDir);
    static xpr_rcode_t rmdir(const xpr::string &aDir);
    static xpr_rcode_t remove(const xpr::string &aFilePath);
    static xpr_rcode_t rename(const xpr::string &aOldFilePath, const xpr::string &aNewFilePath);
    static xpr_uint64_t getFileSize(const xpr::string &aFilePath);
    static xpr_rcode_t chdir(const xpr::string &aCurDir);
    static xpr_rcode_t getcwd(xpr::string &aCurDir);
    static xpr_rcode_t getExePath(xpr::string &aExePath);
    static xpr_rcode_t getExeDir(xpr::string &aExeDir);
    static xpr_rcode_t getTempDir(xpr::string &aTempDir);
};
} // namespace xpr

#endif // __XPR_FILE_SYS_H__
