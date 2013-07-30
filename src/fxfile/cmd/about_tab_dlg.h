//
// Copyright (c) 2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_ABOUT_TAB_DLG_H__
#define __FXFILE_ABOUT_TAB_DLG_H__ 1
#pragma once

namespace fxfile
{
namespace cmd
{
class AboutTabDlg;

class AboutTabDlg : public CDialog
{
    friend class AboutDlg;

    typedef CDialog super;

public:
    AboutTabDlg(xpr_uint_t aResourceId = 0);

protected:
    virtual void OnTabInit(void);

public:
    xpr_bool_t Create(CWnd *sParentWnd);
    virtual xpr_bool_t PreTranslateMessage(MSG* pMsg);

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

protected:
    xpr_uint_t mResourceId;
    DECLARE_MESSAGE_MAP()
};
} // namespace cmd
} // namespace fxfile

#endif // __FXFILE_ABOUT_TAB_DLG_H__
