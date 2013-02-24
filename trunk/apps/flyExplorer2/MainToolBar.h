//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FX_MAIN_TOOL_BAR_H__
#define __FX_MAIN_TOOL_BAR_H__
#pragma once

#include "rgc/DragDropToolBar.h"

class MainToolBar : public DragDropToolBar
{
    typedef DragDropToolBar super;

public:
    MainToolBar(void);
    virtual ~MainToolBar(void);

    // Overrides
public:
    virtual void Init(void);

public:
    TBBUTTONEX *getButton(xpr_sint_t *aCount = XPR_NULL);
    COLORREF    getColorMask(void);

    xpr_bool_t load(const xpr_tchar_t *aPath);
    void save(const xpr_tchar_t *aPath) const;

protected:
    // from DropTargetObserver
    virtual DROPEFFECT OnDragEnter(COleDataObject *aOleDataObject, DWORD aKeyState, CPoint aPoint);
    virtual void       OnDragLeave(void);
    virtual DROPEFFECT OnDragOver(COleDataObject *aOleDataObject, DWORD aKeyState, CPoint aPoint);
    virtual void       OnDragScroll(DWORD aKeyState, CPoint aPoint);
    virtual void       OnDrop(COleDataObject *aOleDataObject, DROPEFFECT aDropEffect, CPoint aPoint);

    void OnDropBookmark(LPITEMIDLIST aFullPidl);
    void OnDropProgramAss(LPITEMIDLIST aFullPidl, xpr_uint_t aType);
    void OnDropWorkingFolder(xpr_sint_t aIndex, LPITEMIDLIST aFullPidl);

protected:
    virtual xpr_bool_t HasButtonText(xpr_uint_t aId);
    virtual xpr_bool_t HasButtonTip(xpr_uint_t aId);
    virtual void GetButtonTextByCommand(xpr_uint_t aId, CString &aText);

protected:
    typedef std::deque<xpr_uint_t> DragDropDeque;
    DragDropDeque mDragDropIds;
    xpr_uint_t mOldDragId;

protected:
    virtual xpr_bool_t OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);

protected:
    DECLARE_MESSAGE_MAP()
    afx_msg void OnContextMenu(CWnd* pWnd, CPoint pt);
};

#endif // __FX_MAIN_TOOL_BAR_H__
