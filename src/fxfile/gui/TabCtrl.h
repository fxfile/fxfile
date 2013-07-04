//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_TAB_CTRL_H__
#define __FXFILE_TAB_CTRL_H__
#pragma once

#include "XPTheme.h"

class TabCtrlObserver;

class TabCtrl : public CWnd
{
    typedef CWnd super;

public:
    enum { InvalidTab = -1 };

public:
    TabCtrl(void);
    virtual ~TabCtrl(void);

public:
    virtual xpr_bool_t Create(CWnd *aParentWnd, xpr_uint_t aId, const RECT &aRect);

public:
    void setObserver(TabCtrlObserver *aObserver);
    void setImageList(CImageList *aImageList);
    void setTabSizeLimit(xpr_sint_t aMinSize, xpr_sint_t aMaxSize);
    void setTabSizeMode(xpr_bool_t aFixedSizeMode, xpr_sint_t aFixedSize = 0);

public:
    xpr_size_t addTab(const xpr_tchar_t *aText, xpr_sint_t aImageIndex = -1, void *aData = XPR_NULL);
    xpr_size_t insertTab(xpr_size_t aTab, const xpr_tchar_t *aText, xpr_sint_t aImageIndex = -1, void *aData = XPR_NULL);
    xpr_size_t getTabCount(void) const;
    xpr_size_t getCurTab(void) const;
    xpr_bool_t getTabText(xpr_size_t aTab, xpr_tchar_t *aText, xpr_size_t aMaxLen) const;
    void *     getTabData(xpr_size_t aTab) const;
    xpr_size_t hitTest(const POINT &aPoint) const;
    xpr_bool_t setCurTab(xpr_size_t aTab);
    xpr_bool_t setTabText(xpr_size_t aTab, const xpr_tchar_t *aText);
    xpr_bool_t swapTab(xpr_size_t aTab1, xpr_size_t aTab2);
    xpr_bool_t moveTab(xpr_size_t aSourceTab, xpr_size_t aTargetTab);
    xpr_bool_t removeTab(xpr_size_t aTab);
    void       removeAllTabs(void);

protected:
    struct TabItem;

    void createFont(void);
    void recreateFont(void);
    void destroyFont(void);

    void recalcLayout(xpr_bool_t aRedraw = XPR_TRUE);

    xpr_size_t insertTabItem(xpr_size_t aTab, TabItem *aTabItem, xpr_bool_t aNotifyObserver = XPR_TRUE);
    TabItem *getTabItem(xpr_size_t aTab) const;

protected:
    TabCtrlObserver *mObserver;

    typedef std::deque<TabItem *> TabDeque;
    TabDeque    mTabDeque;
    xpr_size_t  mCurTab;

    xpr_bool_t  mFixedSizeMode;
    xpr_sint_t  mFixedSize;
    xpr_sint_t  mTabMinSize;
    xpr_sint_t  mTabMaxSize;
    CImageList *mImageList;
    CFont       mFont;
    CFont       mBoldFont;

    xpr_bool_t  mSetCapture;
    xpr_size_t  mHoverTab;
    xpr_size_t  mPressedTab;

    CXPTheme    mXPTheme;

protected:
    afx_msg xpr_sint_t OnCreate(LPCREATESTRUCT aCreateStruct);
    afx_msg void OnDestroy(void);
    afx_msg void OnSize(xpr_uint_t aType, xpr_sint_t cx, xpr_sint_t cy);
    afx_msg void OnPaint(void);
    afx_msg xpr_bool_t OnEraseBkgnd(CDC *aDC);
    afx_msg void OnSettingChange(xpr_uint_t aFlags, const xpr_tchar_t *aSection);
    afx_msg void OnLButtonDblClk(xpr_uint_t aFlags, CPoint aPoint);
    afx_msg void OnLButtonDown(xpr_uint_t aFlags, CPoint aPoint);
    afx_msg void OnMouseMove(xpr_uint_t aFlags, CPoint aPoint);
    afx_msg void OnLButtonUp(xpr_uint_t aFlags, CPoint aPoint);
    afx_msg void OnCaptureChanged(CWnd *aWnd);
    afx_msg void OnContextMenu(CWnd *aWnd, CPoint aPoint);
    DECLARE_MESSAGE_MAP()
};

#endif // __FXFILE_TAB_CTRL_H__
