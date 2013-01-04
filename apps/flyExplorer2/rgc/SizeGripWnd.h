//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FX_SIZE_GRIP_WND_H__
#define __FX_SIZE_GRIP_WND_H__
#pragma once

class SizeGripWnd : public CWnd
{
public:
    SizeGripWnd(void);
    virtual ~SizeGripWnd(void);

public:
    virtual xpr_bool_t Create(CWnd *aParentWnd, CRect aRect, xpr_uint_t aId);

protected:
    DECLARE_MESSAGE_MAP()
    afx_msg xpr_bool_t OnSetCursor(CWnd *aWnd, xpr_uint_t aHitTest, xpr_uint_t aMessage);
};

#endif // __FX_SIZE_GRIP_WND_H__
