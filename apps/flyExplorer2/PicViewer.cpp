//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "PicViewer.h"

#include "fxb/fxb_string_table.h"

#include "rgc/BCMenu.h"

#include "resource.h"
#include "MainFrame.h"
#include "ExplorerView.h"
#include "ExplorerCtrl.h"
#include "DlgStateMgr.h"
#include "DlgState.h"

#include "command_string_table.h"
#include "desktop_background.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

enum
{
    LOCK_NONE = 0,
    LOCK_SIZE,
    LOCK_WINDOW,
    LOCK_100,
    LOCK_THUMBNAIL,
};

// user defined message
enum
{
    WM_LOADED_PICTURE = WM_USER + 0,
};

struct PicViewer::PictureItem
{
    xpr_tchar_t mPath[XPR_MAX_PATH + 1];
    xpr_bool_t  mLoadByExt;
    xpr_bool_t  mActualSize;
    xpr_bool_t  mDocking;
    xpr_sint_t  mLock;
    CSize       mWndSize;
    CSize       mLockImgSize;
};

struct PicViewer::LoadedPicture
{
    CBitmap    *mBitmap;
    xpr_bool_t  mPicture;
    CSize       mImgSize;
    CSize       mRealImgSize;
};

#define MIN_WND_SIZE 70

namespace fxb
{
void GetDIBFromBitmap(GFL_BITMAP *bitmap, BITMAPINFOHEADER *bitmap_info, xpr_uchar_t **data);
} // namespace fxb

PicViewer::PicViewer(void)
    : super(IDD_PIC_VIEWER, XPR_NULL)
{
    mPath[0]     = '\0';
    mImgSize     = CSize(0,0);
    mRealImgSize = CSize(0,0);
    mLockImgSize = CSize(0,0);
    mPopupRect   = CRect(0,0,200,190);
    mPicture     = XPR_FALSE;

    mDocking     = XPR_FALSE;
    mRatio       = XPR_FALSE;
    mLock        = LOCK_WINDOW;
    mAutoHide    = XPR_FALSE;
    mFileName    = XPR_FALSE;
    mHide        = XPR_FALSE;

    mDc          = XPR_NULL;
    mLastRect    = CRect(0,0,0,0);
    mWndRect     = CRect(0,0,0,0);
    mTracking    = XPR_FALSE;
    mHitTest     = -1;

    mThread      = XPR_NULL;
    mEvent       = XPR_NULL;
    mStopEvent   = XPR_NULL;
    mThreadId    = 0;

    mBitmap      = XPR_NULL;
    mDlgState    = XPR_NULL;

    mGflFileInfo.Type = 0;
}

PicViewer::~PicViewer(void)
{
    PictureItem *sPictureItem;
    PictureDeque::iterator sIterator;

    sIterator = mPictureDeque.begin();
    for (; sIterator != mPictureDeque.end(); ++sIterator)
    {
        sPictureItem = *sIterator;
        XPR_SAFE_DELETE(sPictureItem);
    }

    mPictureDeque.clear();
}

void PicViewer::DoDataExchange(CDataExchange *aDX)
{
    super::DoDataExchange(aDX);
}

BEGIN_MESSAGE_MAP(PicViewer, super)
    ON_WM_DESTROY()
    ON_WM_PAINT()
    ON_WM_SIZE()
    ON_WM_CONTEXTMENU()
    ON_WM_ERASEBKGND()
    ON_WM_GETMINMAXINFO()
    ON_WM_LBUTTONDOWN()
    ON_WM_SIZING()
    ON_WM_SETFOCUS()
    ON_WM_NCLBUTTONDOWN()
    ON_WM_MOUSEMOVE()
    ON_WM_LBUTTONUP()
    ON_WM_INITMENUPOPUP()
    ON_WM_CLOSE()
    ON_WM_MEASUREITEM()
    ON_WM_MENUCHAR()
    ON_WM_CREATE()
    ON_COMMAND(ID_VIEW_PIC_HIDE,           OnPicHide)
    ON_COMMAND(ID_VIEW_PIC_SHOW_HIDE,      OnPicShowHide)
    ON_COMMAND(ID_VIEW_PIC_CLOSE,          OnPicClose)
    ON_COMMAND(ID_VIEW_PIC_DOCKING,        OnPicDocking)
    ON_COMMAND(ID_VIEW_PIC_ZOOM_IN,        OnPicZoomIn)
    ON_COMMAND(ID_VIEW_PIC_ZOOM_OUT,       OnPicZoomOut)
    ON_COMMAND(ID_VIEW_PIC_ZOOM_100,       OnPicZoom100)
    ON_COMMAND(ID_VIEW_PIC_RATIO,          OnPicRatio)
    ON_COMMAND(ID_VIEW_PIC_LOCK,           OnPicLock)
    ON_COMMAND(ID_VIEW_PIC_CLIENT,         OnPicClient)
    ON_COMMAND(ID_VIEW_PIC_LOCK_100,       OnPicLock100)
    ON_COMMAND(ID_VIEW_PIC_LOCK_THUMBNAIL, OnPicLockThumbnail)
    ON_COMMAND(ID_VIEW_PIC_BACK_CENTER,    OnPicBackCenter)
    ON_COMMAND(ID_VIEW_PIC_BACK_TILE,      OnPicBackTile)
    ON_COMMAND(ID_VIEW_PIC_BACK_STRETCH,   OnPicBackStretch)
    ON_COMMAND(ID_VIEW_PIC_AUTO_HIDE,      OnPicAutoHide)
    ON_COMMAND(ID_VIEW_PIC_SHOW_FILENAME,  OnPicShowFileName)
    ON_UPDATE_COMMAND_UI(ID_VIEW_PIC_ZOOM_IN,        OnUpdatePicZoomIn)
    ON_UPDATE_COMMAND_UI(ID_VIEW_PIC_ZOOM_OUT,       OnUpdatePicZoomOut)
    ON_UPDATE_COMMAND_UI(ID_VIEW_PIC_ZOOM_100,       OnUpdatePicZoom100)
    ON_UPDATE_COMMAND_UI(ID_VIEW_PIC_RATIO,          OnUpdatePicRatio)
    ON_UPDATE_COMMAND_UI(ID_VIEW_PIC_AUTO_HIDE,      OnUpdatePicAutoHide)
    ON_UPDATE_COMMAND_UI(ID_VIEW_PIC_DOCKING,        OnUpdatePicDocking)
    ON_UPDATE_COMMAND_UI(ID_VIEW_PIC_LOCK,           OnUpdatePicLock)
    ON_UPDATE_COMMAND_UI(ID_VIEW_PIC_CLIENT,         OnUpdatePicClient)
    ON_UPDATE_COMMAND_UI(ID_VIEW_PIC_LOCK_100,       OnUpdatePicLock100)
    ON_UPDATE_COMMAND_UI(ID_VIEW_PIC_LOCK_THUMBNAIL, OnUpdatePicLockThumbnail)
    ON_UPDATE_COMMAND_UI(ID_VIEW_PIC_SHOW_FILENAME,  OnUpdatePicShowFileName)
    ON_MESSAGE(WM_LOADED_PICTURE, OnLoadedPicture)
