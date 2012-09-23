//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FX_BAR_STATE_H__
#define __FX_BAR_STATE_H__
#pragma once

typedef std::map<xpr_uint_t, CControlBar *> BarMap;

class BarState
{
public:
    BarState(void);
    ~BarState(void);

public:
    void addBar(CControlBar *aControlBar);

public:
    xpr_bool_t loadBarState(CFrameWnd *aFrameWnd);
    xpr_bool_t saveBarState(CFrameWnd *aFrameWnd);

protected:
    BarMap mBarMap;
};

#endif // __FX_BAR_STATE_H__
