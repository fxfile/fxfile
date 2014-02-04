//
// Copyright (c) 2001-2014 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "TabCtrl.h"

#include "TabCtrlObserver.h"
#include "gdi.h"
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static const xpr_sint_t kDefaultTabSize     = 150;
static const CRect      kTabOffset          = CRect(3, 2, 3, 0);
static const CRect      kTextOffset         = CRect(5, 2, 5, 2);
static const xpr_sint_t kIconOffset         = 5;
static const xpr_sint_t kDragDist           = 8;
static const xpr_sint_t kBarLineHeight      = 1;
static const xpr_sint_t kInactivedTabOffset = 2;
static const xpr_uint_t kNewTabToolTipId    = 0xfffffffe;

static const xpr_tchar_t kClassName[] = XPR_STRING_LITERAL("TabCtrl");

struct TabCtrl::TabItem
{
    xpr::tstring  mText;
    void         *mData;
    CRect         mTabRect;
    CRect         mCloseRect;
    xpr_sint_t    mImageIndex;
};

TabCtrl::TabCtrl(void)
    : mObserver(XPR_NULL)
    , mCurTab(InvalidTab)
    , mFixedSizeMode(XPR_FALSE), mFixedSize(kDefaultTabSize), mTabMinSize(0), mTabMaxSize(ksint32max)
    , mImageList(XPR_NULL)
    , mShowNewButton(XPR_FALSE), mNewButtonIcon(XPR_NULL), mNewButtonRect(CRect(0,0,0,0)), mNewButtonHover(XPR_FALSE), mNewButtonPressed(XPR_FALSE), mNewButtonPressedLeave(XPR_FALSE)
    , mSetCapture(XPR_FALSE), mTabHover(InvalidTab)
    , mDragMove(XPR_FALSE), mDragTab(InvalidTab), mDragBegunPoint(0,0), mDragBegun(XPR_FALSE), mDragBegunTab(InvalidTab)
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
    ON_WM_SETFOCUS()
END_MESSAGE_MAP()

xpr_sint_t TabCtrl::OnCreate(LPCREATESTRUCT aCreateStruct)
{
    if (super::OnCreate(aCreateStruct) == -1)
        return -1;

    mXPTheme.Open(m_hWnd, XPR_WIDE_STRING_LITERAL("TAB"));

    createFont();

    EnableToolTips();

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
    if (mFixedSizeMode == aFixedSizeMode && mFixedSize == aFixedSize)
        return;

    if (aFixedSize <= 0)
        aFixedSize = kDefaultTabSize;

    mFixedSizeMode = aFixedSizeMode;
    mFixedSize = aFixedSize;

    recalcLayout();
}

void TabCtrl::enableDragMove(xpr_bool_t aDragMove)
{
    mDragMove = aDragMove;
}

void TabCtrl::showNewButton(xpr_bool_t aShowNewButton, const xpr_tchar_t *aToolTipText)
{
    mShowNewButton = aShowNewButton;

    mNewButtonToolTipText.clear();
    if (XPR_IS_NOT_NULL(aToolTipText))
    {
        mNewButtonToolTipText = aToolTipText;
    }

    recalcLayout();
}

void TabCtrl::setTabIcon(HICON aNewButtonIcon)
{
    mNewButtonIcon = aNewButtonIcon;
}

xpr_size_t TabCtrl::addTab(const xpr_tchar_t *aText, xpr_sint_t aImageIndex, void *aData)
{
    return insertTab(InvalidTab, aText, aImageIndex, aData);
}

