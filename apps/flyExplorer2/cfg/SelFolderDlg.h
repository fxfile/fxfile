//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FX_SEL_FOLDER_DLG_H__
#define __FX_SEL_FOLDER_DLG_H__
#pragma once

#include "DlgState.h"

class SelFolderDlg : public CDialog
{
    typedef CDialog super;

public:
    SelFolderDlg(void);

public:
    void setFolder(const xpr_tchar_t *aFolder);
    const xpr_tchar_t *getSelFolder(void);

protected:
    std::tstring mFolder;

protected:
    CComboBox mComboBox;

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

protected:
    DECLARE_MESSAGE_MAP()
    virtual void OnOK(void);
    virtual xpr_bool_t OnInitDialog(void);
    afx_msg void OnDestroy(void);
    afx_msg void OnBrowse(void);
};

#endif // __FX_SEL_FOLDER_DLG_H__
