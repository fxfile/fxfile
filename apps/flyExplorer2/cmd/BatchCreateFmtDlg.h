//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FX_BATCH_CREATE_FMT_DLG_H__
#define __FX_BATCH_CREATE_FMT_DLG_H__
#pragma once

#include "BatchCreateTabDlg.h"

class DlgState;

class BatchCreateFmtDlg : public BatchCreateTabDlg
{
    typedef BatchCreateTabDlg super;

public:
    BatchCreateFmtDlg(void);

public:
    void getFormat(xpr_tchar_t *aFormat, xpr_sint_t aFormatLen);
    xpr_uint_t getStart(void);
    xpr_uint_t getEnd(void);
    xpr_uint_t getCount(void);
    xpr_uint_t getIncrease(void);
    xpr_bool_t getCountType(void);

protected:
    virtual void OnTabInit(void);

    void enableWindow(xpr_bool_t aEnd = XPR_TRUE);

protected:
    DlgState *mDlgState;

protected:
    virtual void DoDataExchange(CDataExchange *aDX);    // DDX/DDV support
    virtual xpr_bool_t OnCommand(WPARAM wParam, LPARAM lParam);

protected:
    DECLARE_MESSAGE_MAP()
    virtual xpr_bool_t OnInitDialog(void);
    afx_msg void OnDestroy(void);
    afx_msg void OnInitMenuPopup(CMenu *aPopupMenu, xpr_uint_t aIndex, xpr_bool_t aSysMenu);
    afx_msg void OnIsNumberEnd(void);
    afx_msg void OnIsNumberCount(void);
    afx_msg void OnFormatMenu(void);
};

#endif // __FX_BATCH_CREATE_FMT_DLG_H__
