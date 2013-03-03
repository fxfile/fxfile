//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "MainCoolBar.h"

#include "Option.h"
#include "MainFrame.h"
#include "ExplorerView.h"
#include "ConfDir.h"
#include "CtrlId.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

class RebarInfo : public REBARINFO
{
public:
    RebarInfo(void)
    {
        memset(this, 0, sizeof(REBARINFO));
        cbSize = sizeof(REBARINFO);
    }
};

class RebarBandInfo : public REBARBANDINFO
{
public:
    RebarBandInfo(void)
    {
        memset(this, 0, sizeof(REBARBANDINFO));
        cbSize = sizeof(REBARBANDINFO);
    }
};

BEGIN_MESSAGE_MAP(MainCoolBar, CSizableReBar)
    ON_WM_SIZE()
    ON_WM_SYSCOLORCHANGE()
    ON_WM_WINDOWPOSCHANGED()
    ON_WM_DESTROY()
END_MESSAGE_MAP()

MainCoolBar::MainCoolBar(void)
{
}

MainCoolBar::~MainCoolBar(void)
{
}

xpr_bool_t MainCoolBar::createBands(void)
{
    DWORD sStyle;
    DWORD sExStyle;

    mMainToolBar.SetObserver(dynamic_cast<CToolBarEx::Observer *>(this));
    mDriveToolBar.SetObserver(dynamic_cast<CToolBarEx::Observer *>(this));
    mBookmarkToolBar.SetObserver(dynamic_cast<CToolBarEx::Observer *>(this));

    sStyle = WS_CHILD | WS_VISIBLE | CBRS_ALIGN_TOP | CBRS_FLYBY;
    sExStyle = TBSTYLE_EX_DRAWDDARROWS;
    if (mMainToolBar.Create(this, sStyle, AFX_IDW_TOOLBAR, sExStyle) == XPR_FALSE)
    {
        XPR_TRACE(XPR_STRING_LITERAL("Failed to create main toolbar\n"));
        return -1;      // fail to create
    }

    sStyle = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_TOOLTIPS | CBRS_SIZE_DYNAMIC | CBRS_FLYBY | CBRS_ALIGN_TOP;
    sExStyle = TBSTYLE_EX_DRAWDDARROWS;
    if (mDriveToolBar.Create(this, sStyle, AFX_IDW_DRIVE_BAR, sExStyle) == XPR_FALSE)
    {
        XPR_TRACE(XPR_STRING_LITERAL("Failed to create drive toolBar\n"));
        return XPR_FALSE; // failed to create
    }

    sStyle = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_TOOLTIPS | CBRS_SIZE_DYNAMIC | CBRS_FLYBY | CBRS_ALIGN_TOP;
    sExStyle = TBSTYLE_EX_DRAWDDARROWS;
    if (mBookmarkToolBar.Create(this, sStyle, AFX_IDW_BOOKMARK_BAR, sExStyle) == XPR_FALSE)
    {
        XPR_TRACE(XPR_STRING_LITERAL("Failed to create bookmark toolBar\n"));
        return XPR_FALSE; // failed to create
    }

    sStyle = RBBS_BREAK | RBBS_USECHEVRON;
    if (AddBar(&mMainToolBar, XPR_NULL, XPR_NULL, sStyle, XPR_STRING_LITERAL("&Toolbar"), false) == XPR_FALSE)
    {
        XPR_TRACE(XPR_STRING_LITERAL("Failed to add rebar\n"));
        return XPR_FALSE;
    }

    sStyle = RBBS_BREAK | RBBS_USECHEVRON;
    if (AddBar(&mDriveToolBar, XPR_NULL, XPR_NULL, sStyle, XPR_STRING_LITERAL("&Drivebar"), false) == XPR_FALSE)
    {
        XPR_TRACE(XPR_STRING_LITERAL("Failed to add rebar\n"));
        return XPR_FALSE;
    }

    sStyle = RBBS_BREAK | RBBS_USECHEVRON;
    if (AddBar(&mBookmarkToolBar, XPR_NULL, XPR_NULL, sStyle, XPR_STRING_LITERAL("&Bookmarkbar"), false) == XPR_FALSE)
    {
        XPR_TRACE(XPR_STRING_LITERAL("Failed to add rebar\n"));
        return XPR_FALSE;
    }

    mMainToolBar.Init();

    mMainToolBar.UpdateToolbarSize();
    mDriveToolBar.UpdateToolbarSize();
    mBookmarkToolBar.UpdateToolbarSize();

    loadStateFile();

    // Dynamic Creation
    if (XPR_IS_FALSE(gOpt->mMain.mDriveBar))
    {
        setBandVisible(AFX_IDW_DRIVE_BAR, XPR_FALSE);
    }
    else
    {
        mDriveToolBar.setShortText(gOpt->mMain.mDriveBarShortText);
        mDriveToolBar.createDriveBar();
    }

    if (isBandVislble(AFX_IDW_BOOKMARK_BAR) == XPR_TRUE)
        mBookmarkToolBar.createBookmarkBar();

    setWrapable(AFX_IDW_BOOKMARK_BAR, gOpt->mMain.mBookmarkBarMultiLine);
    setWrapable(AFX_IDW_DRIVE_BAR,    gOpt->mMain.mDriveBarMultiLine);

    return 0;
}

