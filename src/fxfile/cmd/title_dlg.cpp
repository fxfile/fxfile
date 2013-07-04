//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "title_dlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace fxfile
{
namespace cmd
{
#define TITLE_MARGIN 13

TitleDlg::TitleDlg(const xpr_uint_t aResourceId)
    : CDialog(aResourceId, XPR_NULL)
    , mDescIcon(XPR_NULL)
    , mBold(XPR_FALSE)
    , mOffset(0)
{
}

BEGIN_MESSAGE_MAP(TitleDlg, CDialog)
    ON_WM_PAINT()
    ON_WM_DESTROY()
END_MESSAGE_MAP()

void TitleDlg::setCtrlOffset(xpr_sint_t aOffset)
{
    mOffset = aOffset;
}

xpr_bool_t TitleDlg::OnInitDialog(void)
{
    CDialog::OnInitDialog();

    if (mOffset > 0)
    {
        CRect sRect;
        CWnd *sWnd = GetWindow(GW_CHILD);
        while (sWnd != XPR_NULL)
        {
            sWnd->GetWindowRect(&sRect);
            ScreenToClient(&sRect);

            sRect.InflateRect(0, -mOffset, 0, mOffset);
            sWnd->MoveWindow(sRect, XPR_FALSE);

            sWnd = sWnd->GetWindow(GW_HWNDNEXT);
        }

        GetWindowRect(&sRect);
        SetWindowPos(XPR_NULL, sRect.left, sRect.top, sRect.Width(), sRect.Height()+mOffset, SWP_NOZORDER);
        CenterWindow();
    }

    LOGFONT sLogFont = {0};
    GetFont()->GetLogFont(&sLogFont);
    if (mBold == XPR_TRUE)
        sLogFont.lfWeight = FW_BOLD;
    mTitleFont.CreateFontIndirect(&sLogFont);

    if (mTitle.empty() == XPR_FALSE)
        SetWindowText(mTitle.c_str());

    SetDlgItemText(IDOK,     theApp.loadString(XPR_STRING_LITERAL("popup.common.button.ok")));
    SetDlgItemText(IDCANCEL, theApp.loadString(XPR_STRING_LITERAL("popup.common.button.cancel")));

    return XPR_TRUE;
}

void TitleDlg::setTitle(const xpr_tchar_t *aTitle)
{
    if (aTitle == XPR_NULL)
        return;

    mTitle = aTitle;

    if (m_hWnd != XPR_NULL)
        SetWindowText(mTitle.c_str());
}

void TitleDlg::setDesc(const xpr_tchar_t *aDesc)
{
    if (aDesc == XPR_NULL)
        return;

    mDesc = aDesc;
}

void TitleDlg::setDescIcon(xpr_uint_t aIconId, xpr_bool_t aIconLeft)
{
    setDescIcon(AfxGetApp()->LoadIcon(aIconId), aIconLeft);
}

void TitleDlg::setDescIcon(HICON aIcon, xpr_bool_t aIconLeft, xpr_bool_t aBold)
{
    DESTROY_ICON(mDescIcon);

    mDescIcon = aIcon;
    mIconLeft = aIconLeft;
    mBold     = aBold;
}

void TitleDlg::OnPaint(void)
{
    CPaintDC sPaintDc(this);

    xpr_sint_t sHeight = (mOffset > 0) ? (mOffset - 2) : (50);

    CRect sRect;
    GetClientRect(&sRect);
    sRect.bottom = sHeight;

    sPaintDc.FillSolidRect(sRect, RGB(255,255,255));
    sPaintDc.DrawEdge(sRect, EDGE_ETCHED, BF_BOTTOM);

    CRect sTextRect(TITLE_MARGIN+2, 0, sRect.right-(TITLE_MARGIN+2), sRect.bottom);
    CPoint sIconPoint(0,0);
    if (mDescIcon != XPR_NULL)
    {
        if (mIconLeft)
        {
            sTextRect.left = 60;
            sIconPoint = CPoint(13, (sRect.Height()-32)/2);
        }
        else
        {
            sTextRect.right = sRect.right - 60;
            sIconPoint = CPoint(sRect.right - 50, (sRect.Height()-32)/2);
        }
    }

    CFont *sOldFont = sPaintDc.SelectObject(&mTitleFont);

    CRect sCalcRect(sTextRect);
    sPaintDc.DrawText(mDesc.c_str(), sCalcRect, DT_LEFT | DT_VCENTER | DT_WORDBREAK | DT_CALCRECT);

    sTextRect.top    = sTextRect.top + (sTextRect.Height() - sCalcRect.Height()) / 2;
    sTextRect.bottom = sTextRect.top + sCalcRect.Height();
    sPaintDc.DrawText(mDesc.c_str(), sTextRect, DT_LEFT | DT_VCENTER | DT_WORDBREAK);

    sPaintDc.SelectObject(sOldFont);

    if (mDescIcon != XPR_NULL)
    {
        ::DrawIcon(sPaintDc.m_hDC, sIconPoint.x, sIconPoint.y, mDescIcon);
    }
}

void TitleDlg::OnDestroy(void) 
{
    CDialog::OnDestroy();

    DELETE_OBJECT(mTitleFont);
    DESTROY_ICON(mDescIcon);
}
} // namespace cmd
} // namespace fxfile
