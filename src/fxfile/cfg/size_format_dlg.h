//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_SIZE_FORMAT_DLG_H__
#define __FXFILE_SIZE_FORMAT_DLG_H__ 1
#pragma once

#include "../size_format.h"

namespace fxfile
{
namespace cfg
{
typedef std::deque<SizeFormat::Item *> SizeFormatDeque;

class SizeFormatDlg : public CDialog
{
    typedef CDialog super;

public:
    SizeFormatDlg(void);
    virtual ~SizeFormatDlg(void);

public:
    void setSizeFormatList(SizeFormatDeque *aSizeFormatDeque);

protected:
    xpr_sint_t addSizeFormat(SizeFormat::Item *aItem, xpr_sint_t aIndex = -1);
    void selectItem(xpr_sint_t aIndex);

protected:
    SizeFormatDeque *mSizeFormatDeque;

protected:
    CComboBox mUnitComboBox;
    CListCtrl mListCtrl;

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual xpr_bool_t PreTranslateMessage(MSG* pMsg);

protected:
    DECLARE_MESSAGE_MAP()
    virtual xpr_bool_t OnInitDialog(void);
    virtual void OnOK(void);
    afx_msg void OnLvnGetdispinfoList(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnLvnItemchangedList(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnLvnDeleteitemList(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnAdd(void);
    afx_msg void OnModify(void);
    afx_msg void OnDelete(void);
    afx_msg void OnItemUp(void);
    afx_msg void OnItemDown(void);
};
} // namespace cfg
} // namespace fxfile

#endif // __FXFILE_SIZE_FORMAT_DLG_H__
