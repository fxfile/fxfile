//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "bookmark_toolbar.h"

#include "file_op_thread.h"
#include "context_menu.h"
#include "wnet_mgr.h"
#include "main_frame.h"
#include "option.h"
#include "bookmark_toolbar_observer.h"

#include "gui/DragImage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace fxfile
{
// user defined message
enum
{
    WM_BOOKMARK_ASYNC_ICON = WM_USER + 100,
};

BookmarkToolBar::BookmarkToolBar(void)
    : mObserver(XPR_NULL)
    , mInit(XPR_FALSE)
    , mOldInsert(-1)
    , mOldBookmark(-1)
{
    mToolTipText[0] = '\0';
}

BookmarkToolBar::~BookmarkToolBar(void)
{
}

void BookmarkToolBar::setObserver(BookmarkToolBarObserver *aObserver)
{
    mObserver = aObserver;
}

BEGIN_MESSAGE_MAP(BookmarkToolBar, super)
    ON_WM_CREATE()
    ON_WM_DESTROY()
    ON_WM_CONTEXTMENU()
    ON_NOTIFY_REFLECT(TBN_DELETINGBUTTON, OnDeletingButton)
    ON_NOTIFY_REFLECT(TBN_GETDISPINFO, OnGetDispInfo)
    ON_MESSAGE(WM_BOOKMARK_ASYNC_ICON, OnBookmarkAsyncIcon)
END_MESSAGE_MAP()

void BookmarkToolBar::deleteAllButtons(void)
{
    CToolBarCtrl &sToolBarCtrl = GetToolBarCtrl();

    xpr_sint_t i;
    xpr_sint_t sCount;

    sCount = sToolBarCtrl.GetButtonCount();
    for (i = 0; i < sCount; ++i)
        sToolBarCtrl.DeleteButton(0);
}

void BookmarkToolBar::OnDeletingButton(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMTOOLBAR lpnmtb = (LPNMTOOLBAR)pNMHDR;
    *pResult = 0;
}

xpr_sint_t BookmarkToolBar::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (super::OnCreate(lpCreateStruct) == -1)
        return -1;

    CToolBarCtrl &sToolBarCtrl = GetToolBarCtrl();

    ModifyStyle(0, TBSTYLE_LIST | TBSTYLE_FLAT);
    sToolBarCtrl.SetExtendedStyle(TBSTYLE_EX_DRAWDDARROWS | TBSTYLE_EX_MIXEDBUTTONS);

    sToolBarCtrl.SetBitmapSize(CSize(16,16));

    CRect sRect;
    GetClientRect(&sRect);
    mToolTipCtrl.Create(this);
    mToolTipCtrl.AddTool(this, LPSTR_TEXTCALLBACK, sRect, 200);
    mToolTipCtrl.SetMaxTipWidth(500);

    return 0;
}

LRESULT BookmarkToolBar::OnBookmarkAsyncIcon(WPARAM wParam, LPARAM lParam)
{
    xpr_sint_t sIndex = (xpr_sint_t)wParam;
    BookmarkItem *sBookmarkItem = (BookmarkItem *)lParam;

    if (sBookmarkItem == XPR_NULL)
        return 0;

    if (XPR_IS_RANGE(0, sIndex, BookmarkMgr::instance().getCount()-1))
    {
        TBBUTTONINFO sTbButtonInfo = {0};
        sTbButtonInfo.cbSize = sizeof(TBBUTTONINFO);
        sTbButtonInfo.dwMask = TBIF_BYINDEX | TBIF_IMAGE;
        sTbButtonInfo.iImage = mImgList.Add(sBookmarkItem->getIcon());

        GetToolBarCtrl().SetButtonInfo(sIndex, &sTbButtonInfo);
    }

    return 0;
}

void BookmarkToolBar::createBookmarkBar(void)
{
    mInit = XPR_TRUE;

    BookmarkMgr::instance().regAsyncIcon(*this, WM_BOOKMARK_ASYNC_ICON);

    setBookmark();
}

void BookmarkToolBar::destroyBookmarkBar(void)
{
    mInit = XPR_FALSE;

    BookmarkMgr::instance().unregAsyncIcon(*this);

    if (mImgList.m_hImageList != XPR_NULL)
        mImgList.DeleteImageList();

    CToolBarCtrl &sToolBarCtrl = GetToolBarCtrl();

    xpr_sint_t i;
    xpr_sint_t sCount;

    sCount = sToolBarCtrl.GetButtonCount();
    for (i = 0; i < sCount; ++i)
        sToolBarCtrl.DeleteButton(0);
}

void BookmarkToolBar::OnDestroy(void)
{
    destroyBookmarkBar();

    super::OnDestroy();
}

