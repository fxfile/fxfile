//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_PRINT_H__
#define __FXFILE_PRINT_H__
#pragma once

class Print
{
public:
    Print(void);
    virtual ~Print(void);

public:
    void setDocTitle(const CString &aName);
    void setAppName(const CString &aName);
    void setMarginPixel(CRect aMarginRect);
    void getMarginPixel(CRect &aMarginRect);

public:
    virtual xpr_bool_t onPreparePrinting(CPrintInfo *aPrintInfo);
    virtual void onBeginPrinting(CDC *aDC, CPrintInfo *aPrintInfo);
    virtual void onPrint(CDC *aDC, CPrintInfo *aPrintInfo);
    virtual void onEndPrinting(CDC *aDC, CPrintInfo *aPrintInfo);

protected:
    void drawRect(CDC *aDC, CRect aRect, COLORREF aColor);
    CFont *createFont(CDC *aDC, CString aName = XPR_STRING_LITERAL(""), xpr_sint_t aPoints = 0, xpr_bool_t aBold = XPR_FALSE, xpr_bool_t aItalic = XPR_FALSE);
    xpr_float_t getTextRatioX(CDC *aDC);
    CRect getPaperRect(CDC *aDC);
    CRect getPageRect(void);
    CSize getCharSize(CDC *aDC, CFont *aFont);

protected:   
    CString     mAppName;
    CString     mDocName;
    xpr_float_t mRatioX; 
    CRect       mPageRect;
    CRect       mPaperRect;
    CRect       mMarginRect;
};

#endif // __FXFILE_PRINT_H__
