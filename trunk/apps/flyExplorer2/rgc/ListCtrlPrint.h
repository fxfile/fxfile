//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FX_LIST_CTRL_PRINT_H__
#define __FX_LIST_CTRL_PRINT_H__
#pragma once

#include "Print.h"

class ListCtrlPrint : public Print
{
public:
    ListCtrlPrint(void);
    virtual ~ListCtrlPrint(void);

public:
    void setListCtrl(CListCtrl *aListCtrl);

public:
    virtual xpr_bool_t onPreparePrinting(CPrintInfo *aPrintInfo);
    virtual void onBeginPrinting(CDC *aDC, CPrintInfo *aPrintInfo);
    virtual void onPrint(CDC *aDC, CPrintInfo *aPrintInfo);
    virtual void onEndPrinting(CDC *aDC, CPrintInfo *aPrintInfo);

protected:
    void drawRow(CDC *aDC, xpr_sint_t aRow);
    void printFooter(CDC *aDC, CPrintInfo *aPrintInfo);
    void printBody(CDC *aDC, CPrintInfo *aPrintInfo);
    void printHeader(CDC *aDC, CPrintInfo *aPrintInfo);
    CRect getBodyRect(void);
    CRect getHeaderRect(void);
    CRect getFooterRect(void);
    CRect getCellRect(xpr_sint_t aRow, xpr_sint_t aColumn);
    CString getColumnHeading(xpr_sint_t aColumn);
    CRect getColumnRect(xpr_sint_t aColumn);
    xpr_sint_t getColumnWidth(xpr_sint_t aColumn);
    xpr_sint_t getColumnFormat(xpr_sint_t aColumn);
    xpr_sint_t getPageRows(void);
    xpr_sint_t getPageColumns(void);

    virtual CString getItemText(xpr_sint_t aRow, xpr_sint_t aColumn) const;
    virtual xpr_sint_t getRowCount(void) const;

protected:
    CListCtrl  *mListCtrl;
    CRect       mHeaderRect;
    CRect       mFooterRect;
    CRect       mBodyRect;
    CFont      *mHeaderFont;
    CFont      *mFooterFont;
    CFont      *mColumnFont;
    CFont      *mBodyFont;
    CSize       mHeaderCharSize;
    CSize       mFooterCharSize;
    CSize       mColumnCharSize;
    CSize       mBodyCharSize;
    xpr_sint_t  mPageColumns;
    xpr_sint_t  mPageRows;
    xpr_sint_t  mPageCount;
    xpr_sint_t  mRowCount;
    xpr_sint_t  mColumns[100];
};

#endif // __FX_LIST_CTRL_PRINT_H__