void BookmarkToolBar::addBookmarkButton(xpr_sint_t aIndex, BookmarkItem *aBookmarkItem)
{
    CToolBarCtrl &sToolBarCtrl = GetToolBarCtrl();

    xpr_sint_t nImage = -1;

    HICON hIcon = XPR_NULL;
    BookmarkMgr::Result sResult = BookmarkMgr::instance().getIcon(aIndex, hIcon);
    if (hIcon)
        nImage = mImgList.Add(hIcon);

    xpr_bool_t sSeparator = aBookmarkItem->isSeparator();

    TBBUTTON sTbButton = {0};
    if (sSeparator == XPR_TRUE)
    {
        sTbButton.iBitmap = -1;
        sTbButton.fsState = TBSTATE_ENABLED;
        sTbButton.fsStyle = BTNS_SEP;
        sTbButton.dwData  = (DWORD)aIndex;
    }
    else
    {
        sTbButton.iBitmap   = nImage;
        sTbButton.fsState   = TBSTATE_ENABLED;
        sTbButton.fsStyle   = BTNS_BUTTON | BTNS_SHOWTEXT | TBSTYLE_AUTOSIZE;
        sTbButton.idCommand = ID_BOOKMARK_FIRST + aIndex;
        sTbButton.iString   = 0;
        sTbButton.dwData    = (DWORD)aIndex;
    }

    sToolBarCtrl.InsertButton(aIndex, &sTbButton);

    if (sSeparator == XPR_FALSE)
        SetButtonText(CommandToIndex(sTbButton.idCommand), XPR_IS_TRUE(gOpt->mMain.mBookmarkBarText) ? aBookmarkItem->mName.c_str() : XPR_STRING_LITERAL(""));

    if (sSeparator == XPR_FALSE && gOpt->mMain.mBookmarkBarText == XPR_FALSE)
    {
        TBBUTTONINFO sTbButtonInfo = {0};
        sTbButtonInfo.cbSize = sizeof(TBBUTTONINFO);
        sTbButtonInfo.dwMask = TBIF_BYINDEX | TBIF_STYLE;
        sToolBarCtrl.GetButtonInfo(aIndex, &sTbButtonInfo);

        sTbButtonInfo.fsStyle &= ~BTNS_SHOWTEXT;
        sToolBarCtrl.SetButtonInfo(aIndex, &sTbButtonInfo);
    }
}

void BookmarkToolBar::setBookmark(void)
{
    if (mInit == XPR_FALSE)
        return;

    if (mImgList.m_hImageList != XPR_NULL)
        mImgList.DeleteImageList();

    mImgList.Create(16, 16, ILC_COLOR32 | ILC_MASK, -1, -1);
    GetToolBarCtrl().SetImageList(&mImgList);

    BookmarkMgr &sBookmarkMgr = BookmarkMgr::instance();

    xpr_sint_t sCount = sBookmarkMgr.getCount();
    if (sCount > 0)
    {
        sBookmarkMgr.getAllIcon();

        xpr_sint_t i;
        for (i = 0; i < sCount; ++i)
            addBookmarkButton(i, sBookmarkMgr.getBookmark(i));
    }

    UpdateToolbarSize();
}

xpr_bool_t BookmarkToolBar::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
    NMHDR *pnmhdr = (NMHDR *)lParam;

    if (pnmhdr->code == TTN_GETDISPINFO)
    {
        if (gOpt->mConfig.mBookmarkTooltip == XPR_TRUE)
        {
            LPNMTTDISPINFO sNmTtDispInfo = (LPNMTTDISPINFO)pnmhdr;

            CPoint sPoint;
            GetCursorPos(&sPoint);
            ScreenToClient(&sPoint);

            xpr_sint_t sIndex = GetToolBarCtrl().HitTest(&sPoint);

            BookmarkItem *sBookmarkItem = BookmarkMgr::instance().getBookmark(sIndex);
            if (sBookmarkItem == XPR_NULL)
                return XPR_FALSE;

            sBookmarkItem->getTooltip(mToolTipText, !gOpt->mMain.mBookmarkBarText, XPR_TRUE);

            sNmTtDispInfo->lpszText = mToolTipText;
        }
    }

    return super::OnNotify(wParam, lParam, pResult);
}

void BookmarkToolBar::GetButtonTextByCommand(xpr_uint_t aId, CString &aText)
{
    GetButtonText(CommandToIndex(aId), aText);
}

xpr_bool_t BookmarkToolBar::isText(void)
{
    return gOpt->mMain.mBookmarkBarText;
}

void BookmarkToolBar::setText(xpr_bool_t aText)
{
    if (mInit == XPR_TRUE)
    {
        CToolBarCtrl &sToolBarCtrl = GetToolBarCtrl();

        SetRedraw(XPR_FALSE);

        xpr_sint_t i;
        xpr_sint_t sCount;

        TBBUTTONINFO sTbButtonInfo = {0};
        sTbButtonInfo.cbSize = sizeof(TBBUTTONINFO);
        sTbButtonInfo.dwMask = TBIF_BYINDEX | TBIF_STYLE;

        sCount = sToolBarCtrl.GetButtonCount();
        if (aText == XPR_FALSE)
        {
            for (i = 0; i < sCount; ++i)
            {
                sToolBarCtrl.GetButtonInfo(i, &sTbButtonInfo);
                sTbButtonInfo.fsStyle &= ~BTNS_SHOWTEXT;
                sToolBarCtrl.SetButtonInfo(i, &sTbButtonInfo);

                SetButtonText(i, XPR_STRING_LITERAL(""));
            }
        }
        else
        {
            BookmarkItem *sBookmarkItem;

            for (i = 0; i < sCount; ++i)
            {
                sBookmarkItem = BookmarkMgr::instance().getBookmark(i);

                sToolBarCtrl.GetButtonInfo(i, &sTbButtonInfo);
                sTbButtonInfo.fsStyle |= BTNS_SHOWTEXT;
                sToolBarCtrl.SetButtonInfo(i, &sTbButtonInfo);

                if (sBookmarkItem != XPR_NULL)
                    SetButtonText(i, sBookmarkItem->mName.c_str());
            }
        }

        SetRedraw();

        UpdateToolbarSize();
    }

    gOpt->mMain.mBookmarkBarText = aText;
}

