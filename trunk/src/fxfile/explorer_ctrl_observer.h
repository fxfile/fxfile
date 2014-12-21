//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_EXPLORER_CTRL_OBSERVER_H__
#define __FXFILE_EXPLORER_CTRL_OBSERVER_H__ 1
#pragma once

#include "fxfile_def.h"
#include "folder_layout.h"

namespace fxfile
{
class ExplorerCtrl;

class ExplorerCtrlObserver
{
public:
    virtual void onExplore(ExplorerCtrl &aExplorerCtrl, LPITEMIDLIST aFullPidl, xpr_bool_t aUpdateBuddy, const xpr_tchar_t *aCurPath) = 0;
    virtual xpr_bool_t onLoadFolderLayout(ExplorerCtrl &aExplorerCtrl, FolderType aFolderType, FolderLayout &aFolderLayout) = 0;
    virtual void onSaveFolderLayout(ExplorerCtrl &aExplorerCtrl, FolderType aFolderType, xpr_sint_t aSaveFolderLayout, const FolderLayout &aFolderLayout) = 0;
    virtual void onUpdateStatus(ExplorerCtrl &aExplorerCtrl, xpr_size_t aRealSelCount) = 0;
    virtual void onUpdateFreeSize(ExplorerCtrl &aExplorerCtrl) = 0;
    virtual void onSetFocus(ExplorerCtrl &aExplorerCtrl) = 0;
    virtual void onKillFocus(ExplorerCtrl &aExplorerCtrl) = 0;
    virtual void onRename(ExplorerCtrl &aExplorerCtrl) = 0;
    virtual void onContextMenuDelete(ExplorerCtrl &aExplorerCtrl) = 0;
    virtual void onContextMenuRename(ExplorerCtrl &aExplorerCtrl) = 0;
    virtual void onContextMenuPaste(ExplorerCtrl &aExplorerCtrl) = 0;
    virtual void onGoDrive(ExplorerCtrl &aExplorerCtrl) = 0;
    virtual xpr_sint_t onDrop(ExplorerCtrl &aExplorerCtrl, COleDataObject *aOleDataObject, xpr_tchar_t *aTargetDir) = 0;
    virtual xpr_bool_t onGetExecParam(ExplorerCtrl &aExplorerCtrl, const xpr_tchar_t *aPath, xpr_tchar_t *aParam, xpr_size_t aMaxLen) = 0;
    virtual void onExecError(ExplorerCtrl &aExplorerCtrl, const xpr_tchar_t *aPath) = 0;
    virtual void onFolderLayoutChange(ExplorerCtrl &aExplorerCtrl, const FolderLayoutChange &aFolderLayoutChange) = 0;
    virtual void onMoveFocus(ExplorerCtrl &aExplorerCtrl) = 0;
    virtual void onRunFile(ExplorerCtrl &aExplorerCtrl, LPITEMIDLIST aFullPidl) = 0;
};
} // namespace fxfile

#endif // __FXFILE_EXPLORER_CTRL_OBSERVER_H__
