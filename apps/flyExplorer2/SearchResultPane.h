//
// Copyright (c) 2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FX_SEARCH_RESULT_PANE_H__
#define __FX_SEARCH_RESULT_PANE_H__
#pragma once

#include "SearchResultCtrlObserver.h"
#include "TabPane.h"

class StatusBar;

class SearchResultPane
    : public TabPane
    , public SearchResultCtrlObserver
{
    typedef TabPane super;

public:
    SearchResultPane(void);
    virtual ~SearchResultPane(void);

public:
    xpr_bool_t Create(CWnd *aParentWnd, xpr_uint_t aId, const RECT &aRect);

public:
    // from TabPane
    virtual CWnd *     newSubPane(xpr_uint_t aId);
    virtual CWnd *     getSubPane(xpr_uint_t aId = InvalidId) const;
    virtual xpr_size_t getSubPaneCount(void) const;
    virtual xpr_uint_t getCurSubPaneId(void) const;
    virtual xpr_uint_t setCurSubPane(xpr_uint_t aId);
    virtual void       destroySubPane(xpr_uint_t aId);
    virtual void       destroySubPane(void);

    virtual StatusBar *getStatusBar(void) const;
    virtual const xpr_tchar_t *getStatusPaneText(xpr_sint_t aIndex) const;

    virtual void setChangedOption(Option &aOption);

public:
    SearchResultCtrl *getSearchResultCtrl(void) const;

protected:
    void recalcLayout(void);

protected:
    // from SearchResultCtrlObserver
    virtual void onStartSearch(SearchResultCtrl &aSearchResultCtrl);
    virtual void onEndSearch(SearchResultCtrl &aSearchResultCtrl);
    virtual void onUpdatedResultInfo(SearchResultCtrl &aSearchResultCtrl);
    virtual xpr_bool_t onOpenFolder(SearchResultCtrl &aSearchResultCtrl, const xpr_tchar_t *aDir, const xpr_tchar_t *aSelPath);
    virtual xpr_bool_t onOpenFolder(SearchResultCtrl &aSearchResultCtrl, LPITEMIDLIST aFullPidl);
    virtual void onSetFocus(SearchResultCtrl &aSearchResultCtrl);
    virtual void onMoveFocus(SearchResultCtrl &aSearchResultCtrl);

protected:
    xpr_uint_t        mSubPaneId;
    SearchResultCtrl *mSearchResultCtrl;
    StatusBar        *mStatusBar;

    xpr_tchar_t mStatusText0[0xff];
    xpr_tchar_t mStatusText1[0xff];

protected:
    DECLARE_MESSAGE_MAP()
    afx_msg xpr_sint_t OnCreate(LPCREATESTRUCT aCreateStruct);
    afx_msg void OnDestroy(void);
    afx_msg void OnSize(xpr_uint_t aType, xpr_sint_t cx, xpr_sint_t cy);
    afx_msg void OnSetFocus(CWnd *aOldWnd);
};

#endif // __FX_SEARCH_RESULT_PANE_H__
