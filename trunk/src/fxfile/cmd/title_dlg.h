//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_TITLE_DLG_H__
#define __FXFILE_TITLE_DLG_H__ 1
#pragma once

namespace fxfile
{
namespace cmd
{
class TitleDlg : public CDialog
{
public:
    TitleDlg(const xpr_uint_t aResourceId);

public:
    void setTitle(const xpr_tchar_t *aTitle);
    void setDesc(const xpr_tchar_t *aDesc);
    void setDescIcon(xpr_uint_t aIconId, xpr_bool_t aIconLeft = XPR_FALSE);
    void setDescIcon(HICON aIcon, xpr_bool_t aIconLeft = XPR_FALSE, xpr_bool_t aBold = XPR_FALSE);
    void setCtrlOffset(xpr_sint_t aOffset);

protected:
    xpr_sint_t   mOffset;

    CFont        mTitleFont;
    xpr_bool_t   mBold;

    xpr_bool_t   mIconLeft;
    HICON        mDescIcon;
    xpr::string  mTitle;
    xpr::string  mDesc;

protected:
    DECLARE_MESSAGE_MAP()
    virtual xpr_bool_t OnInitDialog(void);
    afx_msg void OnPaint(void);
    afx_msg void OnDestroy(void);
};
} // namespace cmd
} // namespace fxfile

#endif // __FXFILE_TITLE_DLG_H__
