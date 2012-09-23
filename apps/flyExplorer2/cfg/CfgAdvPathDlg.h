//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FX_CFG_ADV_PATH_DLG_H__
#define __FX_CFG_ADV_PATH_DLG_H__
#pragma once

#include "CfgDlg.h"

class CfgAdvPathDlg : public CfgDlg
{
    typedef CfgDlg super;

public:
    CfgAdvPathDlg(void);

public:
    virtual void OnApply(void);

protected:
    typedef struct Item
    {
        xpr_sint_t   mType;
        std::tstring mPath;
    } Item;

protected:
    CListCtrl  mListCtrl;
    CImageList mImageList;

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual xpr_bool_t PreTranslateMessage(MSG* pMsg);

protected:
    DECLARE_MESSAGE_MAP()
    virtual xpr_bool_t OnInitDialog(void);
    afx_msg xpr_bool_t OnCommand(WPARAM wParam, LPARAM lParam);
    afx_msg void OnLvnGetdispinfoList(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnLvnItemActivate(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnLvnDeleteitemList(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnSetPath(void);
    afx_msg void OnCustomPathBrowse(void);
    afx_msg void OnCfgPath(void);
};

#endif // __FX_CFG_ADV_PATH_DLG_H__
