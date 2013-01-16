//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FX_FOLDER_PANE_H__
#define __FX_FOLDER_PANE_H__
#pragma once

#include "FolderCtrlObserver.h"

class FolderCtrl;
class FolderPaneObserver;

class FolderPane : public CWnd, public FolderCtrlObserver
{
    typedef CWnd super;

public:
    FolderPane(void);
    virtual ~FolderPane(void);

public:
    void setObserver(FolderPaneObserver *aObserver);

public:
    xpr_bool_t Create(CWnd *aParentWnd, xpr_uint_t aId, const RECT &aRect);

public:
    void setViewIndex(xpr_sint_t aViewIndex);
    xpr_sint_t getViewIndex(void) const;

    FolderCtrl *getFolderCtrl(void) const;

protected:
    void recalcLayout(void);

protected:
    // from FolderCtrlObserver
    virtual xpr_bool_t onExplore(FolderCtrl &aFolderCtrl, LPITEMIDLIST aFullPidl, xpr_bool_t aUpdateBuddy);
    virtual void onUpdateStatus(FolderCtrl &aFolderCtrl);
    virtual void onRename(FolderCtrl &aFolderCtrl, HTREEITEM aTreeItem);
    virtual void onContextMenuDelete(FolderCtrl &aFolderCtrl, HTREEITEM aTreeItem);
    virtual void onContextMenuRename(FolderCtrl &aFolderCtrl, HTREEITEM aTreeItem);
    virtual void onContextMenuPaste(FolderCtrl &aFolderCtrl, HTREEITEM aTreeItem);
    virtual xpr_sint_t onDrop(FolderCtrl &aFolderCtrl, COleDataObject *aOleDataObject, xpr_tchar_t *aTargetDir);
    virtual void onMoveFocus(FolderCtrl &aFolderCtrl);

protected:
    FolderPaneObserver *mObserver;

    xpr_sint_t mViewIndex;

    FolderCtrl *mFolderCtrl;

protected:
    DECLARE_MESSAGE_MAP()
    afx_msg xpr_sint_t OnCreate(LPCREATESTRUCT aCreateStruct);
    afx_msg void OnDestroy(void);
    afx_msg void OnSize(xpr_uint_t aType, xpr_sint_t cx, xpr_sint_t cy);
    afx_msg void OnSetFocus(CWnd *aOldWnd);
    afx_msg void OnFolderSetFocus(LPNMHDR aNmHdr, LRESULT *aResult);
};

#endif // __FX_FOLDER_PANE_H__
