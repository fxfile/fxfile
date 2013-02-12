//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "ListCtrlPrint.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static const xpr_sint_t kHeaderLineSize = 5;
static const xpr_sint_t kHeaderLineGap  = 5;
static const xpr_sint_t kColumnTextGap  = 10;
static const xpr_sint_t kColumnLineSize = 5;
static const xpr_sint_t kColumnLineGap  = 5;
static const xpr_sint_t kFooterLineSize = 5;
static const xpr_sint_t kFooterLineGap  = 5;
static const xpr_sint_t kCellPadding    = 5;

ListCtrlPrint::ListCtrlPrint(void)
    : mListCtrl(XPR_NULL)
{
}

ListCtrlPrint::~ListCtrlPrint(void)
{
}

xpr_bool_t ListCtrlPrint::onPreparePrinting(CPrintInfo *aPrintInfo)
{
    if (Print::onPreparePrinting(aPrintInfo) == XPR_FALSE)
        return FALSE;

    mHeaderRect     = CRect(0,0,0,0);
    mFooterRect     = CRect(0,0,0,0);
    mBodyRect       = CRect(0,0,0,0);
    mHeaderFont     = XPR_NULL;
    mFooterFont     = XPR_NULL;
    mColumnFont     = XPR_NULL;
    mBodyFont       = XPR_NULL;
    mHeaderCharSize = CSize(0,0);
    mFooterCharSize = CSize(0,0);
    mColumnCharSize = CSize(0,0);
    mBodyCharSize   = CSize(0,0);
    mPageColumns    = 0;
    mPageRows       = 0;
    mPageCount      = 0;
    mRowCount       = 0;
    memset(mColumns, 0, sizeof(mColumns));

    return TRUE;
}

void ListCtrlPrint::onBeginPrinting(CDC *aDC, CPrintInfo *aPrintInfo)
{
    // Create fonts
    ASSERT(aDC != XPR_NULL && aPrintInfo != XPR_NULL);

    Print::onBeginPrinting(aDC, aPrintInfo);

    NONCLIENTMETRICS sNonClientMetrics = {0};
    sNonClientMetrics.cbSize = sizeof(sNonClientMetrics);
    ::SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), &sNonClientMetrics, 0);

    mHeaderFont = createFont(aDC, sNonClientMetrics.lfMenuFont.lfFaceName, 12, FW_BOLD);
    mFooterFont = createFont(aDC, sNonClientMetrics.lfMenuFont.lfFaceName, 10);
    mColumnFont = createFont(aDC, sNonClientMetrics.lfMenuFont.lfFaceName, 9, FW_BOLD);
    mBodyFont   = createFont(aDC, sNonClientMetrics.lfMenuFont.lfFaceName, 9);

    // Calculate character size
    mHeaderCharSize = getCharSize(aDC, mHeaderFont);
    mFooterCharSize = getCharSize(aDC, mFooterFont);
    mColumnCharSize = getCharSize(aDC, mColumnFont);
    mBodyCharSize   = getCharSize(aDC, mBodyFont);

    mHeaderRect   = getHeaderRect();
    mFooterRect   = getFooterRect();
    mBodyRect     = getBodyRect();
    mRatioX       = getTextRatioX(aDC);
    mRowCount     = getRowCount();
    mPageColumns  = getPageColumns();
    mPageRows     = getPageRows();
    mPageCount    = (mRowCount + mPageRows - 1) / mPageRows;

    // Column order
    ASSERT(mListCtrl);
    mListCtrl->GetColumnOrderArray(mColumns);

    // How many pages?
    aPrintInfo->SetMaxPage(mPageCount);   
}

void ListCtrlPrint::onPrint(CDC *aDC, CPrintInfo *aPrintInfo) 
{
    Print::onPrint(aDC, aPrintInfo);

    printHeader(aDC, aPrintInfo);
    printBody(aDC, aPrintInfo);
    printFooter(aDC, aPrintInfo);
}

void ListCtrlPrint::onEndPrinting(CDC *aDC, CPrintInfo *aPrintInfo)
{
    Print::onEndPrinting(aDC, aPrintInfo);

    XPR_SAFE_DELETE(mHeaderFont);
    XPR_SAFE_DELETE(mFooterFont);
    XPR_SAFE_DELETE(mColumnFont);
    XPR_SAFE_DELETE(mBodyFont);
}

/////////////////////////////////////////////////////////////////////////////
// ListCtrlPrint helpers

