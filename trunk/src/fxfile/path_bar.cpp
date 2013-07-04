//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "path_bar.h"

#include "context_menu.h"
#include "sys_img_list.h"

#include "gui/gdi.h"
#include "gui/MemDC.h"
#include "gui/BCMenu.h"

#include "option.h"
#include "path_bar_observer.h"
#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace fxfile
{
#define GAP         10
#define GAP_ICON    4

#define SEL_ELLIPSIS_LAST   0xff00ff00
#define SEL_ELLIPSIS        0xff00ff01

static const xpr_tchar_t kClassName[] = XPR_STRING_LITERAL("PathBar");

PathBar::PathBar(void)
    : mObserver(XPR_NULL)
    , mFullPidl(XPR_NULL)
    , mRealPathMode(XPR_TRUE)
    , mIsIcon(XPR_FALSE), mIcon(XPR_NULL)
    , mHighlight(XPR_TRUE), mHighlightColor(RGB(0, 0, 0))
    , mPos(-1), mDownPos(-1)
{
    mPath[0]      = 0;
    mPathMouse[0] = 0;

    mHighlightColor = DEF_PATH_BAR_HIGHLIGHT_COLOR;
}

PathBar::~PathBar(void)
{
    COM_FREE(mFullPidl);

    mTextFont.DeleteObject();
    mTextUnderLineFont.DeleteObject();

    DESTROY_ICON(mIcon);
}

xpr_bool_t PathBar::Create(CWnd *aParentWnd, xpr_uint_t aId, const RECT &aRect)
{
    DWORD sStyle = 0;
    sStyle |= WS_VISIBLE;
    sStyle |= WS_CHILD;

    WNDCLASS sWndClass = {0};
    HINSTANCE sInstance = AfxGetInstanceHandle();

    if (::GetClassInfo(sInstance, kClassName, &sWndClass) == XPR_FALSE)
    {
        sWndClass.style         = CS_DBLCLKS;
        sWndClass.lpfnWndProc   = ::DefWindowProc;
        sWndClass.cbClsExtra    = 0;
        sWndClass.cbWndExtra    = 0;
        sWndClass.hInstance     = sInstance;
        sWndClass.hIcon         = XPR_NULL;
        sWndClass.hCursor       = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
        sWndClass.hbrBackground = ::GetSysColorBrush(COLOR_WINDOW);
        sWndClass.lpszMenuName  = XPR_NULL;
        sWndClass.lpszClassName = kClassName;

        if (AfxRegisterClass(&sWndClass) == XPR_FALSE)
        {
            AfxThrowResourceException();
            return XPR_FALSE;
        }
    }

    return CWnd::Create(kClassName, XPR_NULL, sStyle, aRect, aParentWnd, aId);
}

BEGIN_MESSAGE_MAP(PathBar, CWnd)
    ON_WM_CREATE()
    ON_WM_PAINT()
    ON_WM_ERASEBKGND()
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONUP()
    ON_WM_MOUSEMOVE()
    ON_WM_SIZE()
    ON_WM_SETTINGCHANGE()
    ON_WM_CONTEXTMENU()
    ON_WM_DESTROY()
    ON_WM_CAPTURECHANGED()
END_MESSAGE_MAP()

void PathBar::setObserver(PathBarObserver *aObserver)
{
    mObserver = aObserver;
}

xpr_sint_t PathBar::OnCreate(LPCREATESTRUCT aCreateStruct)
{
    if (CWnd::OnCreate(aCreateStruct) == -1)
        return -1;

    updateFonts();
    recalcLayout();

    return 0;
}

void PathBar::OnDestroy(void)
{
    CWnd::OnDestroy();

}

xpr_bool_t PathBar::PreCreateWindow(CREATESTRUCT &aCreateStruct)
{
    return CWnd::PreCreateWindow(aCreateStruct);
}

void PathBar::setMode(xpr_bool_t aRealPathMode)
{
    mRealPathMode = aRealPathMode;
}

void PathBar::setPath(LPITEMIDLIST aFullPidl)
{
    if (XPR_IS_NULL(aFullPidl))
        return;

    COM_FREE(mFullPidl);
    mFullPidl = fxfile::base::Pidl::clone(aFullPidl);

    if (XPR_IS_TRUE(mRealPathMode))
    {
        GetFolderPath(mFullPidl, mPath);
        if ((mPath[0] == '\0') || (mPath[0] == ':' && mPath[1] == ':'))
            GetName(mFullPidl, SHGDN_INFOLDER, mPath);
    }
    else
    {
        GetDispFullPath(mFullPidl, mPath);
    }

    if (XPR_IS_TRUE(mIsIcon))
        mIcon = GetItemIcon(mFullPidl);

    SetWindowText(mPath);

    recalcLayout();
    Invalidate(XPR_FALSE);
    UpdateWindow();
}

void PathBar::OnPaint(void)
{
    CPaintDC sDC(this); // device context for painting

    CMemDC sMemDC(&sDC);

    CRect sClientRect;
    GetClientRect(&sClientRect);

    COLORREF sBackColor = CalculateColor(GetSysColor(COLOR_WINDOW), GetSysColor(COLOR_3DFACE), 150);
    sMemDC.FillSolidRect(sClientRect, sBackColor);

    CFont *sOldFont = sMemDC.SelectObject(&mTextFont);
    sMemDC.SetBkMode(TRANSPARENT);
    sMemDC.SetTextColor(::GetSysColor(COLOR_WINDOWTEXT));

    xpr_uint_t sFormat = DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX | DT_PATH_ELLIPSIS;

    if (XPR_IS_NOT_NULL(mIcon))
    {
        CPoint ptIcon(GAP_ICON, (sClientRect.Height() - 16) / 2);
        ::DrawIconEx(sMemDC.m_hDC, ptIcon.x, ptIcon.y, mIcon, 16, 16, 0, XPR_NULL, DI_NORMAL);
    }

    CRect sTextRect(sClientRect);
    sTextRect.left  += XPR_IS_NOT_NULL(mIcon) ? GAP_ICON : GAP;
    sTextRect.right -= XPR_IS_NOT_NULL(mIcon) ? GAP_ICON : GAP;

    if (XPR_IS_NOT_NULL(mIcon))
        sTextRect.left += 16 + GAP_ICON;

    sMemDC.DrawText(mPath, sTextRect, sFormat);

    sMemDC.SelectObject(sOldFont);

    if (mPathMouse[0] != XPR_STRING_LITERAL('\0'))
    {
        sOldFont = sMemDC.SelectObject(&mTextUnderLineFont);

        if (XPR_IS_TRUE(mHighlight))
            sMemDC.SetTextColor(mHighlightColor);

        CRect sTextUnderLineRect(sTextRect);
        sMemDC.DrawText(mPathMouse, (xpr_sint_t)_tcslen(mPathMouse), sTextUnderLineRect, DT_CALCRECT | sFormat);
        sBackColor = CalculateColor(GetSysColor(COLOR_WINDOW), GetSysColor(COLOR_3DFACE), 150);
        sMemDC.FillSolidRect(sTextUnderLineRect, sBackColor);

        sMemDC.DrawText(mPathMouse, sTextRect, sFormat);

        sMemDC.SelectObject(sOldFont);
    }
}

xpr_bool_t PathBar::OnEraseBkgnd(CDC *aDC)
{
    return XPR_TRUE;
}

void PathBar::OnLButtonDown(xpr_uint_t aFlags, CPoint aPoint)
{
    mDownPos = mPos;

    CWnd::OnLButtonDown(aFlags, aPoint);
}

void PathBar::OnLButtonUp(xpr_uint_t aFlags, CPoint aPoint)
{
    if (mPos != -1 && mDownPos == mPos)
    {
        xpr_tchar_t sPath[XPR_MAX_PATH * 2 + 1] = {0};
        _tcscpy(sPath, mPath);

        xpr_bool_t sTrackMenu = XPR_FALSE;

        if (mPos == SEL_ELLIPSIS)
            sTrackMenu = XPR_TRUE;
        else if (mPos == SEL_ELLIPSIS_LAST)
            mPos = (xpr_sint_t)_tcslen(mPath);

        if (XPR_IS_TRUE(sTrackMenu))
        {
            CPoint sScreenPoint(aPoint);
            ClientToScreen(&sScreenPoint);
            trackGoUpMenu(sScreenPoint);
        }
        else
        {
            sPath[mPos] = XPR_STRING_LITERAL('\0');

            if (_tcslen(sPath) == 2)
                _tcscat(sPath, XPR_STRING_LITERAL("\\"));

            LPITEMIDLIST sFullPidl = XPR_NULL;
            if (XPR_IS_TRUE(mRealPathMode))
            {
                sFullPidl = fxfile::base::Pidl::create(sPath);
            }
            else
            {
                xpr_sint_t i;
                xpr_sint_t sCount = 0;

                for (i = 0; i < mPos; ++i)
                {
                    if (sPath[i] == '\\')
                        sCount++;
                }

                if (sCount >= 0)
                {
                    xpr_sint_t sSize = 0;
                    LPITEMIDLIST sFullPidl2;

                    for (i = 0; i < sCount; ++i)
                    {
                        sFullPidl2 = (LPITEMIDLIST)(((xpr_byte_t *)mFullPidl) + sSize);
                        sSize += sFullPidl2->mkid.cb;
                    }

                    sFullPidl = fxfile::base::Pidl::alloc(sSize + sizeof(xpr_ushort_t));
                    memcpy(sFullPidl, mFullPidl, sSize);
                }
                else
                {
                    sFullPidl = fxfile::base::Pidl::alloc(sizeof(xpr_ushort_t));
                    sFullPidl->mkid.cb = 0;
                }
            }

            if (XPR_IS_NOT_NULL(sFullPidl))
            {
                if (XPR_IS_NOT_NULL(mObserver))
                    mObserver->onExplore(*this, sFullPidl);

                COM_FREE(sFullPidl);
            }
        }

        Invalidate(XPR_FALSE);
    }

    CWnd::OnLButtonUp(aFlags, aPoint);
}

void PathBar::OnMouseMove(xpr_uint_t aFlags, CPoint aPoint)
{
    CPoint sScreenPoint(aPoint);
    ClientToScreen(&sScreenPoint);

    memset(mPathMouse, 0, sizeof(mPathMouse));
    GetWindowText(mPathMouse, XPR_MAX_PATH * 2 + 1);

    CClientDC sClientDC(this);

    CFont *sOldFont = sClientDC.SelectObject(&mTextFont);

    CRect sClientRect;
    GetClientRect(&sClientRect);

    CRect sTextRect(sClientRect);
    sTextRect.left  += XPR_IS_NOT_NULL(mIcon) ? GAP_ICON : GAP;
    sTextRect.right -= XPR_IS_NOT_NULL(mIcon) ? GAP_ICON : GAP;

    if (XPR_IS_NOT_NULL(mIcon))
        sTextRect.left += 16 + GAP_ICON;

    xpr_uint_t sFormat = DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX | DT_PATH_ELLIPSIS;

    xpr_sint_t sLen = (xpr_sint_t)_tcslen(mPathMouse);
    sClientDC.DrawText(mPathMouse, sLen, &sTextRect, DT_CALCRECT | DT_MODIFYSTRING | sFormat);

    xpr_bool_t sPathEllipsis = XPR_FALSE;
    if (_tcscmp(mPath, mPathMouse) != 0)
    {
        mPathMouse[_tcslen(mPathMouse)+1] = XPR_STRING_LITERAL('\0');
        sPathEllipsis = XPR_TRUE;
    }

    // [2007/09/14] bug patched
    // check foreground window
    if (sTextRect.PtInRect(aPoint) == XPR_TRUE && ::WindowFromPoint(sScreenPoint) == m_hWnd)
    {
        SetCapture();

        xpr_sint_t i;
        xpr_sint_t sCount = 0;

        for (i = 0; i < sLen; ++i)
        {
            if (mPathMouse[i] == '\\')
                sCount++;
        }
        sCount++;

        xpr_sint_t sPos = 0;
        CRect sSplitRect;
        xpr_tchar_t *sSplit;
        xpr_tchar_t *sPathMouse = mPathMouse;

        for (i = 0; i < sCount; ++i)
        {
            sSplit = _tcschr(sPathMouse, '\\');
            if (XPR_IS_NOT_NULL(sSplit))
            {
                sPathMouse = sSplit + 1;
                sPos = (xpr_sint_t)(sSplit - mPathMouse);
            }
            else
            {
                sPos = (xpr_sint_t)_tcslen(mPathMouse);
            }

            sSplitRect = sTextRect;
            sClientDC.DrawText(mPathMouse, sPos+1, &sSplitRect, DT_CALCRECT | sFormat);

            if (sSplitRect.right > aPoint.x)
                break;
        }

        if (i != sCount)
            mPathMouse[sPos] = XPR_STRING_LITERAL('\0');

        InvalidateRect(sTextRect, XPR_FALSE);
        UpdateWindow();

        if (XPR_IS_TRUE(sPathEllipsis))
        {
            const xpr_tchar_t *sSplit = _tcschr(mPathMouse+sPos+1, XPR_STRING_LITERAL('\\'));
            if (XPR_IS_NULL(sSplit))
            {
                if (mPathMouse[sPos + 1] == XPR_STRING_LITERAL('\0'))
                    sPos = SEL_ELLIPSIS_LAST;
                else
                    sPos = SEL_ELLIPSIS;
            }
        }

        mPos = sPos;
    }
    else
    {
        mPos = -1;
        ReleaseCapture();

        mPathMouse[0] = XPR_STRING_LITERAL('\0');

        InvalidateRect(sTextRect, XPR_FALSE);
        UpdateWindow();
    }

    sClientDC.SelectObject(sOldFont);

    CWnd::OnMouseMove(aFlags, aPoint);
}

void PathBar::OnCaptureChanged(CWnd *aWnd)
{
    if (XPR_IS_NOT_NULL(aWnd) && aWnd->m_hWnd != m_hWnd && mPos != -1)
    {
        mPos = -1;
        ReleaseCapture();

        mPathMouse[0] = 0;

        Invalidate();
        UpdateWindow();
    }

    CWnd::OnCaptureChanged(aWnd);
}

void PathBar::OnSize(xpr_uint_t aType, xpr_sint_t cx, xpr_sint_t cy) 
{
    CWnd::OnSize(aType, cx, cy);

    recalcLayout();
}

void PathBar::recalcLayout(void)
{
    Invalidate(XPR_FALSE);
    UpdateWindow();
}

void PathBar::updateFonts(void)
{
    DELETE_OBJECT(mTextFont);
    DELETE_OBJECT(mTextUnderLineFont);

    if (XPR_IS_TRUE(gOpt->mConfig.mCustomFont) && gOpt->mConfig.mCustomFontText[0] != 0)
    {
        LOGFONT sLogFont = {0};
        StringToLogFont(gOpt->mConfig.mCustomFontText, sLogFont);

        mTextFont.CreateFontIndirect(&sLogFont);

        sLogFont.lfUnderline = XPR_TRUE;
        mTextUnderLineFont.CreateFontIndirect(&sLogFont);
    }

    if (XPR_IS_NULL(mTextFont.m_hObject))
    {
        // font Creation
        NONCLIENTMETRICS sNonClientMetrics = {0};
        sNonClientMetrics.cbSize = sizeof(sNonClientMetrics);
        ::SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(sNonClientMetrics), &sNonClientMetrics, 0);

        //sNonClientMetrics.lfMenuFont.lfWeight = FW_BOLD;
        mTextFont.CreateFontIndirect(&sNonClientMetrics.lfMenuFont);

        sNonClientMetrics.lfMenuFont.lfUnderline = XPR_TRUE;
        mTextUnderLineFont.CreateFontIndirect(&sNonClientMetrics.lfMenuFont);
    }
}

