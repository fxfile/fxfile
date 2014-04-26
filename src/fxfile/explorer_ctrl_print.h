//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_EXPLORER_CTRL_PRINT_H__
#define __FXFILE_EXPLORER_CTRL_PRINT_H__ 1
#pragma once

#include "gui/ListCtrlPrint.h"

namespace fxfile
{
class ExplorerCtrlPrint : public ListCtrlPrint
{
public:
    ExplorerCtrlPrint(void);
    virtual ~ExplorerCtrlPrint(void);

public:
    virtual void getItemText(xpr_sint_t aRow, xpr_sint_t aColumn, xpr::string &aText) const;
    virtual xpr_sint_t getRowCount(void) const;
};
} // namespace fxfile

#endif // __FXFILE_EXPLORER_CTRL_PRINT_H__
