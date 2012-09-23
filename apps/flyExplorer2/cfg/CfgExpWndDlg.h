//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FX_CFG_EXP_WND_1_DLG_H__
#define __FX_CFG_EXP_WND_1_DLG_H__
#pragma once

#include "CfgDlg.h"
#include "../rgc/ColourPickerXP.h"

class CfgExpWndDlg : public CfgDlg
{
    typedef CfgDlg super;

public:
    CfgExpWndDlg(xpr_size_t aIndex);

protected:
    virtual void OnApply(void);

    void setActive(xpr_bool_t aActive);

protected:
    CColourPickerXP mTextColorCtrl;
    CColourPickerXP mBkgndColorCtrl;
    xpr_size_t mIndex;

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual xpr_bool_t OnCommand(WPARAM wParam, LPARAM lParam);

protected:
    DECLARE_MESSAGE_MAP()
    afx_msg void OnCustomizeFolder(void);
    afx_msg void OnBkgndImageBrowse(void);
    afx_msg void OnInitFolderBrowse(void);
    virtual xpr_bool_t OnInitDialog(void);
    afx_msg void OnInitFolder(void);
    afx_msg void OnInitFolderNone(void);
    afx_msg void OnInitLastFolder(void);
    afx_msg void OnNoLastNetFolder(void);
    afx_msg LRESULT OnSelEndOK(WPARAM wParam, LPARAM lParam);
};

#endif // __FX_CFG_EXP_WND_1_DLG_H__
