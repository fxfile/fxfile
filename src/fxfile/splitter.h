//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_SPLITTER_H__
#define __FXFILE_SPLITTER_H__ 1
#pragma once

namespace fxfile
{
class Splitter;

class SplitterObserver
{
public:
    virtual CWnd *onSplitterPaneCreate(Splitter &aSplitter, xpr_sint_t aRow, xpr_sint_t aColumn) = 0;
    virtual void onSplitterPaneDestroy(Splitter &aSplitter, CWnd *aPaneWnd) = 0;
    virtual void onSplitterActivedPane(Splitter &aSplitter, xpr_sint_t aRow, xpr_sint_t aColumn) = 0;
};

class Splitter
{
public:
    Splitter(void);
    virtual ~Splitter(void);

public:
    void setObserver(SplitterObserver *aObserver);

public:
    void setMaxSplitCount(xpr_sint_t aMaxRowCount, xpr_sint_t aMaxColumnCount);
    void setSplitSize(xpr_sint_t aSplitSize);
    xpr_sint_t getSplitSize(void) const;
    void getMaxSplitCount(xpr_sint_t &aMaxRowCount, xpr_sint_t &aMaxColumnCount) const;

public:
    void split(xpr_sint_t aRowCount, xpr_sint_t aColumnCount);
    void switchPane(xpr_sint_t aOldRow, xpr_sint_t aOldColumn, xpr_sint_t aNewRow, xpr_sint_t aNewColumn);

public:
    xpr_sint_t getPaneCount(xpr_sint_t *aRowCount = XPR_NULL, xpr_sint_t *aColumnCount = XPR_NULL) const;
    xpr_sint_t getRowCount(void) const;
    xpr_sint_t getColumnCount(void) const;

public:
    xpr_sint_t getSplitterCount(void) const;
    xpr_bool_t getSplitterRect(xpr_sint_t aSplitterIndex, CRect &aSplitterRect, xpr_bool_t *aHorzSplitter = XPR_NULL, xpr_sint_t *aRow = XPR_NULL, xpr_sint_t *aColumn = XPR_NULL) const;

public:
    void setWindowRect(CRect aRect);
    void getWindowRect(CRect &aRect) const;

public:
    void moveRow(xpr_sint_t aRow, xpr_sint_t cy);
    void moveColumn(xpr_sint_t aColumn, xpr_sint_t cx);
    void resize(HDWP aHdwp = XPR_NULL);

    void setPaneSizeLimit(xpr_sint_t sMinSize, xpr_sint_t sMaxSize);
    void setPaneSize(xpr_sint_t aRow, xpr_sint_t aColumn, CSize aPaneSize);
    xpr_bool_t getPaneSize(xpr_sint_t aRow, xpr_sint_t aColumn, CSize &aPaneSize) const;
    xpr_bool_t getPaneRect(xpr_sint_t aRow, xpr_sint_t aColumn, CRect &aPaneRect) const;

    void showPane(xpr_bool_t aShow);

public:
    CWnd *getPaneWnd(xpr_sint_t aRow, xpr_sint_t aColumn) const;
    xpr_bool_t setActivePane(xpr_sint_t aRow, xpr_sint_t aColumn);
    CWnd *getActivePane(xpr_sint_t *aRow = XPR_NULL, xpr_sint_t *aColumn = XPR_NULL) const;
    CWnd *getLastActivedPane(xpr_sint_t *aRow = XPR_NULL, xpr_sint_t *aColumn = XPR_NULL) const;
    void setFocus(void);

public:
    bool operator ==(const Splitter &aSplitter) const;

public:
    xpr_bool_t beginTracking(HWND aHwnd, const CPoint &aPoint);
    xpr_bool_t isTracking(void) const;
    xpr_bool_t moveTracking(const CPoint &aPoint);
    void endTracking(const CPoint &aPoint);
    void cancelTracking(void);

protected:
    struct Pane;
    struct Track;

    void clear(void);

    Pane *getPane(xpr_sint_t aRow, xpr_sint_t aColumn) const;

    xpr_bool_t evaluateLastActivedPane(xpr_sint_t aNewRow, xpr_sint_t aNewColumn);

protected:
    SplitterObserver *mObserver;

protected:
    Pane       **mPanes;
    xpr_sint_t   mMaxRowCount;
    xpr_sint_t   mMaxColumnCount;
    xpr_sint_t   mSplitSize;
    xpr_sint_t   mRowCount;
    xpr_sint_t   mColumnCount;
    CRect        mWindowRect;
    xpr_sint_t   mMinPaneSize;
    xpr_sint_t   mMaxPaneSize;

    xpr_sint_t   mActivedPaneRow;
    xpr_sint_t   mActivedPaneColumn;
    xpr_sint_t   mLastActivedPaneRow;
    xpr_sint_t   mLastActivedPaneColumn;

    Track       *mTrack;
};
} // namespace fxfile

#endif // __FXFILE_SPLITTER_H__