DROPEFFECT BookmarkToolBar::OnDragEnter(COleDataObject *aOleDataObject, DWORD aKeyState, CPoint aPoint)
{
    DROPEFFECT sDropEffect = DROPEFFECT_LINK;

    DragImage &sDragImage = DragImage::instance();

    if (sDragImage.isMyDragging() == XPR_TRUE)
    {
        CImageList *sMyDragImage = sDragImage.getMyDragImage();
        if (XPR_IS_NOT_NULL(sMyDragImage))
        {
            sMyDragImage->DragEnter(GetDesktopWindow(), aPoint);
            sMyDragImage->DragShowNolock(XPR_TRUE);
        }
    }
    else
    {
        if (mDropTarget.isUseDropHelper() == XPR_TRUE)
        {
            CPoint sDragPoint(aPoint);
            IDataObject *sDataObject = aOleDataObject->GetIDataObject(XPR_FALSE);
            mDropTarget.getDropHelper()->DragEnter(GetSafeHwnd(), sDataObject, &sDragPoint, sDropEffect);
        }
    }

    mOldInsert = -1;
    mOldBookmark = -1;

    return sDropEffect;
}

void BookmarkToolBar::OnDragLeave(void)
{
    CToolBarCtrl &sToolBarCtrl = GetToolBarCtrl();

    DragImage &sDragImage = DragImage::instance();

    if (sDragImage.isMyDragging() == XPR_TRUE)
    {
        sDragImage.showMyDragNoLock(XPR_FALSE);
    }
    else
    {
        if (mDropTarget.isUseDropHelper() == XPR_TRUE)
            mDropTarget.getDropHelper()->DragLeave();
    }

    if (mOldInsert >= 0)
    {
        TBINSERTMARK sTbInsertMark = {0};
        sTbInsertMark.iButton = -1;
        sToolBarCtrl.SetInsertMark(&sTbInsertMark);
    }

    if (mOldBookmark >= 0)
    {
        TBBUTTON sTbButton = {0};

        sToolBarCtrl.GetButton(mOldBookmark, &sTbButton);
        sToolBarCtrl.SetState(sTbButton.idCommand, TBSTATE_ENABLED);
    }

    if (XPR_IS_NOT_NULL(mObserver))
    {
        mObserver->onBookmarkToolBarStatus(*this, -1, -1, DROPEFFECT_NONE);
    }

    mOldInsert = -1;
    mOldBookmark = -1;
}