void ListCtrlPrint::drawRow(CDC *aDC, xpr_sint_t aRow)
{
    CRect sRect;
    CString sText;
    xpr_sint_t i, sFormat;

    for (i = 0; i < mPageColumns; ++i)
    {
        sRect = getCellRect(aRow, i);
        sText = getItemText(aRow, i);
        sFormat = getColumnFormat(i);

        if (sFormat == -1)     DrawText(aDC->m_hDC, sText, -1, sRect, DT_LEFT   | DT_VCENTER | DT_END_ELLIPSIS);
        else if (sFormat == 1) DrawText(aDC->m_hDC, sText, -1, sRect, DT_RIGHT  | DT_VCENTER | DT_END_ELLIPSIS);
        else                   DrawText(aDC->m_hDC, sText, -1, sRect, DT_CENTER | DT_VCENTER | DT_END_ELLIPSIS);
    }
}

CRect ListCtrlPrint::getHeaderRect(void)
{
    ASSERT(mPageRect != CRect(0,0,0,0));

    CRect sHeaderRect = mPageRect;
    CSize sCharSize = mHeaderCharSize;
    sHeaderRect.DeflateRect(0, 0, 0, sHeaderRect.Height() - (sCharSize.cy + kHeaderLineGap + kHeaderLineSize));

    return sHeaderRect;
}

CRect ListCtrlPrint::getFooterRect(void)
{
    ASSERT(mPageRect != CRect(0,0,0,0));

    CRect sFooterRect = mPageRect;
    CSize sCharSize = mFooterCharSize;
    sFooterRect.DeflateRect(0, sFooterRect.Height() - (sCharSize.cy + kFooterLineSize), 0, 0);

    return sFooterRect;
}

CRect ListCtrlPrint::getBodyRect(void)
{
    ASSERT(mPageRect != CRect(0,0,0,0));
    ASSERT(mHeaderRect != CRect(0,0,0,0));
    ASSERT(mFooterRect != CRect(0,0,0,0));

    CRect sPageRect = mPageRect;
    CRect sHeaderRect = mHeaderRect;
    CRect sFooterRect = mFooterRect;
    sPageRect.DeflateRect(0, sHeaderRect.Height(), 0, sFooterRect.Height());

    return sPageRect;
}

xpr_sint_t ListCtrlPrint::getPageColumns(void)
{
    if (XPR_IS_NULL(mListCtrl))
        return 0;

    CHeaderCtrl *sHeaderCtrl = mListCtrl->GetHeaderCtrl();
    if (XPR_IS_NULL(sHeaderCtrl))
        return 0;

    return sHeaderCtrl->GetItemCount();
}

CString ListCtrlPrint::getColumnHeading(xpr_sint_t aColumn)
{
    if (XPR_IS_NULL(mListCtrl))
        return XPR_STRING_LITERAL("");

    CHeaderCtrl *sHeaderCtrl = mListCtrl->GetHeaderCtrl();
    if (XPR_IS_NULL(sHeaderCtrl))
        return XPR_STRING_LITERAL("");

    xpr_tchar_t sBuffer[1024];

    HDITEM sHdItem = {0};
    sHdItem.mask       = HDI_TEXT;
    sHdItem.cchTextMax = sizeof(sBuffer) / sizeof(sBuffer[0]);
    sHdItem.pszText    = sBuffer;
    sHeaderCtrl->GetItem(mColumns[aColumn], &sHdItem);

    return CString(sBuffer);
}

xpr_sint_t ListCtrlPrint::getColumnFormat(xpr_sint_t aColumn)
{
    if (XPR_IS_NULL(mListCtrl))
        return 0;

    CHeaderCtrl *sHeaderCtrl = mListCtrl->GetHeaderCtrl();
    if (XPR_IS_NULL(sHeaderCtrl))
        return 0;

    HDITEM sHdItem = {0};
    sHdItem.mask = HDI_FORMAT;
    sHeaderCtrl->GetItem(mColumns[aColumn], &sHdItem);

    if (XPR_TEST_BITS(sHdItem.fmt, HDF_CENTER))
        return 0;

    return XPR_TEST_BITS(sHdItem.fmt, HDF_LEFT) ? -1 : 1;
}

