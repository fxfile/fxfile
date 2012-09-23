//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FX_FILE_ASS_SET_DLG_H__
#define __FX_FILE_ASS_SET_DLG_H__
#pragma once

#include "fxb/fxb_filter.h"
#include "fxb/fxb_file_ass.h"

class FileAssSetDlg : public CDialog
{
    typedef CDialog super;

public:
    FileAssSetDlg(fxb::FileAssItem *aFileAssItem = XPR_NULL);
    virtual ~FileAssSetDlg(void);

public:
    fxb::FileAssItem *getFileAssItem(void);
    void setFileAssItem(fxb::FileAssItem *aFileAssItem);

    void addName(const xpr_tchar_t *aName);

protected:
    fxb::FileAssItem mFileAssItem;

    xpr_tchar_t mExts[MAX_FILTER_EXTS + 1];
    xpr_sint_t  mByExt;

    typedef std::set<std::tstring> NameSet;
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

#endif // __FX_FILE_ASS_SET_DLG_H__