DROPEFFECT BookmarkToolBar::OnDragOver(COleDataObject* aOleDataObject, DWORD aKeyState, CPoint aPoint)
{
    DROPEFFECT sDropEffect = DROPEFFECT_LINK;

    DragImage &sDragImage = DragImage::instance();

    if (sDragImage.isMyDragging() == XPR_TRUE)
    {
        CImageList *sMyDragImage = sDragImage.getMyDragImage();
        if (XPR_IS_NOT_NULL(sMyDragImage))
        {
            CPoint sDragPoint(aPoint);
            ClientToScreen(&sDragPoint);
            sMyDragImage->DragMove(sDragPoint);
        }
    }
    else
    {
        if (mDropTarget.isUseDropHelper() == XPR_TRUE)
        {
            CPoint sDragPoint(aPoint);
            mDropTarget.getDropHelper()->DragOver(&sDragPoint, sDropEffect);
        }
    }

    CToolBarCtrl &sToolBarCtrl = GetToolBarCtrl();

    TBBUTTON sTbButton = {0};
    TBINSERTMARK sTbInsertMark = {0};
    xpr_sint_t sHitTest = GetToolBarCtrl().HitTest(&aPoint);
    xpr_sint_t sInsertHitTest = -1;
    if (sToolBarCtrl.InsertMarkHitTest(&aPoint, &sTbInsertMark) && sTbInsertMark.iButton >= 0)
        sInsertHitTest = sTbInsertMark.iButton;

    xpr_sint_t sBookmark, sInsert;
    if (XPR_TEST_BITS(aKeyState, MK_SHIFT) && XPR_TEST_BITS(aKeyState, MK_CONTROL))
    {
        sInsert = -1;
        sBookmark = -1;
    }
    else
    {
        if (sHitTest < 0 || sInsertHitTest >= 0)
        {
            if (sTbInsertMark.dwFlags == TBIMHT_AFTER)
            {
                sTbInsertMark.dwFlags = 0;
                sInsertHitTest++;
            }

            if (sHitTest == -1)
                sInsertHitTest = -1;

            sInsert = sInsertHitTest;
            sBookmark = -1;
        }
        else
        {
            sInsert = -1;
            sBookmark = sHitTest;
        }
    }

    BookmarkMgr &sBookmarkMgr = BookmarkMgr::instance();

    if (sBookmark >= sBookmarkMgr.getCount())
        sBookmark = -1;

    if (sBookmark != -1)
    {
        sDropEffect = DROPEFFECT_LINK;

        BookmarkItem *sBookmarkItem = sBookmarkMgr.getBookmark(sBookmark);
        if (sBookmarkItem != XPR_NULL)
        {
            xpr_bool_t sGetPidl = XPR_TRUE;
            LPITEMIDLIST sFullPidl = XPR_NULL;

            if (IsNetItem(sBookmarkItem->mPath.c_str()))
                sGetPidl = WnetMgr::instance().isConnectedServer(sBookmarkItem->mPath);

            if (sGetPidl == XPR_TRUE)
                sFullPidl = Path2Pidl(sBookmarkItem->mPath);

            if (sFullPidl != XPR_NULL && IsFileSystemFolder(sFullPidl))
            {
                sDropEffect = DROPEFFECT_MOVE;

                if (aKeyState & MK_ALT)          sDropEffect = DROPEFFECT_LINK;
                else if (aKeyState & MK_CONTROL) sDropEffect = DROPEFFECT_COPY;
                else if (aKeyState & MK_SHIFT)   sDropEffect = DROPEFFECT_MOVE;
                else
                {
                    switch (gOpt->mConfig.mDragDefaultFileOp)
                    {
                    case DRAG_FILE_OP_DEFAULT:  sDropEffect = DROPEFFECT_MOVE; break;
                    case DRAG_FILE_OP_COPY:     sDropEffect = DROPEFFECT_COPY; break;
                    case DRAG_FILE_OP_LINK:     sDropEffect = DROPEFFECT_LINK; break;
                    default:                    sDropEffect = DROPEFFECT_MOVE; break;
                    }
                }
            }
            else
            {
                HRESULT       sHResult;
                LPSHELLFOLDER sShellFolder = XPR_NULL;
                LPCITEMIDLIST sPidl        = XPR_NULL;

                fxfile::base::Pidl::getSimplePidl(sFullPidl, sShellFolder, sPidl);

                sHResult = E_FAIL;
                LPDROPTARGET sDropTarget = XPR_NULL;

                if (sShellFolder != XPR_NULL && sPidl != XPR_NULL)
                {
                    sHResult = sShellFolder->GetUIObjectOf(GetSafeHwnd(),
                        1,
                        (LPCITEMIDLIST *)&sPidl,
                        IID_IDropTarget,
                        XPR_NULL,
                        (LPVOID *)&sDropTarget);
                }

                xpr_bool_t sSucceeded = XPR_FALSE;

                if (SUCCEEDED(sHResult) && sDropTarget != XPR_NULL)
                {
                    POINTL sPointL = {0};
                    ClientToScreen(&aPoint);
                    sPointL.x = aPoint.x;
                    sPointL.y = aPoint.y;

                    DWORD sTempDropEffect = DROPEFFECT_MOVE | DROPEFFECT_COPY | DROPEFFECT_LINK;

                    sHResult = sDropTarget->DragEnter(aOleDataObject->m_lpDataObject, aKeyState, sPointL, &sTempDropEffect);
                    if (SUCCEEDED(sHResult))
                    {
                        sHResult = sDropTarget->DragOver(aKeyState, sPointL, &sTempDropEffect);
                        sSucceeded = SUCCEEDED(sHResult) && sTempDropEffect != 0;
                    }
                }

                COM_RELEASE(sDropTarget);
                COM_RELEASE(sShellFolder);

                if (sSucceeded == XPR_TRUE)
                    sDropEffect = DROPEFFECT_MOVE;
                else
                {
                    sBookmark = -1;
                    sDropEffect = DROPEFFECT_NONE;
                }
            }

            COM_FREE(sFullPidl);
        }
        else
        {
            sBookmark = -1;
            sDropEffect = DROPEFFECT_NONE;
        }
    }

    if (sBookmark == -1)
    {
        // Insert Bookmark
        if (mOldBookmark >= 0)
        {
            sToolBarCtrl.GetButton(mOldBookmark, &sTbButton);
            sToolBarCtrl.SetState(sTbButton.idCommand, TBSTATE_ENABLED);
        }

        if (mOldInsert != sInsert)
        {
            sTbInsertMark.iButton = sInsert;
            sToolBarCtrl.SetInsertMark(&sTbInsertMark);
        }

        sDropEffect = DROPEFFECT_LINK;
    }
    else
    {
        // Run Program or File Operation
        if (mOldInsert >= 0)
        {
            sTbInsertMark.iButton = -1;
            sToolBarCtrl.SetInsertMark(&sTbInsertMark);
        }

        if (mOldBookmark != sBookmark)
        {
            sToolBarCtrl.GetButton(mOldBookmark, &sTbButton);
            sToolBarCtrl.SetState(sTbButton.idCommand, TBSTATE_ENABLED);

            sToolBarCtrl.GetButton(sBookmark, &sTbButton);
            sToolBarCtrl.SetState(sTbButton.idCommand, TBSTATE_MARKED | TBSTATE_PRESSED | TBSTATE_ENABLED);
        }
    }

    mOldInsert = sInsert;
    mOldBookmark = sBookmark;

    if (XPR_IS_NOT_NULL(mObserver))
    {
        mObserver->onBookmarkToolBarStatus(*this, mOldBookmark, mOldInsert, sDropEffect);
    }

    return sDropEffect;
}

void BookmarkToolBar::OnDragScroll(DWORD aKeyState, CPoint aPoint)
{
}