END_MESSAGE_MAP()

xpr_bool_t PicViewer::Create(CWnd *aParentWnd) 
{
    return super::Create(IDD_PIC_VIEWER, aParentWnd);
}

xpr_sint_t PicViewer::OnCreate(LPCREATESTRUCT aCreateStruct)
{
    if (super::OnCreate(aCreateStruct) == -1)
        return -1;

    mStopEvent = ::CreateEvent(XPR_NULL, XPR_TRUE, XPR_FALSE, XPR_NULL);
    mEvent     = ::CreateEvent(XPR_NULL, XPR_FALSE, XPR_FALSE, XPR_NULL);
    mThread    = (HANDLE)::_beginthreadex(XPR_NULL, 0, ImgLoadProc, this, 0, &mThreadId);
    ::SetThreadPriority(mThread, THREAD_PRIORITY_BELOW_NORMAL);

    return 0;
}

xpr_bool_t PicViewer::OnInitDialog(void) 
{
    super::OnInitDialog();

    mDlgState = DlgStateMgr::instance().getDlgState(XPR_STRING_LITERAL("PicViewer"));
    if (XPR_IS_NOT_NULL(mDlgState))
    {
        mDlgState->load();

        mDocking     = mDlgState->getStateB(XPR_STRING_LITERAL("Docking"),        XPR_FALSE);
        mRatio       = mDlgState->getStateB(XPR_STRING_LITERAL("Ratio"),          XPR_FALSE);
        mLock        = mDlgState->getStateI(XPR_STRING_LITERAL("Lock"),           LOCK_WINDOW);
        mAutoHide    = mDlgState->getStateB(XPR_STRING_LITERAL("Auto Hide"),      XPR_FALSE);
        mFileName    = mDlgState->getStateB(XPR_STRING_LITERAL("Show File Name"), XPR_TRUE);
        CString sPos = mDlgState->getStateS(XPR_STRING_LITERAL("Window"),         XPR_STRING_LITERAL(""));

        CRect sRect(mPopupRect);
        if (sPos.IsEmpty() == XPR_TRUE)
        {
            CRect sWindowRect;
            gFrame->GetWindowRect(&sWindowRect);
            mPopupRect.left    = sWindowRect.left + 20;
            mPopupRect.top     = sWindowRect.top  + 20;
            mPopupRect.right  += sWindowRect.left + 20;
            mPopupRect.bottom += sWindowRect.top  + 20;
            sRect = mPopupRect;
        }
        _stscanf(sPos, XPR_STRING_LITERAL("%d,%d,%d,%d"), &sRect.left, &sRect.top, &sRect.right, &sRect.bottom);
        SetWindowPos(XPR_NULL, sRect.left, sRect.top, sRect.Width(), sRect.Height(), SWP_NOZORDER);
    }

    if (mDocking == XPR_TRUE)
    {
        mDocking = XPR_FALSE;
        toggleDocking();
    }
    else
    {
        ShowWindow(SW_SHOW);
    }

    ExplorerCtrl *sExplorerCtrl = gFrame->getExplorerCtrl();
    if (XPR_IS_NOT_NULL(sExplorerCtrl))
    {
        if (sExplorerCtrl->GetSelectedCount() == 1)
        {
            xpr_sint_t sIndex = sExplorerCtrl->GetSelectionMark();
            LPLVITEMDATA sLvItemData = (LPLVITEMDATA)sExplorerCtrl->GetItemData(sIndex);
            if (sLvItemData != XPR_NULL && XPR_TEST_BITS(sLvItemData->mShellAttributes, SFGAO_FILESYSTEM))
            {
                xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
                fxb::GetName(sLvItemData->mShellFolder, sLvItemData->mPidl, SHGDN_FORPARSING, sPath);

                setPicture(sPath);
            }
        }
    }

    SetWindowText(theApp.loadString(XPR_STRING_LITERAL("popup.pic_viewer.title")));

    return XPR_TRUE;
}

void PicViewer::OnDestroy(void) 
{
    super::OnDestroy();

    if (mBitmap != XPR_NULL)
    {
        if (mBitmap->m_hObject != XPR_NULL)
            mBitmap->DeleteObject();
        XPR_SAFE_DELETE(mBitmap);
    }
}

xpr_bool_t PicViewer::DestroyWindow(void) 
{
    if (mBitmap && mBitmap->m_hObject)
        mBitmap->DeleteObject();
    XPR_SAFE_DELETE(mBitmap);

    if (XPR_IS_NOT_NULL(mDlgState))
    {
        CRect sRect(mPopupRect);
        if (mDocking == XPR_FALSE)
            GetWindowRect(sRect);

        xpr_tchar_t sPos[0xff] = {0};
        _stprintf(sPos, XPR_STRING_LITERAL("%d,%d,%d,%d"), sRect.left, sRect.top, sRect.right, sRect.bottom);

        mDlgState->reset();
        mDlgState->setStateB(XPR_STRING_LITERAL("Docking"),        mDocking);
        mDlgState->setStateB(XPR_STRING_LITERAL("Ratio"),          mRatio);
        mDlgState->setStateI(XPR_STRING_LITERAL("Lock"),           mLock);
        mDlgState->setStateB(XPR_STRING_LITERAL("Auto Hide"),      mAutoHide);
        mDlgState->setStateB(XPR_STRING_LITERAL("Show File Name"), mFileName);
        mDlgState->setStateS(XPR_STRING_LITERAL("Window"),         sPos);
        mDlgState->save();
    }

    if (mThread != XPR_NULL)
    {
        ::SetEvent(mStopEvent);
        ::WaitForSingleObject(mThread, INFINITE);

        CLOSE_HANDLE(mThread);
        CLOSE_HANDLE(mEvent);
        CLOSE_HANDLE(mStopEvent);
    }

    return super::DestroyWindow();
}

unsigned __stdcall PicViewer::ImgLoadProc(void *aParam)
{
    DWORD sExitCode = 0;

    PicViewer *sDlg = (PicViewer *)aParam;
    if (sDlg != XPR_NULL)
        sExitCode = sDlg->OnImgLoadProc();

    ::_endthreadex(sExitCode);
    return 0;
}

