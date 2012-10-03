//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FX_FILE_SCRAP_DLG_H__
#define __FX_FILE_SCRAP_DLG_H__
#pragma once

#include "fxb/fxb_file_scrap.h"

#include "rgc/ResizingDialog.h"
#include "rgc/DropMenuButton.h"
#include "rgc/BCMenu.h"

class DlgState;

class FileScrapDlg : public CResizingDialog
{
    typedef CResizingDialog super;

public:
    FileScrapDlg(void);
    virtual ~FileScrapDlg(void);

protected:
    xpr_sint_t addGroup(fxb::FileScrap::Group *aGroup, xpr_sint_t aIndex = -1);
    fxb::FileScrap::Group *getCurGroup(void);

    void initList(fxb::FileScrap::Group *aGroup, xpr_bool_t aRedraw);
    void updateDefGroup(void);
    void updateStatus(void);
    xpr_bool_t browse(xpr_tchar_t *aTarget);
    xpr_bool_t operate(fxb::FileScrap::Group *aGroup, xpr_sint_t aFunc);

protected:
    CBrush     mBrush;
    CImageList mGroupImgList;
    BCMenu     mMenu;

protected:
    HICON            mIcon;
    DlgState        *mDlgState;
    CListCtrl        mListCtrl;
    CComboBoxEx      mGroupWnd;
    CDropMenuButton  mNewButton;

protected:
    virtual void DoDataExchange(CDataExchange *aDX);

protected:
    DECLARE_MESSAGE_MAP()
    virtual xpr_bool_t OnInitDialog(void);
    afx_msg HCURSOR OnQueryDragIcon(void);
    afx_msg void OnInitMenuPopup(CMenu *aPopupMenu, xpr_uint_t aIndex, xpr_bool_t aSysMenu);
    afx_msg LRESULT OnMenuChar(xpr_uint_t aChar, xpr_uint_t aFlags, CMenu *aMenu);
    afx_msg void OnMeasureItem(xpr_sint_t aIdCtl, LPMEASUREITEMSTRUCT aMeasureItemStruct);
    afx_msg void OnPaint(void);
    afx_msg HBRUSH OnCtlColor(CDC *aDC, CWnd *aWnd, xpr_uint_t aCtlColor);
    afx_msg void OnCopyToFolder(void);
    afx_msg void OnMoveToFolder(void);
    afx_msg void OnDelete(void);
    afx_msg void OnClear(void);
    afx_msg void OnClearAll(void);
    afx_msg void OnValidate(void);
    afx_msg void OnDestroy(void);
    afx_msg void OnGetdispinfoList(NMHDR *aNmHdr, LRESULT *aResult);
    afx_msg void OnDeleteItem(NMHDR *aNmHdr, LRESULT *aResult);
    afx_msg void OnGroupNew(void);
    afx_msg void OnGroupModify(void);
    afx_msg void OnGroupDelete(void);
    afx_msg void OnGroupDefault(void);
    afx_msg void OnUpdateGroupModify(CCmdUI *aCmdUI);
    afx_msg void OnUpdateGroupDelete(CCmdUI *aCmdUI);
    afx_msg void OnCbnSelchangeGroup(void);
};

#endif // __FX_FILE_SCRAP_DLG_H__