void BookmarkToolBar::OnDrop(COleDataObject *aOleDataObject, DROPEFFECT aDropEffect, CPoint aPoint)
{
    CToolBarCtrl &sToolBarCtrl = GetToolBarCtrl();

    DragImage &sDragImage = DragImage::instance();

    if (sDragImage.isMyDragging() == XPR_TRUE)
    {
        CImageList *sMyDragImage = sDragImage.getMyDragImage();
        if (XPR_IS_NOT_NULL(sMyDragImage))
        {
            sMyDragImage->EndDrag();
            sMyDragImage->DragShowNolock(XPR_FALSE);
        }
    }
    else
    {
        if (mDropTarget.isUseDropHelper() == XPR_TRUE)
            mDropTarget.getDropHelper()->DragLeave();
    }

    if (mOldInsert >= 0)
    {
        TBINSERTMARK sTbInsertMark = {0};
        sTbInsertMark.iButton = -1;
        sToolBarCtrl.SetInsertMark(&sTbInsertMark);
    }

    if (mOldBookmark >= 0)
    {
        TBBUTTON sTbButton = {0};
        sToolBarCtrl.GetButton(mOldBookmark, &sTbButton);
        sToolBarCtrl.SetState(sTbButton.idCommand, TBSTATE_ENABLED);
    }

    if (XPR_IS_NOT_NULL(mObserver))
    {
        mObserver->onBookmarkToolBarStatus(*this, -1, -1, DROPEFFECT_NONE);
    }

    COleDataObject *sOleDataObject = aOleDataObject;
    if (sOleDataObject->IsDataAvailable(mShellIDListClipFormat) == XPR_FALSE)
        return;

    // 3 Operation
    //--------------------------------------------------
    // 1. Insert Bookmark Item
    // 2. Run Program & Bookmark File
    // 3. File Operation

    STGMEDIUM sStgMedium = {0};
    FORMATETC sFormatEtc = {mShellIDListClipFormat, XPR_NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL};
    if (sOleDataObject->GetData(mShellIDListClipFormat, &sStgMedium, &sFormatEtc) == XPR_FALSE)
        return;

    if (mOldBookmark < 0)
    {
        OnDropInsertBookmark(&sStgMedium, mOldInsert);
    }
    else
    {
        xpr_bool_t sFileOperation = XPR_FALSE;

        BookmarkItem *sBookmarkItem = BookmarkMgr::instance().getBookmark(mOldBookmark);
        if (mOldBookmark >= 0 && sBookmarkItem != XPR_NULL)
        {
            LPITEMIDLIST sFullPidl = Path2Pidl(sBookmarkItem->mPath);
            sFileOperation = (sFullPidl != XPR_NULL && IsFileSystemFolder(sFullPidl));
            COM_FREE(sFullPidl);
        }

        if (sFileOperation == XPR_TRUE)
        {
            if (aDropEffect == DROPEFFECT_LINK)
                OnDropShortcut(&sStgMedium, mOldBookmark);
            else
                OnDropFileOperation(&sStgMedium, aDropEffect, mOldBookmark);
        }
        else
        {
            OnDropTarget(sOleDataObject, aDropEffect, aPoint, mOldBookmark);
        }
    }

    ::ReleaseStgMedium(&sStgMedium);
}

void BookmarkToolBar::OnDropFileOperation(STGMEDIUM *aStgMedium, DROPEFFECT aDropEffect, xpr_sint_t aBookmark)
{
    if (aStgMedium == XPR_NULL)
        return;

    BookmarkItem *sBookmarkItem = BookmarkMgr::instance().getBookmark(aBookmark);
    if (sBookmarkItem == XPR_NULL)
        return;

    LPIDA sIda = (LPIDA)::GlobalLock(aStgMedium->hGlobal);
    if (sIda == XPR_NULL)
        return;

    LPITEMIDLIST sFullPidl = Path2Pidl(sBookmarkItem->mPath);
    if (sFullPidl != XPR_NULL && IsFileSystemFolder(sFullPidl) == XPR_TRUE)
    {
        xpr_sint_t sCount = sIda->cidl;
        LPITEMIDLIST sFullPidl2 = XPR_NULL; // fully-qualified PIDL
        LPITEMIDLIST sPidl1 = XPR_NULL; // folder PIDL
        LPITEMIDLIST sPidl2 = XPR_NULL; // item PIDL

        // get folder PIDL
        sPidl1 = (LPITEMIDLIST)(((xpr_byte_t *)sIda)+(sIda)->aoffset[0]);

        xpr_sint_t sSourceCount = 0;
        xpr_tchar_t *sSource = XPR_NULL;
        xpr_tchar_t *sTarget = XPR_NULL;
        xpr_tchar_t *sSourceEnum = XPR_NULL;

        sTarget = new xpr_tchar_t[XPR_MAX_PATH + 1];
        GetName(sFullPidl, SHGDN_FORPARSING, sTarget);
        sTarget[_tcslen(sTarget)+1] = '\0';

        sSource = new xpr_tchar_t[(XPR_MAX_PATH + 1) * sCount];
        sSourceEnum = sSource;

        xpr_sint_t i;
        for (i = 0; i < sCount; ++i)
        {
            // get item PIDL and get full-qualified PIDL
            sPidl2 = (LPITEMIDLIST)(((xpr_byte_t *)sIda)+(sIda)->aoffset[i+1]);
            sFullPidl2 = fxfile::base::Pidl::concat(sPidl1, sPidl2);

            if (IsFileSystem(sFullPidl2) == XPR_TRUE)
            {
                GetName(sFullPidl2, SHGDN_FORPARSING, sSourceEnum);
                sSourceEnum += _tcslen(sSourceEnum) + 1;
                sSourceCount++;
            }

            COM_FREE(sFullPidl2);
        }

        if (sSourceCount > 0)
        {
            // Last Element - 2 Null
            *sSourceEnum = '\0';

            xpr_uint_t sFunc = 0;
            WORD sFlags = 0;
            // Drop Effect
            // 1-Copy, 2-Move, 3-Bookmark, 5-Copy+Bookmark
            if (aDropEffect == DROPEFFECT_MOVE)
                sFunc = FO_MOVE;
            else
            {
                sFunc = FO_COPY;

                CString sCompare;
                AfxExtractSubString(sCompare, sSource, 0, '\0');
                sCompare = sCompare.Left(sCompare.ReverseFind('\\'));
                if (_tcscmp(sCompare, sTarget) == 0)
                    sFlags |= FOF_RENAMEONCOLLISION;
            }

            // windows vista bug
            //if (sSourceCount > 1)
            //    sFlags |= FOF_MULTIDESTFILES;

            SHFILEOPSTRUCT *sShFileOpStruct = new SHFILEOPSTRUCT;
            memset(sShFileOpStruct, 0, sizeof(SHFILEOPSTRUCT));
            sShFileOpStruct->hwnd   = AfxGetMainWnd()->GetSafeHwnd();
            sShFileOpStruct->wFunc  = sFunc;
            sShFileOpStruct->fFlags = sFlags;
            sShFileOpStruct->pFrom  = sSource;
            sShFileOpStruct->pTo    = sTarget;

            FileOpThread *sFileOpThread = new FileOpThread;
            sFileOpThread->start(sShFileOpStruct);
        }
    }

    COM_FREE(sFullPidl);
    ::GlobalUnlock(aStgMedium->hGlobal);
}

