//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_TAB_CTRL_OBSERVER_H__
#define __FXFILE_TAB_CTRL_OBSERVER_H__
#pragma once

class TabCtrl;

class TabCtrlObserver
{
public:
    virtual void       onTabInserted(TabCtrl &aTabCtrl, xpr_size_t aTab) = 0;
    virtual xpr_bool_t onTabChangingCurTab(TabCtrl &aTabCtrl, xpr_size_t aOldTab, xpr_size_t aNewTab) = 0;
    virtual void       onTabChangedCurTab(TabCtrl &aTabCtrl, xpr_size_t aOldTab, xpr_size_t aNewTab) = 0;
    virtual void       onTabRemove(TabCtrl &aTabCtrl, xpr_size_t aTab) = 0;
    virtual void       onTabRemoved(TabCtrl &aTabCtrl) = 0;
    virtual void       onTabRemoveAll(TabCtrl &aTabCtrl) = 0;
    virtual void       onTabEmptied(TabCtrl &aTabCtrl) = 0;
    virtual void       onTabDoubleClicked(TabCtrl &aTabCtrl, xpr_size_t aTab) = 0;
    virtual void       onTabContextMenu(TabCtrl &aTabCtrl, xpr_size_t aTab, const POINT &aPoint) = 0;
};

#endif // __FXFILE_TAB_CTRL_OBSERVER_H__
