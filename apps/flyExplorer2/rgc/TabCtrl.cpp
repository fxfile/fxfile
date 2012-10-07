//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "TabCtrl.h"

#include "TabCtrlObserver.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static const xpr_sint_t kDefaultTabSize = 150;
static const CRect kOffset(3, 2, 3, 0);

static const xpr_tchar_t kClassName[] = XPR_STRING_LITERAL("TabCtrl");

struct TabCtrl::TabItem
{
    std::tstring  mText;
    void         *mData;
    CRect         mTabRect;
    CRect         mCloseRect;
    xpr_sint_t    mImageIndex;
};

TabCtrl::TabCtrl(void)
    : mObserver(XPR_NULL)
    , mCurTab(InvalidTab)
    , mFixedSizeMode(XPR_FALSE), mFixedSize(kDefaultTabSize)
    , mTabMinSize(0), mTabMaxSize(ksint32max)
    , mImageList(XPR_NULL)
    , mSetCapture(XPR_FALSE), mHoverTab(InvalidTab), mPressedTab(InvalidTab)
{
}

TabCtrl::~TabCtrl(void)
{
}

xpr_bool_t TabCtrl::Create(CWnd *aParentWnd, xpr_uint_t aId, const RECT &aRect)
{
    DWORD sStyle = 0;
    sStyle |= WS_VISIBLE;
    sStyle |= WS_CHILD;
    sStyle |= WS_CLIPSIBLINGS;
    sStyle |= WS_CLIPCHILDREN;

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

    return CWnd::Create(kClassName, XPR_NULL, sStyle, aRect, aParentWnd, aId);
}

BEGIN_MESSAGE_MAP(TabCtrl, super)
    ON_WM_CREATE()
    ON_WM_DESTROY()
    ON_WM_SIZE()
    ON_WM_PAINT()
    ON_WM_ERASEBKGND()
    ON_WM_SETTINGCHANGE()
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONUP()
    ON_WM_MOUSEMOVE()
    ON_WM_LBUTTONDBLCLK()
    ON_WM_CAPTURECHANGED()
    ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()

xpr_sint_t TabCtrl::OnCreate(LPCREATESTRUCT aCreateStruct)
{
    if (super::OnCreate(aCreateStruct) == -1)
        return -1;

    mXPTheme.Open(m_hWnd, XPR_WIDE_STRING_LITERAL("TAB"));

    createFont();

    return 0;
}

void TabCtrl::OnDestroy(void)
{
    removeAllTabs();

    super::OnDestroy();

    destroyFont();
}

void TabCtrl::setObserver(TabCtrlObserver *aObserver)
{
    mObserver = aObserver;
}

void TabCtrl::setImageList(CImageList *aImageList)
{
    mImageList = aImageList;
}

void TabCtrl::setTabSizeLimit(xpr_sint_t aMinSize, xpr_sint_t aMaxSize)
{
    mTabMinSize = aMinSize;
    mTabMaxSize = aMaxSize;
}

void TabCtrl::setTabSizeMode(xpr_bool_t aFixedSizeMode, xpr_sint_t aFixedSize)
{
    if (aFixedSize <= 0)
        aFixedSize = kDefaultTabSize;

    mFixedSizeMode = aFixedSizeMode;
    mFixedSize = aFixedSize;
}

xpr_size_t TabCtrl::addTab(const xpr_tchar_t *aText, xpr_sint_t aImageIndex, void *aData)
{
    return insertTab(InvalidTab, aText, aImageIndex, aData);
}

xpr_size_t TabCtrl::insertTab(xpr_size_t aTab, const xpr_tchar_t *aText, xpr_sint_t aImageIndex, void *aData)
{
    if (aTab != InvalidTab && !XPR_STL_IS_INDEXABLE(aTab, mTabDeque))
        return InvalidTab;

    TabItem *sTabItem = new TabItem;
    if (XPR_IS_NULL(sTabItem))
        return InvalidTab;

    if (XPR_IS_NOT_NULL(aText))
        sTabItem->mText = aText;

    sTabItem->mData       = aData;
    sTabItem->mImageIndex = aImageIndex;

    return insertTabItem(aTab, sTabItem);
}