void BookmarkToolBar::OnDropShortcut(STGMEDIUM *aStgMedium, xpr_sint_t aBookmark)
{
    if (aStgMedium == XPR_NULL)
        return;

    BookmarkItem *sBookmarkItem = BookmarkMgr::instance().getBookmark(aBookmark);
    if (sBookmarkItem == XPR_NULL)
        return;

    LPIDA sIda = (LPIDA)::GlobalLock(aStgMedium->hGlobal);
    if (sIda == XPR_NULL)
        return;

    LPITEMIDLIST sFullPidl = Path2Pidl(sBookmarkItem->mPath);
    if (sFullPidl != XPR_NULL && IsFileSystemFolder(sFullPidl) == XPR_TRUE)
    {
        xpr_sint_t sCount = sIda->cidl;
        LPITEMIDLIST sFullPidl2 = XPR_NULL; // fully-qualified PIDL
        LPITEMIDLIST sPidl1 = XPR_NULL; // folder PIDL
        LPITEMIDLIST sPidl2 = XPR_NULL; // item PIDL

        // get folder PIDL
        sPidl1 = (LPITEMIDLIST)(((xpr_byte_t *)sIda)+(sIda)->aoffset[0]);

        xpr_sint_t i;
        xpr_size_t sColon;
        xpr::tstring sDir;
        xpr::tstring sName;
        xpr::tstring sBookmarkPath;
        const xpr_tchar_t *sLinkSuffix = gApp.loadString(XPR_STRING_LITERAL("common.shortcut.suffix"));

        GetName(sFullPidl, SHGDN_FORPARSING, sDir);

        for (i = 0; i < sCount; ++i)
        {
            // get item PIDL and get full-qualified PIDL
            sPidl2 = (LPITEMIDLIST)(((xpr_byte_t *)sIda)+(sIda)->aoffset[i+1]);
            sFullPidl2 = fxfile::base::Pidl::concat(sPidl1, sPidl2);

            GetName(sFullPidl2, SHGDN_INFOLDER, sName);

            sColon = sName.find(XPR_STRING_LITERAL(':'));
            if (sColon != xpr::tstring::npos)
                sName.erase(sColon, 1);

            sName += sLinkSuffix;

            if (SetNewPath(sBookmarkPath, sDir, sName, XPR_STRING_LITERAL(".lnk")))
            {
                CreateShortcut(sBookmarkPath.c_str(), sFullPidl2);
            }

            COM_FREE(sFullPidl2);
        }
    }

    COM_FREE(sFullPidl);
    ::GlobalUnlock(aStgMedium->hGlobal);
}

void BookmarkToolBar::OnDropTarget(COleDataObject *aOleDataObject, DROPEFFECT aDropEffect, CPoint aPoint, xpr_sint_t aBookmark)
{
    if (aOleDataObject == XPR_NULL)
        return;

    BookmarkItem *sBookmarkItem = BookmarkMgr::instance().getBookmark(aBookmark);
    if (sBookmarkItem == XPR_NULL)
        return;

    LPITEMIDLIST sFullPidl = Path2Pidl(sBookmarkItem->mPath);
    if (sFullPidl != XPR_NULL)
    {
        xpr_bool_t    sResult;
        LPSHELLFOLDER sShellFolder = XPR_NULL;
        LPCITEMIDLIST sPidl = XPR_NULL;

        sResult = fxfile::base::Pidl::getSimplePidl(sFullPidl, sShellFolder, sPidl);
        if (XPR_IS_TRUE(sResult))
        {
            POINTL sPointL = {0};
            ClientToScreen(&aPoint);
            sPointL.x = aPoint.x;
            sPointL.y = aPoint.y;

            DWORD sKeyState = mDropTarget.isRightDrag() ? MK_RBUTTON : MK_LBUTTON;
            DWORD sTempEffect = DROPEFFECT_MOVE;

            switch (aDropEffect)
            {
            case DROPEFFECT_MOVE: sKeyState |= MK_SHIFT;              break;
            case DROPEFFECT_COPY: sKeyState |= MK_CONTROL;            break;
            case DROPEFFECT_LINK: sKeyState |= MK_SHIFT | MK_CONTROL; break;
            }

            LPDROPTARGET sDropTarget = XPR_NULL;
            HRESULT sHResult = sShellFolder->GetUIObjectOf(
                GetSafeHwnd(),
                1,
                (LPCITEMIDLIST*)&sPidl,
                IID_IDropTarget,
                XPR_NULL,
                (LPVOID*)&sDropTarget);

            if (SUCCEEDED(sHResult) && sDropTarget != XPR_NULL)
            {
                LPDATAOBJECT sDataObject = aOleDataObject->Detach();
                if (sDataObject != XPR_NULL)
                {
                    sDropTarget->DragEnter(sDataObject, sKeyState, sPointL, &sTempEffect);
                    sDropTarget->DragOver(sKeyState, sPointL, &sTempEffect);

                    if (mDropTarget.isRightDrag() == XPR_TRUE)
                        aDropEffect = DROPEFFECT_MOVE | DROPEFFECT_COPY | DROPEFFECT_LINK;

                    sDropTarget->Drop(sDataObject, sKeyState, sPointL, &aDropEffect);
                }
            }

            COM_RELEASE(sDropTarget);
        }

        COM_RELEASE(sShellFolder);
    }

    COM_FREE(sFullPidl);
}

