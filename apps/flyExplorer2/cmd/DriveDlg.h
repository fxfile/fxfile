//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FX_DRIVE_DLG_H__
#define __FX_DRIVE_DLG_H__
#pragma once

#include "rgc/ResizingDialog.h"
#include "DlgState.h"

class DriveDlg : public CResizingDialog
{
    typedef CResizingDialog super;

public:
    DriveDlg(void);
    xpr_tchar_t getDrive(void) { return mDrive; };

protected:
    void addDrive(xpr_tchar_t *aDrive);
    HTREEITEM getDriveItem(xpr_tchar_t aDrive);
    xpr_tchar_t getDrive(HTREEITEM aTreeItem);

protected:
    CToolBar mToolBar;
    CImageList mImgList;

    xpr_tchar_t mDrive;

protected:
    DlgState mState;
    CTreeCtrl mTreeCtrl;

public:
    virtual xpr_bool_t PreTranslateMessage(MSG* pMsg);

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

protected:
    DECLARE_MESSAGE_MAP()
    virtual xpr_bool_t OnInitDialog(void);
    virtual void OnOK(void);
    afx_msg void OnSize(xpr_uint_t nType, xpr_sint_t cx, xpr_sint_t cy);
    afx_msg void OnRefresh(void);
    afx_msg void OnDblclkDrvTree(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnRclickDrvTree(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnDestroy(void);
};

#endif // __FX_DRIVE_DLG_H__
