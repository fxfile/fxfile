//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_BOOKMARK_WND_H__
#define __FXFILE_BOOKMARK_WND_H__ 1
#pragma once

#include "bookmark.h"

namespace fxfile
{
class BookmarkWnd : public CStatic
{
public:
    BookmarkWnd(void);
    virtual ~BookmarkWnd(void);

public:
    void gotoBookmark(void);
    void setBookmark(xpr_sint_t aIndex, BookmarkItem *aBookmark);
    void getBookmark(xpr_sint_t *aIndex, BookmarkItem **aBookmark);

    void adjustWindow(void);
    void setTooltip(xpr_bool_t aPopup);
    void setBookmarkColor(COLORREF aBookmarkColor);

public:
    virtual xpr_bool_t PreTranslateMessage(MSG *pMsg);

protected:
    virtual void PreSubclassWindow(void);

protected:
    void SetWindowText(const xpr_tchar_t *aText);
    void SetFont(CFont *aFont);

protected:
    BookmarkItem *mBookmarkItem;
    xpr_sint_t    mIndex;

    COLORREF      mBookmarkColor;
    HCURSOR       mCursor;
    xpr_bool_t    mBookmarkActive;
    CFont         mTextFont;
    CToolTipCtrl  mToolTipCtrl;

protected:
    DECLARE_MESSAGE_MAP()
    afx_msg HBRUSH CtlColor(CDC* pDC, xpr_uint_t nCtlColor);
    afx_msg xpr_bool_t OnSetCursor(CWnd* pWnd, xpr_uint_t nHitTest, xpr_uint_t message);
    afx_msg void OnLButtonUp(xpr_uint_t nFlags, CPoint point);
    afx_msg LRESULT OnNcHitTest(CPoint point);
    afx_msg void OnLButtonDown(xpr_uint_t nFlags, CPoint pt);
    afx_msg xpr_sint_t OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnContextMenu(CWnd* pWnd, CPoint pt);
    afx_msg void OnMouseMove(xpr_uint_t nFlags, CPoint pt);
};
} // namespace fxfile

#endif // __FXFILE_BOOKMARK_WND_H__
