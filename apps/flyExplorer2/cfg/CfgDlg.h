//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FX_CFG_DLG_H__
#define __FX_CFG_DLG_H__
#pragma once

#include "../OptionMgr.h"

#define WM_APPLY       (WM_USER + 50)
#define WM_SETMODIFIED (WM_USER + 51)
#define WM_GETMODIFIED (WM_USER + 52)

class CfgDlg : public CDialog
{
    typedef CDialog super;

public:
    CfgDlg(xpr_uint_t aResourceId = 0, CWnd *sParentWnd = XPR_NULL);

public:
    xpr_bool_t getModified(void);
    void setModified(xpr_bool_t aModified = XPR_TRUE);
    virtual void OnApply(void);

private:
    xpr_uint_t mResourceId;
    xpr_sint_t mIndex;

public:
    virtual xpr_bool_t Create(xpr_uint_t aIndex, CWnd *aParentWnd);
    virtual xpr_bool_t PreTranslateMessage(MSG* pMsg);

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

protected:
    DECLARE_MESSAGE_MAP()
    afx_msg LRESULT OnMsgApply(WPARAM wParam, LPARAM lParam);
};

#endif // __FX_CFG_DLG_H__