DWORD PicViewer::OnImgLoadProc(void)
{
    PictureItem *sPictureItem, *sPictureQueue2;

    GFL_BITMAP *sGflBitmap = XPR_NULL;
    GFL_LOAD_PARAMS sGflLoadParams;

    CSize sImgSize, sRealImgSize;

    CBitmap *sBitmap;
    xpr_bool_t sPicture;
    xpr_char_t sPathA[XPR_MAX_PATH + 1];
    xpr_size_t sOutputBytes;

    xpr_uchar_t *sDibData = XPR_NULL;
    BITMAPINFOHEADER sDibInfo;

    DWORD sWait;
    HANDLE sEvents[2] = { mStopEvent, mEvent };

    while (true)
    {
        sWait = ::WaitForMultipleObjects(2, sEvents, XPR_FALSE, INFINITE);
        if (sWait == WAIT_OBJECT_0)
            break;

        if (sWait != WAIT_OBJECT_0+1)
            continue;

        {
            fxb::CsLocker sLocker(mCs);

            if (mPictureDeque.empty() == true)
                continue;

            sPictureItem = mPictureDeque.back();
            mPictureDeque.pop_back();

            PictureDeque::iterator sIterator = mPictureDeque.begin();
            for (; sIterator != mPictureDeque.end(); ++sIterator)
            {
                sPictureQueue2 = *sIterator;
                XPR_SAFE_DELETE(sPictureQueue2);
            }
            mPictureDeque.clear();

            if (sPictureItem == XPR_NULL)
                continue;
        }

        sPicture = XPR_FALSE;

        sOutputBytes = XPR_MAX_PATH * sizeof(xpr_char_t);
        XPR_TCS_TO_MBS(sPictureItem->mPath, _tcslen(sPictureItem->mPath) * sizeof(xpr_tchar_t), sPathA, &sOutputBytes);
        sPathA[sOutputBytes / sizeof(xpr_char_t)] = 0;

        sBitmap = XPR_NULL;
        sPicture = XPR_FALSE;
        sImgSize.cx = sImgSize.cy = 0;
        sRealImgSize.cx = sRealImgSize.cy = 0;

        GFL_ERROR sGflError = GFL_NO_ERROR;

        if (sPictureItem->mDocking == XPR_TRUE || sPictureItem->mLock == LOCK_THUMBNAIL)
        {
            gflGetDefaultPreviewParams(&sGflLoadParams); 
            sGflLoadParams.Flags      |= GFL_LOAD_SKIP_ALPHA | GFL_LOAD_PREVIEW_NO_CANVAS_RESIZE;
            sGflLoadParams.Origin      = GFL_BOTTOM_LEFT;
            sGflLoadParams.ColorModel  = GFL_BGR; 
            sGflLoadParams.LinePadding = 4;

            if (sPictureItem->mLoadByExt == XPR_TRUE)
                sGflLoadParams.Flags |= GFL_LOAD_BY_EXTENSION_ONLY;

            sGflError = gflLoadPreview(
                sPathA,
                sPictureItem->mWndSize.cx,
                sPictureItem->mWndSize.cy,
                &sGflBitmap,
                &sGflLoadParams,
                XPR_NULL);

            if (sGflError == GFL_NO_ERROR)
            {
                sImgSize = sRealImgSize = CSize(sGflBitmap->Width, sGflBitmap->Height);
            }
        }
        else
        {
            gflGetDefaultLoadParams(&sGflLoadParams);
            sGflLoadParams.Flags      |= GFL_LOAD_SKIP_ALPHA;
            sGflLoadParams.Origin      = GFL_BOTTOM_LEFT;
            sGflLoadParams.ColorModel  = GFL_BGR; 
            sGflLoadParams.LinePadding = 4;

            if (sPictureItem->mLoadByExt == XPR_TRUE)
                sGflLoadParams.Flags |= GFL_LOAD_BY_EXTENSION_ONLY;

            sGflError = gflLoadBitmap(
                sPathA,
                &sGflBitmap,
                &sGflLoadParams,
                XPR_NULL/*&mGflFileInfo*/);

            if (sGflError == GFL_NO_ERROR)
            {
                sRealImgSize = CSize(sGflBitmap->Width, sGflBitmap->Height);

                if (sPictureItem->mActualSize == XPR_FALSE)
                {
                    if (sPictureItem->mLock == LOCK_SIZE)
                    {
                        if (sPictureItem->mLockImgSize.cx != 0 && sPictureItem->mLockImgSize.cy != 0)
                            sImgSize = sPictureItem->mLockImgSize;
                        else
                        {
                            if (sRealImgSize.cx <= sPictureItem->mWndSize.cx && sRealImgSize.cy <= sPictureItem->mWndSize.cy)
                                sImgSize = sRealImgSize;
                            else
                                sImgSize = sPictureItem->mWndSize;
                        }
                    }
                    else if (sPictureItem->mLock == LOCK_100)
                        sImgSize = sRealImgSize;
                    else
                        sImgSize = sPictureItem->mWndSize;

                    gflResize(sGflBitmap, XPR_NULL, sImgSize.cx, sImgSize.cy, /*GFL_RESIZE_BILINEAR*/GFL_RESIZE_QUICK, 0);
                }
            }
        }

        if (sGflError == GFL_NO_ERROR)
        {
            fxb::GetDIBFromBitmap(sGflBitmap, &sDibInfo, &sDibData);
            if (sPictureItem->mActualSize == XPR_TRUE)
                sImgSize = CSize(sDibInfo.biWidth, sDibInfo.biHeight);

            CClientDC sClientDc(this);

            CDC sMemDc;
            sMemDc.CreateCompatibleDC(&sClientDc);

            sBitmap = new CBitmap;
            sBitmap->CreateCompatibleBitmap(&sClientDc, sImgSize.cx, sImgSize.cy);

            CBitmap *sOldBitmap = sMemDc.SelectObject(sBitmap);

            StretchDIBits(
                sMemDc.m_hDC,
                0,
                0,
                sImgSize.cx,
                sImgSize.cy,
                0,
                0,
                sImgSize.cx,
                sImgSize.cy,
                (sDibData != XPR_NULL) ? sDibData : sGflBitmap->Data,
                (BITMAPINFO *)&sDibInfo,
                DIB_RGB_COLORS,
                SRCCOPY);

            sMemDc.SelectObject(sOldBitmap);

            if (sDibData != XPR_NULL)  { free(sDibData); sDibData = XPR_NULL; }
            if (sGflBitmap != XPR_NULL) { gflFreeBitmap(sGflBitmap); sGflBitmap = XPR_NULL; }

            sPicture = XPR_TRUE;
        }

        LoadedPicture *sLoadedPicture = new LoadedPicture;
        if (sLoadedPicture != XPR_NULL)
        {
            sLoadedPicture->mBitmap      = sBitmap;
            sLoadedPicture->mPicture     = sPicture;
            sLoadedPicture->mImgSize     = sImgSize;
            sLoadedPicture->mRealImgSize = sRealImgSize;

            if (PostMessage(WM_LOADED_PICTURE, (WPARAM)sLoadedPicture, (LPARAM)0) == XPR_FALSE)
            {
                if (sBitmap != XPR_NULL)
                {
                    DELETE_OBJECT(*sBitmap);
                    XPR_SAFE_DELETE(sBitmap);
                }

                XPR_SAFE_DELETE(sLoadedPicture);
            }
        }
        else
        {
            if (sBitmap != XPR_NULL)
            {
                DELETE_OBJECT(*sBitmap);
                XPR_SAFE_DELETE(sBitmap);
            }
        }

        XPR_SAFE_DELETE(sPictureItem);
    }

    return 0;
}

