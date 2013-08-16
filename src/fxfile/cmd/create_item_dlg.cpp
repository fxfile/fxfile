//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "create_item_dlg.h"

#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace fxfile
{
namespace cmd
{
CreateItemDlg::CreateItemDlg(void)
    : super(IDD_CREATE_ITEM)
    , mExistCheck(XPR_FALSE)
{
    setCtrlOffset(40);
}

void CreateItemDlg::DoDataExchange(CDataExchange* pDX)
{
    super::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CreateItemDlg, super)
END_MESSAGE_MAP()

xpr_bool_t CreateItemDlg::OnInitDialog(void) 
{
    super::OnInitDialog();

    CEdit *sNewNameCtrl = (CEdit *)GetDlgItem(IDC_NEW_NAME);
    if (sNewNameCtrl != XPR_NULL)
    {
        sNewNameCtrl->LimitText(XPR_MAX_PATH);
        sNewNameCtrl->SetWindowText(mPath.c_str());
    }

    return XPR_TRUE;
}

const xpr_tchar_t *CreateItemDlg::getNewItem(void)
{
    return mPath.c_str();
}

void CreateItemDlg::setNewItem(const xpr_tchar_t *aPath)
{
    if (aPath == XPR_NULL)
        return;

    mPath = aPath;
}

void CreateItemDlg::setDir(const xpr_tchar_t *aDir)
{
    if (aDir == XPR_NULL)
        return;

    mDir = aDir;
}

void CreateItemDlg::setExt(const xpr_tchar_t *aExt)
{
    if (aExt == XPR_NULL)
        return;

    mExt = aExt;
}

void CreateItemDlg::setExistCheck(xpr_bool_t aExistCheck)
{
    mExistCheck = aExistCheck;
}

void CreateItemDlg::setMsg(xpr_uint_t aId, const xpr_tchar_t *aMsg)
{
    if (aMsg == XPR_NULL)
        return;

    mMsgMap[aId] = aMsg;
}

const xpr_tchar_t *CreateItemDlg::getMsg(xpr_uint_t aId)
{
    MsgMap::iterator sIterator = mMsgMap.find(aId);
    if (sIterator == mMsgMap.end())
        return XPR_NULL;

    return sIterator->second.c_str();
}

void CreateItemDlg::OnOK(void) 
{
    xpr_tchar_t sNewName[XPR_MAX_PATH + 1] = {0};
    GetDlgItemText(IDC_NEW_NAME, sNewName, XPR_MAX_PATH);

    const xpr_tchar_t *sSetMsg = getMsg(MSG_ID_EMPTY);
    if (sSetMsg != XPR_NULL && _tcslen(sNewName) <= 0)
    {
        MessageBox(sSetMsg, XPR_NULL, MB_OK | MB_ICONWARNING);

        GetDlgItem(IDC_NEW_NAME)->SetFocus();
        return;
    }

    if (VerifyFileName(sNewName) == XPR_FALSE)
    {
        const xpr_tchar_t *sMsg = gApp.loadString(XPR_STRING_LITERAL("popup.batch_create.msg.wrong_filename"));
        MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);
        return;
    }

    xpr::tstring sPath;
    sPath = mDir;

    if (sPath.empty() == XPR_FALSE)
    {
        if (sPath.at(sPath.length()-1) != XPR_STRING_LITERAL('\\'))
            sPath += XPR_STRING_LITERAL('\\');
    }

    sPath += sNewName;

    if (mExt.empty() == XPR_FALSE)
    {
        if (mExt[0] != XPR_STRING_LITERAL('.'))
            sPath += XPR_STRING_LITERAL('.');

        sPath += mExt;
    }

    sSetMsg = getMsg(MSG_ID_EXIST);
    if (sSetMsg != XPR_NULL)
    {
        if (IsExistFile(sPath.c_str()) == XPR_TRUE)
        {
            xpr_tchar_t sMsg[XPR_MAX_PATH * 2] = {0};
            _stprintf(sMsg, XPR_STRING_LITERAL("%s\n\n%s"), sSetMsg, sPath.c_str());
            MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);

            CEdit *sEditCtrl = (CEdit *)GetDlgItem(IDC_NEW_NAME);
            sEditCtrl->SetSel(0, -1);
            sEditCtrl->SetFocus();
            return;
        }
    }

    mPath = sPath;

    super::OnOK();
}
} // namespace cmd
} // namespace fxfile
