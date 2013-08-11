//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "explorer_ctrl_print.h"

#include "option.h"
#include "explorer_ctrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace fxfile
{
ExplorerCtrlPrint::ExplorerCtrlPrint(void)
{
}

ExplorerCtrlPrint::~ExplorerCtrlPrint(void)
{
}

xpr_sint_t ExplorerCtrlPrint::getRowCount(void) const
{
    if (XPR_IS_NULL(mListCtrl))
        return 0;

    ExplorerCtrl *sExplorerCtrl = dynamic_cast<ExplorerCtrl *>(mListCtrl);
    if (XPR_IS_NULL(sExplorerCtrl))
        return mListCtrl->GetItemCount();

    xpr_sint_t i;
    xpr_sint_t sCount;

    sCount = mListCtrl->GetItemCount();
    for (i = sCount - 1; i >= 0; --i)
    {
        if (sExplorerCtrl->isShellItem(i) == XPR_TRUE)
        {
            sCount = i;
            break;
        }
    }

    return sCount;
}

CString ExplorerCtrlPrint::getItemText(xpr_sint_t aRow, xpr_sint_t aColumn) const
{
    ASSERT(aColumn >= 0 && aColumn < mPageColumns);
    ASSERT(aRow >= 0 && aRow < mRowCount);

    aRow++;

    return mListCtrl->GetItemText(aRow, mColumns[aColumn]);
}
} // namespace fxfile
