//
// Copyright (c) 2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "StatusBar.h"

#include "StatusBarObserver.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static const xpr_sint_t kGap = 2;
static const xpr_sint_t kTextOffset = 3;

static const xpr_tchar_t kClassName[] = XPR_STRING_LITERAL("StatusBar");

struct StatusBar::PaneItem
{
public:
    PaneItem(void)
    {
        mPaneId    = 0;
        mData      = XPR_NULL;
        mSize      = 0;
        mMinSize   = 0;
        mIcon      = XPR_NULL;

        mPaneRect  = CRect(0, 0, 0, 0);
    }

public:
    xpr_size_t    mPaneId;
    xpr::tstring  mText;
    void         *mData;
    xpr_size_t    mSize;
    xpr_size_t    mMinSize;
    HICON         mIcon;

    CRect         mPaneRect;
};

StatusBar::StatusBar(void)
    : mObserver(XPR_NULL)
{
    registerWindowClass();
}

StatusBar::~StatusBar(void)
{
}

xpr_bool_t StatusBar::registerWindowClass(void)
{
    WNDCLASS sWndClass = {0};
    HINSTANCE sInstance = AfxGetInstanceHandle();

    //Check weather the class is registerd already
    if (::GetClassInfo(sInstance, kClassName, &sWndClass) == XPR_FALSE)
    {
        //If not then we have to register the new class
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

    return XPR_TRUE;
}

xpr_bool_t StatusBar::Create(CWnd *aParentWnd, const RECT &aRect, xpr_uint_t aId, DWORD aStyle)
{
    return CWnd::Create(kClassName, XPR_NULL, aStyle, aRect, aParentWnd, aId);
}

xpr_bool_t StatusBar::Create(CWnd *aParentWnd, xpr_uint_t aId, const RECT &aRect)
{
    DWORD sStyle = 0;
    sStyle |= WS_VISIBLE;
    sStyle |= WS_CHILD;
    sStyle |= WS_CLIPSIBLINGS;
    sStyle |= WS_CLIPCHILDREN;

    return Create(aParentWnd, aRect, aId, sStyle);
}

BEGIN_MESSAGE_MAP(StatusBar, super)
    ON_WM_CREATE()
    ON_WM_DESTROY()
    ON_WM_SIZE()
    ON_WM_PAINT()
    ON_WM_ERASEBKGND()
    ON_WM_SETTINGCHANGE()
    ON_WM_LBUTTONDBLCLK()
    ON_WM_CONTEXTMENU()
    ON_MESSAGE(WM_SETTEXT, OnSetText)
END_MESSAGE_MAP()

xpr_sint_t StatusBar::OnCreate(LPCREATESTRUCT aCreateStruct)
{
    if (super::OnCreate(aCreateStruct) == -1)
        return -1;

    return 0;
}

void StatusBar::PreSubclassWindow(void)
{
    createFont();

    super::PreSubclassWindow();
}

void StatusBar::OnDestroy(void)
{
    removeAllPanes();

    super::OnDestroy();

    destroyFont();
}

void StatusBar::setObserver(StatusBarObserver *aObserver)
{
    mObserver = aObserver;
}

xpr_size_t StatusBar::addPane(xpr_size_t aPaneId, const xpr_tchar_t *aText, HICON aIcon, void *aData)
{
    return insertPane(InvalidIndex, aPaneId, aText, aIcon, aData);
}

xpr_size_t StatusBar::insertPane(xpr_size_t aIndex, xpr_size_t aPaneId, const xpr_tchar_t *aText, HICON aIcon, void *aData)
{
    if (aIndex != InvalidIndex && !XPR_STL_IS_INDEXABLE(aIndex, mPaneDeque))
        return InvalidIndex;

    PaneItem *sPaneItem = new PaneItem;
    if (XPR_IS_NULL(sPaneItem))
        return InvalidIndex;

    if (XPR_IS_NOT_NULL(aText))
        sPaneItem->mText = aText;

    sPaneItem->mPaneId = aPaneId;
    sPaneItem->mData   = aData;
    sPaneItem->mIcon   = aIcon;

    return insertPaneItem(aIndex, sPaneItem);
}

xpr_size_t StatusBar::insertPaneItem(xpr_size_t aIndex, PaneItem *aPaneItem)
{
    if (XPR_IS_NULL(aPaneItem))
        return InvalidIndex;

    if (aIndex == InvalidIndex)
    {
        mPaneDeque.push_back(aPaneItem);
        aIndex = (xpr_sint_t)mPaneDeque.size() - 1;
    }
    else
    {
        mPaneDeque.insert(mPaneDeque.begin() + aIndex, aPaneItem);
    }

    recalcLayout();

    return aIndex;
}

xpr_bool_t StatusBar::setPaneSize(xpr_size_t aIndex, xpr_size_t aSize, xpr_size_t aMinSize)
{
    PaneItem *sPaneItem = getPaneItem(aIndex);
    if (XPR_IS_NULL(sPaneItem))
        return XPR_FALSE;

    sPaneItem->mSize    = aSize;
    sPaneItem->mMinSize = aMinSize;

    recalcLayout();

    return XPR_TRUE;
}

xpr_bool_t StatusBar::setPaneText(xpr_size_t aIndex, const xpr_tchar_t *aText)
{
    if (XPR_IS_NULL(aText))
        return XPR_FALSE;

    PaneItem *sPaneItem = getPaneItem(aIndex);
    if (XPR_IS_NULL(sPaneItem))
        return XPR_FALSE;

    if (sPaneItem->mText == aText)
        return XPR_TRUE;

    sPaneItem->mText = aText;

    recalcLayout();

    return XPR_TRUE;
}

xpr_bool_t StatusBar::setDynamicPaneText(xpr_sint_t aIndex, const xpr_tchar_t *aText, xpr_sint_t aOffset)
{
    if (XPR_IS_NULL(aText))
        return XPR_FALSE;

    PaneItem *sPaneItem = getPaneItem(aIndex);
    if (XPR_IS_NULL(sPaneItem))
        return XPR_FALSE;

    xpr_size_t sSize = 0;

    if (aText[0] != XPR_STRING_LITERAL('\0'))
    {
        CClientDC sClientDC(this);
        CFont *sOldFont = sClientDC.SelectObject(&mFont);

        SIZE sTextExtent = {0};
        ::GetTextExtentPoint32(sClientDC.m_hDC, aText, (xpr_sint_t)_tcslen(aText), &sTextExtent);

        sClientDC.SelectObject(sOldFont);

        sSize = sTextExtent.cx + aOffset + kTextOffset * 2 + 1;
    }

    if (sPaneItem->mMinSize != 0)
    {
        sSize = max(sSize, sPaneItem->mMinSize);
    }

    sPaneItem->mText = aText;
    sPaneItem->mSize = sSize;

    recalcLayout();

    return XPR_TRUE;
}

xpr_bool_t StatusBar::setPaneIcon(xpr_size_t aIndex, HICON aIcon)
{
    PaneItem *sPaneItem = getPaneItem(aIndex);
    if (XPR_IS_NULL(sPaneItem))
        return XPR_FALSE;

    sPaneItem->mIcon = aIcon;

    recalcLayout();

    return XPR_TRUE;
}

xpr_bool_t StatusBar::setPaneData(xpr_size_t aIndex, void *aData)
{
    PaneItem *sPaneItem = getPaneItem(aIndex);
    if (XPR_IS_NULL(sPaneItem))
        return XPR_FALSE;

    sPaneItem->mData = aData;

    return XPR_TRUE;
}

xpr_size_t StatusBar::getPaneCount(void) const
{
    return mPaneDeque.size();
}

StatusBar::PaneItem *StatusBar::getPaneItem(xpr_size_t aPane) const
{
    if (XPR_STL_IS_INDEXABLE(aPane, mPaneDeque))
        return mPaneDeque[aPane];

    return XPR_NULL;
}

xpr_bool_t StatusBar::getPaneText(xpr_size_t aIndex, xpr_tchar_t *aText, xpr_size_t aMaxLen) const
{
    if (XPR_IS_NULL(aText) || aMaxLen == 0)
        return XPR_FALSE;

    PaneItem *sPaneItem = getPaneItem(aIndex);
    if (XPR_IS_NULL(sPaneItem))
        return XPR_FALSE;

    if (sPaneItem->mText.length() > aMaxLen)
        return XPR_FALSE;

    _tcscpy(aText, sPaneItem->mText.c_str());

    return XPR_TRUE;
}

void *StatusBar::getPaneData(xpr_size_t aIndex) const
{
    PaneItem *sPaneItem = getPaneItem(aIndex);
    if (XPR_IS_NULL(sPaneItem))
        return XPR_NULL;

    return sPaneItem->mData;
}

xpr_bool_t StatusBar::getPaneRect(xpr_size_t aIndex, RECT *aRect) const
{
    if (XPR_IS_NULL(aRect))
        return XPR_FALSE;

    PaneItem *sPaneItem = getPaneItem(aIndex);
    if (XPR_IS_NULL(sPaneItem))
        return XPR_NULL;

    *aRect = sPaneItem->mPaneRect;

    return XPR_TRUE;
}

xpr_sint_t StatusBar::getDefaultHeight(void)
{
    CClientDC sClientDC(this);

    CFont *sOldFont = sClientDC.SelectObject(&mFont);

    CSize sText = sClientDC.GetTextExtent(XPR_STRING_LITERAL("TEST"));

    sClientDC.SelectObject(sOldFont);

    //xpr_sint_t sCyEdge = GetSystemMetrics(SM_CYEDGE);

    return sText.cy + 2;
}

xpr_size_t StatusBar::IdToIndex(xpr_size_t aPaneId) const
{
    xpr_size_t i;
    PaneItem *sPaneItem;
    PaneDeque::const_iterator sIterator;

    sIterator = mPaneDeque.begin();
    for (i = 0; sIterator != mPaneDeque.end(); ++i, ++sIterator)
    {
        sPaneItem = *sIterator;
        if (XPR_IS_NULL(sPaneItem))
            continue;

        if (sPaneItem->mPaneId == aPaneId)
        {
            return i;
        }
    }

    return InvalidIndex;
}

xpr_bool_t StatusBar::removePane(xpr_size_t aIndex)
{
    if (!XPR_STL_IS_INDEXABLE(aIndex, mPaneDeque))
        return XPR_FALSE;

    if (XPR_IS_NOT_NULL(mObserver))
    {
        mObserver->onStatusBarRemove(*this, aIndex);
    }

    PaneDeque::iterator sIterator = mPaneDeque.begin() + aIndex;

    PaneItem *sPaneItem = *sIterator;
    XPR_SAFE_DELETE(sPaneItem);

    mPaneDeque.erase(sIterator);

    recalcLayout();

    if (XPR_IS_NOT_NULL(mObserver))
    {
        mObserver->onStatusBarRemoved(*this);
    }

    return XPR_TRUE;
}

void StatusBar::removeAllPanes(void)
{
    xpr_size_t i;
    PaneItem *sPaneItem;
    PaneDeque::iterator sIterator;

    sIterator = mPaneDeque.begin();
    for (i = 0; sIterator != mPaneDeque.end(); ++sIterator, ++i)
    {
        sPaneItem = *sIterator;

        if (XPR_IS_NOT_NULL(mObserver))
        {
            mObserver->onStatusBarRemove(*this, i);
        }

        XPR_SAFE_DELETE(sPaneItem);
    }

    mPaneDeque.clear();

    recalcLayout();

    if (XPR_IS_NOT_NULL(mObserver))
    {
        mObserver->onStatusBarRemoveAll(*this);
    }
}

void StatusBar::OnSize(xpr_uint_t aType, xpr_sint_t cx, xpr_sint_t cy)
{
    super::OnSize(aType, cx, cy);

    recalcLayout();
}

void StatusBar::recalcLayout(xpr_bool_t aRedraw)
{
    CRect sClientRect;
    GetClientRect(&sClientRect);

    xpr_sint_t sWidth  = sClientRect.Width();
    xpr_sint_t sHeight = sClientRect.Height();

    if (sWidth <= 0 || sHeight <= 0)
        return;

    xpr_size_t sPaneCount = mPaneDeque.size();
    if (sPaneCount == 0)
        return;

    xpr_sint_t sWidthWithoutGap = sWidth - (((xpr_sint_t)sPaneCount - 1) * kGap);

    xpr_sint_t sTotalFixedSize = 0;
    PaneItem *sPaneItem;
    PaneDeque::iterator sIterator;

    sIterator = mPaneDeque.begin() + 1;
    for (; sIterator != mPaneDeque.end(); ++sIterator)
    {
        sPaneItem = *sIterator;
        if (XPR_IS_NULL(sPaneItem))
            continue;

        sTotalFixedSize += (xpr_sint_t)sPaneItem->mSize;
    }

    xpr_sint_t sFirstPaneSize = sWidth - sTotalFixedSize;

    xpr_sint_t sLeft = 0;
    xpr_sint_t sTop = 1;
    xpr_sint_t sBottom = sClientRect.bottom;
    xpr_size_t i;

    sIterator = mPaneDeque.begin();
    for (i = 0; sIterator != mPaneDeque.end(); ++sIterator, ++i)
    {
        sPaneItem = *sIterator;
        if (XPR_IS_NULL(sPaneItem))
            continue;

        sPaneItem->mPaneRect.SetRectEmpty();

        sPaneItem->mPaneRect.left   = sLeft;
        sPaneItem->mPaneRect.right  = sLeft;
        sPaneItem->mPaneRect.top    = sTop;
        sPaneItem->mPaneRect.bottom = sBottom;

        if (i != 0)
        {
            sPaneItem->mPaneRect.right = sLeft + (xpr_sint_t)sPaneItem->mSize;
        }
        else
        {
            if (sFirstPaneSize < (xpr_sint_t)sPaneItem->mMinSize)
                sFirstPaneSize = (xpr_sint_t)sPaneItem->mMinSize;

            sPaneItem->mPaneRect.right = sLeft + sFirstPaneSize;
        }

        sLeft = sPaneItem->mPaneRect.right + kGap;
    }

    if (XPR_IS_TRUE(aRedraw))
        Invalidate(XPR_FALSE);
}

void StatusBar::OnPaint(void)
{
    CPaintDC sPaintDC(this); // device context for painting

    CRect sClientRect;
    GetClientRect(&sClientRect);

    CDC sMemDC;
    sMemDC.CreateCompatibleDC(&sPaintDC);
    sMemDC.SetBkMode(TRANSPARENT);
    sMemDC.SetTextColor(RGB(0,0,0));

    CBitmap sBitmap;
    sBitmap.CreateCompatibleBitmap(&sPaintDC, sClientRect.Width(), sClientRect.Height());

    CBitmap *sOldBitmap = sMemDC.SelectObject(&sBitmap);

    COLORREF sFaceColor   = ::GetSysColor(COLOR_3DFACE);
    COLORREF sShadowColor = ::GetSysColor(COLOR_3DSHADOW);

    sMemDC.FillSolidRect(&sClientRect, sFaceColor);

    CPen sBorderPen(PS_SOLID, 1, fxb::LightenColor(sShadowColor, 0.3));
    CPen sSeparatorPen(PS_SOLID, 1, fxb::LightenColor(sShadowColor, 0.6));

    CPen *sOldPen = sMemDC.SelectObject(&sBorderPen);
    CFont *sOldFont = sMemDC.SelectObject(&mFont);

    sMemDC.MoveTo(0, 0);
    sMemDC.LineTo(sClientRect.right, 0);

    sMemDC.SelectObject(sOldPen);

    sOldPen = sMemDC.SelectObject(&sSeparatorPen);

    xpr_sint_t i;
    PaneDeque::iterator sIterator;
    PaneItem *sPaneItem;
    CRect sPaneRect, sCurPaneButtonRect;

    sIterator = mPaneDeque.begin();
    for (i = 0; sIterator != mPaneDeque.end(); ++sIterator, ++i)
    {
        sPaneItem = *sIterator;
        if (XPR_IS_NULL(sPaneItem))
            continue;

        sPaneRect = sPaneItem->mPaneRect;

        CRect sPaneTextRect(sPaneRect);
        if (i > 0)
        {
            sMemDC.MoveTo(sPaneRect.left, sPaneRect.top + 2);
            sMemDC.LineTo(sPaneRect.left, sPaneRect.bottom - 2);

            sPaneTextRect.DeflateRect(1 + kTextOffset, 0, kTextOffset, 0);
        }
        else
        {
            sPaneTextRect.DeflateRect(kTextOffset, 0, kTextOffset, 0);
        }

        sMemDC.DrawText(
            sPaneItem->mText.c_str(),
            (xpr_sint_t)sPaneItem->mText.length(),
            sPaneTextRect,
            DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
    }

    sMemDC.SelectObject(sOldFont);
    sMemDC.SelectObject(sOldPen);

    sPaintDC.BitBlt(0, 0, sClientRect.Width(), sClientRect.Height(), &sMemDC, 0, 0, SRCCOPY);

    sMemDC.SelectObject(sOldBitmap);
}

xpr_bool_t StatusBar::OnEraseBkgnd(CDC *aDC)
{
    return XPR_TRUE;
    return super::OnEraseBkgnd(aDC);
}

void StatusBar::createFont(void)
{
    NONCLIENTMETRICS sNonClientMetrics = {0};
    sNonClientMetrics.cbSize = sizeof(sNonClientMetrics);
    ::SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(sNonClientMetrics), &sNonClientMetrics, 0);

    LOGFONT sLogFont = sNonClientMetrics.lfMenuFont;

    mFont.CreateFontIndirect(&sLogFont);
    SetFont(&mFont, XPR_TRUE);

    sLogFont.lfWeight = FW_BOLD;
    mBoldFont.CreateFontIndirect(&sLogFont);
}

void StatusBar::recreateFont(void)
{
    destroyFont();
    createFont();
}

void StatusBar::destroyFont(void)
{
    if (XPR_IS_NOT_NULL(mFont.m_hObject)) mFont.DeleteObject();
    if (XPR_IS_NOT_NULL(mBoldFont.m_hObject)) mBoldFont.DeleteObject();
}

void StatusBar::OnSettingChange(xpr_uint_t aFlags, const xpr_tchar_t *aSection)
{
    recreateFont();

    Invalidate();

    super::OnSettingChange(aFlags, aSection);
}

xpr_size_t StatusBar::hitTest(const POINT &aPoint) const
{
    xpr_size_t i;
    PaneItem *sPaneItem;
    PaneDeque::const_iterator sIterator;

    sIterator = mPaneDeque.begin();
    for (i = 0; sIterator != mPaneDeque.end(); ++i, ++sIterator)
    {
        sPaneItem = *sIterator;
        if (XPR_IS_NULL(sPaneItem))
            continue;

        if (sPaneItem->mPaneRect.PtInRect(aPoint) == XPR_TRUE)
            return i;
    }

    return InvalidIndex;
}

void StatusBar::OnLButtonDblClk(xpr_uint_t aFlags, CPoint aPoint)
{
    if (XPR_IS_NOT_NULL(mObserver))
    {
        xpr_size_t sIndex = hitTest(aPoint);

        mObserver->onStatusBarDoubleClicked(*this, sIndex);
    }

    super::OnLButtonDblClk(aFlags, aPoint);
}

void StatusBar::OnContextMenu(CWnd *aWnd, CPoint aPoint)
{
    CPoint sClientPoint(aPoint);
    ScreenToClient(&sClientPoint);

    if (XPR_IS_NOT_NULL(mObserver))
    {
        xpr_size_t aIndex = hitTest(sClientPoint);

        mObserver->onStatuBarContextMenu(*this, aIndex, aPoint);
        return;
    }

    super::OnContextMenu(aWnd, aPoint);
}

LRESULT StatusBar::OnSetText(WPARAM aWParam, LPARAM aLParam)
{
    const xpr_tchar_t *sText = (const xpr_tchar_t *)aLParam;

    setPaneText(0, sText);

    return XPR_TRUE;
}
