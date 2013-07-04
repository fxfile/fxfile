//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "splitter.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

namespace fxfile
{
struct Splitter::Pane
{
    xpr_sint_t  mRow;
    xpr_sint_t  mColumn;

    CWnd       *mPaneWnd;
    CSize       mPaneSize;
    CRect       mPaneRect;

    CRect       mHorzSplitterRect;
    CRect       mVertSplitterRect;
};

struct Splitter::Track
{
    HWND       mHwnd;

    xpr_sint_t mSplitterIndex;
    xpr_bool_t mHorzSplitter;
    xpr_sint_t mRow;
    xpr_sint_t mColumn;

    xpr_sint_t mCurPos;
    xpr_sint_t mCapturedCoord;
    xpr_bool_t mPressed;
    xpr_bool_t mHover;
    xpr_bool_t mPressedLeave;
    xpr_bool_t mSetCapture;
};

Splitter::Splitter(void)
    : mPanes(XPR_NULL)
    , mMaxRowCount(0), mMaxColumnCount(0)
    , mSplitSize(0)
    , mRowCount(0), mColumnCount(0)
    , mWindowRect(CRect(0, 0, 0, 0))
    , mActivedPaneRow(-1), mActivedPaneColumn(-1)
    , mLastActivedPaneRow(-1), mLastActivedPaneColumn(-1)
    , mTrack(XPR_NULL)
    , mMinPaneSize(0), mMaxPaneSize(ksintmax)
{
}

Splitter::~Splitter(void)
{
    clear();
}

void Splitter::clear(void)
{
    xpr_sint_t i;
    for (i = 0; i < mMaxRowCount; ++i)
    {
        XPR_SAFE_DELETE_ARRAY(mPanes[i]);
    }

    XPR_SAFE_DELETE_ARRAY(mPanes);
    XPR_SAFE_DELETE(mTrack);
}

void Splitter::setObserver(SplitterObserver *aObserver)
{
    mObserver = aObserver;
}

void Splitter::setMaxSplitCount(xpr_sint_t aMaxRowCount, xpr_sint_t aMaxColumnCount)
{
    if (aMaxRowCount <= 0 || aMaxColumnCount <= 0)
        return;

    if (mMaxRowCount == aMaxRowCount && mMaxColumnCount == aMaxColumnCount)
        return;

    mMaxRowCount    = aMaxRowCount;
    mMaxColumnCount = aMaxColumnCount;

    xpr_sint_t i, j;

    mPanes = new Pane *[mMaxRowCount];
    for (i = 0; i < mMaxRowCount; ++i)
    {
        mPanes[i] = new Pane[mMaxColumnCount];

        for (j = 0; j < mMaxColumnCount; ++j)
        {
            mPanes[i][j].mRow      = i;
            mPanes[i][j].mColumn   = j;

            mPanes[i][j].mPaneWnd  = XPR_NULL;
            mPanes[i][j].mPaneSize = CSize(0, 0);
            mPanes[i][j].mPaneRect = CRect(0, 0, 0, 0);
        }
    }
}

void Splitter::setSplitSize(xpr_sint_t aSplitSize)
{
    mSplitSize = aSplitSize;
}

xpr_sint_t Splitter::getSplitSize(void) const
{
    return mSplitSize;
}

void Splitter::getMaxSplitCount(xpr_sint_t &aMaxRowCount, xpr_sint_t &aMaxColumnCount) const
{
    aMaxRowCount    = mMaxRowCount;
    aMaxColumnCount = mMaxColumnCount;
}

void Splitter::split(xpr_sint_t aRowCount, xpr_sint_t aColumnCount)
{
    if (aRowCount == mRowCount && aColumnCount == mColumnCount)
        return;

    if (aRowCount > mMaxRowCount || aColumnCount > mMaxColumnCount)
        return;

    mRowCount    = aRowCount;
    mColumnCount = aColumnCount;

    xpr_sint_t i, j;
    Pane *sPane;

    for (i = 0; i < mMaxRowCount; ++i)
    {
        for (j = 0; j < mMaxColumnCount; ++j)
        {
            sPane = mPanes[i] + j;

            if (i >= mRowCount || j >= mColumnCount)
            {
                if (XPR_IS_NOT_NULL(sPane->mPaneWnd))
                {
                    if (XPR_IS_NOT_NULL(mObserver))
                    {
                        mObserver->onSplitterPaneDestroy(*this, sPane->mPaneWnd);
                    }

                    sPane->mPaneSize = CSize(0, 0);
                    sPane->mPaneRect = CRect(0, 0, 0, 0);
                    sPane->mPaneWnd  = XPR_NULL;
                }
            }
            else
            {
                if (XPR_IS_NULL(sPane->mPaneWnd))
                {
                    if (XPR_IS_NOT_NULL(mObserver))
                    {
                        sPane->mPaneWnd = mObserver->onSplitterPaneCreate(*this, i, j);
                    }
                }
            }
        }
    }

    if (evaluateLastActivedPane(mActivedPaneRow, mActivedPaneColumn) == XPR_TRUE)
    {
        if (XPR_IS_NOT_NULL(mObserver))
        {
            mObserver->onSplitterActivedPane(*this, mActivedPaneRow, mActivedPaneColumn);
        }
    }

    //if (XPR_IS_TRUE(aActivate))
    //{
    //    if (mActivedPaneRow <= mRowCount || mActivedPaneColumn <= mColumnCount)
    //        setActivePane(0, 0);
    //}
}

void Splitter::switchPane(xpr_sint_t aOldRow, xpr_sint_t aOldColumn, xpr_sint_t aNewRow, xpr_sint_t aNewColumn)
{
    if (!XPR_IS_RANGE(0, aOldRow, mMaxRowCount - 1) || !XPR_IS_RANGE(0, aOldColumn, mMaxColumnCount - 1) ||
        !XPR_IS_RANGE(0, aNewRow, mMaxRowCount - 1) || !XPR_IS_RANGE(0, aNewColumn, mMaxColumnCount - 1))
        return;

    if (aOldRow == aNewRow && aOldColumn == aNewColumn)
        return;

    Pane sTempPane;
    memcpy(&sTempPane, &mPanes[aOldRow][aOldColumn], sizeof(Pane));
    memcpy(&mPanes[aOldRow][aOldColumn], &mPanes[aNewRow][aNewColumn], sizeof(Pane));
    memcpy(&mPanes[aNewRow][aNewColumn], &sTempPane, sizeof(Pane));
}

xpr_sint_t Splitter::getPaneCount(xpr_sint_t *aRowCount, xpr_sint_t *aColumnCount) const
{
    if (XPR_IS_NOT_NULL(aRowCount   )) *aRowCount    = mRowCount;
    if (XPR_IS_NOT_NULL(aColumnCount)) *aColumnCount = mColumnCount;

    return mRowCount * mColumnCount;
}

xpr_sint_t Splitter::getRowCount(void) const
{
    return mRowCount;
}

xpr_sint_t Splitter::getColumnCount(void) const
{
    return mColumnCount;
}

xpr_sint_t Splitter::getSplitterCount(void) const
{
    xpr_sint_t sSplitterCount = (mRowCount - 1) + (mColumnCount - 1);
    if (sSplitterCount < 0)
        sSplitterCount = 0;

    return sSplitterCount;
}

xpr_bool_t Splitter::getSplitterRect(xpr_sint_t aSplitterIndex, CRect &aSplitterRect, xpr_bool_t *aHorzSplitter, xpr_sint_t *aRow, xpr_sint_t *aColumn) const
{
    xpr_sint_t sRow    = 0;
    xpr_sint_t sColumn = 0;
    xpr_bool_t sHorzSplitter = XPR_FALSE;

    if (mColumnCount > 1)
    {
        sRow    = aSplitterIndex / (mColumnCount - 1);
        sColumn = aSplitterIndex % (mColumnCount - 1);
    }
    else
    {
        sHorzSplitter = XPR_TRUE;
    }

    if (sRow > 0)
    {
        sRow    = aSplitterIndex - (mColumnCount - 1);
        sColumn = 0;

        sHorzSplitter = XPR_TRUE;
    }

    Pane *sPane = getPane(sRow, sColumn);
    if (XPR_IS_NULL(sPane))
        return XPR_FALSE;

    if (XPR_IS_FALSE(sHorzSplitter))
        aSplitterRect = sPane->mVertSplitterRect;
    else
        aSplitterRect = sPane->mHorzSplitterRect;

    if (XPR_IS_NOT_NULL(aHorzSplitter))
        *aHorzSplitter = sHorzSplitter;

    if (XPR_IS_NOT_NULL(aRow   )) *aRow    = sRow;
    if (XPR_IS_NOT_NULL(aColumn)) *aColumn = sColumn;

    return XPR_TRUE;
}

void Splitter::setWindowRect(CRect aRect)
{
    mWindowRect = aRect;
}

void Splitter::getWindowRect(CRect &aRect) const
{
    aRect = mWindowRect;
}

void Splitter::moveRow(xpr_sint_t aRow, xpr_sint_t cy)
{
    if (!XPR_IS_RANGE(0, aRow, mRowCount - 1))
        return;

    xpr_sint_t i;
    for (i = 0; i < mColumnCount; ++i)
    {
        mPanes[aRow][i].mPaneSize.cy = cy;
    }
}

void Splitter::moveColumn(xpr_sint_t aColumn, xpr_sint_t cx)
{
    if (!XPR_IS_RANGE(0, aColumn, mColumnCount - 1))
        return;

    xpr_sint_t i;
    for (i = 0; i < mRowCount; ++i)
    {
        mPanes[i][aColumn].mPaneSize.cx = cx;
    }
}

void Splitter::resize(HDWP aHdwp)
{
    if (mRowCount <= 0 || mColumnCount <= 0)
        return;

    if (mWindowRect.Width() <= 0 || mWindowRect.Height() <= 0)
        return;

    xpr_sint_t i, j;
    xpr_sint_t sOldRight;
    xpr_sint_t sOldBottom;
    CRect sRect(mWindowRect);
    Pane *sPane;

    sOldRight = sOldBottom = 0;

    HDWP sHdwp = aHdwp;
    if (XPR_IS_NULL(aHdwp))
        sHdwp = ::BeginDeferWindowPos(mRowCount * mColumnCount);

    for (i = 0; i < mRowCount; ++i)
    {
        for (j = 0; j < mColumnCount; ++j)
        {
            sPane = mPanes[i] + j;

            if (j == 0)
                sRect.left = mWindowRect.left;
            else
                sRect.left = sOldRight + mSplitSize;

            if (i == 0)
                sRect.top = mWindowRect.top;
            else
                sRect.top = sOldBottom + mSplitSize;

            sRect.right  = sRect.left + sPane->mPaneSize.cx;
            sRect.bottom = sRect.top  + sPane->mPaneSize.cy;

            if (j == (mColumnCount - 1))
                sRect.right = mWindowRect.right;

            if (i == (mRowCount - 1))
                sRect.bottom = mWindowRect.bottom;

            sPane->mVertSplitterRect = mWindowRect;
            sPane->mVertSplitterRect.left  = sRect.right;
            sPane->mVertSplitterRect.right = sPane->mVertSplitterRect.left + mSplitSize;

            sPane->mHorzSplitterRect = mWindowRect;
            sPane->mHorzSplitterRect.top    = sRect.bottom;
            sPane->mHorzSplitterRect.bottom = sPane->mHorzSplitterRect.top + mSplitSize;

            sPane->mPaneSize.cx = sRect.Width();
            sPane->mPaneSize.cy = sRect.Height();

            sPane->mPaneRect = sRect;

            if (XPR_IS_NOT_NULL(sPane->mPaneWnd))
            {
                ::DeferWindowPos(sHdwp, *sPane->mPaneWnd, XPR_NULL, sRect.left, sRect.top, sRect.Width(), sRect.Height(), SWP_NOZORDER);
            }

            sOldRight = sRect.right;
        }

        sOldBottom = sRect.bottom;
    }

    if (XPR_IS_NULL(aHdwp))
        ::EndDeferWindowPos(sHdwp);
}

void Splitter::setPaneSizeLimit(xpr_sint_t sMinSize, xpr_sint_t sMaxSize)
{
    mMinPaneSize = sMinSize;
    mMaxPaneSize = sMaxSize;
}

void Splitter::setPaneSize(xpr_sint_t aRow, xpr_sint_t aColumn, CSize aPaneSize)
{
    Pane *sPane = getPane(aRow, aColumn);
    if (XPR_IS_NULL(sPane))
        return;

    sPane->mPaneSize = aPaneSize;
}

xpr_bool_t Splitter::getPaneSize(xpr_sint_t aRow, xpr_sint_t aColumn, CSize &aPaneSize) const
{
    Pane *sPane = getPane(aRow, aColumn);
    if (XPR_IS_NULL(sPane))
        return XPR_FALSE;

    aPaneSize = sPane->mPaneSize;

    return XPR_TRUE;
}

xpr_bool_t Splitter::getPaneRect(xpr_sint_t aRow, xpr_sint_t aColumn, CRect &aPaneRect) const
{
    Pane *sPane = getPane(aRow, aColumn);
    if (XPR_IS_NULL(sPane))
        return XPR_FALSE;

    aPaneRect = sPane->mPaneRect;

    return XPR_TRUE;
}

CWnd *Splitter::getPaneWnd(xpr_sint_t aRow, xpr_sint_t aColumn) const
{
    Pane *sPane = getPane(aRow, aColumn);
    if (XPR_IS_NULL(sPane))
        return XPR_FALSE;

    CWnd *sPaneWnd = sPane->mPaneWnd;

    return sPaneWnd;
}

xpr_bool_t Splitter::evaluateLastActivedPane(xpr_sint_t aNewRow, xpr_sint_t aNewColumn)
{
    if (!XPR_IS_RANGE(0, aNewRow, mRowCount - 1) ||
        !XPR_IS_RANGE(0, aNewColumn, mColumnCount - 1))
    {
        return XPR_FALSE;
    }

    if (!XPR_IS_RANGE(0, mLastActivedPaneRow, mRowCount - 1) ||
        !XPR_IS_RANGE(0, mLastActivedPaneColumn, mColumnCount - 1) ||
        (mActivedPaneRow == mLastActivedPaneRow && mActivedPaneColumn == mLastActivedPaneColumn))
    {
        mLastActivedPaneRow    = aNewRow;
        mLastActivedPaneColumn = aNewColumn + 1;

        if (mLastActivedPaneColumn >= mColumnCount)
        {
            ++mLastActivedPaneRow;
            mLastActivedPaneColumn = 0;

            if (mLastActivedPaneRow >= mRowCount)
            {
                mLastActivedPaneRow    = 0;
                mLastActivedPaneColumn = 0;
            }
        }

        return XPR_TRUE;
    }

    return XPR_FALSE;
}

xpr_bool_t Splitter::setActivePane(xpr_sint_t aRow, xpr_sint_t aColumn)
{
    Pane *sPane = getPane(aRow, aColumn);
    if (XPR_IS_NULL(sPane))
        return XPR_FALSE;

    if (aRow == mActivedPaneRow && aColumn == mActivedPaneColumn)
        return XPR_TRUE;

    // last actived pane
    mLastActivedPaneRow    = mActivedPaneRow;
    mLastActivedPaneColumn = mActivedPaneColumn;

    // new actived pane
    mActivedPaneRow    = aRow;
    mActivedPaneColumn = aColumn;

    CWnd *sPaneWnd = sPane->mPaneWnd;
    if (XPR_IS_NOT_NULL(sPaneWnd))
    {
        CView *sView = dynamic_cast<CView *>(sPaneWnd);
        CFrameWnd *sFrameWnd = dynamic_cast<CFrameWnd *>(sPaneWnd->GetParent());

        if (XPR_IS_NOT_NULL(sView) && XPR_IS_NOT_NULL(sFrameWnd))
            sFrameWnd->SetActiveView(sView);
        else
            sPaneWnd->SetFocus();
    }

    // evaluate last actived pane
    evaluateLastActivedPane(aRow, aColumn);

    if (XPR_IS_NOT_NULL(mObserver))
    {
        mObserver->onSplitterActivedPane(*this, mActivedPaneRow, mActivedPaneColumn);
    }

    return XPR_TRUE;
}

CWnd *Splitter::getActivePane(xpr_sint_t *aRow, xpr_sint_t *aColumn) const
{
    if (XPR_IS_NOT_NULL(aRow   )) *aRow    = mActivedPaneRow;
    if (XPR_IS_NOT_NULL(aColumn)) *aColumn = mActivedPaneColumn;

    Pane *sPane = getPane(mActivedPaneRow, mActivedPaneColumn);
    if (XPR_IS_NULL(sPane))
        return XPR_NULL;

    return sPane->mPaneWnd;
}

CWnd *Splitter::getLastActivedPane(xpr_sint_t *aRow, xpr_sint_t *aColumn) const
{
    if (XPR_IS_NOT_NULL(aRow   )) *aRow    = mLastActivedPaneRow;
    if (XPR_IS_NOT_NULL(aColumn)) *aColumn = mLastActivedPaneColumn;

    Pane *sPane = getPane(mLastActivedPaneRow, mLastActivedPaneColumn);
    if (XPR_IS_NULL(sPane))
        return XPR_NULL;

    return sPane->mPaneWnd;
}

void Splitter::setFocus(void)
{
    Pane *sPane = getPane(mActivedPaneRow, mActivedPaneColumn);
    if (XPR_IS_NULL(sPane))
        return;

    CWnd *sPaneWnd = sPane->mPaneWnd;
    if (XPR_IS_NOT_NULL(sPaneWnd))
    {
        sPaneWnd->SetFocus();
    }
}

Splitter::Pane *Splitter::getPane(xpr_sint_t aRow, xpr_sint_t aColumn) const
{
    if (!XPR_IS_RANGE(0, aRow, mRowCount - 1) || !XPR_IS_RANGE(0, aColumn, mColumnCount - 1))
        return XPR_NULL;

    return &mPanes[aRow][aColumn];
}

bool Splitter::operator ==(const Splitter &aSplitter) const
{
    return (this == &aSplitter) ? XPR_TRUE : XPR_FALSE;
}

void Splitter::showPane(xpr_bool_t aShow)
{
    xpr_sint_t i, j;
    Pane *sPane;

    for (i = 0; i < mRowCount; ++i)
    {
        for (j = 0; j < mColumnCount; ++j)
        {
            sPane = getPane(i, j);
            if (XPR_IS_NULL(sPane))
                continue;

            if (XPR_IS_NOT_NULL(sPane->mPaneWnd))
            {
                sPane->mPaneWnd->ShowWindow(XPR_IS_TRUE(aShow) ? SW_SHOW : SW_HIDE);
            }
        }
    }
}

xpr_bool_t Splitter::beginTracking(HWND aHwnd, const CPoint &aPoint)
{
    XPR_SAFE_DELETE(mTrack);

    xpr_sint_t i;
    xpr_sint_t sRow = 0, sColumn = 0;
    xpr_sint_t sSplitterCount = getSplitterCount();
    xpr_bool_t sHorzSplitter = XPR_FALSE;
    xpr_bool_t sSplitterHover = XPR_FALSE;
    CRect sSplitterRect;

    for (i = 0; i < sSplitterCount; ++i)
    {
        if (getSplitterRect(i, sSplitterRect, &sHorzSplitter, &sRow, &sColumn) == XPR_FALSE)
            continue;

        if (sSplitterRect.PtInRect(aPoint) == XPR_TRUE)
        {
            sSplitterHover = XPR_TRUE;
            break;
        }
    }

    if (XPR_IS_TRUE(sSplitterHover))
    {
        mTrack = new Track;
        if (XPR_IS_NULL(mTrack))
            return XPR_FALSE;

        mTrack->mHwnd          = aHwnd;

        mTrack->mSplitterIndex = 0;
        mTrack->mHorzSplitter  = sHorzSplitter;
        mTrack->mRow           = sRow;
        mTrack->mColumn        = sColumn;

        mTrack->mCurPos        = 0;
        mTrack->mCapturedCoord = 0;
        mTrack->mPressed       = XPR_FALSE;
        mTrack->mHover         = XPR_FALSE;
        mTrack->mPressedLeave  = XPR_FALSE;
        mTrack->mSetCapture    = XPR_FALSE;

        CSize sPaneSize(0, 0);
        getPaneSize(mTrack->mRow, mTrack->mColumn, sPaneSize);

        if (XPR_IS_FALSE(mTrack->mHorzSplitter))
            mTrack->mCurPos = sPaneSize.cx;
        else
            mTrack->mCurPos = sPaneSize.cy;

        SetCapture(mTrack->mHwnd);
        mTrack->mSetCapture = XPR_TRUE;
        mTrack->mCapturedCoord = XPR_IS_FALSE(mTrack->mHorzSplitter) ? aPoint.x : aPoint.y;

        return XPR_TRUE;
    }

    return XPR_FALSE;
}

xpr_bool_t Splitter::isTracking(void) const
{
    if (XPR_IS_NULL(mTrack))
        return XPR_FALSE;

    return mTrack->mSetCapture;
}

xpr_bool_t Splitter::moveTracking(const CPoint &aPoint)
{
    if (XPR_IS_NOT_NULL(mTrack) && XPR_IS_TRUE(mTrack->mSetCapture))
    {
        SetCursor(::LoadCursor(0, XPR_IS_FALSE(mTrack->mHorzSplitter) ? IDC_SIZEWE : IDC_SIZENS));

        xpr_sint_t sCapturedCoord = XPR_IS_FALSE(mTrack->mHorzSplitter) ? aPoint.x : aPoint.y;
        xpr_sint_t sCurPos = mTrack->mCurPos + (sCapturedCoord - mTrack->mCapturedCoord);

        //XPR_TRACE(XPR_STRING_LITERAL("OnMouseMove - %d\n", sCurPos));

        if (mMinPaneSize > 0 && sCurPos < mMinPaneSize) sCurPos = mMinPaneSize;
        if (mMaxPaneSize > 0 && sCurPos > mMaxPaneSize) sCurPos = mMaxPaneSize;

        xpr_sint_t sOffset = sCurPos - mTrack->mCurPos;

        if (XPR_IS_FALSE(mTrack->mHorzSplitter))
        {
            CSize sRightPaneSize(0, 0);
            getPaneSize(mTrack->mRow, mTrack->mColumn + 1, sRightPaneSize);

            if (mMinPaneSize > 0 && (sRightPaneSize.cx - sOffset) < mMinPaneSize)
                sCurPos -= mMinPaneSize - (sRightPaneSize.cx - sOffset);
        }
        else
        {
            CSize sRightPaneSize(0, 0);
            getPaneSize(mTrack->mRow + 1, mTrack->mColumn, sRightPaneSize);

            if (mMinPaneSize > 0 && (sRightPaneSize.cy - sOffset) < mMinPaneSize)
                sCurPos -= mMinPaneSize - (sRightPaneSize.cy - sOffset);
        }

        if (sCurPos != mTrack->mCurPos)
        {
            sOffset = sCurPos - mTrack->mCurPos; // recalculate offset

            mTrack->mCurPos = sCurPos;
            mTrack->mCapturedCoord += sOffset;

            XPR_TRACE(XPR_STRING_LITERAL("OnMouseMove - %d\n"), mTrack->mCurPos);

            if (XPR_IS_FALSE(mTrack->mHorzSplitter))
            {
                CSize sRightPaneSize(0, 0);
                getPaneSize(mTrack->mRow, mTrack->mColumn + 1, sRightPaneSize);

                moveColumn(mTrack->mColumn, mTrack->mCurPos);
                moveColumn(mTrack->mColumn + 1, sRightPaneSize.cx - sOffset);
            }
            else
            {
                moveRow(mTrack->mRow, mTrack->mCurPos);
            }

            //resize();
            //::UpdateWindow(mTrack->mHwnd);
        }

        return XPR_TRUE;
    }

    xpr_sint_t i;
    xpr_sint_t sSplitterCount = getSplitterCount();
    xpr_bool_t sSplitterHover = XPR_FALSE;
    xpr_bool_t sHorzSplitter = XPR_FALSE;
    CRect sSplitterRect;

    for (i = 0; i < sSplitterCount; ++i)
    {
        if (getSplitterRect(i, sSplitterRect, &sHorzSplitter) == XPR_FALSE)
            continue;

        if (sSplitterRect.PtInRect(aPoint) == XPR_TRUE)
        {
            sSplitterHover = XPR_TRUE;
            break;
        }
    }

    if (XPR_IS_TRUE(sSplitterHover))
    {
        SetCursor(::LoadCursor(0, XPR_IS_FALSE(sHorzSplitter) ? IDC_SIZEWE : IDC_SIZENS));
    }

    return XPR_FALSE;
}

void Splitter::endTracking(const CPoint &aPoint)
{
    if (XPR_IS_NOT_NULL(mTrack) && XPR_IS_TRUE(mTrack->mSetCapture))
    {
        xpr_sint_t sCapturedCoord = XPR_IS_FALSE(mTrack->mHorzSplitter) ? aPoint.x : aPoint.y;
        xpr_sint_t sCurPos = mTrack->mCurPos + (sCapturedCoord - mTrack->mCapturedCoord);

        if (mMinPaneSize > 0 && sCurPos < mMinPaneSize) sCurPos = mMinPaneSize;
        if (mMaxPaneSize > 0 && sCurPos > mMaxPaneSize) sCurPos = mMaxPaneSize;

        xpr_sint_t sOffset = sCurPos - mTrack->mCurPos;

        if (XPR_IS_FALSE(mTrack->mHorzSplitter))
        {
            CSize sRightPaneSize(0, 0);
            getPaneSize(mTrack->mRow, mTrack->mColumn + 1, sRightPaneSize);

            if (mMinPaneSize > 0 && (sRightPaneSize.cx - sOffset) < mMinPaneSize)
                sCurPos -= mMinPaneSize - (sRightPaneSize.cx - sOffset);
        }
        else
        {
            CSize sRightPaneSize(0, 0);
            getPaneSize(mTrack->mRow + 1, mTrack->mColumn, sRightPaneSize);

            if (mMinPaneSize > 0 && (sRightPaneSize.cy - sOffset) < mMinPaneSize)
                sCurPos -= mMinPaneSize - (sRightPaneSize.cy - sOffset);
        }

        if (sCurPos != mTrack->mCurPos)
        {
            sOffset = sCurPos - mTrack->mCurPos;

            mTrack->mCurPos = sCurPos;
            mTrack->mCapturedCoord += sOffset;

            if (XPR_IS_FALSE(mTrack->mHorzSplitter))
            {
                CSize sRightPaneSize(0, 0);
                getPaneSize(mTrack->mRow, mTrack->mColumn + 1, sRightPaneSize);

                moveColumn(mTrack->mColumn, mTrack->mCurPos);
                moveColumn(mTrack->mColumn + 1, sRightPaneSize.cx - sOffset);
            }
            else
            {
                moveRow(mTrack->mRow, mTrack->mCurPos);
            }

            //resize();
            //::UpdateWindow(mTrack->mHwnd);
        }

        mTrack->mHover = XPR_FALSE;
        mTrack->mPressed = XPR_FALSE;
        mTrack->mPressedLeave = XPR_FALSE;

        mTrack->mSetCapture = XPR_FALSE;
        ::ReleaseCapture();

        ::InvalidateRect(mTrack->mHwnd, XPR_NULL, XPR_TRUE);
        //::UpdateWindow(mTrack->mHwnd);

        XPR_SAFE_DELETE(mTrack);
    }
}

void Splitter::cancelTracking(void)
{
    if (XPR_IS_NOT_NULL(mTrack) && XPR_IS_TRUE(mTrack->mSetCapture))
    {
        mTrack->mHover = XPR_FALSE;
        mTrack->mPressed = XPR_FALSE;
        mTrack->mPressedLeave = XPR_FALSE;

        mTrack->mSetCapture = XPR_FALSE;
        ::ReleaseCapture();

        ::InvalidateRect(mTrack->mHwnd, XPR_NULL, XPR_TRUE);

        XPR_SAFE_DELETE(mTrack);
    }
}
} // namespace fxfile
