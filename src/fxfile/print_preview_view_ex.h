//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_PRINT_PREVIEW_VIEW_EX_H__
#define __FXFILE_PRINT_PREVIEW_VIEW_EX_H__ 1
#pragma once

#include <afxpriv.h>
#include "gui/BtnST.h"

namespace fxfile
{
class PrintPreviewViewEx : public CPreviewView
{
    typedef CPreviewView super;

    DECLARE_DYNCREATE(PrintPreviewViewEx)

protected:
    PrintPreviewViewEx();
    virtual ~PrintPreviewViewEx();

public:
    void Close(void);

protected:
    CButtonST mPrintButton;
    CButtonST mPrevButton;
    CButtonST mNextButton;
    CButtonST mZoomInButton;
    CButtonST mZoomOutButton;
    CButtonST mCloseButton;

protected:
    virtual xpr_bool_t PreCreateWindow(CREATESTRUCT& cs);

#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

protected:
    afx_msg xpr_sint_t OnCreate(LPCREATESTRUCT lpCreateStruct);
    DECLARE_MESSAGE_MAP()
};
} // namespace fxfile

#endif // __FXFILE_PRINT_PREVIEW_VIEW_EX_H__