xpr_size_t TabCtrl::insertTabItem(xpr_size_t aTab, TabItem *aTabItem, xpr_bool_t aNotifyObserver)
{
    if (XPR_IS_NULL(aTabItem))
        return InvalidTab;

    if (aTab == InvalidTab)
    {
        mTabDeque.push_back(aTabItem);
        aTab = (xpr_sint_t)mTabDeque.size() - 1;
    }
    else
    {
        mTabDeque.insert(mTabDeque.begin() + aTab, aTabItem);
    }

    recalcLayout();

    if (XPR_IS_TRUE(aNotifyObserver) && XPR_IS_NOT_NULL(mObserver))
    {
        mObserver->onTabInserted(*this, aTab);
    }

    return aTab;
}

xpr_size_t TabCtrl::getTabCount(void) const
{
    return mTabDeque.size();
}

xpr_size_t TabCtrl::getCurTab(void) const
{
    return mCurTab;
}

TabCtrl::TabItem *TabCtrl::getTabItem(xpr_size_t aTab) const
{
    if (XPR_STL_IS_INDEXABLE(aTab, mTabDeque))
        return mTabDeque[aTab];

    return XPR_NULL;
}

xpr_bool_t TabCtrl::getTabText(xpr_size_t aTab, xpr_tchar_t *aText, xpr_size_t aMaxLen) const
{
    TabItem *sTabItem = getTabItem(aTab);
    if (XPR_IS_NULL(sTabItem))
        return XPR_FALSE;

    if (aMaxLen < sTabItem->mText.length())
        return XPR_FALSE;

    _tcscpy(aText, sTabItem->mText.c_str());

    return XPR_TRUE;
}

void *TabCtrl::getTabData(xpr_size_t aTab) const
{
    TabItem *sTabItem = getTabItem(aTab);
    if (XPR_IS_NULL(sTabItem))
        return XPR_NULL;

    return sTabItem->mData;
}

xpr_size_t TabCtrl::hitTest(const POINT &aPoint) const
{
    xpr_size_t i;
    TabItem *sTabItem;
    TabDeque::const_iterator sIterator;

    sIterator = mTabDeque.begin();
    for (i = 0; sIterator != mTabDeque.end(); ++i, ++sIterator)
    {
        sTabItem = *sIterator;
        if (XPR_IS_NULL(sTabItem))
            continue;

        if (sTabItem->mTabRect.PtInRect(aPoint) == XPR_TRUE)
            return i;
    }

    return InvalidTab;
}

xpr_bool_t TabCtrl::setCurTab(xpr_size_t aTab)
{
    if (!XPR_STL_IS_INDEXABLE(aTab, mTabDeque))
        return XPR_FALSE;

    if (mCurTab == aTab)
        return XPR_TRUE;

    xpr_size_t sOldCurTab = mCurTab;

    if (XPR_IS_NOT_NULL(mObserver))
    {
        if (mObserver->onTabChangingCurTab(*this, sOldCurTab, aTab) == XPR_FALSE)
            return XPR_FALSE;
    }

    mCurTab = aTab;

    recalcLayout();

    if (XPR_IS_NOT_NULL(mObserver))
    {
        mObserver->onTabChangedCurTab(*this, sOldCurTab, aTab);
    }

    return XPR_TRUE;
}

xpr_bool_t TabCtrl::setTabText(xpr_size_t aTab, const xpr_tchar_t *aText)
{
    if (XPR_IS_NULL(aText))
        return XPR_FALSE;

    TabItem *sTabItem = getTabItem(aTab);
    if (XPR_IS_NULL(sTabItem))
        return XPR_FALSE;

    sTabItem->mText = aText;

    recalcLayout();

    return XPR_TRUE;
}

xpr_bool_t TabCtrl::swapTab(xpr_size_t aTab1, xpr_size_t aTab2)
{
    if (aTab1 == aTab2)
        return XPR_TRUE;

    TabItem *sTabItem1 = getTabItem(aTab1);
    TabItem *sTabItem2 = getTabItem(aTab2);

    if (XPR_IS_NULL(sTabItem1) || XPR_IS_NULL(sTabItem2))
        return XPR_FALSE;

    TabItem sTempItem = *sTabItem1;
    *sTabItem1 = *sTabItem2;
    *sTabItem2 = sTempItem;

    return XPR_TRUE;
}

xpr_bool_t TabCtrl::moveTab(xpr_size_t aSourceTab, xpr_size_t aTargetTab)
{
    if (aSourceTab == aTargetTab)
        return XPR_TRUE;

    if (!XPR_STL_IS_INDEXABLE(aSourceTab, mTabDeque))
        return XPR_FALSE;

    TabDeque::iterator sIterator = mTabDeque.begin() + aSourceTab;
    if (sIterator != mTabDeque.end())
        return XPR_FALSE;

    TabItem *sTabItem = getTabItem(aSourceTab);

    if (insertTabItem(aTargetTab, sTabItem, XPR_FALSE) == InvalidTab)
    {
        return XPR_FALSE;
    }

    mTabDeque.erase(sIterator);

    return XPR_TRUE;
}

