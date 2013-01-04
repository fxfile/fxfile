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
#include "rgc/StatusBar.h"
#include "rgc/BtnST.h"

class DlgState;

namespace fxb
{
class SearchFile;
} // namespace fxb

class SearchDlgObserver;
class SearchResultCtrl;

class SearchDlg : public CResizingDialog
{
    typedef CResizingDialog super;

public:
    SearchDlg(void);
    virtual ~SearchDlg(void);

public:
    void setObserver(SearchDlgObserver *aObserver);

public:
    virtual xpr_bool_t Create(CWnd *aParentWnd);

public:
    xpr_bool_t insertLocation(LPITEMIDLIST aFullPidl);
    xpr_sint_t insertLocation(const xpr_tchar_t *aPaths);

public:
    void loadUserLoc(fxb::SearchUserLocDeque *aUserLocDeque);
    void saveUserLoc(fxb::SearchUserLocDeque *aUserLocDeque);
    void getUserLoc(fxb::SearchUserLocDeque *aUserLocDeque);
    void updateUserLoc(fxb::SearchUserLocDeque *aUserLocDeque);

public:
    SearchResultCtrl *getSearchResultCtrl(void) const;

protected:
    void enableWindow(xpr_bool_t aEnable);
    void getStatusNumberFormat(xpr_tchar_t *aItemCountText,
                               xpr_size_t   aMaxItemCountTextLen,
                               xpr_tchar_t *aElapsedMinuteTimeText,
                               xpr_size_t   aMaxElapsedMinuteTimeTextLen,
                               xpr_tchar_t *aElapsedSecondTimeText,
                               xpr_size_t   aMaxElapsedSecondTimeTextLen);
    void getStatusText(xpr_tchar_t *aStatusText, xpr_tchar_t *aElapsedTimeText);
    void startAnimation(void);
    void stopAnimation(void);

protected:
    SearchDlgObserver *mObserver;

    fxb::SearchFile *mSearchFile;

    typedef struct SearchDir
    {
        std::tstring mPath;
        xpr_bool_t   mSubFolder;
    } SearchDir;

    typedef std::deque<SearchDir *> SearchDirDeque;

    xpr_bool_t   mAnimation;
    xpr_double_t mDegree;
    xpr_sint_t   mCurSel;

    xpr_sint_t mSearchResultViewIndex;
    xpr_sint_t mSearchResultCtrlId;

protected:
    CImageList mLocImageList;

protected:
    DlgState     *mDlgState;
    CComboBoxEx   mLocComboBox;
    CButtonST     mCurLocButton;
    StatusBar     mStatusBar;

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual xpr_bool_t PreTranslateMessage(MSG* pMsg);
    virtual void PostNcDestroy(void);

protected:
    DECLARE_MESSAGE_MAP()
    virtual xpr_bool_t OnInitDialog(void);
    afx_msg void OnStart(void);
    virtual void OnCancel(void);
    afx_msg void OnSelchangeLocation(void);
    afx_msg void OnDestroy(void);
    afx_msg void OnClose(void);
    afx_msg void OnDeleteitemLocation(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnPaint(void);
    afx_msg void OnTimer(xpr_uint_t aIdEvent);
    afx_msg void OnViewBarSearch(void);
    afx_msg void OnCurLocation(void);
    afx_msg LRESULT OnFinalize(WPARAM wParam, LPARAM lParam);
};

#endif // __FX_SEARCH_DLG_H__
