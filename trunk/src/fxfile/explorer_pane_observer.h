//
// Copyright (c) 2014 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_EXPLORER_PANE_OBSERVER_H__
#define __FXFILE_EXPLORER_PANE_OBSERVER_H__ 1
#pragma once

#include "fxfile_def.h"
#include "folder_layout.h"

namespace fxfile
{
class ExplorerCtrl;

class ExplorerPaneObserver
{
public:
    virtual xpr_bool_t onLoadFolderLayout(ExplorerCtrl &aExplorerCtrl, FolderType aFolderType, FolderLayout &aFolderLayout) = 0;
    virtual void onFolderLayoutChange(ExplorerCtrl &aExplorerCtrl, const FolderLayoutChange &aFolderLayoutChange) = 0;
};
} // namespace fxfile

#endif // __FXFILE_EXPLORER_PANE_OBSERVER_H__
