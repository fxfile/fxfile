//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FX_PATH_BAR_OBSERVER_H__
#define __FX_PATH_BAR_OBSERVER_H__
#pragma once

class PathBar;

class PathBarObserver
{
public:
    virtual xpr_bool_t onExplore(PathBar &aPathBar, LPITEMIDLIST aFullPidl) = 0;
};

#endif // __FX_PATH_BAR_OBSERVER_H__
