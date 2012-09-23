//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FX_LIST_CTRL_EX_H__
#define __FX_LIST_CTRL_EX_H__
#pragma once

class FlatHeaderCtrl;

class ListCtrlEx : public CListCtrl
{
    DECLARE_DYNAMIC(ListCtrlEx);

public:
    ListCtrlEx(void);

protected:
    xpr_bool_t subclassHeader(xpr_bool_t aBoldFont = XPR_FALSE);
    xpr_sint_t setSortImage(xpr_sint_t aColumn, xpr_bool_t aAscending);

protected:
    FlatHeaderCtrl *mHeaderCtrl;

protected:
    virtual LRESULT WindowProc(xpr_uint_t message, WPARAM wParam, LPARAM lParam);

protected:
    DECLARE_MESSAGE_MAP()
    afx_msg xpr_sint_t OnCreate(LPCREATESTRUCT aCreateStruct);
    afx_msg void OnDestroy(void);
};

#endif // __FX_LIST_CTRL_EX_H__