xpr_size_t TabCtrl::insertTab(xpr_size_t aTab, const xpr_tchar_t *aText, xpr_sint_t aImageIndex, void *aData)
{
    xpr_size_t sTab = aTab;

    if (sTab > InvalidTab)
        return InvalidTab;

    if (sTab != InvalidTab)
    {
        xpr_size_t sTabCount = mTabDeque.size();
        if (XPR_IS_OUT_OF_RANGE(0, sTab, sTabCount))
            return InvalidTab;

        if (sTab == sTabCount)
            sTab = InvalidTab;
    }

    TabItem *sTabItem = new TabItem;
    if (XPR_IS_NULL(sTabItem))
        return InvalidTab;

    if (XPR_IS_NOT_NULL(aText))
        sTabItem->mText = aText;

    sTabItem->mData       = aData;
    sTabItem->mImageIndex = aImageIndex;

    return insertTabItem(sTab, sTabItem);
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
    if (FXFILE_STL_IS_INDEXABLE(aTab, mTabDeque))
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

xpr_bool_t TabCtrl::getTabText(xpr_size_t aTab, xpr::tstring &aText) const
{
    TabItem *sTabItem = getTabItem(aTab);
    if (XPR_IS_NULL(sTabItem))
        return XPR_FALSE;

    aText = sTabItem->mText;

    return XPR_TRUE;
}

void *TabCtrl::getTabData(xpr_size_t aTab) const
{
    TabItem *sTabItem = getTabItem(aTab);
    if (XPR_IS_NULL(sTabItem))
        return XPR_NULL;

    return sTabItem->mData;
}

xpr_bool_t TabCtrl::getNewButtonRect(CRect &aNewButtonRect) const
{
    if (XPR_IS_FALSE(mShowNewButton))
        return XPR_FALSE;

    aNewButtonRect = mNewButtonRect;

    return XPR_TRUE;
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
    if (!FXFILE_STL_IS_INDEXABLE(aTab, mTabDeque))
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

xpr_bool_t TabCtrl::setTab(xpr_size_t aTab, const xpr_tchar_t *aText, xpr_sint_t aImageIndex)
{
    TabItem *sTabItem = getTabItem(aTab);
    if (XPR_IS_NULL(sTabItem))
        return XPR_FALSE;

    sTabItem->mText.clear();

    if (XPR_IS_NOT_NULL(aText))
    {
        sTabItem->mText = aText;
    }

    sTabItem->mImageIndex = aImageIndex;

    recalcLayout();

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

xpr_bool_t TabCtrl::setTabImage(xpr_size_t aTab, xpr_sint_t aImageIndex)
{
    TabItem *sTabItem = getTabItem(aTab);
    if (XPR_IS_NULL(sTabItem))
        return XPR_FALSE;

    sTabItem->mImageIndex = aImageIndex;

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

xpr_bool_t TabCtrl::moveTab(xpr_size_t aFromTab, xpr_size_t aToTab)
{
    if (aFromTab == aToTab)
        return XPR_TRUE;

    if (!FXFILE_STL_IS_INDEXABLE(aFromTab, mTabDeque))
        return XPR_FALSE;

    TabDeque::iterator sIterator = mTabDeque.begin() + aFromTab;
    if (sIterator == mTabDeque.end())
        return XPR_FALSE;

    TabItem *sTabItem = getTabItem(aFromTab);

    mTabDeque.erase(sIterator);

    if (insertTabItem(aToTab, sTabItem, XPR_FALSE) == InvalidTab)
    {
        return XPR_FALSE;
    }

    return XPR_TRUE;
}

xpr_bool_t TabCtrl::removeTab(xpr_size_t aTab)
{
    if (!FXFILE_STL_IS_INDEXABLE(aTab, mTabDeque))
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

    sWidth -= kTabOffset.left;

    SIZE sNewButtonIconSize = {0};
    if (XPR_IS_TRUE(mShowNewButton))
    {
        getIconInfo(mNewButtonIcon, &sNewButtonIconSize);

        sWidth -= sNewButtonIconSize.cx + kIconOffset * 2;
    }
    else
    {
        sWidth -= kTabOffset.right;
    }

    xpr_sint_t sIconWidth = 0;
    xpr_sint_t sIconHeight = 0;
    xpr_sint_t sImageListCount = 0;
    if (XPR_IS_NOT_NULL(mImageList))
    {
        ImageList_GetIconSize(mImageList->m_hImageList, &sIconWidth, &sIconHeight);
        sImageListCount = mImageList->GetImageCount();
    }

    xpr_size_t  i;
    xpr_sint_t  sLeft   = kTabOffset.left;
    xpr_sint_t  sTop    = kTabOffset.top;
    xpr_sint_t  sBottom = sHeight - kBarLineHeight;
    TabItem    *sTabItem;
    CRect       sLastTabRect(0, sTop, 0, sBottom);
    TabDeque::iterator sIterator;

    if (XPR_IS_TRUE(mFixedSizeMode))
    {
        xpr_double_t sAvgTabSize = (xpr_double_t)sWidth / (xpr_double_t)mTabDeque.size();
        if (sAvgTabSize >= kDefaultTabSize)
            sAvgTabSize = kDefaultTabSize;

        sIterator = mTabDeque.begin();
        for (i = 0; sIterator != mTabDeque.end(); ++sIterator, ++i)
        {
            sTabItem = *sIterator;

            XPR_ASSERT(sTabItem != XPR_NULL);

            sTabItem->mTabRect.SetRectEmpty();

            sTabItem->mTabRect.left   = sLeft + (xpr_slong_t)(sAvgTabSize * (xpr_double_t)(i + 0));
            sTabItem->mTabRect.right  = sLeft + (xpr_slong_t)(sAvgTabSize * (xpr_double_t)(i + 1));
            sTabItem->mTabRect.top    = sTop;
            sTabItem->mTabRect.bottom = sBottom;

            sLastTabRect = sTabItem->mTabRect;
        }
    }
    else
    {
        CClientDC    sDC(this);
        CFont       *sOldFont;
        CRect        sTabTextRect;
        xpr_slong_t  sAutoFitWidth = 0;
        xpr_slong_t  sTotalAutoFitWidth = 0;
        xpr_slong_t  sTabLeft = sLeft;
        xpr_double_t sFitRatio = 1.0f;
        xpr_double_t sAutoFitFloatWidth;
        xpr_double_t sAccumulatedAutoFitFloatWidth = 0;
        xpr_slong_t  sAccumulatedAutoFitIntegerWidth = 0;

        // calcualte tab text width
        sIterator = mTabDeque.begin();
        for (i = 0; sIterator != mTabDeque.end(); ++sIterator, ++i)
        {
            sTabItem = *sIterator;

            XPR_ASSERT(sTabItem != XPR_NULL);

            sTabItem->mTabRect.SetRectEmpty();

            if (i == mCurTab)
                sOldFont = sDC.SelectObject(&mBoldFont);
            else
                sOldFont = sDC.SelectObject(&mFont);

            sTabTextRect.SetRect(0, 0, 0, 0);

            sDC.DrawText(
                sTabItem->mText.c_str(),
                (xpr_sint_t)sTabItem->mText.length(),
                sTabTextRect,
                DT_CALCRECT | DT_LEFT | DT_VCENTER | DT_SINGLELINE);

            sTabTextRect.right += kTextOffset.left + kTextOffset.right;

            if (XPR_IS_RANGE(0, sTabItem->mImageIndex, sImageListCount - 1))
            {
                sTabTextRect.right += sIconWidth + kIconOffset;
            }

            sDC.SelectObject(sOldFont);

            sAutoFitWidth = sTabTextRect.Width();

            sTabItem->mTabRect.left = sAutoFitWidth;

            sTotalAutoFitWidth += sAutoFitWidth;
        }

        // calcualte auto fit tab width
        if (sTotalAutoFitWidth > sWidth)
        {
            sFitRatio = (xpr_double_t)sWidth / (xpr_double_t)sTotalAutoFitWidth;
        }

        sIterator = mTabDeque.begin();
        for (i = 0; sIterator != mTabDeque.end(); ++sIterator, ++i)
        {
            sTabItem = *sIterator;

            XPR_ASSERT(sTabItem != XPR_NULL);

            sAutoFitFloatWidth = (xpr_double_t)sTabItem->mTabRect.left * sFitRatio;

            sAutoFitWidth = (xpr_slong_t)(sAutoFitFloatWidth + (sAccumulatedAutoFitFloatWidth - sAccumulatedAutoFitIntegerWidth + 0.5f));

            sAccumulatedAutoFitFloatWidth   += sAutoFitFloatWidth;
            sAccumulatedAutoFitIntegerWidth += sAutoFitWidth;

            sTabItem->mTabRect.SetRectEmpty();

            sTabItem->mTabRect.left   = sTabLeft;
            sTabItem->mTabRect.right  = sTabLeft + sAutoFitWidth;
            sTabItem->mTabRect.top    = sTop;
            sTabItem->mTabRect.bottom = sBottom;

            sLastTabRect = sTabItem->mTabRect;

            sTabLeft += sAutoFitWidth;
        }
    }

    // calcuate new tab button
    if (XPR_IS_TRUE(mShowNewButton))
    {
        mNewButtonRect.left   = sLastTabRect.right + kIconOffset;
        mNewButtonRect.top    = sLastTabRect.top + (sLastTabRect.Height() + kInactivedTabOffset - sNewButtonIconSize.cy) / 2;
        mNewButtonRect.right  = mNewButtonRect.left + sNewButtonIconSize.cx;
        mNewButtonRect.bottom = mNewButtonRect.top + sNewButtonIconSize.cy;
    }

    if (XPR_IS_TRUE(aRedraw))
        Invalidate(XPR_FALSE);
}

void TabCtrl::OnPaint(void)
{
    CPaintDC sPaintDC(this); // device context for painting

    CRect sClientRect;
    GetClientRect(&sClientRect);

    COLORREF sFaceColor   = ::GetSysColor(COLOR_3DFACE);
    COLORREF sShadowColor = ::GetSysColor(COLOR_3DSHADOW);
    COLORREF sTextColor   = ::GetSysColor(COLOR_BTNTEXT);

    CDC sMemDC;
    sMemDC.CreateCompatibleDC(&sPaintDC);
    sMemDC.SetBkMode(TRANSPARENT);
    sMemDC.SetTextColor(sTextColor);

    CBitmap sBitmap;
    sBitmap.CreateCompatibleBitmap(&sPaintDC, sClientRect.Width(), sClientRect.Height());

    CBitmap *sOldBitmap = sMemDC.SelectObject(&sBitmap);

    // draw background
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

    // draw tab button
    xpr_sint_t  i;
    TabItem    *sTabItem;
    CFont      *sOldFont;
    CRect       sTabRect, sCurTabButtonRect;
    CPen        sPen(PS_SOLID, 1, sShadowColor);
    CPen       *sOldPen;
    xpr_sint_t  sIconWidth = 0, sIconHeight = 0, sImageListCount = 0;
    TabDeque::iterator sIterator;

    if (XPR_IS_NOT_NULL(mImageList))
    {
        ImageList_GetIconSize(mImageList->m_hImageList, &sIconWidth, &sIconHeight);
        sImageListCount = mImageList->GetImageCount();
    }

    sOldPen = sMemDC.SelectObject(&sPen);

    sIterator = mTabDeque.begin();
    for (i = 0; sIterator != mTabDeque.end(); ++sIterator, ++i)
    {
        sTabItem = *sIterator;
        if (XPR_IS_NULL(sTabItem))
            continue;

        sTabRect = sTabItem->mTabRect;

        CRect sTabButtonRect(sTabRect);
        if (i == mCurTab || i == mTabHover)
        {
            if (mXPTheme.IsAppThemed() == XPR_TRUE)
            {
                sTabButtonRect.InflateRect(0, 0, 1, 2);

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
                sTabButtonRect.DeflateRect(0, kInactivedTabOffset, -1, 0);

                mXPTheme.DrawBackground(sMemDC, TABP_TABITEM, TIS_NORMAL, &sTabButtonRect, XPR_NULL);
            }
            else
            {
                sTabButtonRect.DeflateRect(0, kInactivedTabOffset, -1, 0);

                sMemDC.FillSolidRect(sTabButtonRect, sFaceColor);

                sMemDC.MoveTo(sTabButtonRect.left,  sTabButtonRect.bottom);
                sMemDC.LineTo(sTabButtonRect.left,  sTabButtonRect.top);
                sMemDC.LineTo(sTabButtonRect.right, sTabButtonRect.top);
                sMemDC.LineTo(sTabButtonRect.right, sTabButtonRect.bottom);
            }
        }

        CRect sTabTextRect(sTabButtonRect);
        sTabTextRect.DeflateRect(kTextOffset);

        if (XPR_IS_NOT_NULL(mImageList))
        {
            if (XPR_IS_RANGE(0, sTabItem->mImageIndex, sImageListCount - 1))
            {
                CRect sTabIconRect(sTabTextRect);
                sTabIconRect.right   = sTabIconRect.left + sIconWidth;
                sTabIconRect.top     = sTabIconRect.top + (sTabIconRect.Height() - sIconHeight) / 2;
                sTabIconRect.bottom  = sTabIconRect.top + sIconHeight;

                mImageList->Draw(&sMemDC,
                                 sTabItem->mImageIndex,
                                 sTabIconRect.TopLeft(),
                                 ILD_NORMAL | ILD_TRANSPARENT);

                sTabTextRect.left += sIconWidth + kIconOffset;
            }
        }

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

    // draw new tab button
    if (XPR_IS_TRUE(mShowNewButton))
    {
        CPoint sNewButtonPoint(mNewButtonRect.TopLeft());
        if (XPR_IS_TRUE(mNewButtonPressed))
        {
            if (XPR_IS_TRUE(mNewButtonPressedLeave))
            {
                --sNewButtonPoint.x;
                --sNewButtonPoint.y;
            }
        }
        else if (XPR_IS_TRUE(mNewButtonHover))
        {
            --sNewButtonPoint.x;
            --sNewButtonPoint.y;
        }

        SIZE sNewButtonIconSize = {0};
        getIconInfo(mNewButtonIcon, &sNewButtonIconSize);

        ::DrawIconEx(sMemDC,
                     sNewButtonPoint.x,
                     sNewButtonPoint.y,
                     mNewButtonIcon,
                     sNewButtonIconSize.cx,
                     sNewButtonIconSize.cy,
                     0,
                     XPR_NULL,
                     DI_NORMAL);
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
        xpr_size_t sTabHover = hitTest(aPoint);

        mObserver->onTabDoubleClicked(*this, sTabHover);
    }

    super::OnLButtonDblClk(aFlags, aPoint);
}

void TabCtrl::OnLButtonDown(xpr_uint_t aFlags, CPoint aPoint)
{
    xpr_size_t sTabHover       = hitTest(aPoint);
    xpr_bool_t sNewButtonHover = mNewButtonRect.PtInRect(aPoint);

    if (sTabHover != InvalidTab)
    {
        setCurTab(sTabHover);

        if (XPR_IS_TRUE(mDragMove))
        {
            mTabHover       = InvalidTab;
            mDragBegun      = XPR_TRUE;
            mDragBegunTab   = sTabHover;
            mDragBegunPoint = aPoint;
            mDragTab        = InvalidTab;

            SetCapture();
            mSetCapture = XPR_TRUE;
        }
    }
    else if (XPR_IS_TRUE(sNewButtonHover))
    {
        mNewButtonHover        = XPR_TRUE;
        mNewButtonPressed      = XPR_TRUE;
        mNewButtonPressedLeave = XPR_FALSE;

        SetCapture();
        mSetCapture = XPR_TRUE;
        InvalidateRect(&mNewButtonRect, XPR_FALSE);
    }

    super::OnLButtonDown(aFlags, aPoint);
}

void TabCtrl::OnMouseMove(xpr_uint_t aFlags, CPoint aPoint)
{
    xpr_size_t sTabHover       = hitTest(aPoint);
    xpr_bool_t sNewButtonHover = mNewButtonRect.PtInRect(aPoint);

    if (XPR_IS_FALSE(mDragBegun))
    {
        if (XPR_IS_FALSE(mSetCapture))
        {
            if (sTabHover != InvalidTab)
            {
                mTabHover = sTabHover;
                mSetCapture = XPR_TRUE;
                Invalidate(XPR_FALSE);
            }
            else if (XPR_IS_TRUE(sNewButtonHover))
            {
                mNewButtonHover = sNewButtonHover;
                if (XPR_IS_TRUE(mNewButtonPressed))
                    mNewButtonPressedLeave = XPR_FALSE;

                mSetCapture = XPR_TRUE;
                InvalidateRect(&mNewButtonRect, XPR_FALSE);
            }

            if (XPR_IS_TRUE(mSetCapture))
            {
                SetCapture();
            }
        }
        else
        {
            if (XPR_IS_TRUE(mNewButtonPressed))
            {
                if (mNewButtonHover != sNewButtonHover)
                {
                    mNewButtonHover = sNewButtonHover;
                    mNewButtonPressedLeave = !sNewButtonHover;
                    InvalidateRect(&mNewButtonRect, XPR_FALSE);
                }
            }
            else
            {
                if (mNewButtonHover != sNewButtonHover)
                {
                    mNewButtonHover = sNewButtonHover;
                    InvalidateRect(&mNewButtonRect, XPR_FALSE);
                }

                if (sTabHover != InvalidTab)
                {
                    if (mTabHover != sTabHover)
                    {
                        mTabHover = sTabHover;
                        Invalidate(XPR_FALSE);
                    }
                }

                if (sTabHover == InvalidTab && XPR_IS_FALSE(mNewButtonHover))
                {
                    if (XPR_IS_FALSE(mNewButtonPressed))
                    {
                        mTabHover = InvalidTab;

                        ReleaseCapture();
                        mSetCapture = XPR_FALSE;
                        Invalidate(XPR_FALSE);
                    }
                }
            }
        }
    }
    else
    {
        if (mDragTab == InvalidTab)
        {
            xpr_sint_t sXDist = mDragBegunPoint.x - aPoint.x;
            xpr_sint_t sYDist = mDragBegunPoint.y - aPoint.y;
            xpr_sint_t sRoundDist = (xpr_sint_t)sqrt((xpr_float_t)(sXDist * sXDist + sYDist * sYDist));

            if (sRoundDist > kDragDist)
            {
                mDragTab = mDragBegunTab;
            }
        }
        else
        {
            if (mTabDeque.empty() == false)
            {
                TabItem *sLastTabItem = *mTabDeque.rbegin();

                CPoint sDragPoint(aPoint.x);
                sDragPoint.x = XPR_MAX(sDragPoint.x, kTabOffset.left);
                sDragPoint.x = XPR_MIN(sDragPoint.x, sLastTabItem->mTabRect.right - 1);
                sDragPoint.y = XPR_MAX(sDragPoint.y, kTabOffset.top);
                sDragPoint.y = XPR_MIN(sDragPoint.y, sLastTabItem->mTabRect.bottom - 1);

                xpr_size_t sDragTab = hitTest(sDragPoint);

                if (sDragTab != InvalidTab)
                {
                    if (mDragTab != sDragTab)
                    {
                        moveTab(mDragTab, sDragTab);
                        setCurTab(sDragTab);

                        mDragTab = sDragTab;
                    }
                }
            }
        }
    }

    super::OnMouseMove(aFlags, aPoint);
}

void TabCtrl::OnLButtonUp(xpr_uint_t aFlags, CPoint aPoint)
{
    xpr_bool_t sNewButtonHover = mNewButtonRect.PtInRect(aPoint);

    if (XPR_IS_TRUE(mSetCapture))
    {
        mSetCapture = XPR_FALSE;
        ReleaseCapture();

        if (XPR_IS_TRUE(mNewButtonHover) && XPR_IS_TRUE(mNewButtonPressed))
        {
            if (XPR_IS_NOT_NULL(mObserver))
            {
                mObserver->onTabNewButton(*this);
            }
        }

        mDragBegun    = XPR_FALSE;
        mDragBegunTab = InvalidTab;
        mDragTab      = InvalidTab;

        mNewButtonHover        = XPR_FALSE;
        mNewButtonPressed      = XPR_FALSE;
        mNewButtonPressedLeave = XPR_FALSE;

        Invalidate(XPR_FALSE);
    }

    super::OnLButtonUp(aFlags, aPoint);
}

void TabCtrl::OnCaptureChanged(CWnd *aWnd)
{
    if (aWnd != this)
    {
        if (XPR_IS_TRUE(mSetCapture))
        {
            mTabHover     = InvalidTab;
            mDragBegun    = XPR_FALSE;
            mDragBegunTab = InvalidTab;
            mDragTab      = InvalidTab;

            mNewButtonHover        = XPR_FALSE;
            mNewButtonPressed      = XPR_FALSE;
            mNewButtonPressedLeave = XPR_FALSE;

            mSetCapture = XPR_FALSE;
            Invalidate(XPR_FALSE);
        }
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

void TabCtrl::OnSetFocus(CWnd *aOldWnd)
{
    super::OnSetFocus(aOldWnd);

    if (XPR_IS_NOT_NULL(mObserver))
    {
        mObserver->onSetFocus(*this);
    }
}

INT_PTR TabCtrl::OnToolHitTest(CPoint aPoint, TOOLINFO *aToolInfo) const
{
    XPR_ASSERT(aToolInfo != XPR_NULL);

    xpr_size_t sTab = hitTest(aPoint);
    if (sTab != InvalidTab)
    {
        TabItem *sTabItem = getTabItem(sTab);

        XPR_ASSERT(sTabItem != XPR_NULL);

        aToolInfo->hwnd     = *this;
        aToolInfo->uId      = sTab;
        aToolInfo->rect     = sTabItem->mTabRect;
        aToolInfo->lpszText = LPSTR_TEXTCALLBACK;
    }
    else if (mNewButtonRect.PtInRect(aPoint) == XPR_TRUE)
    {
        aToolInfo->hwnd     = *this;
        aToolInfo->uId      = kNewTabToolTipId;
        aToolInfo->rect     = mNewButtonRect;
        aToolInfo->lpszText = LPSTR_TEXTCALLBACK;
    }
    else
    {
        aToolInfo->uId      = -1;
        aToolInfo->lpszText = XPR_NULL;
    }

    return aToolInfo->uId;
}

xpr_bool_t TabCtrl::OnNotify(WPARAM aWParam, LPARAM aLParam, LRESULT *aResult)
{
    if (((LPNMHDR)aLParam)->code == TTN_GETDISPINFO)
    {
        LPNMTTDISPINFO sNMTTDispInfo = (LPNMTTDISPINFO)aLParam;

        if (sNMTTDispInfo->hdr.idFrom == kNewTabToolTipId)
        {
            sNMTTDispInfo->lpszText = (xpr_tchar_t *)mNewButtonToolTipText.c_str();
        }
        else
        {
            xpr_size_t sTab = (xpr_size_t)sNMTTDispInfo->hdr.idFrom;

            if (sTab != InvalidTab)
            {
                TabItem *sTabItem = getTabItem(sTab);

                XPR_ASSERT(sTabItem != XPR_NULL);

                sNMTTDispInfo->lpszText = (xpr_tchar_t *)sTabItem->mText.c_str();
            }
            else
            {
                sNMTTDispInfo->lpszText = XPR_NULL;
            }
        }
    }

    return super::OnNotify(aWParam, aLParam, aResult);
}

LRESULT TabCtrl::WindowProc(xpr_uint_t aMsg, WPARAM aWParam, LPARAM aLParam)
{
    switch (aMsg)
    {
    case WM_LBUTTONDOWN:
    case WM_MBUTTONDOWN:
    case WM_RBUTTONDOWN:
    case WM_XBUTTONDOWN:
        SendMessage(WM_ACTIVATE, WA_CLICKACTIVE); // activate on mouse capture
        break;
    }

    return super::WindowProc(aMsg, aWParam, aLParam);
}
