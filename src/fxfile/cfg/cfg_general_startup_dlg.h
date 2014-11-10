//
// Copyright (c) 2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_CFG_GENERAL_STARTUP_DLG_H__
#define __FXFILE_CFG_GENERAL_STARTUP_DLG_H__ 1
#pragma once

#include "../option.h"
#include "cfg_dlg.h"

namespace fxfile
{
namespace cfg
{
class CfgGeneralStartupViewDlg;

class CfgGeneralStartupDlg : public CfgDlg
{
    typedef CfgDlg super;

public:
    CfgGeneralStartupDlg(void);

protected:
    void showTabDlg(xpr_sint_t aViewIndex);

protected:
    // from CfgDlg
    virtual void onInit(const Option::Config &aConfig);
    virtual void onApply(Option::Config &aConfig);

private:
    struct ViewStartup
    {
        xpr_sint_t  mFileListInitFolderType;
        xpr_bool_t  mFileListNoNetLastFolder;
        xpr::string mFileListInitFolder;
    };

    void loadViewStartup(const ViewStartup &aViewStartup);
    void saveViewStartup(ViewStartup &aViewStartup);
    void saveViewStartup(void);

private:
    xpr_sint_t mOldViewIndex;

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

protected:
    DECLARE_MESSAGE_MAP()
    virtual xpr_bool_t OnInitDialog(void);
    afx_msg void OnDestroy(void);
    afx_msg void OnSelChangeView(void);
    afx_msg void OnApplyAll(void);
    afx_msg void OnFolderBrowse(void);
};
} // namespace cfg
} // namespace fxfile

#endif // __FXFILE_CFG_GENERAL_STARTUP_DLG_H__