void PathBar::setCustomFont(xpr_tchar_t *aFontText)
{
    updateFonts();
}

void PathBar::setCustomFont(CFont *aFont)
{
    updateFonts();
}

void PathBar::setHighlightColor(xpr_bool_t aHighlight, COLORREF aHighlightColor)
{
    mHighlight  = aHighlight;
    mHighlightColor = aHighlightColor;
}

void PathBar::setIconDisp(xpr_bool_t aIsIcon)
{
    if (mIsIcon == aIsIcon)
        return;

    DESTROY_ICON(mIcon);

    if (XPR_IS_TRUE(aIsIcon))
        mIcon = GetItemIcon(mFullPidl);

    mIsIcon = aIsIcon;

    Invalidate(XPR_FALSE);
}

void PathBar::OnSettingChange(xpr_uint_t aFlags, const xpr_tchar_t *aSection)
{
    CWnd::OnSettingChange(aFlags, aSection);

    updateFonts();
}

void PathBar::trackGoUpMenu(CPoint aPoint)
{
    BCMenu sMenu;
    if (sMenu.LoadMenu(IDR_GO_UP) == XPR_TRUE)
    {
        BCMenu *sPopupMenu = (BCMenu *)sMenu.GetSubMenu(0);
        if (XPR_IS_NOT_NULL(sPopupMenu))
        {
            sPopupMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, aPoint, AfxGetMainWnd(), XPR_NULL);

            cancelHighlight();
        }
    }
}

