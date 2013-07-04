//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "wait_dlg.h"

#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace fxfile
{
WaitDlg::WaitDlg(void)
: super(IDD_WAIT)
{
}

WaitDlg::~WaitDlg(void)
{
}

void WaitDlg::DoDataExchange(CDataExchange* pDX)
{
    super::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(WaitDlg, super)
    ON_BN_CLICKED(IDC_STOP, OnStop)
END_MESSAGE_MAP()

xpr_bool_t WaitDlg::OnInitDialog(void) 
{
    super::OnInitDialog();

    SetWindowText(mTitle.c_str());
    SetDlgItemText(IDC_WAIT_DESC, theApp.loadString(XPR_STRING_LITERAL("popup.wait.label.desc")));
    SetDlgItemText(IDC_STOP,      theApp.loadString(XPR_STRING_LITERAL("popup.wait.button.stop")));

    return XPR_TRUE;
}

void WaitDlg::setTitle(const xpr_tchar_t *aTitle)
{
    if (aTitle != XPR_NULL)
        mTitle = aTitle;
}

void WaitDlg::end(void)
{
    if (!m_hWnd)
        return;

    OnOK();
}

xpr_bool_t WaitDlg::isStopped(void)
{
    return (m_hWnd == XPR_NULL);
}

void WaitDlg::OnStop(void)
{
    OnCancel();
}
} // namespace fxfile