void MainCoolBar::OnDestroy(void) 
{
    saveStateFile();

    CSizableReBar::OnDestroy();
}

void MainCoolBar::OnSize(xpr_uint_t nType, xpr_sint_t cx, xpr_sint_t cy)
{
    CSizableReBar::OnSize(nType, cx, cy);

    xpr_sint_t i;
    xpr_sint_t sCount;
    CToolBarEx *sToolBar;
    CReBarCtrl &sReBarCtrl = GetReBarCtrl();
    RebarBandInfo sRebarBandInfo;
    sRebarBandInfo.fMask = RBBIM_CHILD;

    sCount = sReBarCtrl.GetBandCount();
    for (i = 0; i < sCount; ++i)
    {
        sReBarCtrl.GetBandInfo(i, &sRebarBandInfo);

        sToolBar = DYNAMIC_DOWNCAST(CToolBarEx, CWnd::FromHandle(sRebarBandInfo.hwndChild));
        if (XPR_IS_NOT_NULL(sToolBar))
        {
            if (XPR_TEST_BITS(sToolBar->GetStyle(), TBSTYLE_WRAPABLE))
            {
                sToolBar->UpdateToolbarSize();
            }
        }
    }
}

xpr_sint_t MainCoolBar::getToolbarText(void) const
{
    return mMainToolBar.GetTextType();
}

void MainCoolBar::setToolbarText(xpr_sint_t aText)
{
    mMainToolBar.SetTextType(aText);
}

void MainCoolBar::setToobarIcon(xpr_sint_t aIcon)
{
    mMainToolBar.SetIconType(aIcon);
}

xpr_sint_t MainCoolBar::getToolbarIcon(void) const
{
    return mMainToolBar.GetIconType();
}

