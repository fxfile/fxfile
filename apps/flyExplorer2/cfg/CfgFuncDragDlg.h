//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FX_CFG_FUNC_DRAG_DLG_H__
#define __FX_CFG_FUNC_DRAG_DLG_H__
#pragma once

#include "CfgDlg.h"

class CfgFuncDragDlg : public CfgDlg
{
    typedef CfgDlg super;

public:
    CfgFuncDragDlg(void);

protected:
    virtual void onInit(Option::Config &aConfig);
    virtual void onApply(Option::Config &aConfig);

protected:
    void enableWindowDrag(void);

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

protected:
    DECLARE_MESSAGE_MAP()
    virtual xpr_bool_t OnInitDialog(void);
};

#endif // __FX_CFG_FUNC_DRAG_DLG_H__
