//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FX_CONTENTS_WND_H__
#define __FX_CONTENTS_WND_H__
#pragma once

class BookmarkSetWnd;

class ContentsWnd : public CWnd
{
public:
    enum Mode
    {
        ModeNone = -1,
        ModeNormal,
        ModeSingleItem,
        ModeMultiItem,
        ModeDrive,
    };

    enum
    {
        TypeNone = 0,
        TypeHelp,
        TypeInfotip,
        TypeSingleSel,
        TypeMultiSel,
        TypeDriveSpace,
    };

    class Item
    {
    public:
        Item(xpr_size_t aType);

    public:
        virtual xpr_sint_t draw(CDC *aDc, xpr_sint_t aTop, xpr_sint_t aWidth, xpr_sint_t aHeight, CFont *aTextFont, CFont *aBoldTextFont) { return 0; }

    public:
        xpr_size_t mType;
    };

    class HelpItem : public Item
    {
    public:
        HelpItem(const xpr_tchar_t *aHelp = XPR_NULL);

    public:
        virtual xpr_sint_t draw(CDC *aDc, xpr_sint_t aTop, xpr_sint_t aWidth, xpr_sint_t aHeight, CFont *aTextFont, CFont *aBoldTextFont);

    public:
        std::tstring mHelp;
    };

    class InfotipItem : public Item
    {
    public:
        InfotipItem(const xpr_tchar_t *aInfotip = XPR_NULL);

    public:
        virtual xpr_sint_t draw(CDC *aDc, xpr_sint_t aTop, xpr_sint_t aWidth, xpr_sint_t aHeight, CFont *aTextFont, CFont *aBoldTextFont);

    public:
        std::tstring mInfotip;
    };

    class SingleSelItem : public Item
    {
    public:
        SingleSelItem(void);

    public:
        virtual xpr_sint_t draw(CDC *aDc, xpr_sint_t aTop, xpr_sint_t aWidth, xpr_sint_t aHeight, CFont *aTextFont, CFont *aBoldTextFont);

    public:
        std::tstring mName;
        std::tstring mType;
        std::tstring mDate;
        std::tstring mSize;
        std::tstring mAttr;
    };

    class MultiSelItem : public Item
    {
    public:
        MultiSelItem(void);

    public:
        virtual xpr_sint_t draw(CDC *aDc, xpr_sint_t aTop, xpr_sint_t aWidth, xpr_sint_t aHeight, CFont *aTextFont, CFont *aBoldTextFont);

    public:
        std::tstring mCount;
        std::tstring mSize;
        std::tstring mNames;
    };

    class DriveSpaceItem : public Item
    {
    public:
        DriveSpaceItem(void);

    public:
        virtual xpr_sint_t draw(CDC *aDc, xpr_sint_t aTop, xpr_sint_t aWidth, xpr_sint_t aHeight, CFont *aTextFont, CFont *aBoldTextFont);

    public:
        std::tstring mTotalSize;
        std::tstring mUsedSize;
        std::tstring mFreeSize;
        xpr_sint_t   mUsedPercentage;
    };

public:
    ContentsWnd(void);
    virtual ~ContentsWnd(void);

public:
    xpr_bool_t Create(CWnd *aParentWnd, xpr_uint_t aId, const RECT &aRect);

public:
    void setContentsFolder(LPTVITEMDATA aTvItemData);
    void setContentsNormal(LPTVITEMDATA aTvItemData, xpr_bool_t aUpdate = XPR_FALSE);
    void setContentsSingleItem(LPLVITEMDATA aLvItemData, const xpr_tchar_t *aName = XPR_NULL, const xpr_tchar_t *aType = XPR_NULL, const xpr_tchar_t *aDate = XPR_NULL, const xpr_tchar_t *aSize = XPR_NULL, const xpr_tchar_t *aAttr = XPR_NULL);
    void setContentsMultiItem(xpr_size_t aCount, const xpr_tchar_t *aSize, const xpr_tchar_t *aNames);

    void enableBookmark(xpr_bool_t aEnable);
    void updateBookmark(void);
    void updateBookmarkPosition(void);
    xpr_bool_t isEnableBookmark(void) { return mEnableBookmark; }
    xpr_bool_t isVisibleBookmark(void) { return mVisibleBookmark; }
    void setBookmarkPopup(xpr_bool_t aPopup);
    void setBookmarkColor(COLORREF aBookmarkColor);

protected:
    void setContentsDrive(LPLVITEMDATA sLvItemData, xpr_tchar_t aDrive, xpr_bool_t aShowBookmark, xpr_bool_t aUpdate = XPR_FALSE);
    void setContentsDrive(LPTVITEMDATA sTvItemData, xpr_tchar_t aDrive, xpr_bool_t aShowBookmark, xpr_bool_t aUpdate = XPR_FALSE);
    void setContentsDrive(LPSHELLFOLDER sShellFolder, LPITEMIDLIST aPidl, xpr_ulong_t aAttributes, xpr_tchar_t aDrive, xpr_bool_t aShowBookmark, xpr_bool_t aUpdate = XPR_FALSE);

    void updateChildWnd(void);
    void updateBookmark(xpr_sint_t aTop);

    xpr_sint_t drawItem(CDC *aDc, xpr_sint_t aTop);

    void clear(void);

protected:
    Mode            mMode;

    std::tstring    mFolderName;
    HICON           mFolderIcon;

    typedef std::deque<Item *> DispDeque;
    DispDeque       mDispDeque;

    BookmarkSetWnd *mBookmarkSetWnd;
    xpr_sint_t      mBookmarkTop;
    xpr_bool_t      mEnableBookmark;
    xpr_bool_t      mVisibleBookmark;

    CFont          *mTitleFont;
    CFont          *mTextFont;
    CFont          *mFileNameFont;
    CFont          *mFullPathFont;

protected:
    DECLARE_MESSAGE_MAP()
    afx_msg xpr_sint_t OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnDestroy(void);
    afx_msg void OnPaint(void);
    afx_msg xpr_bool_t OnEraseBkgnd(CDC* pDC);
    afx_msg void OnSettingChange(xpr_uint_t uFlags, const xpr_tchar_t *lpszSection);
    afx_msg void OnSize(xpr_uint_t nType, xpr_sint_t cx, xpr_sint_t cy);
};

#endif // __FX_CONTENTS_WND_H__