void MainCoolBar::setBandVisible(xpr_uint_t aBandId, xpr_bool_t aVisible)
{
    CReBarCtrl &sReBarCtrl = GetReBarCtrl();

    xpr_sint_t sBandIndex = sReBarCtrl.IDToIndex(aBandId);

    RebarBandInfo sRebarBandInfo;
    sRebarBandInfo.fMask = RBBIM_STYLE;
    sReBarCtrl.GetBandInfo(sBandIndex, &sRebarBandInfo);

    if (XPR_IS_TRUE(aVisible)) sRebarBandInfo.fStyle &= ~RBBS_HIDDEN;
    else                       sRebarBandInfo.fStyle |= RBBS_HIDDEN;

    if (XPR_IS_TRUE(aVisible))
    {
        if (aBandId == AFX_IDW_DRIVE_BAR)
        {
            mDriveToolBar.createDriveBar();
        }

        if (aBandId == AFX_IDW_BOOKMARK_BAR)
        {
            mBookmarkToolBar.createBookmarkBar();
        }
    }

    sReBarCtrl.SetBandInfo(sBandIndex, &sRebarBandInfo);

    if (XPR_IS_FALSE(aVisible))
    {
        if (aBandId == AFX_IDW_DRIVE_BAR)
            mDriveToolBar.destroyDriveBar();

        if (aBandId == AFX_IDW_BOOKMARK_BAR)
            mBookmarkToolBar.destroyBookmarkBar();
    }
}

xpr_bool_t MainCoolBar::isBandVislble(xpr_uint_t aBandId)
{
    CReBarCtrl &sReBarCtrl = GetReBarCtrl();

    xpr_sint_t sBandIndex = sReBarCtrl.IDToIndex(aBandId);

    RebarBandInfo sRebarBandInfo;
    sRebarBandInfo.fMask = RBBIM_STYLE;
    sReBarCtrl.GetBandInfo(sBandIndex, &sRebarBandInfo);

    return (sRebarBandInfo.fStyle & RBBS_HIDDEN) ? XPR_FALSE : XPR_TRUE;
}

xpr_bool_t MainCoolBar::isWrapable(xpr_uint_t aBandId)
{
    xpr_bool_t sResult = XPR_FALSE;

    CReBarCtrl &sReBarCtrl = GetReBarCtrl();
    xpr_sint_t sBandIndex = sReBarCtrl.IDToIndex(aBandId);

    RebarBandInfo sRebarBandInfo;
    sRebarBandInfo.fMask = RBBIM_CHILD;
    if (sReBarCtrl.GetBandInfo(sBandIndex, &sRebarBandInfo))
    {
        xpr_tchar_t sClassName[MAX_CLASS_NAME + 1] = {0};
        GetClassName(sRebarBandInfo.hwndChild, sClassName, MAX_CLASS_NAME);
        if (_tcsicmp(sClassName, XPR_STRING_LITERAL("ToolbarWindow32")) == 0)
        {
            CToolBarEx *sToolBar = (CToolBarEx *)CWnd::FromHandle(sRebarBandInfo.hwndChild);
            if (XPR_IS_NOT_NULL(sToolBar) && XPR_IS_NOT_NULL(sToolBar->m_hWnd) && sToolBar->GetStyle() & TBSTYLE_WRAPABLE)
                sResult = XPR_TRUE;
        }
    }

    return sResult;
}

