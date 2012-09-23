//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FX_CFG_FUNC_FILE_ASS_H__
#define __FX_CFG_FUNC_FILE_ASS_H__
#pragma once

#include "../fxb/fxb_file_ass.h"

#include "CfgDlg.h"

class CfgFuncFileAssDlg : public CfgDlg
{
    typedef CfgDlg super;

public:
    CfgFuncFileAssDlg(void);

protected:
    xpr_sint_t insertFileAssItem(xpr_sint_t aIndex, fxb::FileAssItem *aFileAssItem);
    xpr_sint_t addFileAssItem(fxb::FileAssItem *aFileAssItem);
    void selectItem(xpr_sint_t aIndex);

protected:
    virtual void OnApply(void);

protected:
    CListCtrl mListCtrl;

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual xpr_bool_t OnCommand(WPARAM wParam, LPARAM lParam);

public:
    virtual xpr_bool_t PreTranslateMessage(MSG* pMsg);

protected:
    DECLARE_MESSAGE_MAP()
    virtual xpr_bool_t OnInitDialog(void);
    afx_msg void OnItemAdd(void);
    afx_msg void OnItemModify(void);
    afx_msg void OnItemDelete(void);
    afx_msg void OnItemUp(void);
    afx_msg void OnItemDown(void);
    afx_msg void OnGetdispinfoList(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnDeleteitemList(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnItemActivateList(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnDefault(void);
};

#endif // __FX_CFG_FUNC_FILE_ASS_H__