LRESULT PicViewer::OnLoadedPicture(WPARAM wParam, LPARAM lParam)
{
    LoadedPicture *sLoadedPicture = (LoadedPicture *)wParam;
    if (sLoadedPicture == XPR_NULL)
        return 0;

    if (mBitmap != XPR_NULL)
    {
        DELETE_OBJECT(*mBitmap);
        XPR_SAFE_DELETE(mBitmap);
    }

    mBitmap      = sLoadedPicture->mBitmap;
    mPicture     = sLoadedPicture->mPicture;
    mImgSize     = sLoadedPicture->mImgSize;
    mRealImgSize = sLoadedPicture->mRealImgSize;
    if (mLockImgSize.cx == 0 || mLockImgSize.cy == 0)
        mLockImgSize = mImgSize;

    if (mBitmap != XPR_NULL && isDocking() == XPR_FALSE)
    {
        CRect sRect, sClientRect;
        GetWindowRect(&sRect);
        GetClientRect(&sClientRect);
        CSize sEdgeSize(sRect.Width()-sClientRect.Width(), sRect.Height()-sClientRect.Height());

        if (mLock == LOCK_100)
            SetWindowPos(XPR_NULL, 0, 0, mImgSize.cx+sEdgeSize.cx, mImgSize.cy+sEdgeSize.cy, SWP_NOMOVE | SWP_NOACTIVATE);

        if (sRect.left < 0 || sRect.top < 0)
        {
            if (sRect.left < 0) sRect.left = 0;
            if (sRect.top  < 0) sRect.top  = 0;

            SetWindowPos(XPR_NULL, sRect.left, sRect.top, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE);
        }
    }

    xpr_bool_t sHide = (!mBitmap) && (isAutoHide() == XPR_TRUE || isDocking() == XPR_TRUE);

    setTitle();
    ShowWindow(sHide ? SW_HIDE : SW_SHOWNOACTIVATE);
    Invalidate();
    UpdateWindow();

    XPR_SAFE_DELETE(sLoadedPicture);

    return 0;
}

void PicViewer::setPicture(const xpr_tchar_t *aPath, xpr_bool_t aActualSize)
{
    if (aPath == XPR_NULL || _tcslen(aPath) == 0)
    {
        if (isAutoHide() == XPR_TRUE || isDocking() == XPR_TRUE)
            ShowWindow(SW_HIDE);

        return;
    }

    if (isHide() == XPR_TRUE)
    {
        if (_tcsicmp(mPath, aPath) == 0)
        {
            ShowWindow(SW_HIDE);
            return;
        }
    }

    {
        fxb::CsLocker sLocker(mCs);

        mPath[0] = XPR_STRING_LITERAL('\0');

        PictureItem *sPictureItem = new PictureItem;
        sPictureItem->mPath[0] = 0;

        if (aPath != XPR_NULL)
        {
            _tcscpy(sPictureItem->mPath, aPath);

            _tcscpy(mPath, aPath);
        }

        CRect sClientRect;
        GetClientRect(&sClientRect);

        sPictureItem->mLoadByExt   = gOpt->mThumbnailLoadByExt;
        sPictureItem->mDocking     = isDocking();
        sPictureItem->mLock        = mLock;
        sPictureItem->mWndSize.cx  = sClientRect.Width();
        sPictureItem->mWndSize.cy  = sClientRect.Height();
        sPictureItem->mLockImgSize = mLockImgSize;
        sPictureItem->mActualSize  = aActualSize;
        mPictureDeque.push_back(sPictureItem);

        ::SetEvent(mEvent);
    }
}

void PicViewer::OnPaint(void)
{
    CPaintDC sPaintDc(this);

    if (mBitmap != XPR_NULL && mBitmap->m_hObject != XPR_NULL)
    {
        CRect sClientRect;
        GetClientRect(&sClientRect);

        CDC sMemDc;
        sMemDc.CreateCompatibleDC(&sPaintDc);

        CBitmap sBitmap;
        sBitmap.CreateCompatibleBitmap(&sPaintDc, sClientRect.Width(), sClientRect.Height());
        CBitmap *sOldBitmap = sMemDc.SelectObject(&sBitmap);

        CRect sRect(sClientRect);
        sMemDc.FillSolidRect(sRect, RGB(255,255,255));

        sRect.left   = (sRect.Width()  - mImgSize.cx) / 2;
        sRect.right  = (sRect.left     + mImgSize.cx);
        sRect.top    = (sRect.Height() - mImgSize.cy) / 2;
        sRect.bottom = (sRect.top      + mImgSize.cy);

        CDC sTmpDc;
        sTmpDc.CreateCompatibleDC(&sPaintDc);
        CBitmap *sOldBitmap2 = sTmpDc.SelectObject(mBitmap);
        sMemDc.BitBlt(sRect.left, sRect.top, sRect.right, sRect.bottom, &sTmpDc, 0, 0, SRCCOPY);
        sTmpDc.SelectObject(sOldBitmap2);

        sPaintDc.BitBlt(sClientRect.left, sClientRect.top, sClientRect.right, sClientRect.bottom, &sMemDc, 0, 0, SRCCOPY);
        sMemDc.SelectObject(sOldBitmap);
    }
}

void PicViewer::OnSizing(xpr_uint_t aSide, LPRECT aRect) 
{
    super::OnSizing(aSide, aRect);

    if (mRatio == XPR_FALSE)
        return;

    if (mRealImgSize.cx == 0 || mRealImgSize.cy == 0)
        return;

    if (isDocking() == XPR_TRUE)
        return;

    CRect sRect(aRect);
    xpr_sint_t sWidth  = sRect.Width();
    xpr_sint_t sHeight = sRect.Height();

    if (aSide == WMSZ_LEFT || aSide == WMSZ_RIGHT)
    {
        sHeight = (mRealImgSize.cy * sWidth) / mRealImgSize.cx;
        aRect->bottom = aRect->top + sHeight;
    }
    else
    {
        sWidth = (mRealImgSize.cx * sHeight) / mRealImgSize.cy;
        aRect->right = aRect->left + sWidth;
    }
}

void PicViewer::OnSize(xpr_uint_t aType, xpr_sint_t cx, xpr_sint_t cy) 
{
    super::OnSize(aType, cx, cy);

    if (isDocking() == XPR_TRUE)
        return;

    if (mLock != LOCK_100)
    {
        std::tstring sPath = mPath;
        setPicture(sPath.c_str());
    }

    Invalidate(XPR_TRUE);
}

