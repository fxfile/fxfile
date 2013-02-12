//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FX_CFG_APPEARANCE_FILTERING_H__
#define __FX_CFG_APPEARANCE_FILTERING_H__
#pragma once

#include "../fxb/fxb_filter.h"

#include "CfgDlg.h"

class CfgAppearanceFilteringDlg : public CfgDlg
{
    typedef CfgDlg super;

public:
    CfgAppearanceFilteringDlg(void);

protected:
    virtual void onInit(Option::Config &aConfig);
    virtual void onApply(Option::Config &aConfig);

protected:
    xpr_sint_t insertFilterItem(xpr_sint_t aIndex, fxb::FilterItem *aFilterItem);
    xpr_sint_t addFilterItem(fxb::FilterItem *aFilterItem);
    void selectItem(xpr_sint_t aIndex);

protected:
    CImageList mImageList;
    CListCtrl  mListCtrl;

public:
    virtual xpr_bool_t PreTranslateMessage(MSG* pMsg);

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

protected:
    DECLARE_MESSAGE_MAP()
    virtual xpr_bool_t OnInitDialog(void);
    afx_msg void OnDestroy(void);
    afx_msg void OnListAdd(void);
    afx_msg void OnListModify(void);
    afx_msg void OnListDelete(void);
    afx_msg void OnItemUp(void);
    afx_msg void OnItemDown(void);
    afx_msg void OnDefault(void);
    afx_msg void OnCustomDrawList(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnDeleteitemList(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnItemActivateList(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnGetdispinfoList(NMHDR* pNMHDR, LRESULT* pResult);
};

#endif // __FX_CFG_APPEARANCE_FILTERING_H__
