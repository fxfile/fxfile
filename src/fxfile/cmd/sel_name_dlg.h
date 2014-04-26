//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_SEL_NAME_DLG_H__
#define __FXFILE_SEL_NAME_DLG_H__ 1
#pragma once

namespace fxfile
{
class DlgState;

namespace cmd
{
class SelNameDlg : public CDialog
{
    typedef CDialog super;

public:
    typedef std::list<xpr::string> PatternList;

public:
    SelNameDlg(xpr_bool_t aSelect);

public:
    const PatternList &getPatternList(void) { return mPatternList; }
    xpr_bool_t isOnlySel(void) const { return mOnlySel; }

protected:
    xpr_bool_t  mSelect;
    xpr_bool_t  mOnlySel;
    PatternList mPatternList;

protected:
    DlgState  *mDlgState;
    CComboBox  mComboBox;

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

protected:
    DECLARE_MESSAGE_MAP()
    virtual void OnOK(void);
    virtual xpr_bool_t OnInitDialog(void);
    afx_msg void OnDestroy(void);
};
} // namespace cmd
} // namespace fxfile

#endif // __FXFILE_SEL_NAME_DLG_H__