LRESULT PicViewer::OnMenuChar(xpr_uint_t aChar, xpr_uint_t aFlags, CMenu *aMenu) 
{
    LRESULT sResult;

    if (BCMenu::IsMenu(aMenu))
        sResult = BCMenu::FindKeyboardShortcut(aChar, aFlags, aMenu);
    else
        sResult = super::OnMenuChar(aChar, aFlags, aMenu);

    return sResult;
}

void PicViewer::OnInitMenuPopup(CMenu *aPopupMenu, xpr_uint_t aIndex, xpr_bool_t aSysMenu) 
{
    ASSERT(aPopupMenu != XPR_NULL);
    // Check the enabled state of various menu items.

    BCMenu *sBCPopupMenu = dynamic_cast<BCMenu *>(aPopupMenu);
    if (sBCPopupMenu != XPR_NULL)
    {
        xpr_uint_t sId;
        xpr_sint_t sCount = sBCPopupMenu->GetMenuItemCount();

        const xpr_tchar_t *sStringId;
        const xpr_tchar_t *sString;
        CString sMenuText;
        CommandStringTable &sCommandStringTable = CommandStringTable::instance();

        xpr_sint_t i;
        for (i = 0; i < sCount; ++i)
        {
            sId = sBCPopupMenu->GetMenuItemID(i);

            // apply string table
            if (sId != 0) // if sId is 0, it's separator.
            {
                if (sId == -1)
                {
                    // if sId(xpr_uint_t) is -1, it's sub-menu.
                    sBCPopupMenu->GetMenuText(i, sMenuText, MF_BYPOSITION);

                    sString = theApp.loadString(sMenuText.GetBuffer());
                    sBCPopupMenu->SetMenuText(i, (xpr_tchar_t *)sString, MF_BYPOSITION);
                }
                else
                {
                    sStringId = sCommandStringTable.loadString(sId);
                    if (sStringId != XPR_NULL)
                    {
                        sString = theApp.loadString(sStringId);

                        sBCPopupMenu->SetMenuText(sId, (xpr_tchar_t *)sString, MF_BYCOMMAND);
                    }
                }
            }
        }
    }

    CCmdUI sState;
    sState.m_pMenu = aPopupMenu;
    ASSERT(sState.m_pOther == XPR_NULL);
    ASSERT(sState.m_pParentMenu == XPR_NULL);

    // Determine if menu is popup in top-level menu and set m_pOther to
    // it if so (m_pParentMenu == XPR_NULL indicates that it is secondary popup).
    HMENU sParentMenu;
    if (AfxGetThreadState()->m_hTrackingMenu == aPopupMenu->m_hMenu)
        sState.m_pParentMenu = aPopupMenu;    // Parent == child for tracking popup.
    else if ((sParentMenu = ::GetMenu(m_hWnd)) != XPR_NULL)
    {
        CWnd *sParentWnd = this;
        // Child windows don't have menus--need to go to the top!
        if (sParentWnd != XPR_NULL &&
            (sParentMenu = ::GetMenu(sParentWnd->m_hWnd)) != XPR_NULL)
        {
            xpr_sint_t sIndex;
            xpr_sint_t sIndexMax = ::GetMenuItemCount(sParentMenu);
            for (sIndex = 0; sIndex < sIndexMax; ++sIndex)
            {
                if (::GetSubMenu(sParentMenu, sIndex) == aPopupMenu->m_hMenu)
                {
                    // When popup is found, m_pParentMenu is containing menu.
                    sState.m_pParentMenu = CMenu::FromHandle(sParentMenu);
                    break;
                }
            }
        }
    }

    sState.m_nIndexMax = aPopupMenu->GetMenuItemCount();
    for (sState.m_nIndex = 0; sState.m_nIndex < sState.m_nIndexMax; ++sState.m_nIndex)
    {
        sState.m_nID = aPopupMenu->GetMenuItemID(sState.m_nIndex);
        if (sState.m_nID == 0)
            continue; // Menu separator or invalid cmd - ignore it.

        ASSERT(sState.m_pOther == XPR_NULL);
        ASSERT(sState.m_pMenu != XPR_NULL);
        if (sState.m_nID == (xpr_uint_t)-1)
        {
            // Possibly a popup menu, route to first item of that popup.
            sState.m_pSubMenu = aPopupMenu->GetSubMenu(sState.m_nIndex);
            if (sState.m_pSubMenu == XPR_NULL ||
                (sState.m_nID = sState.m_pSubMenu->GetMenuItemID(0)) == 0 ||
                sState.m_nID == (xpr_uint_t)-1)
            {
                continue;       // First item of popup can't be routed to.
            }
            sState.DoUpdate(this, XPR_TRUE);   // Popups are never auto disabled.
        }
        else
        {
            // Normal menu item.
            // Auto enable/disable if frame window has m_bAutoMenuEnable
            // set and command is _not_ a system command.
            sState.m_pSubMenu = XPR_NULL;
            sState.DoUpdate(this, XPR_FALSE);
        }

        // Adjust for menu deletions and additions.
        xpr_uint_t sCount = aPopupMenu->GetMenuItemCount();
        if (sCount < sState.m_nIndexMax)
        {
            sState.m_nIndex -= (sState.m_nIndexMax - sCount);
            while (sState.m_nIndex < sCount &&
                aPopupMenu->GetMenuItemID(sState.m_nIndex) == sState.m_nID)
            {
                sState.m_nIndex++;
            }
        }

        sState.m_nIndexMax = sCount;
    }
}

void PicViewer::OnMeasureItem(xpr_sint_t aIDCtl, LPMEASUREITEMSTRUCT aMeasureItemStruct) 
{
    xpr_bool_t sSetFlag = XPR_FALSE;
    if (aMeasureItemStruct->CtlType == ODT_MENU)
    {
        if (IsMenu((HMENU)(uintptr_t)aMeasureItemStruct->itemID))
        {
            CMenu *sMenu = CMenu::FromHandle((HMENU)(uintptr_t)aMeasureItemStruct->itemID);
            if (BCMenu::IsMenu(sMenu))
            {
                mMenu.MeasureItem(aMeasureItemStruct);
                sSetFlag = XPR_TRUE;
            }
        }
    }

    if (sSetFlag == XPR_FALSE)
        super::OnMeasureItem(aIDCtl, aMeasureItemStruct);
}

void PicViewer::OnContextMenu(CWnd *aWnd, CPoint aPoint) 
{
    BCMenu sMenu;
    if (sMenu.LoadMenu(IDR_PIC_VIEWER) == XPR_TRUE)
    {
        BCMenu *sPopupMenu = (BCMenu *)sMenu.GetSubMenu(0);
        if (XPR_IS_NOT_NULL(sPopupMenu))
        {
            sPopupMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, aPoint, this);
        }
    }
}

