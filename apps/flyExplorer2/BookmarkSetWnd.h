//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FX_BOOKMARK_SET_WND_H__
#define __FX_BOOKMARK_SET_WND_H__
#pragma once

class BookmarkWnd;

class BookmarkSetWnd : public CWnd
{
    typedef CWnd super;

public:
    BookmarkSetWnd(void);
    virtual ~BookmarkSetWnd(void);

public:
    xpr_bool_t Create(CWnd *aParentWnd, xpr_uint_t aId, const RECT &aRect);

public:
    void updateBookmark(void);

    void enableBookmark(xpr_bool_t aEnable);
    void setBookmarkOption(COLORREF aColor, xpr_bool_t aPopup);

protected:
    void recalcLayout(void);

protected:
    BookmarkWnd *mBookmarkWnd;
    xpr_sint_t   mBookmarkCount;
    xpr_sint_t   mScrollDelta;

protected:
    DECLARE_MESSAGE_MAP()
    afx_msg xpr_sint_t OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnDestroy(void);
    afx_msg void OnPaint(void);
    afx_msg xpr_bool_t OnEraseBkgnd(CDC* pDC);
    afx_msg void OnSettingChange(xpr_uint_t uFlags, const xpr_tchar_t *lpszSection);
    afx_msg void OnVScroll(xpr_uint_t nSBCode, xpr_uint_t nPos, CScrollBar* pScrollBar);
    afx_msg void OnSize(xpr_uint_t nType, xpr_sint_t cx, xpr_sint_t cy);
};

#endif // __FX_BOOKMARK_SET_WND_H__
