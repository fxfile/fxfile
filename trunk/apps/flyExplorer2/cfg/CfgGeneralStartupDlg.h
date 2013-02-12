//
// Copyright (c) 2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FX_CFG_GENERAL_STARTUP_DLG_H__
#define __FX_CFG_GENERAL_STARTUP_DLG_H__
#pragma once

#include "../Option.h"
#include "CfgDlg.h"

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
    virtual void onInit(Option::Config &aConfig);
    virtual void onApply(Option::Config &aConfig);

protected:
    CTabCtrl mTabCtrl;
    CfgGeneralStartupViewDlg *mViewDlg[MAX_VIEW_SPLIT];
    xpr_sint_t                mOldViewIndex;

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

protected:
    DECLARE_MESSAGE_MAP()
    virtual xpr_bool_t OnInitDialog(void);
    afx_msg void OnDestroy(void);
    afx_msg void OnSelChangeTab(NMHDR *aNMHDR, LRESULT *aResult);
};

#endif // __FX_CFG_GENERAL_STARTUP_DLG_H__
