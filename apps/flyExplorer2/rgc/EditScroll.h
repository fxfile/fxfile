//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FX_EDIT_SCROLL_H__
#define __FX_EDIT_SCROLL_H__
#pragma once

class EditScroll : public CEdit
{
public:
    EditScroll(void);
    virtual ~EditScroll(void);

protected:
    DECLARE_MESSAGE_MAP()
    afx_msg void OnVScroll(xpr_uint_t aSBCode, xpr_uint_t aPos, CScrollBar *aScrollBar);
};

#endif // __FX_EDIT_SCROLL_H__
