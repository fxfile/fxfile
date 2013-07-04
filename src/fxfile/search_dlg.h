//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_SEARCH_DLG_H__
#define __FXFILE_SEARCH_DLG_H__ 1
#pragma once

#include "search.h"

#include "gui/ResizingDialog.h"
#include "gui/StatusBar.h"
#include "gui/BtnST.h"

namespace fxfile
{
class DlgState;

class SearchFile;

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
    void loadUserLoc(SearchUserLocDeque *aUserLocDeque);
    void saveUserLoc(SearchUserLocDeque *aUserLocDeque);
    void getUserLoc(SearchUserLocDeque *aUserLocDeque);
    void updateUserLoc(SearchUserLocDeque *aUserLocDeque);

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

    SearchFile *mSearchFile;

    typedef struct SearchDir
    {
        xpr::tstring mPath;
        xpr_bool_t   mSubFolder;
    } SearchDir;

    typedef std::deque<SearchDir *> SearchDirDeque;

    xpr_bool_t   mAnimation;
    xpr_double_t mDegree;
    xpr_sint_t   mCurSel;

    xpr_sint_t mSearchResultViewIndex;
    xpr_sint_t mSearchResultPaneId;

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
    afx_msg void OnFileSearch(void);
    afx_msg void OnCurLocation(void);
    afx_msg LRESULT OnFinalize(WPARAM wParam, LPARAM lParam);
};
} // namespace fxfile

#endif // __FXFILE_SEARCH_DLG_H__
