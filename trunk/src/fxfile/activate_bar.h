//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_ACTIVATE_BAR_H__
#define __FXFILE_ACTIVATE_BAR_H__ 1
#pragma once

namespace fxfile
{
class ActivateBar : public CWnd
{
public:
    ActivateBar(void);
    virtual ~ActivateBar(void);

public:
    void activate(xpr_bool_t aActivate, xpr_bool_t aLastActivated);
    xpr_bool_t isActivated(void) const;

    void setActivateBackColor(COLORREF aBackColor);
    void setBackColor(COLORREF aBackColor);
    COLORREF getBackColor(void) const;

protected:
    xpr_bool_t mActivate;
    xpr_bool_t mLastActivated;

    COLORREF mBackColor;

public:
    virtual xpr_bool_t Create(CWnd *aParentWnd, xpr_uint_t aId, const RECT &aRect);

protected:
    DECLARE_MESSAGE_MAP()
    afx_msg xpr_sint_t OnCreate(LPCREATESTRUCT aCreateStruct);
    afx_msg void OnDestroy(void);
    afx_msg void OnPaint(void);
    afx_msg xpr_bool_t OnEraseBkgnd(CDC *aDC);
};
} // namespace fxfile

#endif // __FXFILE_ACTIVATE_BAR_H__
