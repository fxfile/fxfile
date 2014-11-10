//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_PICTURE_VIEWER_H__
#define __FXFILE_PICTURE_VIEWER_H__ 1
#pragma once

#include "xpr_mutex.h"

#include "gui/BCMenu.h"
#include "gfl/libgfl.h"

namespace fxfile
{
class DlgState;

class PictureViewer : public CDialog
{
    typedef CDialog super;

public:
    PictureViewer(void);
    virtual ~PictureViewer(void);

public:
    void setPicture(const xpr_tchar_t *aPath, xpr_bool_t aActualSize = XPR_FALSE);
    xpr_bool_t isDocking(void) { return mDocking; }
    xpr_bool_t isAutoHide(void) { return mAutoHide; }
    xpr_bool_t isHide(void) { return mHide; }
    void setDesktop(xpr_sint_t aStyle);
    void setDocking(xpr_bool_t aDocking);

    void toggleDocking(void);

    void zoomIn(void);
    void zoomOut(void);
    void zoom100(void);

    xpr_bool_t isRatioResize(void);
    void toggleRatioResize(void);

    void toggleAutoHide(void);

    xpr_bool_t isLockSize(void);
    xpr_bool_t isLockWindow(void);
    xpr_bool_t isLock100(void);
    xpr_bool_t isLockThumbnail(void);

    void setLockSize(void);
    void setLockWindow(void);
    void setLock100(void);
    void setLockThumbnail(void);

    void toggleFileName(void);
    xpr_bool_t isShowFileName(void);

    void hide(void);
    void toggleShowHide(void);

protected:
    xpr_tchar_t  mPath[XPR_MAX_PATH + 1];
    CBitmap     *mBitmap;
    CSize        mImgSize;
    CSize        mRealImgSize;
    CSize        mLockImgSize;
    CRect        mPopupRect;

    xpr_bool_t mDocking;
    xpr_bool_t mRatio;
    xpr_sint_t mLock;
    xpr_bool_t mAutoHide;
    xpr_bool_t mFileName;
    xpr_bool_t mHide;

    // for Tracking
    void startTracking(CPoint aPoint);
    void endTracking(CPoint aPoint);
    void drawTracking(CPoint aPoint, xpr_bool_t aRemove);

    void _TOP(CRect &aRect, CPoint aDragPoint, xpr_bool_t aRatio = XPR_TRUE);
    void _LEFT(CRect &aRect, CPoint aDragPoint, xpr_bool_t aRatio = XPR_TRUE);
    void _BOTTOM(CRect &aRect, CPoint aDragPoint, xpr_bool_t aRatio = XPR_TRUE);
    void _RIGHT(CRect &aRect, CPoint aDragPoint, xpr_bool_t aRatio = XPR_TRUE);
    void _RATIO_RIGHT(CRect &aRect, xpr_bool_t aRatio = XPR_TRUE);
    void _RATIO_BOTTOM(CRect &aRect, xpr_bool_t aRatio = XPR_TRUE);

    CDC *mDc;
    CRect mLastRect;
    CRect mWndRect;
    xpr_bool_t mTracking;
    xpr_sint_t mHitTest;

    void setTitle(void);
    xpr_bool_t mPicture;

protected:
    struct PictureItem;
    struct LoadedPicture;

    HANDLE     mThread;
    HANDLE     mEvent;
    HANDLE     mStopEvent;
    xpr_uint_t mThreadId;
    xpr::Mutex mMutex;

    static xpr_uint_t _stdcall ImgLoadProc(void *aParam);
    DWORD OnImgLoadProc(void);

    typedef std::deque<PictureItem *> PictureDeque;
    PictureDeque mPictureDeque;

    GFL_FILE_INFORMATION mGflFileInfo;

protected:
    BCMenu    mMenu;
    DlgState *mDlgState;

public:
    virtual xpr_bool_t Create(CWnd* pParentWnd);
    virtual xpr_bool_t PreTranslateMessage(MSG* pMsg);
    virtual xpr_bool_t DestroyWindow(void);

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual void PostNcDestroy(void);

protected:
    DECLARE_MESSAGE_MAP()
    virtual xpr_bool_t OnInitDialog(void);
    afx_msg xpr_sint_t OnCreate(LPCREATESTRUCT aCreateStruct);
    afx_msg void OnDestroy(void);
    afx_msg void OnPaint(void);
    afx_msg void OnSize(xpr_uint_t aType, xpr_sint_t cx, xpr_sint_t cy);
    afx_msg void OnContextMenu(CWnd *aWnd, CPoint aPoint);
    afx_msg void OnPicShowHide(void);
    afx_msg void OnPicHide(void);
    afx_msg void OnPicClose(void);
    afx_msg void OnPicDocking(void);
    afx_msg xpr_bool_t OnEraseBkgnd(CDC *aDC);
    afx_msg void OnPicZoomIn(void);
    afx_msg void OnPicZoomOut(void);
    afx_msg void OnPicZoom100(void);
    afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* aMMI);
    afx_msg void OnLButtonDown(xpr_uint_t aFlags, CPoint aPoint);
    afx_msg void OnSizing(xpr_uint_t aSide, LPRECT aRect);
    afx_msg void OnPicRatio(void);
    afx_msg void OnPicLock(void);
    afx_msg void OnPicClient(void);
    afx_msg void OnPicLock100(void);
    afx_msg void OnPicLockThumbnail(void);
    afx_msg void OnPicBackCenter(void);
    afx_msg void OnPicBackTile(void);
    afx_msg void OnPicBackStretch(void);
    afx_msg void OnSetFocus(CWnd* pOldWnd);
    afx_msg void OnNcLButtonDown(xpr_uint_t aHitTest, CPoint aPoint);
    afx_msg void OnMouseMove(xpr_uint_t aFlags, CPoint aPoint);
    afx_msg void OnLButtonUp(xpr_uint_t aFlags, CPoint aPoint);
    afx_msg void OnInitMenuPopup(CMenu *aPopupMenu, xpr_uint_t aIndex, xpr_bool_t aSysMenu);
    afx_msg void OnPicAutoHide(void);
    afx_msg void OnPicShowFileName(void);
    afx_msg void OnUpdatePicZoomIn(CCmdUI *aCmdUI);
    afx_msg void OnUpdatePicZoomOut(CCmdUI *aCmdUI);
    afx_msg void OnUpdatePicZoom100(CCmdUI *aCmdUI);
    afx_msg void OnUpdatePicRatio(CCmdUI *aCmdUI);
    afx_msg void OnUpdatePicAutoHide(CCmdUI *aCmdUI);
    afx_msg void OnUpdatePicDocking(CCmdUI *aCmdUI);
    afx_msg void OnUpdatePicLock(CCmdUI *aCmdUI);
    afx_msg void OnUpdatePicClient(CCmdUI *aCmdUI);
    afx_msg void OnUpdatePicLock100(CCmdUI *aCmdUI);
    afx_msg void OnUpdatePicLockThumbnail(CCmdUI *aCmdUI);
    afx_msg void OnUpdatePicShowFileName(CCmdUI *aCmdUI);
    afx_msg void OnClose(void);
    afx_msg void OnMeasureItem(xpr_sint_t aIDCtl, LPMEASUREITEMSTRUCT aMeasureItemStruct);
    afx_msg LRESULT OnMenuChar(xpr_uint_t aChar, xpr_uint_t aFlags, CMenu *aMenu);
    afx_msg LRESULT OnLoadedPicture(WPARAM wParam, LPARAM lParam);
};
} // namespace fxfile

#endif // __FXFILE_PICTURE_VIEWER_H__
