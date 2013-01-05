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

class SearchResultPaneObserver;
class StatusBar;

class SearchResultPane : public CWnd, public SearchResultCtrlObserver
{
    typedef CWnd super;

public:
    SearchResultPane(void);
    virtual ~SearchResultPane(void);

public:
    void setObserver(SearchResultPaneObserver *aObserver);

public:
    xpr_bool_t Create(CWnd *aParentWnd, xpr_uint_t aId, const RECT &aRect);

public:
    void setViewIndex(xpr_sint_t aViewIndex);
    xpr_sint_t getViewIndex(void) const;

    SearchResultCtrl *getSearchResultCtrl(void) const;

protected:
    void recalcLayout(void);

protected:
    // from SearchResultCtrlObserver
    virtual void onStartSearch(SearchResultCtrl &aSearchResultCtrl);
    virtual void onEndSearch(SearchResultCtrl &aSearchResultCtrl);
    virtual void onUpdatedResultInfo(SearchResultCtrl &aSearchResultCtrl);
    virtual xpr_bool_t onExplore(SearchResultCtrl &aSearchResultCtrl, const xpr_tchar_t *aDir, const xpr_tchar_t *aSelPath);
    virtual xpr_bool_t onExplore(SearchResultCtrl &aSearchResultCtrl, LPITEMIDLIST aFullPidl);
    virtual void onSetFocus(SearchResultCtrl &aSearchResultCtrl);

protected:
    SearchResultPaneObserver *mObserver;

    xpr_sint_t mViewIndex;

    SearchResultCtrl *mSearchResultCtrl;
    StatusBar        *mStatusBar;

protected:
    DECLARE_MESSAGE_MAP()
    afx_msg xpr_sint_t OnCreate(LPCREATESTRUCT aCreateStruct);
    afx_msg void OnDestroy(void);
    afx_msg void OnSize(xpr_uint_t aType, xpr_sint_t cx, xpr_sint_t cy);
    afx_msg void OnSetFocus(CWnd *aOldWnd);
};

#endif // __FX_SEARCH_RESULT_PANE_H__