void MainCoolBar::setWrapable(xpr_uint_t aBandId, xpr_bool_t aWrapable)
{
    CReBarCtrl &sReBarCtrl = GetReBarCtrl();

    xpr_sint_t sBandIndex = sReBarCtrl.IDToIndex(aBandId);

    RebarBandInfo sRebarBandInfo;
    sRebarBandInfo.fMask = RBBIM_CHILD | RBBIM_STYLE;
    if (sReBarCtrl.GetBandInfo(sBandIndex, &sRebarBandInfo) == XPR_TRUE)
    {
        xpr_tchar_t sClassName[MAX_CLASS_NAME + 1] = {0};
        GetClassName(sRebarBandInfo.hwndChild, sClassName, MAX_CLASS_NAME);
        if (_tcsicmp(sClassName, XPR_STRING_LITERAL("ToolbarWindow32")) == 0)
        {
            CToolBarEx *sToolBar = (CToolBarEx *)CWnd::FromHandle(sRebarBandInfo.hwndChild);
            if (XPR_IS_NOT_NULL(sToolBar) && XPR_IS_NOT_NULL(sToolBar->m_hWnd))
            {
                sRebarBandInfo.fMask &= ~RBBIM_CHILD;
                sRebarBandInfo.fMask |= RBBIM_IDEALSIZE;

                if (XPR_IS_TRUE(aWrapable))
                {
                    sToolBar->ModifyStyle(0, TBSTYLE_WRAPABLE);

                    sRebarBandInfo.fStyle &= ~RBBS_USECHEVRON;
                    sRebarBandInfo.cxIdeal = 0;
                }
                else
                {
                    sToolBar->ModifyStyle(TBSTYLE_WRAPABLE, 0);

                    CSize sToolBarHorzMinSize;
                    sToolBarHorzMinSize = sToolBar->CalcDynamicLayout(-1, 0);

                    sRebarBandInfo.cxIdeal = sToolBarHorzMinSize.cx;
                    sRebarBandInfo.fStyle |= RBBS_USECHEVRON;
                }

                sReBarCtrl.SetBandInfo(sBandIndex, &sRebarBandInfo);

                sToolBar->UpdateToolbarSize();    // CReBarCtrl update
                GetParentFrame()->RecalcLayout(); // CSizableReBar update

                if (aBandId == AFX_IDW_BOOKMARK_BAR) gOpt->mMain.mBookmarkBarMultiLine = aWrapable;
                if (aBandId == AFX_IDW_DRIVE_BAR)    gOpt->mMain.mDriveBarMultiLine    = aWrapable;
            }
        }
    }
}

void MainCoolBar::onUpdatedToolbarSize(CToolBarEx &theToolBar)
{
    CToolBarCtrl &sToolBarCtrl = theToolBar.GetToolBarCtrl();
    CReBarCtrl &sReBarCtrl = GetReBarCtrl();

    xpr_sint_t sBandIndex = sReBarCtrl.IDToIndex((xpr_uint_t)theToolBar.GetDlgCtrlID());
    if (sBandIndex < 0)
        return;

    // Calculate desired height and ideal width of the bar whether TBSTYLE_WRAPABLE or not
    xpr_sint_t sFirstTop = 0;

    DWORD sStyle = theToolBar.GetStyle();

    CRect sItemRect;
    xpr_sint_t i;
    xpr_sint_t cyChild = 0;
    xpr_sint_t cxIdeal = 0;
    xpr_sint_t sCount = sToolBarCtrl.GetButtonCount();
    for (i = 0; i < sCount; ++i)
    {
        if (sToolBarCtrl.GetItemRect(i, sItemRect) == XPR_TRUE)
        {
            if (i == 0)
                sFirstTop = sItemRect.top;

            cxIdeal += sItemRect.Width();

            if (XPR_TEST_BITS(sStyle, TBSTYLE_WRAPABLE))
                cyChild = max(cyChild, sItemRect.bottom - sFirstTop);
            else
                cyChild = max(cyChild, sItemRect.Height());
        }
    }

    // Modify parent band info accordingly
    REBARBANDINFO sRebarBandInfo;
    sRebarBandInfo.cbSize     = sizeof(sRebarBandInfo);
    sRebarBandInfo.fMask      = RBBIM_CHILDSIZE | RBBIM_IDEALSIZE;
    sRebarBandInfo.cxIdeal    = cxIdeal;
    sRebarBandInfo.cxMinChild = 0;
    sRebarBandInfo.cyMinChild = cyChild;
    VERIFY(sReBarCtrl.SetBandInfo(sBandIndex, &sRebarBandInfo));
}

void MainCoolBar::OnSysColorChange(void) 
{
    CSizableReBar::OnSysColorChange();

}

void MainCoolBar::OnWindowPosChanged(WINDOWPOS FAR* lpwndpos) 
{
    CSizableReBar::OnWindowPosChanged(lpwndpos);

    //xpr_uint_t iBand = IDToIndex(AFX_IDW_MENUBAR);

    //RebarBandInfo sRebarBandInfo;
    //sRebarBandInfo.fMask = RBBIM_CHILDSIZE;
    //GetBandInfo(iBand, &sRebarBandInfo);

    //sRebarBandInfo.cyMinChild = m_wndMenuBar.CalcFixedLayout(XPR_TRUE, XPR_FALSE).cy;
    //SetBandInfo(iBand, &sRebarBandInfo);
}

