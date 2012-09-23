//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FX_BATCH_CREATE_TEXT_DLG_H__
#define __FX_BATCH_CREATE_TEXT_DLG_H__
#pragma once

#include "BatchCreateTabDlg.h"
#include "resource.h"

class BatchCreateTextDlg : public BatchCreateTabDlg
{
    typedef BatchCreateTabDlg super;

public:
    BatchCreateTextDlg(void);

public:
    xpr_sint_t getLineCount(void);
    xpr_bool_t getLineText(xpr_sint_t aLine, xpr_tchar_t *aText, xpr_sint_t aMaxLen);

protected:
    virtual void OnTabInit(void);

protected:
    CEdit mEditWnd;

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

protected:
    DECLARE_MESSAGE_MAP()
    virtual xpr_bool_t OnInitDialog(void);
    afx_msg void OnDestroy(void);
};

#endif // __FX_BATCH_CREATE_TEXT_DLG_H__
