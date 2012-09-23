//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "SingleRenameDlg.h"

#include "fxb/fxb_rename_helper.h"
#include "fxb/fxb_sys_img_list.h"

#include "Option.h"
#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// user defined timer
enum
{
    TM_ID_SELITEM = 1,
};

SingleRenameDlg::SingleRenameDlg(void)
    : super(IDD_CREATE_ITEM)
    , mRenameHelper(new fxb::RenameHelper)
{
    setCtrlOffset(40);
}

SingleRenameDlg::~SingleRenameDlg(void)
{
    XPR_SAFE_DELETE(mRenameHelper);
}

void SingleRenameDlg::DoDataExchange(CDataExchange* pDX)
{
    super::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(SingleRenameDlg, super)
    ON_WM_TIMER()
END_MESSAGE_MAP()

xpr_bool_t SingleRenameDlg::OnInitDialog(void) 
{
    super::OnInitDialog();

    const xpr_tchar_t *sEditName = mRenameHelper->getEditName();
    xpr_sint_t sIconIndex = mRenameHelper->getIconIndex();
    HICON sIcon = fxb::SysImgListMgr::instance().mSysImgList32.ExtractIcon(sIconIndex);

    xpr_tchar_t sDesc[XPR_MAX_PATH * 2] = {0};
    _stprintf(sDesc, theApp.loadFormatString(XPR_STRING_LITERAL("popup.rename.label.top_desc"), XPR_STRING_LITERAL("%s")), sEditName);

    setTitle(theApp.loadString(XPR_STRING_LITERAL("popup.rename.title")));
    setDesc(sDesc);
    setDescIcon(sIcon, XPR_TRUE);

    CEdit *sNewNameCtrl = (CEdit *)GetDlgItem(IDC_NEW_NAME);
    if (sNewNameCtrl != XPR_NULL)
    {
        sNewNameCtrl->LimitText(XPR_MAX_PATH);
        sNewNameCtrl->SetWindowText(sEditName);
    }

    SetTimer(TM_ID_SELITEM, 0, XPR_NULL);

    return XPR_TRUE;
}

void SingleRenameDlg::setItem(LPSHELLFOLDER aShellFolder, LPITEMIDLIST aPidl, LPITEMIDLIST aFullPidl, xpr_ulong_t aAttributes)
{
    xpr_bool_t sCtrlKey = GetAsyncKeyState(VK_CONTROL) < 0;

    fxb::RenameHelper::RenameStyle sRenameStyle = fxb::RenameHelper::RenameStyleNone;
    if (sCtrlKey == XPR_FALSE)
    {
        switch (gOpt->mRenameExtType)
        {
        case RENAME_EXT_TYPE_KEEP:               sRenameStyle = fxb::RenameHelper::RenameStyleKeepExt;         break;
        case RENAME_EXT_TYPE_SEL_EXCEPT_FOR_EXT: sRenameStyle = fxb::RenameHelper::RenameStyleSelExceptForExt; break;
        }
    }

    mRenameHelper->setRenameStyle(sRenameStyle);
    mRenameHelper->setItem(aShellFolder, aPidl, aFullPidl, aAttributes);
}

const xpr_tchar_t *SingleRenameDlg::getNewName(void)
{
    return mRenameHelper->getNewName();
}

void SingleRenameDlg::OnTimer(xpr_uint_t nIDEvent) 
{
    if (nIDEvent == TM_ID_SELITEM)
    {
        KillTimer(TM_ID_SELITEM);

        CEdit *sNewNameCtrl = (CEdit *)GetDlgItem(IDC_NEW_NAME);
        if (sNewNameCtrl != XPR_NULL)
        {
            xpr_sint_t sSelItem0 = -1;
            xpr_sint_t sSelItem1 = -1;
            mRenameHelper->getEditSel(&sSelItem0, &sSelItem1);

            sNewNameCtrl->SetSel(sSelItem0, sSelItem1);
        }
    }

    super::OnTimer(nIDEvent);
}

void SingleRenameDlg::OnOK(void) 
{
    xpr_tchar_t sNewName[XPR_MAX_PATH + 1] = {0};
    GetDlgItemText(IDC_NEW_NAME, sNewName, XPR_MAX_PATH);

    fxb::RenameHelper::Result sResult = mRenameHelper->rename(m_hWnd, sNewName);
    if (sResult != fxb::RenameHelper::ResultSucceeded)
        return;

    super::OnOK();
}
