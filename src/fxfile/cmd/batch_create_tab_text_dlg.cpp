//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "batch_create_tab_text_dlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace fxfile
{
namespace cmd
{
BatchCreateTabTextDlg::BatchCreateTabTextDlg(void)
    : super(IDD_BATCH_CREATE_TAB_TEXT)
{
}

void BatchCreateTabTextDlg::DoDataExchange(CDataExchange* pDX)
{
    super::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_CREATE_EDIT, mEditWnd);
}

BEGIN_MESSAGE_MAP(BatchCreateTabTextDlg, super)
    ON_WM_DESTROY()
END_MESSAGE_MAP()

xpr_bool_t BatchCreateTabTextDlg::OnInitDialog(void) 
{
    super::OnInitDialog();

    // CResizingDialog -------------------------------------------
    HideSizeIcon();

    //sizeNone:     Don't resize at all  
    //sizeResize:   The control will be stretched in the appropriate direction 
    //sizeRepos:    The control will be moved in the appropriate direction 
    //sizeRelative: The control will be moved proportionally in the appropriate direction 

    AddControl(IDC_CREATE_EDIT, sizeResize, sizeResize);
    //------------------------------------------------------------

    return XPR_TRUE;
}

void BatchCreateTabTextDlg::OnDestroy(void) 
{
    super::OnDestroy();

}

void BatchCreateTabTextDlg::OnTabInit(void)
{
}

xpr_sint_t BatchCreateTabTextDlg::getLineCount(void)
{
    return mEditWnd.GetLineCount();
}

xpr_bool_t BatchCreateTabTextDlg::getLineText(xpr_sint_t aLine, xpr_tchar_t *aText, xpr_sint_t aMaxLen)
{
    if (aText == XPR_NULL || aMaxLen <= 0)
        return XPR_FALSE;

    xpr_sint_t sLen = mEditWnd.LineLength(mEditWnd.LineIndex(aLine));
    if (sLen < 0)
        return XPR_FALSE;

    if (sLen > aMaxLen)
        return XPR_FALSE;

    aText[0] = 0;
    mEditWnd.GetLine(aLine, aText, sLen+1);
    aText[sLen] = 0;

    return XPR_TRUE;
}
} // namespace cmd
} // namespace fxfile
