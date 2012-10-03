//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FX_INPUT_DLG_H__
#define __FX_INPUT_DLG_H__
#pragma once

#include "rgc/ResizingDialog.h"

class DlgState;

class InputDlg : public CResizingDialog
{
    typedef CResizingDialog super;

public:
    InputDlg(xpr_sint_t aDlgWidth = -1);
    virtual ~InputDlg(void);

public:
    void setProfile(const xpr_tchar_t *aProfile);
    void setTitle(const xpr_tchar_t *aTitle);
    void setDesc(const xpr_tchar_t *aDesc);
    void setText(const xpr_tchar_t *aText);
    void setFileRename(xpr_bool_t aFileRename = XPR_TRUE);
    void setCheckEmpty(xpr_bool_t aCheckEmpty = XPR_TRUE);
    void setLimitText(xpr_sint_t aLimitText);

    const xpr_tchar_t *getText(void);

protected:
    std::tstring mProfile;
    std::tstring mTitle;
    std::tstring mDesc;
    xpr_tchar_t mText[1024];

    xpr_sint_t mDlgWidth;
    xpr_bool_t mFileRename;
    xpr_bool_t mCheckEmpty;
    xpr_sint_t mLimitText;

protected:
    HICON     mIcon;
    DlgState *mDlgState;

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

protected:
    DECLARE_MESSAGE_MAP()
    virtual void OnOK(void);
    virtual xpr_bool_t OnInitDialog(void);
    afx_msg void OnDestroy(void);
};

#endif // __FX_INPUT_DLG_H__
