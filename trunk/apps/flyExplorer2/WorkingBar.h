//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FX_WORKING_BAR_H__
#define __FX_WORKING_BAR_H__
#pragma once

#include "rgc/scbarcf.h"
#include "rgc/bcgtabwnd.h"

#include "SearchResultCtrl.h"

class WorkingBar : public CSizingControlBarCF
{
    typedef CSizingControlBarCF super;

    DECLARE_DYNAMIC(WorkingBar);

public:
    WorkingBar(void);
    virtual ~WorkingBar(void);

public:
    SearchResultCtrl *getSearchResultCtrl(void) const;

    void createChild(void);
    void destroyChild(void);

public:
    virtual void OnBarClose(void);

protected:
    void recalcLayout(void);

protected:
    CBCGTabWnd       *mTabWnd;
    SearchResultCtrl *mSearchCtrl;

public:
    virtual xpr_bool_t PreTranslateMessage(MSG *aMsg);
    virtual xpr_bool_t DestroyWindow(void);

protected:
    afx_msg xpr_sint_t OnCreate(LPCREATESTRUCT aCreateStruct);
    afx_msg void OnSize(xpr_uint_t aType, xpr_sint_t cx, xpr_sint_t cy);
    afx_msg void OnSetFocus(CWnd *aOldWnd);
    afx_msg void OnWindowPosChanging(WINDOWPOS FAR *aWindowPos);
    DECLARE_MESSAGE_MAP()
};

#endif // __FX_WORKING_BAR_H__
