//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "about_dlg.h"
#include "about_tab_dlg.h"
#include "about_tab_info_dlg.h"
#include "about_tab_license_dlg.h"
#include "about_tab_credits_dlg.h"
#include "resource.h"
#include "app_ver.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace fxfile;
using namespace fxfile::base;

namespace fxfile
{
namespace cmd
{
AboutDlg::AboutDlg(void)
    : super(IDD_ABOUT)
    , mOldShowDlg(-1)
    , mProgram(FXFILE_PROGRAM_NAME)
    , mLicense(XPR_STRING_LITERAL("This program is free software, licensed under GNU General Public License."))
{
}

void AboutDlg::DoDataExchange(CDataExchange* pDX)
{
    super::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_ABOUT_TAB, mTabCtrl);
}

BEGIN_MESSAGE_MAP(AboutDlg, super)
    ON_WM_DESTROY()
    ON_WM_PAINT()
    ON_NOTIFY(TCN_SELCHANGE, IDC_ABOUT_TAB, OnSelchangeTab)
END_MESSAGE_MAP()

xpr_bool_t AboutDlg::OnInitDialog(void) 
{
    super::OnInitDialog();

    addTabDialog(new AboutTabInfoDlg,    gApp.loadString(XPR_STRING_LITERAL("popup.about.tab.info")));
    addTabDialog(new AboutTabLicenseDlg, gApp.loadString(XPR_STRING_LITERAL("popup.about.tab.license")));
    addTabDialog(new AboutTabCreditsDlg, gApp.loadString(XPR_STRING_LITERAL("popup.about.tab.credits")));

    showTabDialog(0);

    SetDlgItemText(IDOK,     gApp.loadString(XPR_STRING_LITERAL("popup.common.button.ok")));
    SetDlgItemText(IDCANCEL, gApp.loadString(XPR_STRING_LITERAL("popup.common.button.cancel")));

    return XPR_TRUE;
}

void AboutDlg::OnDestroy(void) 
{
    AboutTabDlg *sDlg;
    TabDeque::iterator sIterator;

    sIterator = mTabDeque.begin();
    for (; sIterator != mTabDeque.end(); ++sIterator)
    {
        sDlg = *sIterator;
        DESTROY_DELETE(sDlg);
    }

    mTabDeque.clear();

    super::OnDestroy();
}

void AboutDlg::OnPaint(void) 
{
    CPaintDC sPaintDc(this);

    const CRect sOffsetRect(10, 10, 7, 7);
    const CPoint sTextOffset(15, 0);

    CRect sRect;
    GetClientRect(&sRect);

    CRect sTabRect;
    GetDlgItem(IDC_ABOUT_TAB)->GetWindowRect(&sTabRect);
    ScreenToClient(&sTabRect);
    sRect.bottom = sTabRect.top - sOffsetRect.bottom;

    // draw background
    sPaintDc.FillSolidRect(sRect, RGB(255,255,255));

    HICON sIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
    if (sIcon != XPR_NULL)
    {
        CPoint sIconPoint(sOffsetRect.left, sOffsetRect.top);
        ::DrawIconEx(sPaintDc.m_hDC, sIconPoint.x, sIconPoint.y, sIcon, 32, 32, 0, XPR_NULL, DI_NORMAL);
        DESTROY_ICON(sIcon);
    }

    CPen sPen(PS_SOLID, 1, RGB(200,200,200));
    CPen *sOldPen = sPaintDc.SelectObject(&sPen);
    sPaintDc.MoveTo(sRect.left,  sRect.bottom);
    sPaintDc.LineTo(sRect.right, sRect.bottom);
    sPaintDc.SelectObject(sOldPen);

    CRect sTitleRect(sRect), sCalcRect;
    sTitleRect.left   = sOffsetRect.left + 32 + sTextOffset.x;
    sTitleRect.top    = sOffsetRect.top;
    sTitleRect.bottom = sTitleRect.top + 32;

    LOGFONT sLogFont = {0};
    GetFont()->GetLogFont(&sLogFont);

    CFont sTextFont;
    sTextFont.CreateFontIndirect(&sLogFont);

    CFont sTitleFont;
    sLogFont.lfHeight = -MulDiv(13, GetDeviceCaps(sPaintDc.m_hDC, LOGPIXELSY), 72);
    sTitleFont.CreateFontIndirect(&sLogFont);

    sPaintDc.SetTextColor(RGB(0,50,150));

    CFont *sOldFont = sPaintDc.SelectObject(&sTitleFont);

    sPaintDc.DrawText(mProgram.c_str(), (xpr_sint_t)mProgram.length(), sTitleRect, DT_SINGLELINE | DT_LEFT | DT_TOP);

    sCalcRect = sTitleRect;
    sPaintDc.DrawText(mProgram.c_str(), (xpr_sint_t)mProgram.length(), sCalcRect,  DT_SINGLELINE | DT_LEFT | DT_TOP | DT_CALCRECT);

    sPaintDc.SelectObject(sOldFont);

    sPaintDc.SetTextColor(RGB(0,0,0));

    sOldFont = sPaintDc.SelectObject(&sTextFont);

    CRect sTextRect;
    sTextRect.left   = sTitleRect.left;
    sTextRect.top    = sCalcRect.bottom + 0;
    sTextRect.right  = sRect.right  - sOffsetRect.right;
    sTextRect.bottom = sTextRect.top + 32;

    sPaintDc.DrawText(mLicense.c_str(), (xpr_sint_t)mLicense.length(), sTextRect, DT_LEFT | DT_TOP);

    sPaintDc.SelectObject(sOldFont);
}

void AboutDlg::addTabDialog(AboutTabDlg *aDlg, const xpr_tchar_t *aTitle)
{
    xpr_sint_t sIndex = mTabCtrl.GetItemCount();

    mTabCtrl.InsertItem(sIndex, aTitle);
    mTabDeque.push_back(aDlg);
}

AboutTabDlg *AboutDlg::getTabDialog(xpr_sint_t aIndex)
{
    if (!FXFILE_STL_IS_INDEXABLE(aIndex, mTabDeque))
        return XPR_NULL;

    return mTabDeque[aIndex];
}

void AboutDlg::OnSelchangeTab(NMHDR* pNMHDR, LRESULT* pResult) 
{
    showTabDialog(mTabCtrl.GetCurSel());
    *pResult = 0;
}

void AboutDlg::showTabDialog(xpr_sint_t aIndex)
{
    if (!FXFILE_STL_IS_INDEXABLE(aIndex, mTabDeque))
        return;

    AboutTabDlg *sOldDlg;
    AboutTabDlg *sNewDlg;

    sNewDlg = mTabDeque[aIndex];
    if (sNewDlg == XPR_NULL)
        return;

    // Hide
    if (FXFILE_STL_IS_INDEXABLE(mOldShowDlg, mTabDeque))
    {
        sOldDlg = mTabDeque[mOldShowDlg];
        if (sOldDlg != XPR_NULL)
        {
            if (sOldDlg->m_hWnd != XPR_NULL)
                sOldDlg->ShowWindow(SW_HIDE);
        }
    }

    // Show
    if (sNewDlg->m_hWnd == XPR_NULL)
        sNewDlg->Create(this);

    sNewDlg->OnTabInit();

    CRect sTabRect(0,0,0,0);
    mTabCtrl.GetWindowRect(&sTabRect);
    ScreenToClient(&sTabRect);

    CRect sItemRect(0,0,0,0);
    mTabCtrl.GetItemRect(0, &sItemRect);

    sTabRect.DeflateRect(3, sItemRect.Height()+5, 3, 3);

    sNewDlg->MoveWindow(sTabRect);
    sNewDlg->SetWindowPos(&wndTop, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
    sNewDlg->ShowWindow(SW_SHOW);
    sNewDlg->SetFocus();
    UpdateWindow();

    mTabCtrl.SetFocus();
    mTabCtrl.SetCurSel(aIndex);
    mOldShowDlg = aIndex;
}
} // namespace cmd
} // namespace fxfile
