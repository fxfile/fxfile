//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_BATCH_RENAME_TAB_FORMAT_DLG_H__
#define __FXFILE_BATCH_RENAME_TAB_FORMAT_DLG_H__ 1
#pragma once

#include "gui/BCMenu.h"

#include "batch_rename_tip_dlg.h"
#include "batch_rename_tab_dlg.h"

namespace fxfile
{
class DlgState;

namespace cmd
{
class BatchRenameTabFormatDlg : public BatchRenameTabDlg
{
    typedef BatchRenameTabDlg super;

public:
    BatchRenameTabFormatDlg(void);

public:
    virtual void OnTabInit(void);

public:
    RenameTipDlg *mTipDlg;

protected:
    DlgState     *mDlgState;
    BCMenu        mMenu;
    CToolTipCtrl  mToolTipCtrl;

public:
    virtual xpr_bool_t PreTranslateMessage(MSG* pMsg);

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual xpr_bool_t OnCommand(WPARAM wParam, LPARAM lParam);

protected:
    virtual xpr_bool_t OnInitDialog(void);
    afx_msg void OnApply(void);
    afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, xpr_uint_t nIndex, xpr_bool_t bSysMenu);
    afx_msg void OnMeasureItem(xpr_sint_t nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
    afx_msg LRESULT OnMenuChar(xpr_uint_t nChar, xpr_uint_t nFlags, CMenu* pMenu);
    afx_msg void OnFormatDesc(void);
    afx_msg void OnDestroy(void);
    afx_msg void OnFormatMenu(void);
    DECLARE_MESSAGE_MAP()
};
} // namespace cmd
} // namespace fxfile

#endif // __FXFILE_BATCH_RENAME_TAB_FORMAT_DLG_H__
