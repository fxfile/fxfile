//
// Copyright (c) 2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_ABOUT_TAB_INFO_DLG_H__
#define __FXFILE_ABOUT_TAB_INFO_DLG_H__ 1
#pragma once

#include "about_tab_dlg.h"

namespace fxfile
{
namespace base
{
class UpdateInfoManager;
} // namespace base
} // namespace fxfile

namespace fxfile
{
namespace cmd
{
class AboutTabInfoDlg : public AboutTabDlg
{
    typedef AboutTabDlg super;

public:
    AboutTabInfoDlg(void);

protected:
    fxfile::base::UpdateInfoManager *mUpdateInfoManager;

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

protected:
    DECLARE_MESSAGE_MAP()
    virtual xpr_bool_t OnInitDialog(void);
    afx_msg void OnDestroy(void);
    afx_msg void OnTimer(UINT_PTR aIdEvent);
    afx_msg void OnReport(void);
    afx_msg void OnSystem(void);
    afx_msg void OnDownload(void);
};
} // namespace cmd
} // namespace fxfile

#endif // __FXFILE_ABOUT_TAB_INFO_DLG_H__