void MainCoolBar::loadState(void)
{
    //// CSizableReBar
    //CSizableReBar::LoadState(XPR_STRING_LITERAL("FrameWnd"), XPR_STRING_LITERAL("MainCoolBar"));

    //RebarBandInfo sRebarBandInfo;
    //sRebarBandInfo.fMask = RBBIM_STYLE;
    //GetBandInfo(0, &sRebarBandInfo);

    //m_bLock = (sRebarBandInfo.fStyle & RBBS_NOGRIPPER) ? XPR_TRUE : XPR_FALSE;

    //// toolbar
    //mMainToolBar.LoadState(XPR_STRING_LITERAL("FrameWnd"), XPR_STRING_LITERAL("MainToolBar"));
}

void MainCoolBar::saveState(void)
{
    //// CSizableReBar
    //CSizableReBar::SaveState(XPR_STRING_LITERAL("FrameWnd"), XPR_STRING_LITERAL("MainCoolBar"));

    //// toolbar
    //mMainToolBar.SaveState(XPR_STRING_LITERAL("FrameWnd"), XPR_STRING_LITERAL("MainToolBar"));
}

void MainCoolBar::loadStateFile(void)
{
    // CSizableReBar
    {
        xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
        ConfDir::instance().getLoadPath(ConfDir::TypeCoolBar, sPath, XPR_MAX_PATH);

        loadStateFile(sPath);
    }

    CReBarCtrl &sReBarCtrl = GetReBarCtrl();

    RebarBandInfo sRebarBandInfo;
    sRebarBandInfo.fMask = RBBIM_STYLE;
    sReBarCtrl.GetBandInfo(0, &sRebarBandInfo);

    if ((sRebarBandInfo.fStyle & RBBS_NOGRIPPER) == RBBS_NOGRIPPER)
        Lock();

    // toolbar
    {
        xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
        ConfDir::instance().getLoadPath(ConfDir::TypeToolBar, sPath, XPR_MAX_PATH);

        if (mMainToolBar.load(sPath) == XPR_FALSE)
            mMainToolBar.SetDefaultButtons();
    }
}

void MainCoolBar::saveStateFile(void)
{
    // CSizableReBar
    {
        xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
        ConfDir::instance().getSavePath(ConfDir::TypeCoolBar, sPath, XPR_MAX_PATH);

        saveStateFile(sPath);
    }

    // toolbar
    {
        xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
        ConfDir::instance().getSavePath(ConfDir::TypeToolBar, sPath, XPR_MAX_PATH);

        mMainToolBar.save(sPath);
    }
}

#pragma pack(push, 1)

typedef struct CoolBarStateHeader
{
    xpr_sint32_t cbSize;
    xpr_uint32_t nBandCount;
} CoolBarStateHeader;

typedef struct CoolBarState
{
    xpr_sint32_t cbSize;
    xpr_uint32_t wID;
    xpr_uint32_t cx;
    xpr_uint32_t fStyle;
    xpr_uint32_t cxMinChild;
} CoolBarState;

#pragma pack(pop)