xpr_bool_t TabCtrl::removeTab(xpr_size_t aTab)
{
    if (!XPR_STL_IS_INDEXABLE(aTab, mTabDeque))
        return XPR_FALSE;

    if (XPR_IS_NOT_NULL(mObserver))
    {
        mObserver->onTabRemove(*this, aTab);
    }

    TabDeque::iterator sIterator = mTabDeque.begin() + aTab;

    TabItem *sTabItem = *sIterator;
    XPR_SAFE_DELETE(sTabItem);

    mTabDeque.erase(sIterator);

    recalcLayout();

    if (XPR_IS_NOT_NULL(mObserver))
    {
        mObserver->onTabRemoved(*this);
    }

    if (aTab < mCurTab)
    {
        mCurTab--;
    }
    else if (aTab == mCurTab)
    {
        xpr_size_t sOldCurTab = mCurTab;

        if (aTab == mTabDeque.size())
        {
            if (mCurTab != 0)
            {
                mCurTab--;
            }
        }

        if (XPR_IS_NOT_NULL(mObserver))
        {
            mObserver->onTabChangedCurTab(*this, sOldCurTab, mCurTab);
        }
    }

    if (mTabDeque.size() == 0)
    {
        mCurTab = InvalidTab;

        if (XPR_IS_NOT_NULL(mObserver))
        {
            mObserver->onTabEmptied(*this);
        }
    }

    return XPR_TRUE;
}

void TabCtrl::removeAllTabs(void)
{
    xpr_size_t i;
    TabItem *sTabItem;
    TabDeque::iterator sIterator;

    sIterator = mTabDeque.begin();
    for (i = 0; sIterator != mTabDeque.end(); ++sIterator, ++i)
    {
        sTabItem = *sIterator;

        if (XPR_IS_NOT_NULL(mObserver))
        {
            mObserver->onTabRemove(*this, i);
        }

        XPR_SAFE_DELETE(sTabItem);
    }

    mTabDeque.clear();

    recalcLayout();

    if (XPR_IS_NOT_NULL(mObserver))
    {
        mObserver->onTabRemoveAll(*this);
    }

    mCurTab = InvalidTab;

    if (XPR_IS_NOT_NULL(mObserver))
    {
        mObserver->onTabEmptied(*this);
    }
}

void TabCtrl::OnSize(xpr_uint_t aType, xpr_sint_t cx, xpr_sint_t cy)
{
    super::OnSize(aType, cx, cy);

    recalcLayout();
}

void TabCtrl::recalcLayout(xpr_bool_t aRedraw)
{
    CRect sClientRect;
    GetClientRect(&sClientRect);

    xpr_sint_t sWidth  = sClientRect.Width();
    xpr_sint_t sHeight = sClientRect.Height();

    if (sWidth <= 0 || sHeight <= 0)
        return;

    sWidth -= kOffset.left + kOffset.right;

    xpr_double_t fw = (xpr_double_t)sWidth / (xpr_double_t)mTabDeque.size();
    if (fw >= kDefaultTabSize)
        fw = kDefaultTabSize;

    xpr_sint_t sLeft = kOffset.left;
    xpr_sint_t sTop = kOffset.top;
    xpr_sint_t sBottom = sHeight - sTop - 1; // 1 is underline

    xpr_size_t i;
    TabItem *sTabItem;
    TabDeque::iterator sIterator;

    sIterator = mTabDeque.begin();
    for (i = 0; sIterator != mTabDeque.end(); ++sIterator, ++i)
    {
        sTabItem = *sIterator;
        if (XPR_IS_NULL(sTabItem))
            continue;

        sTabItem->mTabRect.SetRectEmpty();

        sTabItem->mTabRect.left   = sLeft + (xpr_slong_t)(fw * (xpr_double_t)(i + 0));
        sTabItem->mTabRect.right  = sLeft + (xpr_slong_t)(fw * (xpr_double_t)(i + 1));
        sTabItem->mTabRect.top    = sTop;
        sTabItem->mTabRect.bottom = sBottom;
    }

    if (XPR_IS_TRUE(aRedraw))
        Invalidate(XPR_FALSE);
}

