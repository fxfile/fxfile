//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FX_FOLDER_CTRL_OBSERVER_H__
#define __FX_FOLDER_CTRL_OBSERVER_H__
#pragma once

class FolderCtrl;

class FolderCtrlObserver
{
public:
    virtual xpr_bool_t onExplore(FolderCtrl &aFolderCtrl, LPITEMIDLIST aFullPidl, xpr_bool_t aUpdateBuddy) = 0;
    virtual void onUpdateStatus(FolderCtrl &aFolderCtrl) = 0;
    virtual void onRename(FolderCtrl &aFolderCtrl, HTREEITEM aTreeItem) = 0;
    virtual void onContextMenuDelete(FolderCtrl &aFolderCtrl, HTREEITEM aTreeItem) = 0;
    virtual void onContextMenuRename(FolderCtrl &aFolderCtrl, HTREEITEM aTreeItem) = 0;
    virtual void onContextMenuPaste(FolderCtrl &aFolderCtrl, HTREEITEM aTreeItem) = 0;
    virtual xpr_sint_t onDrop(FolderCtrl &aFolderCtrl, COleDataObject *aOleDataObject, xpr_tchar_t *aTargetDir) = 0;
    virtual void onMoveFocus(FolderCtrl &aFolderCtrl) = 0;
};

#endif // __FX_FOLDER_CTRL_OBSERVER_H__
