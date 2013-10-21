//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "DlgToolBar.h"

#include <afxpriv.h>              // for WM_IDLEUPDATECMDUI

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// user defined timer
enum
{
    TimerIdIdle = 3456,
};

DlgToolBar::DlgToolBar(void)
    : mImageListId(0), mBitmapSize(0,0), mEnableToolTips(XPR_FALSE)
{
}

DlgToolBar::~DlgToolBar(void)
{
}


BEGIN_MESSAGE_MAP(DlgToolBar, super)
    ON_WM_DESTROY()
    ON_WM_TIMER()
    ON_MESSAGE(WM_IDLEUPDATECMDUI, OnIdleUpdateCmdUI)
END_MESSAGE_MAP()

void DlgToolBar::OnDestroy(void)
{
    super::OnDestroy();

    KillTimer(TimerIdIdle);

    if (XPR_IS_NOT_NULL(mImageList.m_hImageList))
        mImageList.DeleteImageList();
}

void DlgToolBar::enableButton(xpr_sint_t aId, xpr_bool_t aEnable)
{
    GetToolBarCtrl().EnableButton(aId, aEnable);

    if (XPR_IS_TRUE(aEnable))
        mButtonStateMap.RemoveKey(aId);
    else
        mButtonStateMap.SetAt(aId, aEnable);
}

void DlgToolBar::setImageListId(xpr_uint_t aId, CSize aBitmapSize)
{
    mImageListId = aId;
    mBitmapSize = aBitmapSize;
}

void DlgToolBar::enableToolTips(xpr_bool_t aEnable)
{
    mEnableToolTips = aEnable;
}

xpr_bool_t DlgToolBar::createToolBar(CDialog *aDialog)
{
    if (Create(aDialog) == XPR_FALSE)
        return XPR_FALSE;

    CBitmap sBitmap;
    sBitmap.LoadBitmap(mImageListId);

    mImageList.Create(mBitmapSize.cx, mBitmapSize.cy, ILC_COLORDDB | ILC_MASK, 10, 1);
    mImageList.Add(&sBitmap, RGB(255,0,255));
    sBitmap.DeleteObject();

    GetToolBarCtrl().SetImageList(&mImageList);
    GetToolBarCtrl().SetBitmapSize(mBitmapSize);

    ModifyStyle(0, TBSTYLE_LIST | TBSTYLE_FLAT);
    GetToolBarCtrl().SetExtendedStyle(TBSTYLE_EX_DRAWDDARROWS | TBSTYLE_EX_MIXEDBUTTONS);

    SetBorders(0, 0, 0, 0);
    GetToolBarCtrl().AutoSize();

    DWORD sStyle = GetBarStyle();
    sStyle &= ~CBRS_BORDER_TOP;
    sStyle |= CBRS_FLYBY;
    sStyle |= CBRS_BORDER_BOTTOM;
    SetBarStyle(sStyle);

    if (XPR_IS_TRUE(mEnableToolTips))
    {
        ((super *)this)->EnableToolTips();

        SetTimer(TimerIdIdle, 100, NULL);
    }

    return XPR_TRUE;
}

void DlgToolBar::reposition(CDialog *aDialog)
{
    if (XPR_IS_NULL(aDialog))
        return;

    CRect sClientStartRect;
    aDialog->GetClientRect(sClientStartRect);

    CRect sClientNowRect;
    aDialog->RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0, reposQuery, sClientNowRect);

    CPoint sOffsetPoint(sClientNowRect.left - sClientStartRect.left, sClientNowRect.top - sClientStartRect.top);

    CRect sChildRect;
    CWnd *sChildWnd = aDialog->GetWindow(GW_CHILD);
    while (XPR_IS_NOT_NULL(sChildWnd))
    {
        sChildWnd->GetWindowRect(sChildRect);

        aDialog->ScreenToClient(sChildRect);

        sChildRect.OffsetRect(sOffsetPoint);

        sChildWnd->MoveWindow(sChildRect, XPR_FALSE);

        sChildWnd = sChildWnd->GetNextWindow();
    }

    CRect sWindowRect;
    aDialog->GetWindowRect(sWindowRect);

    sWindowRect.right += sClientStartRect.Width() - sClientNowRect.Width();
    sWindowRect.bottom += sClientStartRect.Height() - sClientNowRect.Height();
    aDialog->MoveWindow(sWindowRect, XPR_FALSE);

    aDialog->RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);
}

void DlgToolBar::setToolBarButtons(TBBUTTON *aTbButtons, xpr_sint_t aCount)
{
    if (XPR_IS_NULL(aTbButtons) || aCount <= 0)
        return;

    GetToolBarCtrl().AddButtons(aCount, aTbButtons);
}

void DlgToolBar::OnTimer(UINT_PTR aIdEvent)
{
    if (aIdEvent == TimerIdIdle)
        SendMessage(WM_IDLEUPDATECMDUI, XPR_TRUE);

    super::OnTimer(aIdEvent);
}

LRESULT DlgToolBar::OnIdleUpdateCmdUI(WPARAM wParam, LPARAM lParam)
{
    if (mButtonStateMap.GetCount() == 0)
    {
        if (IsWindowVisible() == XPR_TRUE)
        {
            CFrameWnd *sParentWnd = (CFrameWnd *)GetParent();
            if (XPR_IS_NOT_NULL(sParentWnd))
                OnUpdateCmdUI(sParentWnd, (xpr_bool_t)wParam);
        }
    }

    if (IsWindowVisible() == XPR_TRUE)
    {
        xpr_sint_t sId;
        xpr_bool_t sValue;

        POSITION sPos = mButtonStateMap.GetStartPosition();
        while (XPR_IS_NOT_NULL(sPos))
        {
            mButtonStateMap.GetNextAssoc(sPos, sId, sValue);

            GetToolBarCtrl().EnableButton(sId, sValue);
        }
    }

    return 0;
}
