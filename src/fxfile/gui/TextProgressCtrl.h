//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_TEXT_PROGRESS_CTRL_H__
#define __FXFILE_TEXT_PROGRESS_CTRL_H__
#pragma once

#define WM_PROGRESS_DBLCK (WM_USER + 10)

class TextProgressCtrl : public CProgressCtrl
{
    typedef CProgressCtrl super;

public:
    TextProgressCtrl(void);
    virtual ~TextProgressCtrl(void);

public:
    void setBoldFont(xpr_bool_t aBoldFont);
    void setShowText(xpr_bool_t aShow);
    void setProgressColor(COLORREF aColor);
    void setBackColor(COLORREF aColor);
    void setTextProgressColor(COLORREF aColor);
    void setTextBackColor(COLORREF aColor);

public:
    xpr_sint_t setPos(xpr_sint_t aPos);
    void       setRange(xpr_sint_t aMin, xpr_sint_t aMax);

protected:
    xpr_sint_t mMin;
    xpr_sint_t mMax;
    xpr_sint_t mPos;

    xpr_bool_t mShowText;

    xpr_bool_t mBoldFont;
    COLORREF   mProgressColor;
    COLORREF   mBackColor;
    COLORREF   mTextProgressColor;
    COLORREF   mTextBackColor;
    CFont      mTextFont;

protected:
    DECLARE_MESSAGE_MAP()
    afx_msg xpr_sint_t OnCreate(LPCREATESTRUCT aCreateStruct);
    afx_msg void OnDestroy(void);
    afx_msg xpr_bool_t OnEraseBkgnd(CDC *aDC);
    afx_msg void OnPaint(void);
    afx_msg void OnNcPaint(void);
    afx_msg void OnSize(xpr_uint_t aType, xpr_sint_t cx, xpr_sint_t cy);
    afx_msg void OnLButtonDblClk(xpr_uint_t aFlags, CPoint aPoint);
};

#endif // __FXFILE_TEXT_PROGRESS_CTRL_H__
