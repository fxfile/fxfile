//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FX_CFG_EXP_THUMB_DLG_H__
#define __FX_CFG_EXP_THUMB_DLG_H__
#pragma once

#include "CfgDlg.h"

class CfgExpThumbDlg : public CfgDlg 
{
    typedef CfgDlg super;

public:
    CfgExpThumbDlg(void);

protected:
    virtual void OnApply(void);

protected:
    CComboBox mPriorityComboBox;
    CComboBox mImageFormatComboBox;

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

protected:
    DECLARE_MESSAGE_MAP()
    virtual xpr_bool_t OnInitDialog(void);
    virtual xpr_bool_t OnCommand(WPARAM wParam, LPARAM lParam);
    afx_msg void OnCacheClean(void);
    afx_msg void OnCacheInit(void);
};

#endif // __FX_CFG_EXP_THUMB_DLG_H__