void PicViewer::toggleShowHide(void)
{
    DWORD sStyle = GetStyle();
    if (XPR_TEST_BITS(sStyle, WS_VISIBLE))
    {
        mHide = XPR_TRUE;
        ShowWindow(SW_HIDE);
    }
    else
    {
        mHide = XPR_FALSE;
        ShowWindow(SW_SHOW);
    }
}

void PicViewer::OnPicShowHide(void)
{
    toggleShowHide();
}

void PicViewer::hide(void)
{
    mHide = XPR_TRUE;
    ShowWindow(SW_HIDE);
}

void PicViewer::OnPicHide(void)
{
    hide();
}

void PicViewer::OnPicClose(void)
{
    DestroyWindow();
    //ShowWindow(SW_HIDE);
}

void PicViewer::PostNcDestroy(void) 
{
    gFrame->mPicViewer = XPR_NULL;
    delete this;

    //super::PostNcDestroy();
}

void PicViewer::OnClose(void) 
{
    DestroyWindow();

    //super::OnClose();
}

xpr_bool_t PicViewer::PreTranslateMessage(MSG* pMsg) 
{
    if (pMsg->message == WM_KEYDOWN)
    {
        switch (pMsg->wParam)
        {
        case VK_ADD:      OnPicZoomIn();  return XPR_TRUE;
        case VK_SUBTRACT: OnPicZoomOut(); return XPR_TRUE;

        case VK_MULTIPLY:
        case VK_DIVIDE:   OnPicZoom100(); return XPR_TRUE;
        case VK_RETURN:   return XPR_TRUE;

        case VK_ESCAPE:
            DestroyWindow();
            return XPR_TRUE;

        default:
            break;
        }
    }

    return super::PreTranslateMessage(pMsg);
}

void PicViewer::toggleDocking(void)
{
    xpr_sint_t sViewSplitRowCount = 0, sViewSplitColumnCount = 0;
    gFrame->getViewSplit(sViewSplitRowCount, sViewSplitColumnCount);

    if (sViewSplitRowCount > 1 || sViewSplitColumnCount > 1)
    {
        const xpr_tchar_t *sMsg = theApp.loadString(XPR_STRING_LITERAL("popup.pic_viewer.msg.docking_single_split"));
        gFrame->MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);
        return;
    }

    if (gOpt->mContentsStyle != CONTENTS_EXPLORER)
    {
        const xpr_tchar_t *sMsg = theApp.loadString(XPR_STRING_LITERAL("popup.pic_viewer.msg.docking_contents"));
        gFrame->MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);
        return;
    }

    setDocking(!isDocking());
}

void PicViewer::OnPicDocking(void) 
{
    toggleDocking();
}

void PicViewer::setDocking(xpr_bool_t aDocking)
{
    if (mDocking == aDocking)
        return;

    mDocking = aDocking;

    if (aDocking == XPR_TRUE)
    {
        GetWindowRect(&mPopupRect);
        ModifyStyle(WS_POPUP | WS_CAPTION | WS_THICKFRAME, WS_CHILD);
        ModifyStyleEx(0, WS_EX_CONTROLPARENT);

        ExplorerView *sExplorerView = (ExplorerView *)gFrame->getExplorerView();
        SetParent(sExplorerView);
        SetOwner(sExplorerView);
        MoveWindow(5, 250, 190, 170);
        sExplorerView->SetFocus();

        ShowWindow(SW_HIDE);
        ExplorerCtrl *sExplorerCtrl = sExplorerView->getExplorerCtrl();
        if (sExplorerCtrl->GetSelectedCount() == 1)
        {
            xpr_sint_t sIndex = sExplorerCtrl->GetSelectionMark();
            LPLVITEMDATA sLvItemData = (LPLVITEMDATA)sExplorerCtrl->GetItemData(sIndex);
            if (sLvItemData != XPR_NULL && XPR_TEST_BITS(sLvItemData->mShellAttributes, SFGAO_FILESYSTEM))
            {
                xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
                fxb::GetName(sLvItemData->mShellFolder, sLvItemData->mPidl, SHGDN_FORPARSING, sPath);

                setPicture(sPath);
            }
        }
    }
    else
    {
        HWND sFocusHwnd = ::GetFocus();

        ModifyStyle(WS_CHILD, WS_POPUP | WS_CAPTION | WS_THICKFRAME);
        ModifyStyleEx(WS_EX_CONTROLPARENT, 0);

        SetParent(XPR_NULL);
        MoveWindow(mPopupRect.left, mPopupRect.top, mPopupRect.Width(), mPopupRect.Height());

        if (sFocusHwnd != XPR_NULL)
        {
            ExplorerCtrl *sExplorerCtrl = gFrame->getExplorerCtrl();
            if (sExplorerCtrl != XPR_NULL && sExplorerCtrl->m_hWnd != XPR_NULL)
            {
                if (sExplorerCtrl->m_hWnd == sFocusHwnd)
                    ::SetFocus(sFocusHwnd);
            }
        }
    }
}

xpr_bool_t PicViewer::OnEraseBkgnd(CDC *aDC)
{
    if (mBitmap == XPR_NULL || mBitmap->m_hObject == XPR_NULL)
    {
        CRect sClientRect;
        GetClientRect(&sClientRect);
        aDC->FillSolidRect(0, 0, sClientRect.Width(), sClientRect.Height(), RGB(255,255,255));
    }

    return XPR_TRUE;
}

void PicViewer::zoomIn(void)
{
    if (isDocking() == XPR_TRUE)
        return;

    CRect sDesktopRect;
    GetDesktopWindow()->GetClientRect(&sDesktopRect);
    xpr_sint_t sXInc = sDesktopRect.Width() / 50;
    xpr_sint_t sYInc = sDesktopRect.Height() / 50;

    CRect sRect;
    GetWindowRect(&sRect);
    ScreenToClient(&sRect);
    SetWindowPos(XPR_NULL, 0, 0, sRect.Width()+sXInc, sRect.Height()+sYInc, SWP_NOMOVE | SWP_NOACTIVATE);
}

void PicViewer::zoomOut(void)
{
    if (isDocking() == XPR_TRUE)
        return;

    CRect sDesktopRect;
    GetDesktopWindow()->GetClientRect(&sDesktopRect);
    xpr_sint_t sXDec = sDesktopRect.Width() / 50;
    xpr_sint_t sYDec = sDesktopRect.Height() / 50;

    CRect sRect;
    GetWindowRect(&sRect);
    ScreenToClient(&sRect);
    SetWindowPos(XPR_NULL, 0, 0, sRect.Width()-sXDec, sRect.Height()-sYDec, SWP_NOMOVE | SWP_NOACTIVATE);
}

