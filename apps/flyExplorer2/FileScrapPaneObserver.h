//
// Copyright (c) 2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FX_FILE_SCRAP_PANE_OBSERVER_H__
#define __FX_FILE_SCRAP_PANE_OBSERVER_H__
#pragma once

class FileScrapPane;

class FileScrapPaneObserver
{
public:
    virtual void onCreated(FileScrapPane &aFileScrapPane) = 0;
    virtual void onDestroyed(FileScrapPane &aFileScrapPane) = 0;
    virtual void onSetFocus(FileScrapPane &aFileScrapPane) = 0;
    virtual xpr_bool_t onExplore(FileScrapPane &aFileScrapPane, const xpr_tchar_t *aDir, const xpr_tchar_t *aSelPath) = 0;
    virtual xpr_bool_t onExplore(FileScrapPane &aFileScrapPane, LPITEMIDLIST aFullPidl) = 0;
};

#endif // __FX_FILE_SCRAP_PANE_OBSERVER_H__
