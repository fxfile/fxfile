//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FX_BOOKMARK_ITEM_EDIT_DLG_H__
#define __FX_BOOKMARK_ITEM_EDIT_DLG_H__
#pragma once

#include "fxb/fxb_bookmark.h"

class BookmarkItemEditDlg : public CDialog
{
    typedef CDialog super;

public:
    BookmarkItemEditDlg(void);
    virtual ~BookmarkItemEditDlg(void);

public:
    void add(LPITEMIDLIST aFullPidl);
    void add(fxb::BookmarkItem *aBookmark);

    void copyBookmark(fxb::BookmarkItem *aBookmark);

    LPITEMIDLIST      mFullPidl;
    fxb::BookmarkItem mBookmark;

    xpr_bool_t isSeparator(void) { return mSeparator; }

protected:
    void fillItem(void);
    void enableWindow(xpr_bool_t aCustomIcon = XPR_FALSE, xpr_bool_t aSeparator = XPR_FALSE);

    void getPathFromPidl(LPITEMIDLIST aFullPidl, std::tstring &strPath);

    HICON       mNotCustomIcon;
    HICON       mCustomSmallIcon;
    xpr_tchar_t mOldName[MAX_BOOKMARK_NAME + 1];
    xpr_tchar_t mOldPath[XPR_MAX_PATH + 1];
    xpr_bool_t  mSeparator;

protected:
    CStatic     mSmallIconWnd;
    CHotKeyCtrl mHotKeyCtrl;

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

protected:
    DECLARE_MESSAGE_MAP()
    virtual xpr_bool_t OnInitDialog(void);
    afx_msg void OnPathBrowse(void);
    afx_msg void OnStartupBrowse(void);
    virtual void OnOK(void);
    afx_msg void OnCustomIcon(void);
    afx_msg void OnIconBrowse(void);
    afx_msg void OnSeparator(void);
};

#endif // __FX_BOOKMARK_ITEM_EDIT_DLG_H__
