//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FX_PATH_BAR_H__
#define __FX_PATH_BAR_H__
#pragma once

class PathBarObserver;

class PathBar : public CWnd
{
public:
    PathBar(void);
    virtual ~PathBar(void);

public:
    void setObserver(PathBarObserver *aObserver);

public:
    void setPath(LPITEMIDLIST aFullPidl);
    void setMode(xpr_bool_t aRealPathMode);

    void setCustomFont(xpr_tchar_t *aFontText);
    void setCustomFont(CFont *aFont);
    void setHighlightColor(xpr_bool_t aHighlight, COLORREF aHighlightColor);
    void setIconDisp(xpr_bool_t aIsIcon);

    xpr_sint_t getHeight(void);
    void cancelHighlight(void);

    static COLORREF getDefaultHighlightColor(void);

protected:
    void recalcLayout(void);
    void updateFonts(void);
    void trackGoUpMenu(CPoint aPoint);
    void trackContextMenu(CPoint aPoint);

protected:
    PathBarObserver *mObserver;

    LPITEMIDLIST mFullPidl;
    xpr_tchar_t  mPath[XPR_MAX_PATH * 2 + 1];
    xpr_tchar_t  mPathMouse[XPR_MAX_PATH * 2 + 1];

    xpr_bool_t   mRealPathMode;

    xpr_bool_t   mIsIcon;
    HICON        mIcon;

    CFont        mTextFont;
    CFont        mTextUnderLineFont;
    xpr_bool_t   mHighlight;
    xpr_bool_t   mHighlightColor;

    xpr_sint_t   mPos;
    xpr_sint_t   mDownPos;

public:
    virtual xpr_bool_t Create(CWnd *aParentWnd, xpr_uint_t aId, const RECT &aRect);

protected:
    virtual xpr_bool_t PreCreateWindow(CREATESTRUCT &aCreateStruct);

protected:
    afx_msg xpr_sint_t OnCreate(LPCREATESTRUCT aCreateStruct);
    afx_msg void OnPaint(void);
    afx_msg xpr_bool_t OnEraseBkgnd(CDC *aDC);
    afx_msg void OnLButtonDown(xpr_uint_t aFlags, CPoint aPoint);
    afx_msg void OnLButtonUp(xpr_uint_t aFlags, CPoint aPoint);
    afx_msg void OnMouseMove(xpr_uint_t aFlags, CPoint aPoint);
    afx_msg void OnSize(xpr_uint_t aType, xpr_sint_t cx, xpr_sint_t cy);
    afx_msg void OnSettingChange(xpr_uint_t aFlags, const xpr_tchar_t *aSection);
    afx_msg void OnContextMenu(CWnd *aWnd, CPoint aPoint);
    afx_msg void OnDestroy(void);
    afx_msg void OnCaptureChanged(CWnd *aWnd);
    DECLARE_MESSAGE_MAP()
};

#endif // __FX_PATH_BAR_H__
