//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FX_SHORTCUT_DLG_H__
#define __FX_SHORTCUT_DLG_H__
#pragma once

#include "TitleDlg.h"

class ShortcutDlg : public TitleDlg
{
    typedef TitleDlg super;

public:
    ShortcutDlg(const xpr_tchar_t *aPath);
    virtual ~ShortcutDlg(void);

protected:
    xpr_tchar_t mPath[XPR_MAX_PATH + 1];
    LPITEMIDLIST mFullPidl;

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

protected:
    DECLARE_MESSAGE_MAP()
    virtual xpr_bool_t OnInitDialog(void);
    virtual void OnOK(void);
    afx_msg void OnTargetBrowse(void);
};

#endif // __FX_SHORTCUT_DLG_H__
