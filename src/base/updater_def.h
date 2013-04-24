//
// Copyright (c) 2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_BASE_UPDATER_DEF_H__
#define __FXFILE_BASE_UPDATER_DEF_H__ 1
#pragma once

namespace fxfile
{
namespace base
{
const xpr_tchar_t kUpdaterClassName [] = XPR_STRING_LITERAL("fxfile-updater");
const xpr_tchar_t kUpdateRootDir    [] = XPR_STRING_LITERAL("update");
const xpr_tchar_t kUpdateKeyFileName[] = XPR_STRING_LITERAL(".key");

const xpr_uint_t WM_UPDATER_COMMAND_RELOAD_CONF = ::RegisterWindowMessage(XPR_STRING_LITERAL("fxfile-updater: reload_conf"));
const xpr_uint_t WM_UPDATER_COMMAND_CHECK_NOW   = ::RegisterWindowMessage(XPR_STRING_LITERAL("fxfile-updater: check_now"));
const xpr_uint_t WM_UPDATER_COMMAND_EXIT        = ::RegisterWindowMessage(XPR_STRING_LITERAL("fxfile-updater: exit"));

const xpr_tchar_t kUpdaterConfKeyEnable         [] = XPR_STRING_LITERAL("config.update_check.enable");
const xpr_tchar_t kUpdaterConfKeyCurVer         [] = XPR_STRING_LITERAL("config.update_check.current_version");
const xpr_tchar_t kUpdaterConfKeyCheckMinorVer  [] = XPR_STRING_LITERAL("config.update_check.check_minor_version");
const xpr_tchar_t kUpdaterConfKeyCheckPeriodTime[] = XPR_STRING_LITERAL("config.update_check.check_period_time");

#define FXFILE_UPDATE_MAX_VERSION_LEN   (50)
#define FXFILE_UPDATE_MAX_FILE_PATH_LEN (XPR_MAX_PATH)

enum
{
    kUpdateStatusUnknown = 0,
    kUpdateStatusCheckInProgress,
    kUpdateStatusExistNewVer,
    kUpdateStatusLastestVer,
    kUpdateStatusCheckFailed,
    kUpdateStatusDownloading,
    kUpdateStatusDownloaded,
    kUpdateStatusDownloadFailed,
};

#pragma pack(1)
struct UpdateInfo
{
    xpr_sint32_t mStatus;
    xpr_wchar_t  mCheckedVersion[FXFILE_UPDATE_MAX_VERSION_LEN + 1];
    xpr_wchar_t  mDownloadedFilePath[FXFILE_UPDATE_MAX_FILE_PATH_LEN + 1];
    xpr_sint32_t mVerified;
};
#pragma pack()
} // namespace base
} // namespace fxfile

#endif // __FXFILE_BASE_UPDATER_DEF_H__
