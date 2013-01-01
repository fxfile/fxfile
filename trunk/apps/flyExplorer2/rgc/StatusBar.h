//
// Copyright (c) 2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FX_STATUS_CTRL_H__
#define __FX_STATUS_CTRL_H__
#pragma once

class StatusBarObserver;

class StatusBar : public CWnd
{
    typedef CWnd super;

public:
    enum { InvalidIndex = -1 };

public:
    StatusBar(void);
    virtual ~StatusBar(void);

public:
    virtual xpr_bool_t Create(CWnd *aParentWnd, xpr_uint_t aId, const RECT &aRect);

public:
    void setObserver(StatusBarObserver *aObserver);

public:
    xpr_size_t addPane(xpr_size_t aPandId, const xpr_tchar_t *aText, HICON aIcon = XPR_NULL, void *aData = XPR_NULL);
    xpr_size_t insertPane(xpr_size_t aIndex, xpr_size_t aPaneId, const xpr_tchar_t *aText, HICON aIcon = XPR_NULL, void *aData = XPR_NULL);
    xpr_bool_t setPaneSize(xpr_size_t aIndex, xpr_size_t aSize, xpr_size_t aMinSize);
    xpr_bool_t setPaneText(xpr_size_t aIndex, const xpr_tchar_t *aText);
    xpr_bool_t setDynamicPaneText(xpr_sint_t aIndex, const xpr_tchar_t *aText, xpr_sint_t aOffset);
    xpr_bool_t setPaneIcon(xpr_size_t aIndex, HICON aIcon);
    xpr_bool_t setPaneData(xpr_size_t aIndex, void *aData);
    xpr_size_t getPaneCount(void) const;
    xpr_bool_t getPaneText(xpr_size_t aIndex, xpr_tchar_t *aText, xpr_size_t aMaxLen) const;
    void *     getPaneData(xpr_size_t aIndex) const;
    xpr_bool_t getPaneRect(xpr_size_t aIndex, RECT *aRect) const;
    xpr_size_t hitTest(const POINT &aPoint) const;
    xpr_size_t IdToIndex(xpr_size_t aPaneId) const;
    xpr_bool_t removePane(xpr_size_t aIndex);
    void       removeAllPanes(void);

public:
    xpr_sint_t getDefaultHeight(void);

protected:
    struct PaneItem;

    void createFont(void);
    void recreateFont(void);
    void destroyFont(void);

    void recalcLayout(xpr_bool_t aRedraw = XPR_TRUE);

    xpr_size_t insertPaneItem(xpr_size_t aPane, PaneItem *aPaneItem);
    PaneItem *getPaneItem(xpr_size_t aIndex) const;

protected:
    StatusBarObserver *mObserver;

    typedef std::deque<PaneItem *> PaneDeque;
    PaneDeque mPaneDeque;

    CFont       mFont;
    CFont       mBoldFont;

protected:
    afx_msg xpr_sint_t OnCreate(LPCREATESTRUCT aCreateStruct);
    afx_msg void OnDestroy(void);
    afx_msg void OnSize(xpr_uint_t aType, xpr_sint_t cx, xpr_sint_t cy);
    afx_msg void OnPaint(void);
    afx_msg xpr_bool_t OnEraseBkgnd(CDC *aDC);
    afx_msg void OnSettingChange(xpr_uint_t aFlags, const xpr_tchar_t *aSection);
    afx_msg void OnLButtonDblClk(xpr_uint_t aFlags, CPoint aPoint);
    afx_msg void OnContextMenu(CWnd *aWnd, CPoint aPoint);
    afx_msg LRESULT OnSetText(WPARAM aWParam, LPARAM aLParam);
    DECLARE_MESSAGE_MAP()
};

#endif // __FX_STATUS_CTRL_H__
