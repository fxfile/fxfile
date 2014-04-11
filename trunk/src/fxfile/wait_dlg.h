//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_WAIT_DLG_H__
#define __FXFILE_WAIT_DLG_H__ 1
#pragma once

#include "pattern.h"

namespace fxfile
{
class WaitDlg : public CDialog, public fxfile::base::Singleton<WaitDlg>
{
    friend class fxfile::base::Singleton<WaitDlg>;

    typedef CDialog super;

protected: WaitDlg(void);
public:   ~WaitDlg(void);

public:
    void setTitle(const xpr_tchar_t *aTitle);

    void end(void);
    xpr_bool_t isStopped(void);

protected:
    xpr::string mTitle;

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

protected:
    DECLARE_MESSAGE_MAP()
    virtual xpr_bool_t OnInitDialog(void);
    afx_msg void OnStop(void);
};
} // namespace fxfile

#endif // __FXFILE_WAIT_DLG_H__