xpr_bool_t MainCoolBar::loadStateFile(const xpr_tchar_t *aPath)
{
    if (XPR_IS_NULL(aPath))
        return XPR_FALSE;

    xpr_rcode_t sRcode;
    xpr_ssize_t sReadSize;
    xpr::FileIo sFileIo;

    sRcode = sFileIo.open(aPath, xpr::FileIo::OpenModeReadOnly);
    if (XPR_RCODE_IS_ERROR(sRcode))
        return XPR_FALSE;

    CoolBarStateHeader sCoolBarStateHeader;
    sCoolBarStateHeader.cbSize = sizeof(sCoolBarStateHeader);
    sRcode = sFileIo.read(&sCoolBarStateHeader, sCoolBarStateHeader.cbSize, &sReadSize);
    if (XPR_RCODE_IS_ERROR(sRcode))
        return XPR_FALSE;

    RebarBandInfo sRebarBandInfo;
    sRebarBandInfo.fMask = RBBIM_STYLE | RBBIM_SIZE | RBBIM_ID | RBBIM_CHILDSIZE;

    xpr_sint_t i;
    xpr_sint_t sCount;
    CoolBarState sCoolBarState;
    CReBarCtrl &sReBarCtrl = GetReBarCtrl();

    sCount = (xpr_sint_t)sCoolBarStateHeader.nBandCount;
    for (i = 0; i < sCount; ++i)
    {
        sRcode = sFileIo.read(&sCoolBarState, sizeof(sCoolBarState), &sReadSize);
        if (XPR_RCODE_IS_ERROR(sRcode))
            break;

        sReBarCtrl.MoveBand(sReBarCtrl.IDToIndex(sCoolBarState.wID), i);

        sReBarCtrl.GetBandInfo(i, &sRebarBandInfo);
        sRebarBandInfo.fStyle     = sCoolBarState.fStyle;
        sRebarBandInfo.cx         = sCoolBarState.cx;
        sRebarBandInfo.cxMinChild = sCoolBarState.cxMinChild;
        sReBarCtrl.SetBandInfo(i, &sRebarBandInfo);
    }

    sFileIo.close();

    return XPR_TRUE;
}

void MainCoolBar::saveStateFile(const xpr_tchar_t *aPath)
{
    if (XPR_IS_NULL(aPath))
        return;

    xpr_rcode_t sRcode;
    xpr_ssize_t sWrittenSize;
    xpr_sint_t  sOpenMode;
    xpr::FileIo sFileIo;

    sOpenMode = xpr::FileIo::OpenModeCreate | xpr::FileIo::OpenModeTruncate | xpr::FileIo::OpenModeWriteOnly;
    sRcode = sFileIo.open(aPath, sOpenMode);
    if (XPR_RCODE_IS_ERROR(sRcode))
        return;

    CReBarCtrl &sReBarCtrl = GetReBarCtrl();

    RebarBandInfo sRebarBandInfo;
    sRebarBandInfo.fMask = RBBIM_STYLE | RBBIM_SIZE | RBBIM_ID | RBBIM_CHILDSIZE;

    xpr_sint_t i;
    xpr_sint_t sCount;
    sCount = sReBarCtrl.GetBandCount();

    CoolBarStateHeader sCoolBarStateHeader;
    sCoolBarStateHeader.cbSize = sizeof(sCoolBarStateHeader);
    sCoolBarStateHeader.nBandCount = sCount;

    sRcode = sFileIo.write(&sCoolBarStateHeader, sCoolBarStateHeader.cbSize, &sWrittenSize);
    if (XPR_RCODE_IS_ERROR(sRcode))
        return;

    CoolBarState sCoolBarState;
    sCoolBarState.cbSize = sizeof(sCoolBarState);

    for (i = 0; i < sCount; ++i)
    {
        sReBarCtrl.GetBandInfo(i, &sRebarBandInfo);

        sCoolBarState.wID        = sRebarBandInfo.wID;
        sCoolBarState.cx         = sRebarBandInfo.cx;
        sCoolBarState.fStyle     = sRebarBandInfo.fStyle;
        sCoolBarState.cxMinChild = sRebarBandInfo.cxMinChild;
        sRcode = sFileIo.write(&sCoolBarState, sCoolBarState.cbSize, &sWrittenSize);
        if (XPR_RCODE_IS_ERROR(sRcode))
            break;
    }

    sFileIo.close();
}
