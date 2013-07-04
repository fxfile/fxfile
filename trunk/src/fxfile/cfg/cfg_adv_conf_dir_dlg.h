//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_CFG_ADV_CONF_DIR_DLG_H__
#define __FXFILE_CFG_ADV_CONF_DIR_DLG_H__ 1
#pragma once

#include "cfg_dlg.h"

namespace fxfile
{
namespace cfg
{
class CfgAdvConfDirDlg : public CfgDlg
{
    typedef CfgDlg super;

public:
    CfgAdvConfDirDlg(void);

public:
    virtual void onInit(Option::Config &aConfig);
    virtual void onApply(Option::Config &aConfig);

protected:
    void loadConfDir(const xpr_tchar_t *aConfDir);

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual xpr_bool_t PreTranslateMessage(MSG* pMsg);

protected:
    DECLARE_MESSAGE_MAP()
    virtual xpr_bool_t OnInitDialog(void);
    afx_msg void OnSetPath(void);
    afx_msg void OnCustomPathBrowse(void);
    afx_msg void OnConfDir(void);
};
} // namespace cfg
} // namespace fxfile

#endif // __FXFILE_CFG_ADV_CONF_DIR_DLG_H__
