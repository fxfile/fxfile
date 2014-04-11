//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_FOLDER_SYNC_OPT_DLG_H__
#define __FXFILE_FOLDER_SYNC_OPT_DLG_H__ 1
#pragma once

#include "gui/ResizingDialog.h"

namespace fxfile
{
namespace cmd
{
class FolderSyncOptDlg : public CDialog
{
    typedef CDialog super;

public:
    FolderSyncOptDlg(void);
    virtual ~FolderSyncOptDlg(void);

public:
    void setPath(xpr_sint_t aIndex, const xpr_tchar_t *aPath);
    void setFiles(xpr_sint_t aIndex, xpr_sint_t aFiles, xpr_sint64_t aSize);

    xpr_uint_t getDirection(void);

protected:
    xpr_uint_t   mDirection;
    xpr::string  mPath1;
    xpr::string  mPath2;
    xpr_sint_t   mFiles1;
    xpr_sint_t   mFiles2;
    xpr_sint64_t mSize1;
    xpr_sint64_t mSize2;

protected:
    CComboBox mDirectionComboBox;

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

protected:
    DECLARE_MESSAGE_MAP()
    virtual xpr_bool_t OnInitDialog(void);
    afx_msg void OnOK(void);
    afx_msg void OnSelchangeDirection(void);
    afx_msg void OnToLeftRight(void);
};
} // namespace cmd
} // namespace fxfile

#endif // __FXFILE_FOLDER_SYNC_OPT_DLG_H__
