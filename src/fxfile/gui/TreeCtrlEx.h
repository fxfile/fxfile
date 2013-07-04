//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_TREE_CTRL_EX_H__
#define __FXFILE_TREE_CTRL_EX_H__
#pragma once

#if (_WIN32_WINNT < 0x0600)
#define TVS_EX_MULTISELECT          0x0002
#define TVS_EX_DOUBLEBUFFER         0x0004
#define TVS_EX_NOINDENTSTATE        0x0008
#define TVS_EX_RICHTOOLTIP          0x0010
#define TVS_EX_AUTOHSCROLL          0x0020
#define TVS_EX_FADEINOUTEXPANDOS    0x0040
#define TVS_EX_PARTIALCHECKBOXES    0x0080
#define TVS_EX_EXCLUSIONCHECKBOXES  0x0100
#define TVS_EX_DIMMEDCHECKBOXES     0x0200
#define TVS_EX_DRAWIMAGEASYNC       0x0400
#endif

class TreeCtrlEx : public CTreeCtrl
{
    typedef CTreeCtrl super;

public:
    TreeCtrlEx(void);
    virtual ~TreeCtrlEx(void);

public:
    xpr_bool_t hasChildItem(HTREEITEM aTreeItem) const;

    xpr_bool_t isVistaEnhanced(void) const;
    void enableVistaEnhanced(xpr_bool_t aEnable);

    DWORD GetExtendedStyle() const;
    DWORD SetExtendedStyle(DWORD dwExMask, DWORD dwExStyles);

protected:
    xpr_bool_t mVistaEnhanced;

protected:
    DECLARE_MESSAGE_MAP()
};

#endif // __FXFILE_TREE_CTRL_EX_H__
