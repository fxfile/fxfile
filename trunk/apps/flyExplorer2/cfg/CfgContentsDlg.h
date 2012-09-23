//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FX_CFG_CONTENTS_H__
#define __FX_CFG_CONTENTS_H__
#pragma once

#include "../rgc/ColourPickerXP.h"
#include "CfgDlg.h"

class CfgContentsDlg : public CfgDlg
{
    typedef CfgDlg super;

public:
    CfgContentsDlg(void);

protected:
    void setActive(xpr_uint_t aContentsType);
    virtual void OnApply(void);

protected:
    CColourPickerXP mBookmarkColorCtrl;

public:
    virtual xpr_bool_t PreTranslateMessage(MSG* pMsg);
    virtual xpr_bool_t DestroyWindow(void);

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual xpr_bool_t OnCommand(WPARAM wParam, LPARAM lParam);

protected:
    DECLARE_MESSAGE_MAP()
    virtual xpr_bool_t OnInitDialog(void);
    afx_msg void OnContentsNone(void);
    afx_msg void OnContentsStandard(void);
    afx_msg void OnContentsBasic(void);
    afx_msg LRESULT OnSelEndOK(WPARAM wParam, LPARAM lParam);
};

#endif // __FX_CFG_CONTENTS_H__
