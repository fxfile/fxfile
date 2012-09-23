//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FX_EXPLORER_CTRL_PRINT_H__
#define __FX_EXPLORER_CTRL_PRINT_H__
#pragma once

#include "rgc/ListCtrlPrint.h"

class ExplorerCtrlPrint : public ListCtrlPrint
{
public:
    ExplorerCtrlPrint(void);
    virtual ~ExplorerCtrlPrint(void);

public:
    virtual CString GetItemText(xpr_sint_t aRow, xpr_sint_t aColumn);
    virtual xpr_sint_t GetRowCount(void);
};

#endif // __FX_EXPLORER_CTRL_PRINT_H__
