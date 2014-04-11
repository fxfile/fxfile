//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_TIP_DLG_H__
#define __FXFILE_TIP_DLG_H__ 1
#pragma once

namespace fxfile
{
namespace cmd
{
class TipDlg : public CDialog
{
    typedef CDialog super;

public:
    TipDlg(void);

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

protected:
    typedef std::deque<xpr::string> TipDeque;
    TipDeque   mTipDeque;
    xpr_sint_t mIndex;

protected:
    virtual xpr_bool_t OnInitDialog(void);
    afx_msg void OnPaint(void);
    afx_msg void OnNext(void);
    afx_msg void OnDestroy(void);
    afx_msg void OnTipStartup(void);
    DECLARE_MESSAGE_MAP()
};
} // namespace cmd
} // namespace fxfile

#endif // __FXFILE_TIP_DLG_H__
