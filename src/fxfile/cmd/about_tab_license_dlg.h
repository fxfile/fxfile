//
// Copyright (c) 2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_ABOUT_TAB_LICENSE_DLG_H__
#define __FXFILE_ABOUT_TAB_LICENSE_DLG_H__ 1
#pragma once

#include "about_tab_dlg.h"

namespace fxfile
{
namespace cmd
{
class AboutTabLicenseDlg : public AboutTabDlg
{
    typedef AboutTabDlg super;

public:
    AboutTabLicenseDlg(void);

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

protected:
    DECLARE_MESSAGE_MAP()
    virtual xpr_bool_t OnInitDialog(void);
    afx_msg void OnSize(xpr_uint_t aType, xpr_sint_t cx, xpr_sint_t cy);
};
} // namespace cmd
} // namespace fxfile

#endif // __FXFILE_ABOUT_TAB_LICENSE_DLG_H__
