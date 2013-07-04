//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_FLAT_HEADER_CTRL_H__
#define __FXFILE_FLAT_HEADER_CTRL_H__
#pragma once

#include "XPTheme.h"

class FlatHeaderCtrl : public CHeaderCtrl
{
public:
    FlatHeaderCtrl(void);
    virtual ~FlatHeaderCtrl(void);

public:
    void initHeader(xpr_bool_t aBoldFont);
    xpr_sint_t setSortImage(xpr_sint_t aColumn, xpr_bool_t aAscending);
    void drawFlatBorder(CDC *aDC);
    void resizeWindow(void);

protected:
    void drawItem(CDC *aDC, CRect aItemRect, xpr_uint_t aState, xpr_sint_t aIndex);

protected:
    xpr_bool_t  mCallDrawItem;

    xpr_bool_t  mBoldFont;
    xpr_bool_t  mSortAscending;
    CFont       mFont;
    xpr_sint_t  mOffset;
    xpr_sint_t  mSortColumn;
    CWnd       *mParentWnd;

    CXPTheme    mXPTheme;

    xpr_bool_t  mDragging;
    xpr_bool_t  mTracking;
    xpr_bool_t  mMouseHover;
    xpr_sint_t  mMouseHoverColumn;

public:
    virtual void DrawItem(LPDRAWITEMSTRUCT aDrawItemStruct);

protected:
    virtual void PreSubclassWindow(void);

protected:
    DECLARE_MESSAGE_MAP()
    afx_msg xpr_sint_t OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnWindowPosChanged(WINDOWPOS FAR *aWindowPos);
    afx_msg void OnPaint(void);
    afx_msg xpr_bool_t OnEraseBkgnd(CDC *aDC);
    afx_msg void OnMouseMove(xpr_uint_t aFlags, CPoint aPoint);
    afx_msg LRESULT OnMouseHover(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnThemeChanged(WPARAM wParam, LPARAM lParam);
    afx_msg void OnHdnBegindrag(NMHDR *aNmHdr, LRESULT *aResult);
    afx_msg void OnHdnEnddrag(NMHDR *aNmHdr, LRESULT *aResult);
};

#endif // __FXFILE_FLAT_HEADER_CTRL_H__
