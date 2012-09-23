//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FX_ABOUT_DLG_H__
#define __FX_ABOUT_DLG_H__
#pragma once

class AboutDlg : public CDialog
{
    typedef CDialog super;

public:
    AboutDlg(void);

protected:
    CFont mBoldFont;

protected:
    CStatic mProgramWnd;

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

protected:
    DECLARE_MESSAGE_MAP()
    virtual xpr_bool_t OnInitDialog(void);
    afx_msg void OnLButtonDown(xpr_uint_t nFlags, CPoint point);
    afx_msg void OnDestroy(void);
    afx_msg void OnBug(void);
    afx_msg void OnSystem(void);
};

#endif // __FX_ABOUT_DLG_H__