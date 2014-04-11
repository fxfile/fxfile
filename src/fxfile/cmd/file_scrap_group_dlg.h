//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_FILE_SCRAP_GROUP_DLG_H__
#define __FXFILE_FILE_SCRAP_GROUP_DLG_H__ 1
#pragma once

namespace fxfile
{
namespace cmd
{
class FileScrapGroupDlg : public CDialog
{
public:
    FileScrapGroupDlg(void);
    virtual ~FileScrapGroupDlg(void);

public:
    void setTitle(const xpr::string &aTitle);
    xpr_uint_t getSelGroupId(void);

protected:
    xpr::string mTitle;
    xpr_uint_t  mSelGroupId;

protected:
    CListBox mListBox;

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

protected:
    DECLARE_MESSAGE_MAP()
    virtual xpr_bool_t OnInitDialog(void);
    afx_msg void OnDblclkGrpList(void);
    afx_msg void OnOK(void);
};
} // namespace cmd
} // namespace fxfile

#endif // __FXFILE_FILE_SCRAP_GROUP_DLG_H__
