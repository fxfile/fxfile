//
// Copyright (c) 2012-2013 Leon Lee author. All rights reserved.
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __XPR_FILE_SYS_H__
#define __XPR_FILE_SYS_H__
#pragma once

#include "xpr_config.h"
#include "xpr_types.h"
#include "xpr_dlsym.h"

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

class XPR_DL_API FileSys
{
public:
    static xpr_bool_t  exist(const xpr_char_t *aFilePath);
    static xpr_bool_t  exist(const xpr_wchar_t *aFilePath);

    static xpr_rcode_t mkdir(const xpr_char_t  *aDir);
    static xpr_rcode_t mkdir(const xpr_wchar_t *aDir);

    static xpr_rcode_t mkdir_recursive(const xpr_char_t  *aDir);
    static xpr_rcode_t mkdir_recursive(const xpr_wchar_t *aDir);

    static xpr_rcode_t rmdir(const xpr_char_t  *aDir);
    static xpr_rcode_t rmdir(const xpr_wchar_t *aDir);

    static xpr_rcode_t remove(const xpr_char_t  *aFilePath);
    static xpr_rcode_t remove(const xpr_wchar_t *aFilePath);

    static xpr_rcode_t rename(const xpr_char_t  *aOldFilePath, const xpr_char_t  *aNewFilePath);
    static xpr_rcode_t rename(const xpr_wchar_t *aOldFilePath, const xpr_wchar_t *aNewFilePath);

    static xpr_uint64_t getFileSize(const xpr_char_t  *aFilePath);
    static xpr_uint64_t getFileSize(const xpr_wchar_t *aFilePath);

    static xpr_rcode_t chdir(const xpr_char_t  *aCurDir);
    static xpr_rcode_t chdir(const xpr_wchar_t *aCurDir);

    static xpr_rcode_t getcwd(xpr_char_t  *aCurDir, xpr_size_t aMaxLen);
    static xpr_rcode_t getcwd(xpr_wchar_t *aCurDir, xpr_size_t aMaxLen);

    static xpr_rcode_t getExePath(xpr_char_t  *aExePath, xpr_size_t aMaxLen);
    static xpr_rcode_t getExePath(xpr_wchar_t *aExePath, xpr_size_t aMaxLen);

    static xpr_rcode_t getExeDir(xpr_char_t  *aExeDir, xpr_size_t aMaxLen);
    static xpr_rcode_t getExeDir(xpr_wchar_t *aExeDir, xpr_size_t aMaxLen);

    static xpr_rcode_t getTempDir(xpr_char_t  *aTempDir, xpr_size_t aMaxLen);
    static xpr_rcode_t getTempDir(xpr_wchar_t *aTempDir, xpr_size_t aMaxLen);
};
} // namespace xpr

#endif // __XPR_FILE_SYS_H__