void PicViewer::zoom100(void)
{
    if (isDocking() == XPR_TRUE)
        return;

    CRect sRect;
    GetWindowRect(&sRect);
    CSize sTempSize(sRect.Width()-mImgSize.cx, sRect.Height()-mImgSize.cy);

    std::tstring strPath = mPath;
    setPicture(strPath.c_str(), XPR_TRUE);

    SetWindowPos(XPR_NULL, 0, 0, mImgSize.cx+sTempSize.cx, mImgSize.cy+sTempSize.cy, SWP_NOMOVE | SWP_NOACTIVATE);
}

void PicViewer::OnPicZoomIn(void)
{
    zoomIn();
}

void PicViewer::OnPicZoomOut(void)
{
    zoomOut();
}

void PicViewer::OnPicZoom100(void)
{
    zoom100();
}

void PicViewer::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI) 
{
    CPoint sPoint(MIN_WND_SIZE, MIN_WND_SIZE);
    if (mLock == LOCK_SIZE)
    {
        if (mRealImgSize.cx > 0 && mRealImgSize.cy > 0)
        {
            sPoint.x = (mRealImgSize.cx * MIN_WND_SIZE) / mRealImgSize.cy;
            sPoint.y = (mRealImgSize.cy * MIN_WND_SIZE) / mRealImgSize.cx;
        }
    }

    lpMMI->ptMinTrackSize = sPoint;

    super::OnGetMinMaxInfo(lpMMI);
}

xpr_bool_t PicViewer::isRatioResize(void)
{
    return mRatio;
}

void PicViewer::toggleRatioResize(void)
{
    mRatio = !mRatio;
}

void PicViewer::OnPicRatio(void) 
{
    toggleRatioResize();
}

xpr_bool_t PicViewer::isLockSize(void)      { return (mLock == LOCK_SIZE)      ? XPR_TRUE : XPR_FALSE; }
xpr_bool_t PicViewer::isLockWindow(void)    { return (mLock == LOCK_WINDOW)    ? XPR_TRUE : XPR_FALSE; }
xpr_bool_t PicViewer::isLock100(void)       { return (mLock == LOCK_100)       ? XPR_TRUE : XPR_FALSE; }
xpr_bool_t PicViewer::isLockThumbnail(void) { return (mLock == LOCK_THUMBNAIL) ? XPR_TRUE : XPR_FALSE; }

void PicViewer::setLockSize(void)
{
    if (isDocking() == XPR_TRUE)
        return;

    mLock = LOCK_SIZE;

    mLockImgSize = mImgSize;
}

void PicViewer::setLockWindow(void)
{
    if (isDocking() == XPR_TRUE)
        return;

    mLock = LOCK_WINDOW;

    std::tstring sPath = mPath;
    setPicture(sPath.c_str());

    Invalidate();
}

void PicViewer::setLock100(void)
{
    if (isDocking() == XPR_TRUE)
        return;

    mLock = LOCK_100;

    std::tstring sPath = mPath;
    setPicture(sPath.c_str(), XPR_TRUE);
}

void PicViewer::setLockThumbnail(void)
{
    if (isDocking() == XPR_TRUE)
        return;

    mLock = LOCK_THUMBNAIL;

    std::tstring sPath = mPath;
    setPicture(sPath.c_str(), XPR_TRUE);
}

void PicViewer::OnPicLock(void)
{
    setLockSize();
}

void PicViewer::OnPicClient(void)
{
    setLockWindow();
}

void PicViewer::OnPicLock100(void)
{
    setLock100();
}

void PicViewer::OnPicLockThumbnail(void)
{
    setLockThumbnail();
}

void PicViewer::OnPicBackCenter(void) 
{
    setDesktop(DesktopBackground::StyleCenter);
}

void PicViewer::OnPicBackTile(void) 
{
    setDesktop(DesktopBackground::StyleTile);
}

void PicViewer::OnPicBackStretch(void) 
{
    setDesktop(DesktopBackground::StyleStretch);
}

void PicViewer::setDesktop(xpr_sint_t aStyle)
{
    DesktopBackground sDesktopBackground;
    sDesktopBackground.apply((DesktopBackground::Style)aStyle, mPath);
}

void PicViewer::OnSetFocus(CWnd* pOldWnd) 
{
    super::OnSetFocus(pOldWnd);
}

void PicViewer::OnLButtonDown(xpr_uint_t nFlags, CPoint point) 
{
    if (isDocking() == XPR_FALSE)
        PostMessage(WM_NCLBUTTONDOWN, HTCAPTION);

    super::OnLButtonDown(nFlags, point);
}

void PicViewer::OnNcLButtonDown(xpr_uint_t nHitTest, CPoint pt) 
{
    switch (nHitTest)
    {
    case HTTOP:
    case HTTOPLEFT:
    case HTLEFT:
    case HTBOTTOMLEFT:
    case HTBOTTOM:
    case HTBOTTOMRIGHT:
    case HTRIGHT:
    case HTTOPRIGHT:
        startTracking(pt);
        mHitTest = nHitTest;
        break;

    default:
        mHitTest = -1;
    }

    super::OnNcLButtonDown(nHitTest, pt);
}

void PicViewer::OnMouseMove(xpr_uint_t nFlags, CPoint pt) 
{
    if (mTracking == XPR_TRUE)
        drawTracking(pt, XPR_FALSE);

    super::OnMouseMove(nFlags, pt);
}

void PicViewer::OnLButtonUp(xpr_uint_t nFlags, CPoint pt) 
{
    if (mTracking == XPR_TRUE)
        endTracking(pt);

    super::OnLButtonUp(nFlags, pt);
}

void PicViewer::startTracking(CPoint pt)
{
    GetWindowRect(&mWndRect);
    mLastRect.SetRect(0,0,0,0);

    CWnd *sWnd = GetDesktopWindow();
    CWindowDC *sWindowDc = new CWindowDC(sWnd);
    mDc = sWindowDc;

    mTracking = XPR_TRUE;
    SetCapture();
}

void PicViewer::endTracking(CPoint pt)
{
    drawTracking(pt, XPR_TRUE);

    CWnd *sWnd = CWnd::GetDesktopWindow();
    if (mDc != XPR_NULL)
    {
        sWnd->ReleaseDC(mDc);
        delete mDc;
        mDc = XPR_NULL;
    }

    mTracking = XPR_FALSE;
    ReleaseCapture();
    MoveWindow(mLastRect);
}

void PicViewer::_TOP(CRect &aRect, CPoint aDragPoint, xpr_bool_t aRatio)
{
    aRect.top = aDragPoint.y;
    aRect.top = (aRect.top+MIN_WND_SIZE > aRect.bottom) ? aRect.bottom-MIN_WND_SIZE : aRect.top;

    _RATIO_RIGHT(aRect, aRatio);
}

