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
	SizeGripWnd();
	virtual ~SizeGripWnd();

public:
	virtual BOOL Create(CWnd* pParentWnd, CRect rc, UINT nID);

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
};

#endif // __FX_SIZE_GRIP_WND_H__
