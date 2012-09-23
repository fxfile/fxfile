//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "ExplorerCtrlPrint.h"

#include "Option.h"
#include "ExplorerCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

ExplorerCtrlPrint::ExplorerCtrlPrint(void)
{
}

ExplorerCtrlPrint::~ExplorerCtrlPrint(void)
{
}

xpr_sint_t ExplorerCtrlPrint::GetRowCount(void)
{
    if (XPR_IS_NULL(mListCtrl))
        return 0;

    ExplorerCtrl *sExplorerCtrl = dynamic_cast<ExplorerCtrl *>(mListCtrl);
    if (XPR_IS_NULL(sExplorerCtrl))
        return mListCtrl->GetItemCount();

    xpr_sint_t sCount = mListCtrl->GetItemCount();
    if (gOpt->mExplorerParentFolder == XPR_TRUE)
        sCount--;

    if (gOpt->mExplorerShowDrive == XPR_TRUE || gOpt->mExplorerShowDriveItem == XPR_TRUE)
    {
        xpr_sint_t i;
        for (i = sCount - 1; i >= 0; --i)
        {
            if (sExplorerCtrl->isShellItem(i) == XPR_TRUE)
            {
                sCount = i;
                break;
            }
        }
    }

    return sCount;
}

CString ExplorerCtrlPrint::GetItemText(xpr_sint_t aRow, xpr_sint_t aColumn)
{
    ASSERT(aColumn >= 0 && aColumn < mPageColumns);
    ASSERT(aRow >= 0 && aRow < mRowCount);

    aRow++;

    return mListCtrl->GetItemText(aRow, mColumns[aColumn]);
}