CRect ListCtrlPrint::getColumnRect(xpr_sint_t aColumn)
{
    if (!XPR_IS_RANGE(0, aColumn, mPageColumns-1))
        return CRect(0,0,0,0);

    CSize sCharSize = mColumnCharSize;
    CRect sBodyRect = mBodyRect;

    xpr_sint_t i, sLeft;
    for (sLeft = 0, i = 0; i < aColumn; ++i) 
        sLeft += getColumnWidth(mColumns[i]);
    xpr_sint_t sRight = sLeft + getColumnWidth(mColumns[i]);

    CRect sColumnRect(sBodyRect.left + sLeft, sBodyRect.top, sBodyRect.left + sRight, sBodyRect.top + sCharSize.cy);
    if (sColumnRect.right > sBodyRect.right)
        sColumnRect.right = sBodyRect.right;
    sColumnRect.left += (xpr_sint_t)(kCellPadding * mRatioX);

    sColumnRect.bottom += kColumnTextGap;

    return sColumnRect;
}

CRect ListCtrlPrint::getCellRect(xpr_sint_t aRow, xpr_sint_t aColumn)
{
    ASSERT(aColumn >= 0 && aColumn < mPageColumns);
    ASSERT(aRow >= 0 && aRow < mRowCount);

    CSize sCharSize = mBodyCharSize;
    CRect sBodyRect = mBodyRect;
    CRect sColumnRect = getColumnRect(0);

    xpr_sint_t i, sLeft;
    for (sLeft = 0, i = 0; i < aColumn; ++i) 
        sLeft += getColumnWidth(mColumns[i]);
    xpr_sint_t sRight = sLeft + getColumnWidth(mColumns[i]);   
    xpr_sint_t sPageRow =  aRow % mPageRows;
    ASSERT(sPageRow <= mPageRows);

    CRect sCellRect;
    sCellRect.left   = sBodyRect.left + sLeft;
    sCellRect.top    = sBodyRect.top  + sColumnRect.Height() + kColumnLineSize + kColumnLineGap + sCharSize.cy * sPageRow;
    sCellRect.right  = sBodyRect.left + sRight;
    sCellRect.bottom = sCellRect.top  + sCharSize.cy;

    if (sCellRect.right > sBodyRect.right)
        sCellRect.right = sBodyRect.right;

    sCellRect.left += (xpr_sint_t)(kCellPadding * mRatioX);

    return sCellRect;
}

CString ListCtrlPrint::getItemText(xpr_sint_t aRow, xpr_sint_t aColumn) const
{
    ASSERT(aColumn >= 0 && aColumn < mPageColumns);
    ASSERT(aRow >= 0 && aRow < mRowCount);
    return mListCtrl->GetItemText(aRow, mColumns[aColumn]);
}

xpr_sint_t ListCtrlPrint::getPageRows(void)
{
    CSize sCharSize = mBodyCharSize;
    CRect sBodyRect = mBodyRect;
    CRect sColumnRect = getColumnRect(0);

    return (sBodyRect.Height() - sColumnRect.Height())/ sCharSize.cy;
}

xpr_sint_t ListCtrlPrint::getColumnWidth(xpr_sint_t aColumn)
{
    if (XPR_IS_NULL(mListCtrl))
        return 0;

    CHeaderCtrl *sHeaderCtrl = mListCtrl->GetHeaderCtrl();
    if (XPR_IS_NULL(sHeaderCtrl))
        return 0;

    HDITEM sHdItem = {0};
    sHdItem.mask = HDI_WIDTH;
    sHeaderCtrl->GetItem(aColumn, &sHdItem);

    if (aColumn == 0)
    {
        xpr_sint_t sColumnExcept0 = 0;
        xpr_sint_t i, sCount = sHeaderCtrl->GetItemCount();

        for (i = 1; i < sCount; ++i)
            sColumnExcept0 += getColumnWidth(i);

        return getPageRect().Width() - (sColumnExcept0);
    }

    return (xpr_sint_t)((sHdItem.cxy + 8) * mRatioX);
}

xpr_sint_t ListCtrlPrint::getRowCount(void) const
{
    ASSERT(mListCtrl);
    return mListCtrl->GetItemCount();
}

