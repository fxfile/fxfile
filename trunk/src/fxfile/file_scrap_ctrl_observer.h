//
// Copyright (c) 2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_FILE_SCRAP_CTRL_OBSERVER_H__
#define __FXFILE_FILE_SCRAP_CTRL_OBSERVER_H__ 1
#pragma once

namespace fxfile
{
class FileScrapCtrl;

class FileScrapCtrlObserver
{
public:
    virtual xpr_bool_t onOpenFolder(FileScrapCtrl &aFileScrapCtrl, const xpr_tchar_t *aDir, const xpr_tchar_t *aSelPath) = 0;
    virtual xpr_bool_t onOpenFolder(FileScrapCtrl &aFileScrapCtrl, LPCITEMIDLIST aFullPid) = 0;
    virtual void onUpdateStatus(FileScrapCtrl &aFileScrapCtrl) = 0;
    virtual void onSetFocus(FileScrapCtrl &aFileScrapCtrl) = 0;
    virtual void onMoveFocus(FileScrapCtrl &aFileScrapCtrl) = 0;
};
} // namespace fxfile

#endif // __FXFILE_FILE_SCRAP_CTRL_OBSERVER_H__
