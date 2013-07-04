//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_FOLDER_VIEW_OBSERVER_H__
#define __FXFILE_FOLDER_VIEW_OBSERVER_H__ 1
#pragma once

namespace fxfile
{
class FolderView;

class FolderViewObserver
{
public:
    virtual xpr_bool_t onExplore(FolderView &aFolderView, LPITEMIDLIST aFullPidl, xpr_bool_t aUpdateBuddy) = 0;
    virtual void onRename(FolderView &aFolderView, HTREEITEM aTreeItem) = 0;
    virtual void onContextMenuDelete(FolderView &aFolderView, HTREEITEM aTreeItem) = 0;
    virtual void onContextMenuRename(FolderView &aFolderView, HTREEITEM aTreeItem) = 0;
    virtual void onContextMenuPaste(FolderView &aFolderView, HTREEITEM aTreeItem) = 0;
    virtual xpr_sint_t onDrop(FolderView &aFolderView, COleDataObject *aOleDataObject, xpr_tchar_t *aTargetDir) = 0;
    virtual void onMoveFocus(FolderView &aFolderView, xpr_sint_t aCurWnd) = 0;
};
} // namespace fxfile

#endif // __FXFILE_FOLDER_VIEW_OBSERVER_H__
