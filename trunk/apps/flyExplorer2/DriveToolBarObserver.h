//
// Copyright (c) 2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FX_DRIVE_TOOL_BAR_OBSERVER_H__
#define __FX_DRIVE_TOOL_BAR_OBSERVER_H__
#pragma once

class DriveToolBar;

class DriveToolBarObserver : public DragDropToolBar
{
public:
    virtual void onDriveToolBarStatus(DriveToolBar &aDriveToolBar, xpr_tchar_t aDriveChar, DROPEFFECT aDropEffect) = 0;
};

#endif // __FX_DRIVE_TOOL_BAR_OBSERVER_H__
