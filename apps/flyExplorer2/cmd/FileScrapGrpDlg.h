//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FX_FILE_SCRAP_GRP_DLG_H__
#define __FX_FILE_SCRAP_GRP_DLG_H__
#pragma once

class FileScrapGrpDlg : public CDialog
{
public:
    FileScrapGrpDlg(void);
    virtual ~FileScrapGrpDlg(void);

public:
    void setTitle(const std::tstring &aTitle);
    xpr_uint_t getSelGroupId(void);

protected:
    std::tstring mTitle;
    xpr_uint_t   mSelGroupId;

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

#endif // __FX_FILE_SCRAP_GRP_DLG_H__
