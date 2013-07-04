//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_PROGRAM_ASS_SET_DLG_H__
#define __FXFILE_PROGRAM_ASS_SET_DLG_H__ 1
#pragma once

#include "../filter.h"
#include "../program_ass.h"

namespace fxfile
{
namespace cfg
{
class ProgramAssSetDlg : public CDialog
{
    typedef CDialog super;

public:
    ProgramAssSetDlg(ProgramAssItem *aProgramAssItem = XPR_NULL);
    virtual ~ProgramAssSetDlg(void);

public:
    ProgramAssItem *getProgramAssItem(void);
    void setProgramAssItem(ProgramAssItem *aProgramAssItem);

    void addName(const xpr_tchar_t *aName);

protected:
    ProgramAssItem mProgramAssItem;

    xpr_tchar_t mExts[MAX_FILTER_EXTS + 1];
    xpr_sint_t  mByExt;

    typedef std::set<xpr::tstring> NameSet;
    NameSet mNameSet;

protected:
    CComboBox mFilterComboBox;

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

public:
    DECLARE_MESSAGE_MAP()
    virtual xpr_bool_t OnInitDialog(void);
    afx_msg void OnOK(void);
    afx_msg void OnAssType(void);
    afx_msg void OnProgramBrowse(void);
    afx_msg void OnCbnSelchangeFilter(void);
};
} // namespace cfg
} // namespace fxfile

#endif // __FXFILE_PROGRAM_ASS_SET_DLG_H__