void BookmarkToolBar::OnDropExecFile(STGMEDIUM *aStgMedium, xpr_sint_t aBookmark)
{
    if (aStgMedium == XPR_NULL)
        return;

    BookmarkItem *sBookmarkItem = BookmarkMgr::instance().getBookmark(aBookmark);
    if (sBookmarkItem == XPR_NULL)
        return;

    LPIDA sIda = (LPIDA)::GlobalLock(aStgMedium->hGlobal);
    if (sIda == XPR_NULL)
        return;

    LPITEMIDLIST sFullPidl = Path2Pidl(sBookmarkItem->mPath);
    if (sFullPidl != XPR_NULL && IsFileSystem(sFullPidl) == XPR_TRUE && IsFileSystemFolder(sFullPidl) == XPR_FALSE)
    {
        xpr_sint_t sCount = sIda->cidl;
        LPITEMIDLIST sFullPidl2 = XPR_NULL; // fully-qualified PIDL
        LPITEMIDLIST sPidl1 = XPR_NULL; // folder PIDL
        LPITEMIDLIST sPidl2 = XPR_NULL; // item PIDL

        xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
        xpr_tchar_t sStartup[XPR_MAX_PATH + 1] = {0};
        xpr_tchar_t *sSplit;
        DWORD sAttributes;

        // get folder PIDL
        sPidl1 = (LPITEMIDLIST)(((xpr_byte_t *)sIda)+(sIda)->aoffset[0]);

        xpr_sint_t i;
        for (i = 0; i < sCount; ++i)
        {
            // get item PIDL and get full-qualified PIDL
            sPidl2 = (LPITEMIDLIST)(((xpr_byte_t *)sIda)+(sIda)->aoffset[i+1]);
            sFullPidl2 = fxfile::base::Pidl::concat(sPidl1, sPidl2);

            {
                sPath[0] = '\"';
                GetName(sFullPidl2, SHGDN_FORPARSING, sPath+1);
                _tcscpy(sStartup, sPath+1);
                _tcscat(sPath, XPR_STRING_LITERAL("\""));

                sAttributes = GetFileAttributes(sStartup);
                if (!XPR_TEST_BITS(sAttributes, FILE_ATTRIBUTE_DIRECTORY))
                {
                    sSplit = _tcsrchr(sStartup, '\\');
                    if (sSplit != XPR_NULL)
                        *sSplit = '\0';
                    if (_tcslen(sStartup) == 2) // Drive Root
                        _tcscat(sStartup, XPR_STRING_LITERAL("\\"));
                }

                ExecFile(sBookmarkItem->mPath.c_str(), sStartup, sPath);
            }

            COM_FREE(sFullPidl2);
        }
    }

    COM_FREE(sFullPidl);
    ::GlobalUnlock(aStgMedium->hGlobal);
}

void BookmarkToolBar::OnDropInsertBookmark(STGMEDIUM *aStgMedium, xpr_sint_t aInsert)
{
    if (aStgMedium == XPR_NULL)
        return;

    LPIDA sIda = (LPIDA)::GlobalLock(aStgMedium->hGlobal);
    if (sIda == XPR_NULL)
        return;

    {
        xpr_sint_t sCount = sIda->cidl;
        LPITEMIDLIST sFullPidl2 = XPR_NULL; // fully-qualified PIDL
        LPITEMIDLIST sPidl1 = XPR_NULL; // folder PIDL
        LPITEMIDLIST sPidl2 = XPR_NULL; // item PIDL

        // get folder PIDL
        sPidl1 = (LPITEMIDLIST)(((xpr_byte_t *)sIda)+(sIda)->aoffset[0]);

        xpr_sint_t i;
        xpr_sint_t sResult;

        for (i = 0; i < sCount; ++i)
        {
            // get item PIDL and get full-qualified PIDL
            sPidl2 = (LPITEMIDLIST)(((xpr_byte_t *)sIda)+(sIda)->aoffset[i+1]);
            sFullPidl2 = fxfile::base::Pidl::concat(sPidl1, sPidl2);

            {
                sResult = gFrame->addBookmark(sFullPidl2, aInsert);
                if (sResult == -1)
                    break;

                if (sResult == 1)
                {
                    updateBookmarkButton(aInsert);
                    gFrame->updateBookmark();
                }
            }

            COM_FREE(sFullPidl2);
        }
    }

    ::GlobalUnlock(aStgMedium->hGlobal);
}

