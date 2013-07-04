//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_COLUMN_SET_DLG_H__
#define __FXFILE_COLUMN_SET_DLG_H__ 1
#pragma once

#include "view_set.h"

namespace fxfile
{
namespace cmd
{
class ColumnSetDlg : public CDialog
{
    typedef CDialog super;

public:
    ColumnSetDlg(void);
    virtual ~ColumnSetDlg(void);

public:
    void setShellFolder2(LPSHELLFOLDER2 aShellFolder2);
    void setAvgCharWidth(xpr_sint_t aAvgCharWidth);
    void setColumnList(ColumnDataList &aColumnList);

    xpr_size_t getUseColumnCount(void);
    void getUseColumnList(ColumnDataList &aUseColumnList);

protected:
    xpr_sint_t addList(ColumnItemData *aColumnItemData);
    xpr_sint_t insertList(xpr_sint_t aIndex, ColumnItemData *aColumnItemData);
    void selectItem(xpr_sint_t aIndex);

protected:
    LPSHELLFOLDER2 mShellFolder2;
    xpr_sint_t mAvgCharWidth;

    typedef std::deque<ColumnItemData *> ColumnDeque;
    ColumnDeque mListDeque;
    ColumnDeque mUseListDeque;

    typedef std::map<ColumnId, ColumnItemData *> ColumnMap;
    ColumnMap mListMap;

protected:
    CListCtrl mListCtrl;

protected:
    virtual void DoDataExchange(CDataExchange* pDX);

public:
    virtual xpr_bool_t PreTranslateMessage(MSG* pMsg);

protected:
    DECLARE_MESSAGE_MAP()
    virtual xpr_bool_t OnInitDialog(void);
    afx_msg void OnOK(void);
    afx_msg void OnLvnGetdispinfoList(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnLvnItemchangedList(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnItemUp(void);
    afx_msg void OnItemDown(void);
    afx_msg void OnItemShow(void);
    afx_msg void OnItemHide(void);
    afx_msg void OnItemShowAll(void);
    afx_msg void OnItemHideAll(void);
};
} // namespace cmd
} // namespace fxfile

#endif // __FXFILE_COLUMN_SET_DLG_H__
