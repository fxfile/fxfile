//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_BOOKMARK_TOOLBAR_H__
#define __FXFILE_BOOKMARK_TOOLBAR_H__ 1
#pragma once

#include "bookmark.h"
#include "gui/DragDropToolBar.h"

namespace fxfile
{
class BookmarkToolBarObserver;

class BookmarkToolBar : public DragDropToolBar
{
    typedef DragDropToolBar super;

public:
    BookmarkToolBar(void);
    virtual ~BookmarkToolBar(void);

public:
    void setObserver(BookmarkToolBarObserver *aObserver);

public:
    void createBookmarkBar(void);
    void destroyBookmarkBar(void);

    void setText(xpr_bool_t aText);
    xpr_bool_t isText(void);

    virtual void GetButtonTextByCommand(xpr_uint_t aId, CString &aText);
    void updateBookmarkButton(void);

    void setBookmark(void);

protected:
    BookmarkToolBarObserver *mObserver;

    xpr_bool_t   mInit;
    CImageList   mImgList;

    CToolTipCtrl mToolTipCtrl;
    xpr_tchar_t  mToolTipText[XPR_MAX_PATH * 2];

    xpr_sint_t   mOldInsert;
    xpr_sint_t   mOldBookmark;

    void addBookmarkButton(xpr_sint_t aIndex, BookmarkItem *aBookmarkItem);
    void updateBookmarkButton(xpr_sint_t aIndex);
    void deleteAllButtons(void);

    virtual DROPEFFECT OnDragEnter(COleDataObject *aOleDataObject, DWORD aKeyState, CPoint aPoint);
    virtual void       OnDragLeave(void);
    virtual DROPEFFECT OnDragOver(COleDataObject *pOleDataObject, DWORD aKeyState, CPoint aPoint);
    virtual void       OnDragScroll(DWORD aKeyState, CPoint aPoint);
    virtual void       OnDrop(COleDataObject *aOleDataObject, DROPEFFECT aDropEffect, CPoint aPoint);

    void OnDropFileOperation(STGMEDIUM *aStgMedium, DROPEFFECT aDropEffect, xpr_sint_t aBookmark);
    void OnDropShortcut(STGMEDIUM *aStgMedium, xpr_sint_t aBookmark);
    void OnDropTarget(COleDataObject *aOleDataObject, DROPEFFECT aDropEffect, CPoint aPoint, xpr_sint_t aBookmark);
    void OnDropExecFile(STGMEDIUM *aStgMedium, xpr_sint_t aBookmark);
    void OnDropInsertBookmark(STGMEDIUM *aStgMedium, xpr_sint_t aInsert);

protected:
    virtual xpr_bool_t OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
    virtual xpr_bool_t PreTranslateMessage(MSG* pMsg);

protected:
    DECLARE_MESSAGE_MAP()
    afx_msg xpr_sint_t OnCreate(LPCREATESTRUCT lpcs);
    afx_msg void OnDestroy(void);
    afx_msg LRESULT OnSHChangeNotify(WPARAM wParam, LPARAM lParam);
    afx_msg void OnDeletingButton(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnGetDispInfo(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnContextMenu(CWnd* pWnd, CPoint pt);
    afx_msg LRESULT OnBookmarkAsyncIcon(WPARAM wParam, LPARAM lParam);
};
} // namespace fxfile

#endif // __FXFILE_BOOKMARK_TOOLBAR_H__
