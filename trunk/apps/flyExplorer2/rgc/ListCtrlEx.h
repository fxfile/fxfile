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

#if (_WIN32_WINNT < 0x0600)
#define LVS_EX_JUSTIFYCOLUMNS   0x00200000  // Icons are lined up in columns that use up the whole view area.
#define LVS_EX_TRANSPARENTBKGND 0x00400000  // Background is painted by the parent via WM_PRINTCLIENT
#define LVS_EX_TRANSPARENTSHADOWTEXT 0x00800000  // Enable shadow text on transparent backgrounds only (useful with bitmaps)
#define LVS_EX_AUTOAUTOARRANGE  0x01000000  // Icons automatically arrange if no icon positions have been set
#define LVS_EX_HEADERINALLVIEWS 0x02000000  // Display column header in all view modes
#define LVS_EX_AUTOCHECKSELECT  0x08000000
#define LVS_EX_AUTOSIZECOLUMNS  0x10000000
#define LVS_EX_COLUMNSNAPPOINTS 0x40000000
#define LVS_EX_COLUMNOVERFLOW   0x80000000
#endif

class FlatHeaderCtrl;

class ListCtrlEx : public CListCtrl
{
    typedef CListCtrl super;

    DECLARE_DYNAMIC(ListCtrlEx);

public:
    ListCtrlEx(void);

public:
    xpr_bool_t isVistaEnhanced(void) const;
    void enableVistaEnhanced(xpr_bool_t aEnable);

protected:
    xpr_bool_t subclassHeader(xpr_bool_t aBoldFont = XPR_FALSE);
    xpr_sint_t setSortImage(xpr_sint_t aColumn, xpr_bool_t aAscending);

protected:
    xpr_bool_t      mVistaEnhanced;
    FlatHeaderCtrl *mHeaderCtrl;

protected:
    virtual LRESULT WindowProc(xpr_uint_t message, WPARAM wParam, LPARAM lParam);

protected:
    DECLARE_MESSAGE_MAP()
    afx_msg xpr_sint_t OnCreate(LPCREATESTRUCT aCreateStruct);
    afx_msg void OnDestroy(void);
};

#endif // __FX_LIST_CTRL_EX_H__