void PathBar::trackContextMenu(CPoint aPoint)
{
    LPITEMIDLIST sFullPidl = XPR_NULL;

    xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
    _tcscpy(sPath, mPathMouse);

    xpr_sint_t sLen = (xpr_sint_t)_tcslen(sPath);
    if (sLen == 2)
        _tcscat(sPath, XPR_STRING_LITERAL("\\"));

    if (XPR_IS_TRUE(mRealPathMode))
        sFullPidl = fxfile::base::Pidl::create(sPath);
    else
        sFullPidl = GetDispFullPidl(sPath);

    if (XPR_IS_NULL(sFullPidl))
        return;

    xpr_bool_t    sResult;
    LPSHELLFOLDER sShellFolder = XPR_NULL;
    LPCITEMIDLIST sPidl        = XPR_NULL;

    sResult = fxfile::base::Pidl::getSimplePidl(sFullPidl, sShellFolder, sPidl);
    if (XPR_IS_TRUE(sResult) && XPR_IS_NOT_NULL(sShellFolder) && XPR_IS_NOT_NULL(sPidl))
    {
        ContextMenu sContextMenu(GetSafeHwnd());
        if (sContextMenu.init(sShellFolder, &sPidl, 1) == XPR_TRUE)
        {
            xpr_uint_t sId = sContextMenu.trackPopupMenu(TPM_LEFTALIGN | TPM_RETURNCMD | TPM_RIGHTBUTTON, &aPoint, CMF_EXPLORE);
            if (sId != -1)
            {
                sId -= sContextMenu.getFirstId();

                xpr_tchar_t sVerb[0xff] = {0};
                sContextMenu.getCommandVerb(sId, sVerb, 0xfe);

                xpr_bool_t sSelfInvoke = XPR_FALSE;

                if (_tcsicmp(sVerb, CMID_VERB_OPEN) == 0)
                {
                    if (XPR_IS_NOT_NULL(mObserver))
                        mObserver->onExplore(*this, sFullPidl);

                    sSelfInvoke = XPR_TRUE;
                }

                if (XPR_IS_FALSE(sSelfInvoke))
                    sContextMenu.invokeCommand(sId);
            }
        }

        sContextMenu.destroySubclass();
        sContextMenu.release();
    }

    COM_RELEASE(sShellFolder);
    COM_FREE(sFullPidl);

    cancelHighlight();
}

void PathBar::cancelHighlight(void)
{
    mPos     = -1;
    mDownPos = -1;

    mPathMouse[0] = 0;

    Invalidate(XPR_TRUE);
}

void PathBar::OnContextMenu(CWnd *aWnd, CPoint aPoint)
{
    xpr_bool_t sCtrlKey = GetAsyncKeyState(VK_CONTROL) < 0;

    if (XPR_IS_TRUE(sCtrlKey))
    {
        trackContextMenu(aPoint);
    }
    else
    {
        trackGoUpMenu(aPoint);
    }
}

xpr_sint_t PathBar::getHeight(void)
{
    CClientDC sClientDC(this);

    CFont *sOldFont = sClientDC.SelectObject(&mTextFont);

    CRect sPathRect(0, 0, 0, 0);
    sClientDC.DrawText(XPR_STRING_LITERAL("TEST"), &sPathRect, DT_CALCRECT | DT_SINGLELINE | DT_LEFT);

    sClientDC.SelectObject(sOldFont);

    return sPathRect.Height() + 6;
}
} // namespace fxfile
