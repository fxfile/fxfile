//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FX_SEARCH_DLG_H__
#define __FX_SEARCH_DLG_H__
#pragma once

#include "fxb/fxb_search.h"

#include "rgc/ResizingDialog.h"
#include "rgc/BtnST.h"

class DlgState;

namespace fxb
{
class SearchFile;
} // namespace fxb

class SearchResultCtrl;

class SearchDlg : public CResizingDialog
{
    typedef CResizingDialog super;

public:
    SearchDlg(void);
    virtual ~SearchDlg(void);

public:
    xpr_bool_t insertLocation(LPITEMIDLIST aFullPidl);
    xpr_sint_t insertLocation(const xpr_tchar_t *aPaths);
    void createAccelTable(void);

public:
    void loadUserLoc(fxb::SearchUserLocDeque *aUserLocDeque);
    void saveUserLoc(fxb::SearchUserLocDeque *aUserLocDeque);
    void getUserLoc(fxb::SearchUserLocDeque *aUserLocDeque);
    void updateUserLoc(fxb::SearchUserLocDeque *aUserLocDeque);

    void setNameFocus(void);

protected:
    void enableWindow(xpr_bool_t aEnable);
    void getStatusText(xpr_tchar_t *aStatusText, xpr_tchar_t *aElapsedTimeText);
    void startAnimation(void);
    void stopAnimation(void);

protected:
    fxb::SearchFile  *mSearchFile;
    SearchResultCtrl *mResultCtrl;

    typedef struct tagSearchDir
    {
        std::tstring mPath;
        xpr_bool_t   mSubFolder;
    } SearchDir;

    typedef std::deque<SearchDir *> SearchDirDeque;

    xpr_bool_t   mAnimation;
    xpr_double_t mDegree;
    xpr_sint_t   mCurSel;

protected:
    CImageList mLocImageList;
    CBrush     mBrush;
    HACCEL     mAccelTable;

protected:
    DlgState     *mDlgState;
    CAnimateCtrl  mAnimateCtrl;
    CComboBoxEx   mLocComboBox;
    CButtonST     mCurLocButton;

public:
    virtual xpr_bool_t Create(CWnd* pParentWnd);
    virtual xpr_bool_t PreTranslateMessage(MSG* pMsg);

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual xpr_bool_t PreCreateWindow(CREATESTRUCT& cs);

protected:
    DECLARE_MESSAGE_MAP()
    virtual xpr_bool_t OnInitDialog(void);
    afx_msg xpr_bool_t OnEraseBkgnd(CDC* pDC);
    afx_msg void OnLButtonDown(xpr_uint_t nFlags, CPoint point);
    afx_msg void OnStart(void);
    afx_msg void OnStop(void);
    afx_msg void OnSelchangeLocation(void);
    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, xpr_uint_t nCtlColor);
    afx_msg void OnDestroy(void);
    afx_msg void OnDeleteitemLocation(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnPaint(void);
    afx_msg void OnTimer(xpr_uint_t nIDEvent);
    afx_msg void OnViewBarSearch(void);
    afx_msg void OnCurLocation(void);
    afx_msg LRESULT OnFinalize(WPARAM wParam, LPARAM lParam);
};

#endif // __FX_SEARCH_DLG_H__
