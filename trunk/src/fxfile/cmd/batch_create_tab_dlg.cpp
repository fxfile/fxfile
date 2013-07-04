//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "batch_create_tab_dlg.h"

#include "batch_create_dlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace fxfile
{
namespace cmd
{
BatchCreateTabDlg::BatchCreateTabDlg(xpr_uint_t aResourceId)
    : super(aResourceId, XPR_NULL)
    , mResourceId(aResourceId)
{
}

void BatchCreateTabDlg::DoDataExchange(CDataExchange* pDX)
{
    super::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(BatchCreateTabDlg, super)
END_MESSAGE_MAP()

xpr_bool_t BatchCreateTabDlg::create(CWnd *sParentWnd) 
{
    return super::Create(mResourceId, sParentWnd);
}

xpr_bool_t BatchCreateTabDlg::PreTranslateMessage(MSG* pMsg) 
{
    static xpr_tchar_t sClassName[MAX_CLASS_NAME + 1] = {0};
    sClassName[0] = XPR_STRING_LITERAL('\0');

    // Skip Edit Control
    if (pMsg->message == WM_KEYDOWN)
    {
        xpr_bool_t sSkip = XPR_FALSE;

        switch (pMsg->wParam)
        {
        case VK_ESCAPE: sSkip = XPR_TRUE;
        }

        if (sSkip == XPR_TRUE)
        {
            static const xpr_tchar_t *sEditClassName = XPR_STRING_LITERAL("Edit");
            ::GetClassName(pMsg->hwnd, sClassName, MAX_CLASS_NAME);
            if (_tcsicmp(sClassName, sEditClassName) == 0)
                return XPR_FALSE;
        }
    }

    if (((BatchCreateDlg *)GetParent())->translateHotKey(pMsg))
        return XPR_TRUE;

    return super::PreTranslateMessage(pMsg);
}

void BatchCreateTabDlg::OnTabInit(void)
{
}
} // namespace cmd
} // namespace fxfile
