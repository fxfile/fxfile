//
// Copyright (c) 2013-2014 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_BASE_UPDATE_CHECKER_DEF_H__
#define __FXFILE_BASE_UPDATE_CHECKER_DEF_H__ 1
#pragma once

namespace fxfile
{
namespace base
{
const xpr_tchar_t kUpcheckerClassName[] = XPR_STRING_LITERAL("fxfile-upchecker");
const xpr_tchar_t kUpdateRootDir     [] = XPR_STRING_LITERAL("update");
const xpr_tchar_t kUpdateKeyFileName [] = XPR_STRING_LITERAL(".key");

const xpr_uint_t WM_UPCHECKER_COMMAND_RELOAD_CONF = ::RegisterWindowMessage(XPR_STRING_LITERAL("fxfile-upchecker: reload_conf"));
const xpr_uint_t WM_UPCHECKER_COMMAND_CHECK_NOW   = ::RegisterWindowMessage(XPR_STRING_LITERAL("fxfile-upchecker: check_now"));
const xpr_uint_t WM_UPCHECKER_COMMAND_EXIT        = ::RegisterWindowMessage(XPR_STRING_LITERAL("fxfile-upchecker: exit"));

const xpr_tchar_t kUpcheckerConfKeyEnable         [] = XPR_STRING_LITERAL("config.update_check.enable");
const xpr_tchar_t kUpcheckerConfKeyCheckPeriodTime[] = XPR_STRING_LITERAL("config.update_check.check_period_time");

#define FXFILE_UPDATE_MAX_VERSION_LEN   (50)
#define FXFILE_UPDATE_MAX_FILE_PATH_LEN (XPR_MAX_PATH)

enum
{
    kUpdateStatusUnknown = 0,
    kUpdateStatusCheckInProgress,
    kUpdateStatusCheckFailed,
    kUpdateStatusChecked,
};

#pragma pack(1)
struct UpdateInfo
{
    xpr_sint32_t mStatus;
    xpr_sint32_t mMajorVer;
    xpr_sint32_t mMinorVer;
    xpr_sint32_t mPatchVer;
    xpr_wchar_t  mNewUrl[XPR_MAX_URL_LENGTH + 1];
};
#pragma pack()
} // namespace base
} // namespace fxfile

#endif // __FXFILE_BASE_UPDATE_CHECKER_DEF_H__
