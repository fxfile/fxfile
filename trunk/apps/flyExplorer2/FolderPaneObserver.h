//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FX_FOLDER_PANE_OBSERVER_H__
#define __FX_FOLDER_PANE_OBSERVER_H__
#pragma once

class FolderPane;

class FolderPaneObserver
{
public:
    virtual xpr_bool_t onExplore(FolderPane &aFolderPane, LPITEMIDLIST aFullPidl, xpr_bool_t aUpdateBuddy) = 0;
    virtual void onUpdateStatus(FolderPane &aFolderPane) = 0;
    virtual void onRename(FolderPane &aFolderPane, HTREEITEM aTreeItem) = 0;
    virtual void onContextMenuDelete(FolderPane &aFolderPane, HTREEITEM aTreeItem) = 0;
    virtual void onContextMenuRename(FolderPane &aFolderPane, HTREEITEM aTreeItem) = 0;
    virtual void onContextMenuPaste(FolderPane &aFolderPane, HTREEITEM aTreeItem) = 0;
    virtual xpr_sint_t onDrop(FolderPane &aFolderPane, COleDataObject *aOleDataObject, xpr_tchar_t *aTargetDir) = 0;
    virtual void onSetFocus(FolderPane &aFolderPane) = 0;
    virtual void onMoveFocus(FolderPane &aFolderPane, xpr_sint_t aCurWnd) = 0;
};

#endif // __FX_FOLDER_PANE_OBSERVER_H__