void TabCtrl::OnPaint(void)
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

    sMemDC.FillSolidRect(&sClientRect, RGB(255,255,255));

    // Alpha Blend
    {
        BLENDFUNCTION sBlendFunction = {0};
        sBlendFunction.BlendOp             = AC_SRC_OVER;
        sBlendFunction.BlendFlags          = 0;
        sBlendFunction.SourceConstantAlpha = 100;
        sBlendFunction.AlphaFormat         = 0;

        CDC sTempDC;
        sTempDC.CreateCompatibleDC(&sMemDC);

        CBitmap sBitmap;
        sBitmap.CreateCompatibleBitmap(&sTempDC, sClientRect.Width(), sClientRect.Height());

        CBitmap *sOldBitmap = sTempDC.SelectObject(&sBitmap);

        sTempDC.FillSolidRect(&sClientRect, sFaceColor);

        ::AlphaBlend(
            sMemDC.m_hDC,
            sClientRect.left, sClientRect.top, sClientRect.Width(), sClientRect.Height(),
            sTempDC.m_hDC,
            0, 0, sClientRect.Width(), sClientRect.Height(),
            sBlendFunction);

        sTempDC.SelectObject(sOldBitmap);
    }

    CPen sPen(PS_SOLID, 1, sShadowColor);

    CPen *sOldPen = sMemDC.SelectObject(&sPen);

    xpr_sint_t i;
    TabDeque::iterator sIterator;
    TabItem *sTabItem;
    CFont *sOldFont;
    CRect sTabRect, sCurTabButtonRect;

    sIterator = mTabDeque.begin();
    for (i = 0; sIterator != mTabDeque.end(); ++sIterator, ++i)
    {
        sTabItem = *sIterator;
        if (XPR_IS_NULL(sTabItem))
            continue;

        sTabRect = sTabItem->mTabRect;

        CRect sTabButtonRect(sTabRect);
        if (i == mCurTab || i == mHoverTab)
        {
            if (mXPTheme.IsAppThemed() == XPR_TRUE)
            {
                sTabButtonRect.InflateRect(1, 0, 1, 4);

                if (i == mCurTab)
                {
                    mXPTheme.DrawBackground(sMemDC, TABP_TABITEM, TIS_SELECTED, &sTabButtonRect, XPR_NULL);
                }
                else
                {
                    mXPTheme.DrawBackground(sMemDC, TABP_TABITEM, TIS_HOT, &sTabButtonRect, XPR_NULL);
                }
            }
            else
            {
                sTabButtonRect.InflateRect(1, 0, 0, 2);

                sMemDC.FillSolidRect(sTabButtonRect, ::GetSysColor(COLOR_WINDOW));

                sMemDC.MoveTo(sTabButtonRect.left,  sTabButtonRect.bottom);
                sMemDC.LineTo(sTabButtonRect.left,  sTabButtonRect.top);
                sMemDC.LineTo(sTabButtonRect.right, sTabButtonRect.top);
                sMemDC.LineTo(sTabButtonRect.right, sTabButtonRect.bottom);
            }

            if (i == mCurTab)
            {
                sCurTabButtonRect = sTabButtonRect;
            }
        }
        else
        {
            if (mXPTheme.IsAppThemed() == XPR_TRUE)
            {
                sTabButtonRect.DeflateRect(0, 2, -1, -1);

                mXPTheme.DrawBackground(sMemDC, TABP_TABITEM, TIS_NORMAL, &sTabButtonRect, XPR_NULL);
            }
            else
            {
                sTabButtonRect.DeflateRect(0, 2, -1, -1);

                sMemDC.FillSolidRect(sTabButtonRect, sFaceColor);

                sMemDC.MoveTo(sTabButtonRect.left, sTabButtonRect.bottom);
                sMemDC.LineTo(sTabButtonRect.left, sTabButtonRect.top);
                sMemDC.LineTo(sTabButtonRect.right, sTabButtonRect.top);
                sMemDC.LineTo(sTabButtonRect.right, sTabButtonRect.bottom);
            }
        }

        CRect sTabTextRect(sTabButtonRect);
        sTabTextRect.DeflateRect(5, 2, 5, 2);
        if (i == mCurTab)
            sOldFont = sMemDC.SelectObject(&mBoldFont);
        else
            sOldFont = sMemDC.SelectObject(&mFont);

        sMemDC.DrawText(
            sTabItem->mText.c_str(),
            (xpr_sint_t)sTabItem->mText.length(),
            sTabTextRect,
            DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);

        sMemDC.SelectObject(sOldFont);
    }

    // draw underline
    CRect sUnderlineRect(sClientRect);
    if (mCurTab != InvalidTab)
    {
        sMemDC.MoveTo(sUnderlineRect.left,     sUnderlineRect.bottom - 1);
        sMemDC.LineTo(sCurTabButtonRect.left,  sUnderlineRect.bottom - 1);
        sMemDC.MoveTo(sCurTabButtonRect.right, sUnderlineRect.bottom - 1);
        sMemDC.LineTo(sUnderlineRect.right,    sUnderlineRect.bottom - 1);
    }
    else
    {
        sMemDC.MoveTo(sUnderlineRect.left,  sUnderlineRect.bottom - 1);
        sMemDC.LineTo(sUnderlineRect.right, sUnderlineRect.bottom - 1);
    }

    sMemDC.SelectObject(sOldPen);

    sPaintDC.BitBlt(0, 0, sClientRect.Width(), sClientRect.Height(), &sMemDC, 0, 0, SRCCOPY);

    sMemDC.SelectObject(sOldBitmap);
}

