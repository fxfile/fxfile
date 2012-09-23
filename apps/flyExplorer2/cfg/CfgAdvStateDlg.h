//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FX_CFG_ADV_STATE_DLG_H__
#define __FX_CFG_ADV_STATE_DLG_H__
#pragma once

#include "CfgDlg.h"

class CfgAdvStateDlg : public CfgDlg
{
public:
    CfgAdvStateDlg(void);

protected:
    virtual void OnApply(void);

protected:
    CListCtrl  mListCtrl;
    CImageList mImageList;

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual xpr_bool_t PreTranslateMessage(MSG* pMsg);

protected:
    DECLARE_MESSAGE_MAP()
    virtual xpr_bool_t OnInitDialog(void);
    afx_msg void OnLvnGetdispinfoList(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnInit(void);
    afx_msg void OnInitAll(void);
};

#endif // __FX_CFG_ADV_STATE_DLG_H__
