//
// Copyright (c) 2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_UPDATER_MAIN_WND_H__
#define __FXFILE_UPDATER_MAIN_WND_H__ 1
#pragma once

#include "updater_def.h"

namespace fxfile
{
namespace base
{
class UpdateInfoManager;
} // namespace base

using namespace base;

namespace updater
{
class UpdateChecker;

class MainWindow : public CWnd
{
public:
    MainWindow(void);
    virtual ~MainWindow(void);

public:
    BOOL Create(void);

protected:
    xpr_bool_t registerWindowClass(void);

    xpr_bool_t getConfHomeDir(xpr::string &aDir);
    xpr_bool_t getConfHomeDirFromConfFile(const xpr::string &aConfPath, xpr::string &aDir);
    xpr_bool_t loadConfig(void);

    xpr_bool_t getUpdateHomeDir(xpr::string &aUpdateHomeDir) const;

protected:
    struct Config
    {
        xpr_sint_t  mEnable;
        xpr::string mCurVer;
        xpr_sint_t  mCheckMinorVer;
        xpr_sint_t  mCheckPeriodTime; // unit: hour
    };

    Config mConfig;

    UpdateChecker     *mUpdateChecker;
    UpdateInfoManager *mUpdateInfoManager;
    UpdateInfo         mUpdateInfo;

protected:
    DECLARE_MESSAGE_MAP()
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnDestroy(void);
    afx_msg void OnTimer(UINT_PTR aIdEvent);
    afx_msg LRESULT OnCommandReloadConf(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnCommandCheckNow(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnCommandExit(WPARAM wParam, LPARAM lParam);
};
} // namespace fxfile
} // namespace updater

#endif // __FXFILE_UPDATER_MAIN_WND_H__