xpr_bool_t TabCtrl::OnEraseBkgnd(CDC *aDC)
{
    return XPR_TRUE;
    return super::OnEraseBkgnd(aDC);
}

void TabCtrl::createFont(void)
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

void TabCtrl::recreateFont(void)
{
    destroyFont();
    createFont();
}

void TabCtrl::destroyFont(void)
{
    if (XPR_IS_NOT_NULL(mFont.m_hObject)) mFont.DeleteObject();
    if (XPR_IS_NOT_NULL(mBoldFont.m_hObject)) mBoldFont.DeleteObject();
}

void TabCtrl::OnSettingChange(xpr_uint_t aFlags, const xpr_tchar_t *aSection)
{
    recreateFont();

    Invalidate();

    super::OnSettingChange(aFlags, aSection);
}

void TabCtrl::OnLButtonDblClk(xpr_uint_t aFlags, CPoint aPoint)
{
    if (XPR_IS_NOT_NULL(mObserver))
    {
        xpr_size_t sHoverTab = hitTest(aPoint);

        mObserver->onTabDoubleClicked(*this, sHoverTab);
    }

    super::OnLButtonDblClk(aFlags, aPoint);
}

void TabCtrl::OnLButtonDown(xpr_uint_t aFlags, CPoint aPoint)
{
    xpr_size_t sHoverTab = hitTest(aPoint);
    if (sHoverTab != InvalidTab)
    {
        setCurTab(sHoverTab);
    }

    super::OnLButtonDown(aFlags, aPoint);
}

void TabCtrl::OnMouseMove(xpr_uint_t aFlags, CPoint aPoint)
{
    xpr_size_t sHoverTab = hitTest(aPoint);

    if (XPR_IS_FALSE(mSetCapture))
    {
        if (sHoverTab != InvalidTab)
        {
            mHoverTab = sHoverTab;
            mSetCapture = XPR_TRUE;
        }

        if (XPR_IS_TRUE(mSetCapture))
        {
            SetCapture();
            Invalidate(XPR_FALSE);
        }
    }
    else
    {
        if (sHoverTab != InvalidTab)
        {
            if (mHoverTab != sHoverTab)
            {
                mHoverTab = sHoverTab;
                Invalidate(XPR_FALSE);
            }
        }
        else
        {
            mHoverTab = InvalidTab;
            mSetCapture = XPR_FALSE;

            ReleaseCapture();
            Invalidate(XPR_FALSE);
        }
    }

    super::OnMouseMove(aFlags, aPoint);
}

void TabCtrl::OnLButtonUp(xpr_uint_t aFlags, CPoint aPoint)
{

    super::OnLButtonUp(aFlags, aPoint);
}

void TabCtrl::OnCaptureChanged(CWnd *aWnd)
{
    if (XPR_IS_TRUE(mSetCapture))
    {
        mHoverTab = InvalidTab;
        mSetCapture = XPR_FALSE;

        Invalidate(XPR_FALSE);
    }

    super::OnCaptureChanged(aWnd);
}

void TabCtrl::OnContextMenu(CWnd *aWnd, CPoint aPoint)
{
    CPoint sClientPoint(aPoint);
    ScreenToClient(&sClientPoint);

    if (XPR_IS_NOT_NULL(mObserver))
    {
        xpr_size_t aTab = hitTest(sClientPoint);

        mObserver->onTabContextMenu(*this, aTab, aPoint);
        return;
    }

    super::OnContextMenu(aWnd, aPoint);
}
