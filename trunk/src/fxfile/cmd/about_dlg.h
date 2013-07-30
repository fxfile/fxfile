//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_ABOUT_DLG_H__
#define __FXFILE_ABOUT_DLG_H__ 1
#pragma once

namespace fxfile
{
namespace cmd
{
class AboutTabDlg;

class AboutDlg : public CDialog
{
    typedef CDialog super;

public:
    AboutDlg(void);

public:
    void addTabDialog(AboutTabDlg *aTabDlg, const xpr_tchar_t *aTitle);
    AboutTabDlg *getTabDialog(xpr_sint_t aIndex);
    void showTabDialog(xpr_sint_t aIndex);

protected:
    xpr::tstring mProgram;
    xpr::tstring mLicense;

    typedef std::deque<AboutTabDlg *> TabDeque;
    TabDeque   mTabDeque;
    xpr_sint_t mOldShowDlg;

protected:
    CTabCtrl mTabCtrl;

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

protected:
    DECLARE_MESSAGE_MAP()
    virtual xpr_bool_t OnInitDialog(void);
    afx_msg void OnDestroy(void);
    afx_msg void OnPaint(void);
    afx_msg void OnSelchangeTab(NMHDR* pNMHDR, LRESULT* pResult);
};
} // namespace cmd
} // namespace fxfile

#endif // __FXFILE_ABOUT_DLG_H__
