//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_ATTR_TIME_DLG_H__
#define __FXFILE_ATTR_TIME_DLG_H__ 1
#pragma once

#include "gui/ResizingDialog.h"

namespace fxfile
{
class DlgState;

namespace cmd
{
class AttrTime;

class AttrTimeDlg : public CResizingDialog
{
    typedef CResizingDialog super;

public:
    typedef std::deque<xpr::tstring> PathList;

public:
    AttrTimeDlg(void);
    virtual ~AttrTimeDlg(void);

public:
    void setDir(const xpr::tstring &aDir);
    void setItemList(const PathList &aList);

protected:
    void setStatus(const xpr_tchar_t *aStatus);
    void enableWindow(xpr_bool_t aEnable);

protected:
    AttrTime *mAttrTime;

    xpr::tstring mDir;
    PathList mList;

protected:
    HICON          mIcon;
    DlgState      *mDlgState;
    CProgressCtrl  mProgressCtrl;
    CDateTimeCtrl  mCreatedDateCtrl;
    CDateTimeCtrl  mCreatedTimeCtrl;
    CDateTimeCtrl  mModifiedDateCtrl;
    CDateTimeCtrl  mModifiedTimeCtrl;
    CDateTimeCtrl  mAccessDateCtrl;
    CDateTimeCtrl  mAccessTimeCtrl;
    CListCtrl      mListCtrl;

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

protected:
    DECLARE_MESSAGE_MAP()
    afx_msg void OnOK(void);
    virtual xpr_bool_t OnInitDialog(void);
    afx_msg void OnDestroy(void);
    afx_msg void OnCreation(void);
    afx_msg void OnModified(void);
    afx_msg void OnAccess(void);
    afx_msg void OnAttribute(void);
    afx_msg void OnTime(void);
    afx_msg LRESULT OnFinalize(WPARAM wParam, LPARAM lParam);
    afx_msg void OnTimer(UINT_PTR nIDEvent);
};
} // namespace cmd
} // namespace fxfile

#endif // __FXFILE_ATTR_TIME_DLG_H__