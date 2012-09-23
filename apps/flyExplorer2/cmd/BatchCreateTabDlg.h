//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FX_BATCH_CREATE_TAB_DLG_H__
#define __FX_BATCH_CREATE_TAB_DLG_H__
#pragma once

#include "rgc/ResizingDialog.h"

class BatchCreateTabDlg : public CResizingDialog
{
    friend class BatchCreateDlg;

    typedef CResizingDialog super;

public:
    BatchCreateTabDlg(xpr_uint_t aResourceId = 0);

protected:
    virtual void OnTabInit(void);

public:
    xpr_bool_t create(CWnd *sParentWnd);
    virtual xpr_bool_t PreTranslateMessage(MSG* pMsg);

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

protected:
    xpr_uint_t mResourceId;
    DECLARE_MESSAGE_MAP()
};

#endif // __FX_BATCH_CREATE_TAB_DLG_H__