void BookmarkToolBar::updateBookmarkButton(xpr_sint_t aIndex)
{
    xpr_sint_t i;
    xpr_sint_t sCount;

    sCount = BookmarkMgr::instance().getCount();
    if (aIndex == -1)
        aIndex = sCount - 1;

    BookmarkItem *sBookmarkItem = BookmarkMgr::instance().getBookmark(aIndex);

    addBookmarkButton(aIndex, sBookmarkItem);

    CToolBarCtrl &sToolBarCtrl = GetToolBarCtrl();

    for (i = aIndex + 1; i < sCount; ++i)
        sToolBarCtrl.SetCmdID(i, ID_BOOKMARK_FIRST+i);

    UpdateToolbarSize();
}

void BookmarkToolBar::updateBookmarkButton(void)
{
    CToolBarCtrl &sToolBarCtrl = GetToolBarCtrl();

    xpr_sint_t i;
    xpr_sint_t sCount;

    sCount = sToolBarCtrl.GetButtonCount();
    for (i = sCount - 1; i >= 0; --i)
        sToolBarCtrl.DeleteButton(i);

    if (mImgList.m_hImageList != XPR_NULL)
        mImgList.DeleteImageList();

    mImgList.Create(16, 16, ILC_COLOR32 | ILC_MASK, -1, -1);
    GetToolBarCtrl().SetImageList(&mImgList);

    setBookmark();
}

xpr_bool_t BookmarkToolBar::PreTranslateMessage(MSG* pMsg) 
{
    return super::PreTranslateMessage(pMsg);
}

void BookmarkToolBar::OnGetDispInfo(NMHDR *pNMHDR, LRESULT *pResult)
{
    NMTBDISPINFO *sNmTbDispInfo = (NMTBDISPINFO *)pNMHDR;
    *pResult = 0;

    if (sNmTbDispInfo->dwMask == TBNF_IMAGE)
    {
        xpr_sint_t sIndex = CommandToIndex(sNmTbDispInfo->idCommand);

        sNmTbDispInfo->iImage = sIndex;
    }

    sNmTbDispInfo->dwMask = TBNF_DI_SETITEM;
}

void BookmarkToolBar::OnContextMenu(CWnd* pWnd, CPoint aPoint)
{
    CToolBarCtrl &sToolBarCtrl = GetToolBarCtrl();

    CPoint sClientPoint(aPoint);
    ScreenToClient(&sClientPoint);

    xpr_bool_t sSucceeded = XPR_FALSE;
    xpr_sint_t sIndex = sToolBarCtrl.HitTest(&sClientPoint);
    if (GetKeyState(VK_CONTROL) < 0 && sIndex >= 0)
    {
        BookmarkItem *sBookmarkItem = BookmarkMgr::instance().getBookmark(sIndex);
        if (sBookmarkItem != XPR_NULL)
        {
            LPITEMIDLIST sFullPidl = sBookmarkItem->getPidl();
            if (sFullPidl != XPR_NULL)
            {
                xpr_bool_t    sResult;
                LPSHELLFOLDER sShellFolder = XPR_NULL;
                LPCITEMIDLIST sPidl        = XPR_NULL;

                sResult = fxfile::base::Pidl::getSimplePidl(sFullPidl, sShellFolder, sPidl);
                if (SUCCEEDED(sResult) && sShellFolder != XPR_NULL && sPidl != XPR_NULL)
                {
                    ContextMenu sContextMenu(GetSafeHwnd());
                    if (sContextMenu.init(sShellFolder, &sPidl, 1))
                    {
                        TBBUTTON sTbButton = {0};
                        sToolBarCtrl.GetButton(sIndex, &sTbButton);
                        sToolBarCtrl.SetState(sTbButton.idCommand, TBSTATE_MARKED | TBSTATE_ENABLED);

                        xpr_uint_t sId = sContextMenu.trackPopupMenu(TPM_LEFTALIGN | TPM_RETURNCMD | TPM_RIGHTBUTTON, &aPoint, CMF_EXPLORE);
                        if (sId != -1)
                        {
                            sId -= sContextMenu.getFirstId();

                            xpr_tchar_t sVerb[0xff] = {0};
                            sContextMenu.getCommandVerb(sId, sVerb, 0xfe);

                            xpr_bool_t sSelfInvoke = XPR_FALSE;

                            if (_tcsicmp(sVerb, CMID_VERB_OPEN) == 0)
                            {
                                gFrame->gotoBookmark(sIndex);
                                sSelfInvoke = XPR_TRUE;
                            }

                            if (sSelfInvoke == XPR_FALSE)
                                sContextMenu.invokeCommand(sId);
                        }

                        sToolBarCtrl.GetButton(sIndex, &sTbButton);
                        sToolBarCtrl.SetState(sTbButton.idCommand, TBSTATE_ENABLED);
                    }

                    sContextMenu.destroy();

                    sSucceeded = XPR_TRUE;
                }

                COM_RELEASE(sShellFolder);
                COM_FREE(sFullPidl);
            }
        }
    }

    if (sSucceeded == XPR_FALSE)
    {
        BCMenu sMenu;
        if (sMenu.LoadMenu(IDR_COOLBAR_BOOKMARKBAR) == XPR_TRUE)
        {
            BCMenu *sPopupMenu = (BCMenu *)sMenu.GetSubMenu(0);
            if (XPR_IS_NOT_NULL(sPopupMenu))
                sPopupMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, aPoint, AfxGetMainWnd());
        }
    }
}
} // namespace fxfile
