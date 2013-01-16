//
// Copyright (c) 2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FX_FILE_SCRAP_CTRL_OBSERVER_H__
#define __FX_FILE_SCRAP_CTRL_OBSERVER_H__
#pragma once

class FileScrapCtrl;

class FileScrapCtrlObserver
{
public:
    virtual void onSetFocus(FileScrapCtrl &aFileScrapCtrl) = 0;
    virtual xpr_bool_t onExplore(FileScrapCtrl &aFileScrapCtrl, const xpr_tchar_t *aDir, const xpr_tchar_t *aSelPath) = 0;
    virtual xpr_bool_t onExplore(FileScrapCtrl &aFileScrapCtrl, LPITEMIDLIST aFullPid) = 0;
    virtual void onUpdateStatus(FileScrapCtrl &aFileScrapCtrl) = 0;
};

#endif // __FX_FILE_SCRAP_CTRL_OBSERVER_H__