void PicViewer::_LEFT(CRect &aRect, CPoint aDragPoint, xpr_bool_t aRatio)
{
    aRect.left = aDragPoint.x;
    aRect.left = (aRect.left+MIN_WND_SIZE > aRect.right) ? aRect.right-MIN_WND_SIZE : aRect.left;

    _RATIO_BOTTOM(aRect, aRatio);
}

void PicViewer::_BOTTOM(CRect &aRect, CPoint aDragPoint, xpr_bool_t aRatio)
{
    aRect.bottom = aDragPoint.y;
    aRect.bottom = (aRect.top+MIN_WND_SIZE > aRect.bottom) ? aRect.top+MIN_WND_SIZE : aRect.bottom;

    _RATIO_RIGHT(aRect, aRatio);
}

void PicViewer::_RIGHT(CRect &aRect, CPoint aDragPoint, xpr_bool_t aRatio)
{
    aRect.right = aDragPoint.x;
    aRect.right = (aRect.left+MIN_WND_SIZE > aRect.right) ? aRect.left+MIN_WND_SIZE : aRect.right;

    _RATIO_BOTTOM(aRect, aRatio);
}

void PicViewer::_RATIO_RIGHT(CRect &aRect, xpr_bool_t aRatio)
{
    if (aRatio == XPR_TRUE)
    {
        if (mRatio == XPR_FALSE)
            return;

        if (mRealImgSize.cx == 0 || mRealImgSize.cy == 0)
            return;

        if (isDocking() == XPR_TRUE)
            return;

        aRect.right = aRect.left + (mRealImgSize.cx * mLastRect.Height()) / mRealImgSize.cy;
    }
}

void PicViewer::_RATIO_BOTTOM(CRect &aRect, xpr_bool_t aRatio)
{
    if (aRatio == XPR_TRUE)
    {
        if (mRatio == XPR_FALSE)
            return;

        if (mRealImgSize.cx == 0 || mRealImgSize.cy == 0)
            return;

        if (isDocking() == XPR_TRUE)
            return;

        aRect.bottom = aRect.top + (mRealImgSize.cy * mLastRect.Width()) / mRealImgSize.cx;
    }
}

void PicViewer::drawTracking(CPoint sPoint, xpr_bool_t aRemove)
{
    CSize sEdgeSize(3,3);
    if (aRemove == XPR_TRUE)
        sEdgeSize.cx = sEdgeSize.cy = 0;

    ClientToScreen(&sPoint);
    CRect sRect(mWndRect);
    switch (mHitTest)
    {
    case HTTOP:         _TOP(sRect, sPoint);                                      break;
    case HTTOPLEFT:     _TOP(sRect, sPoint, XPR_FALSE);    _LEFT(sRect, sPoint);  break;
    case HTLEFT:        _LEFT(sRect, sPoint);                                     break;
    case HTBOTTOMLEFT:  _BOTTOM(sRect, sPoint, XPR_FALSE); _LEFT(sRect, sPoint);  break;
    case HTBOTTOM:      _BOTTOM(sRect, sPoint);                                   break;
    case HTBOTTOMRIGHT: _BOTTOM(sRect, sPoint, XPR_FALSE); _RIGHT(sRect, sPoint); break;
    case HTRIGHT:       _RIGHT(sRect, sPoint);                                    break;
    case HTTOPRIGHT:    _TOP(sRect, sPoint, XPR_FALSE);    _RIGHT(sRect, sPoint); break;
    }

    mDc->DrawDragRect(sRect, sEdgeSize, mLastRect, CSize(3,3), XPR_NULL, XPR_NULL);

    mLastRect = sRect;
}

void PicViewer::toggleAutoHide(void)
{
    mAutoHide = !mAutoHide;
}

void PicViewer::OnPicAutoHide(void) 
{
    toggleAutoHide();
}

void PicViewer::OnUpdatePicAutoHide(CCmdUI *aCmdUI) 
{
    aCmdUI->Enable(!isDocking());
    aCmdUI->SetCheck(isAutoHide());
}

void PicViewer::OnUpdatePicDocking(CCmdUI *aCmdUI) 
{
    aCmdUI->SetCheck(isDocking());
}

void PicViewer::OnUpdatePicZoomIn(CCmdUI *aCmdUI)
{
    aCmdUI->Enable(!isDocking());
}

void PicViewer::OnUpdatePicZoomOut(CCmdUI *aCmdUI)
{
    aCmdUI->Enable(!isDocking());
}

void PicViewer::OnUpdatePicZoom100(CCmdUI *aCmdUI)
{
    aCmdUI->Enable(!isDocking());
}

void PicViewer::OnUpdatePicRatio(CCmdUI *aCmdUI) 
{
    aCmdUI->Enable(!isDocking());
    aCmdUI->SetCheck(mRatio);
}

void PicViewer::OnUpdatePicLock(CCmdUI *aCmdUI) 
{
    aCmdUI->Enable(!isDocking());
    aCmdUI->SetRadio(mLock == LOCK_SIZE);
}

void PicViewer::OnUpdatePicClient(CCmdUI *aCmdUI) 
{
    aCmdUI->Enable(!isDocking());
    aCmdUI->SetRadio(mLock == LOCK_WINDOW);
}

void PicViewer::OnUpdatePicLock100(CCmdUI *aCmdUI) 
{
    aCmdUI->Enable(!isDocking());
    aCmdUI->SetRadio(mLock == LOCK_100);
}

void PicViewer::OnUpdatePicLockThumbnail(CCmdUI *aCmdUI)
{
    aCmdUI->Enable(!isDocking());
    aCmdUI->SetRadio(mLock == LOCK_THUMBNAIL);
}

void PicViewer::toggleFileName(void)
{
    mFileName = !mFileName;

    setTitle();
}

xpr_bool_t PicViewer::isShowFileName(void)
{
    return mFileName;
}

void PicViewer::OnPicShowFileName(void)
{
    toggleFileName();
}

void PicViewer::OnUpdatePicShowFileName(CCmdUI *aCmdUI)
{
    aCmdUI->Enable(!isDocking());
    aCmdUI->SetCheck(mFileName);
}

void PicViewer::setTitle(void)
{
    xpr_tchar_t sTitle[XPR_MAX_PATH + fxb::StringTable::kMaxStringLength + 1] = {0};
    _tcscpy(sTitle, theApp.loadString(XPR_STRING_LITERAL("popup.pic_viewer.title")));

    if (mPicture == XPR_TRUE)
    {
        _tcscat(sTitle, XPR_STRING_LITERAL(" - "));

        if (mFileName == XPR_TRUE)
        {
            const xpr_tchar_t *sSplit = _tcsrchr(mPath, XPR_STRING_LITERAL('\\'));
            if (sSplit != XPR_NULL)
            {
                sSplit++;
                _tcscat(sTitle, sSplit);
            }
        }
        else
        {
            _tcscat(sTitle, mPath);
        }
    }

    SetWindowText(sTitle);
}