void ListCtrlPrint::printHeader(CDC *aDC, CPrintInfo *aPrintInfo)
{
    ASSERT(mHeaderFont && aPrintInfo);
    UNUSED_ALWAYS(aPrintInfo);
    CFont *sOldFont = aDC->SelectObject(mHeaderFont);

#ifdef XPR_CFG_BUILD_DEBUG

    drawRect(aDC, mHeaderRect, RGB(0,255,0));

#endif

    // Create black brush
    CBrush sBrush;
    sBrush.CreateSolidBrush(RGB(0,0,0));
    CBrush *sOldBrush = aDC->SelectObject(&sBrush);

    // Draw line
    CRect sLineRect = mHeaderRect;
    sLineRect.top += mHeaderCharSize.cy + kHeaderLineGap;
    sLineRect.bottom = sLineRect.top + kHeaderLineSize;
    aDC->FillRect(&sLineRect, &sBrush);
    aDC->SelectObject(sOldBrush);

    // Draw "Program" and "Document"
    CRect sHeaderRect = mHeaderRect;
    aDC->DrawText(mDocName, sHeaderRect, DT_LEFT | DT_BOTTOM | DT_PATH_ELLIPSIS);
    aDC->SelectObject(sOldFont);
}

void ListCtrlPrint::printBody(CDC *aDC, CPrintInfo *aPrintInfo)
{
    ASSERT(mBodyFont && mColumnFont && aPrintInfo);

#ifdef XPR_CFG_BUILD_DEBUG

    drawRect(aDC, mBodyRect, RGB(255,255,0));

    xpr_sint_t j;
    for (j = 0; j < mPageColumns; ++j)
        drawRect(aDC, getColumnRect(j), RGB(128,64,0));

#endif

    // Print column headings
    xpr_sint_t i;
    CFont *sOldFont = aDC->SelectObject(mColumnFont); 
    for (i = 0; i < mPageColumns; ++i)
        DrawText(aDC->m_hDC, getColumnHeading(i), -1, getColumnRect(i), DT_SINGLELINE | DT_LEFT | DT_VCENTER);

    CBrush sBrush;
    sBrush.CreateSolidBrush(RGB(0,0,0));
    CBrush *sOldBrush = aDC->SelectObject(&sBrush);

    // Draw line
    CRect sLineRect = mBodyRect;
    sLineRect.top += getColumnRect(0).Height();
    sLineRect.bottom = sLineRect.top + kColumnLineSize;
    aDC->FillRect(&sLineRect, &sBrush);
    aDC->SelectObject(sOldBrush);

    // Start and end row
    aDC->SelectObject(mBodyFont);
    xpr_sint_t sStartRow = (aPrintInfo->m_nCurPage - 1) * mPageRows;
    xpr_sint_t sEndRow = sStartRow + mPageRows;
    if (sEndRow > mRowCount)
        sEndRow = mRowCount;

    // Prepare document and print rows
    for (i = sStartRow; i < sEndRow; ++i)
        drawRow(aDC, i);

    aDC->SelectObject(sOldFont);
}

void ListCtrlPrint::printFooter(CDC *aDC, CPrintInfo *aPrintInfo)
{
    ASSERT(mFooterFont != XPR_NULL && aPrintInfo != XPR_NULL);
    CFont *sOldFont;

#ifdef XPR_CFG_BUILD_DEBUG

    drawRect(aDC, mFooterRect, RGB(0,255,0));

#endif

    // Create black brush
    CBrush sBrush;
    sBrush.CreateSolidBrush(RGB(0,0,0));
    CBrush *sOldBrush = aDC->SelectObject(&sBrush);

    // Draw line
    CRect sLineRect = mFooterRect;
    sLineRect.bottom = sLineRect.top + kHeaderLineSize;
    aDC->FillRect(&sLineRect, &sBrush);
    aDC->SelectObject(sOldBrush);

    // Draw "Application Name"
    CRect sAppNameRect = mFooterRect;
    sAppNameRect.top += kHeaderLineGap;

    sOldFont = aDC->SelectObject(mColumnFont);
    aDC->DrawText(mAppName, sAppNameRect, DT_LEFT | DT_BOTTOM | DT_SINGLELINE);
    aDC->SelectObject(sOldFont);

    // Draw "Pages"
    CRect sPagesRect = mFooterRect;
    sPagesRect.top += kHeaderLineGap;

    CString sPages;
    sPages.Format(XPR_STRING_LITERAL("%d / %d"), aPrintInfo->m_nCurPage, mPageCount);

    sOldFont = aDC->SelectObject(mFooterFont);
    aDC->DrawText(sPages, sPagesRect, DT_RIGHT | DT_BOTTOM);
    aDC->SelectObject(sOldFont);
}

void ListCtrlPrint::setListCtrl(CListCtrl *aListCtrl)
{
    ASSERT(aListCtrl != XPR_NULL);
    mListCtrl = aListCtrl;
}
