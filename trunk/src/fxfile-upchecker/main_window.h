//
// Copyright (c) 2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_UPCHECKER_MAIN_WINDOW_H__
#define __FXFILE_UPCHECKER_MAIN_WINDOW_H__ 1
#pragma once

#include "upchecker_def.h"

namespace fxfile
{
namespace base
{
class UpdateInfoManager;
} // namespace base

using namespace base;

namespace upchecker
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
        xpr_sint_t mEnable;
        xpr_sint_t mCheckPeriodTime; // unit: hour
    };

    Config mConfig;

    xpr::string        mConfHomeDir;

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
} // namespace upchecker

#endif // __FXFILE_UPCHECKER_MAIN_WINDOW_H__
