//
// Copyright (c) 2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_STATUS_BAR_OBSERVER_H__
#define __FXFILE_STATUS_BAR_OBSERVER_H__
#pragma once

class StatusBar;

class StatusBarObserver
{
public:
    virtual void onStatusBarRemove(StatusBar &aStatusBar, xpr_size_t aIndex) = 0;
    virtual void onStatusBarRemoved(StatusBar &aStatusBar) = 0;
    virtual void onStatusBarRemoveAll(StatusBar &aStatusBar) = 0;
    virtual void onStatusBarDoubleClicked(StatusBar &aStatusBar, xpr_size_t sIndex) = 0;
    virtual void onStatuBarContextMenu(StatusBar &aStatusBar, xpr_size_t aIndex, const POINT &aPoint) = 0;
};

#endif // __FXFILE_STATUS_BAR_OBSERVER_H__
