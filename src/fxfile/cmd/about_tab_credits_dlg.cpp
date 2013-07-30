//
// Copyright (c) 2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "about_tab_credits_dlg.h"
#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace fxfile;
using namespace fxfile::base;

namespace fxfile
{
namespace cmd
{
AboutTabCreditsDlg::AboutTabCreditsDlg(void)
    : super(IDD_ABOUT_TAB_CREDITS)
{
}

void AboutTabCreditsDlg::DoDataExchange(CDataExchange* pDX)
{
    super::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(AboutTabCreditsDlg, super)
    ON_WM_SIZE()
END_MESSAGE_MAP()

xpr_bool_t AboutTabCreditsDlg::OnInitDialog(void) 
{
    super::OnInitDialog();

    HANDLE sResource = FindResource(AfxGetResourceHandle(), (xpr_tchar_t *)IDR_CREDITS, XPR_STRING_LITERAL("TEXT"));
    if (XPR_IS_NOT_NULL(sResource))
    {
        HGLOBAL sText = LoadResource(AfxGetResourceHandle(), (HRSRC)sResource);
        if (XPR_IS_NOT_NULL(sText))
        {
            xpr_char_t *sResourceText = (xpr_char_t *)LockResource(sText);
            if (XPR_IS_NOT_NULL(sResourceText))
            {
                xpr_size_t sResourceTextLength = strlen(sResourceText);

                xpr_tchar_t *sCreditsText = new xpr_tchar_t[sResourceTextLength + 1];

                xpr_size_t sInputBytes = sResourceTextLength * sizeof(xpr_char_t);
                xpr_size_t sOutputBytes = sResourceTextLength * sizeof(xpr_tchar_t);
                XPR_MBS_TO_TCS(sResourceText, &sInputBytes, sCreditsText, &sOutputBytes);
                sCreditsText[sOutputBytes / sizeof(xpr_tchar_t)] = 0;

                GetDlgItem(IDC_ABOUT_CREDITS)->SetWindowText(sCreditsText);

                XPR_SAFE_DELETE_ARRAY(sCreditsText);

                UnlockResource(sText);
            }
        }

        FreeResource(sResource);
    }

    return XPR_TRUE;
}

void AboutTabCreditsDlg::OnSize(xpr_uint_t aType, xpr_sint_t cx, xpr_sint_t cy)
{
    super::OnSize(aType, cx, cy);

    if (GetDlgItem(IDC_ABOUT_CREDITS) != XPR_NULL)
    {
        GetDlgItem(IDC_ABOUT_CREDITS)->MoveWindow(5, 5, cx-10, cy-10);
    }
}
} // namespace cmd
} // namespace fxfile
