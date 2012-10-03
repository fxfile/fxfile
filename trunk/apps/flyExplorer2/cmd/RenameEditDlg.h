//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FX_RENAME_EDIT_DLG_H__
#define __FX_RENAME_EDIT_DLG_H__
#pragma once

#include "rgc/ResizingDialog.h"
#include "rgc/EditScroll.h"

class DlgState;

class RenameEditDlg : public CResizingDialog
{
    typedef CResizingDialog super;

public:
    RenameEditDlg(void);
    virtual ~RenameEditDlg(void);

public:
    void add(const xpr_tchar_t *aOld, const xpr_tchar_t *aNew);
    xpr_size_t getNewCount(void);
    const xpr_tchar_t *getNewName(xpr_size_t aIndex);

protected:
    std::deque<xpr_tchar_t *> mOldList;
    std::deque<xpr_tchar_t *> mNewList;
    std::deque<xpr_tchar_t *> mEditedList;

    xpr_bool_t mSyncScroll;

    xpr_bool_t mOldEditScroll;
    xpr_bool_t mNewEditScroll;

    xpr_sint_t mPrevOldVert;
    xpr_sint_t mPrevNewVert;

    xpr_sint_t mWidth1;
    xpr_sint_t mWidth2;
    xpr_sint_t mHeight1;

protected:
    HICON       mIcon;
    DlgState   *mDlgState;
    EditScroll  mNewEditCtrl;
    EditScroll  mOldEditCtrl;

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

protected:
    DECLARE_MESSAGE_MAP()
    virtual xpr_bool_t OnInitDialog(void);
    virtual void OnOK(void);
    afx_msg void OnVscrollSrnNewEdit(void);
    afx_msg void OnDestroy(void);
    afx_msg void OnVscrollSrnOldEdit(void);
    afx_msg void OnSyncScroll(void);
    afx_msg void OnSize(xpr_uint_t nType, xpr_sint_t cx, xpr_sint_t cy);
};

#endif // __FX_RENAME_EDIT_DLG_H__
