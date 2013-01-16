//
// Copyright (c) 2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FX_FILE_SCRAP_TOOL_BAR_H__
#define __FX_FILE_SCRAP_TOOL_BAR_H__
#pragma once

#include "rgc/ToolBarEx.h"

class FileScrapToolBar : public CToolBarEx
{
    typedef CToolBarEx super;

public:
    FileScrapToolBar(void);
    virtual ~FileScrapToolBar(void);

public:
    virtual void Init(void);

public:
    void getToolBarSize(xpr_sint_t *aWidth, xpr_sint_t *aHeight);

protected:
    virtual BOOL HasButtonText( UINT nID );
    virtual BOOL HasButtonTip( UINT nID );
    virtual void GetButtonTextByCommand( UINT nID, CString& strText );

protected:
    afx_msg xpr_sint_t OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg xpr_bool_t OnEraseBkgnd(CDC* pDC);
    afx_msg void OnPaint(void);
    DECLARE_MESSAGE_MAP()
};

#endif // __FX_FILE_SCRAP_TOOL_BAR_H__
