//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXB_BOOKMARK_H__
#define __FXB_BOOKMARK_H__
#pragma once

namespace fxb
{
#define MAX_BOOKMARK       100
#define MAX_BOOKMARK_NAME  256
#define MAX_BOOKMARK_PARAM (XPR_MAX_PATH * 2)

const xpr_tchar_t BOOKMARK_SEPARATOR[] = XPR_STRING_LITERAL("----------");

class ShellIcon;
class BookmarkMgr;

class BookmarkItem
{
    friend class BookmarkMgr;

public:
    BookmarkItem(void);
    ~BookmarkItem(void);

    void clear(xpr_bool_t aNewSignature = XPR_FALSE);

public:
    xpr_bool_t isSeparator(void);
    xpr_bool_t isFastIcon(void);

    xpr_uint_t getSignature(void);
    HICON getIcon(void);
    LPITEMIDLIST getPidl(void);
    xpr_bool_t getPidl(LPITEMIDLIST *aFullPidl);
    void getTooltip(xpr_tchar_t *aTooltip, xpr_bool_t aWithPending = XPR_FALSE);

    xpr_uint_t newSignature(void);
    void setPending(xpr_bool_t aPending);
    void setIcon(HICON aSetIcon);
    void destroyIcon(void);

public:
    BookmarkItem& operator=(const BookmarkItem &aSrc);

public:
    std::tstring mName;
    std::tstring mPath;
    std::tstring mIconPath;
    xpr_sint_t   mIconIndex;
    std::tstring mParam;
    std::tstring mStartup;
    xpr_sint_t   mShowCmd;
    DWORD        mHotKey;

protected:
    xpr_uint_t mSignature;

    // icon cache
    HICON      mIconCache;
    xpr_bool_t mPending;
};

class BookmarkMgr : public CWnd, public xpr::Singleton<BookmarkMgr>
{
    friend class xpr::Singleton<BookmarkMgr>;

public:
    enum Result
    {
        ResultSucceeded,
        ResultFailed,
        ResultPending,
    };

    enum IconType
    {
        IconTypeBegin,
        IconTypeNot = IconTypeBegin,
        IconTypePending,
        IconTypeEnd,
    };

    typedef std::deque<BookmarkItem *> BookmarkDeque;

protected: BookmarkMgr(void);
public:   ~BookmarkMgr(void);

public:
    xpr_sint_t addBookmark(BookmarkItem *aBookmark);
    xpr_sint_t addBookmark(const xpr_tchar_t *aName, const xpr_tchar_t *aPath, const xpr_tchar_t *aIconPath, xpr_sint_t aIconIndex, const xpr_tchar_t *aParam, const xpr_tchar_t *aStartup, xpr_sint_t aShowCmd, DWORD aHotKey);
    xpr_sint_t insertBookmark(xpr_sint_t aBookmarkIndex, BookmarkItem *aBookmark);
    xpr_sint_t insertBookmark(xpr_sint_t aBookmarkIndex, const xpr_tchar_t *aName, const xpr_tchar_t *aPath, const xpr_tchar_t *aIconPath, xpr_sint_t aIconIndex, const xpr_tchar_t *aParam, const xpr_tchar_t *aStartup, xpr_sint_t aShowCmd, DWORD aHotKey);
    xpr_sint_t getCount(void);
    BookmarkItem *getBookmark(xpr_sint_t aBookmarkIndex);
    xpr_sint_t findSignature(xpr_uint_t aSignature);
    void clear(void);

public:
    void regAsyncIcon(HWND aHwnd, xpr_uint_t aMsg);
    void unregAsyncIcon(HWND aHwnd);

    void getAllIcon(xpr_bool_t aLarge = XPR_FALSE);
    Result getIcon(xpr_sint_t aBookmarkIndex, HICON &aIcon, xpr_bool_t aAvailableSync = XPR_FALSE, xpr_bool_t aLarge = XPR_FALSE);
    HICON getTypeIcon(IconType aIconType, xpr_bool_t aDuplicate = XPR_FALSE);

    xpr_uint_t generateSignature(void);

public:
    xpr_bool_t initDefault(void);
    static xpr_bool_t getDefaultBookmark(BookmarkDeque &aBookmarkDeque);

    xpr_bool_t loadFromFile(const xpr_tchar_t *aPath);
    xpr_bool_t saveToFile(const xpr_tchar_t *aPath);

    xpr_bool_t load(void);
    xpr_bool_t save(void);

protected:
    BookmarkDeque mBookmarkDeque;
    xpr_uint_t    mSignature;

    typedef std::map<HWND, xpr_uint_t> ASYNC_NOTIFY_MAP;
    ASYNC_NOTIFY_MAP mAsyncNotifyMap;

    ShellIcon *mShellIcon;

    typedef std::map<IconType, HICON> ICON_TYPE_MAP;
    ICON_TYPE_MAP mIconTypeMap;

protected:
    DECLARE_MESSAGE_MAP()
    afx_msg LRESULT OnShellAsyncIcon(WPARAM wParam, LPARAM lParam);
};
} // namespace fxb

#endif // __FXB_BOOKMARK_H__
