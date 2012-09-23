//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FX_REN_TAB_DLG_H__
#define __FX_REN_TAB_DLG_H__
#pragma once

#include "rgc/ResizingDialog.h"

#define WM_BATCH_RENAME_APPLY (WM_USER+500)

class RenTabDlg : public CResizingDialog
{
    typedef CResizingDialog super;

public:
    RenTabDlg(xpr_uint_t aResourceId);

public:
    virtual void OnTabInit(void);

public:
    xpr_bool_t Create(CWnd* pParentWnd);
    virtual xpr_bool_t PreTranslateMessage(MSG* pMsg);

protected:
    xpr_uint_t mResourceId;

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

protected:
    DECLARE_MESSAGE_MAP()
};

#endif // __FX_REN_TAB_DLG_H__
