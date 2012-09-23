//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "CfgAdvStateDlg.h"

#include "../resource.h"
#include "../DlgStateMgr.h"
#include "../CfgPath.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CfgAdvStateDlg::CfgAdvStateDlg(void)
    : CfgDlg(IDD_CFG_ADV_STATE, XPR_NULL)
{
}

void CfgAdvStateDlg::DoDataExchange(CDataExchange* pDX)
{
    CfgDlg::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_CFG_ADV_STATE_LIST, mListCtrl);
}

BEGIN_MESSAGE_MAP(CfgAdvStateDlg, CfgDlg)
    ON_NOTIFY(LVN_GETDISPINFO, IDC_CFG_ADV_STATE_LIST, OnLvnGetdispinfoList)
    ON_BN_CLICKED(IDC_CFG_ADV_STATE_INIT,     OnInit)
    ON_BN_CLICKED(IDC_CFG_ADV_STATE_INIT_ALL, OnInitAll)
END_MESSAGE_MAP()

xpr_bool_t CfgAdvStateDlg::OnInitDialog(void) 
{
    CfgDlg::OnInitDialog();

    CBitmap sBitmap;
    sBitmap.LoadBitmap(IDB_CONFIG_TREE);
    mImageList.Create(16, 16, ILC_COLORDDB | ILC_MASK, 18, 18);
    mImageList.Add(&sBitmap, RGB(255,0,255));
    sBitmap.DeleteObject();

    mListCtrl.SetImageList(&mImageList, LVSIL_SMALL);

    DWORD sExStyle = mListCtrl.GetExtendedStyle();
    sExStyle |= LVS_EX_FULLROWSELECT;
    mListCtrl.SetExtendedStyle(sExStyle);

    mListCtrl.InsertColumn(0, theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.advanced.dialog_state.list.column.item")), LVCFMT_LEFT, 170, -1);
    mListCtrl.InsertColumn(1, theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.advanced.dialog_state.list.column.file")), LVCFMT_LEFT, 200, -1);

    xpr_uint_t sType;
    xpr_sint_t sIndex = 0;
    LVITEM sLvItem = {0};

    for (sType = DlgStateMgr::TypeBegin; sType < DlgStateMgr::TypeEnd; ++sType)
    {
        sLvItem.mask     = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
        sLvItem.iItem    = sIndex++;
        sLvItem.iSubItem = 0;
        sLvItem.iImage   = I_IMAGECALLBACK;
        sLvItem.pszText  = LPSTR_TEXTCALLBACK;
        sLvItem.lParam   = (LPARAM)sType;
        mListCtrl.InsertItem(&sLvItem);
    }

    std::tstring sDir;
    CfgPath::instance().getCfgPath(CfgPath::TypeSettings, sDir);
    SetDlgItemText(IDC_CFG_ADV_STATE_PATH, sDir.c_str());

    SetDlgItemText(IDC_CFG_ADV_STATE_LABEL_PATH, theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.advanced.dialog_state.label.path")));
    SetDlgItemText(IDC_CFG_ADV_STATE_LABEL_LIST, theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.advanced.dialog_state.label.list")));
    SetDlgItemText(IDC_CFG_ADV_STATE_INIT_ALL,   theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.advanced.dialog_state.button.initialize_all")));
    SetDlgItemText(IDC_CFG_ADV_STATE_INIT,       theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.advanced.dialog_state.button.initialize")));

    return XPR_TRUE;
}

void CfgAdvStateDlg::OnApply(void)
{
}

void CfgAdvStateDlg::OnLvnGetdispinfoList(NMHDR *pNMHDR, LRESULT *pResult)
{
    NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
    *pResult = 0;

    xpr_uint_t sType = (xpr_uint_t)pDispInfo->item.lParam;
    LVITEM &sLvItem = pDispInfo->item;

    if (XPR_TEST_BITS(sLvItem.mask, LVIF_TEXT))
    {
        sLvItem.pszText[0] = XPR_STRING_LITERAL('\0');

        switch (sLvItem.iSubItem)
        {
        case 0:
            {
                std::tstring sName;
                DlgStateMgr::instance().getName(sType, sName);

                _tcscpy(sLvItem.pszText, sName.c_str());
                break;
            }

        case 1:
            {
                DlgStateMgr::PathDeque sPathDeque;
                DlgStateMgr::instance().getPathList(sType, sPathDeque);

                DlgStateMgr::PathDeque::iterator sIterator = sPathDeque.begin();
                for (; sIterator != sPathDeque.end(); ++sIterator)
                {
                    _tcscat(sLvItem.pszText, sIterator->c_str());
                    _tcscat(sLvItem.pszText, XPR_STRING_LITERAL(";"));
                }

                // remove last semi-colon
                if (sLvItem.pszText[0] != XPR_STRING_LITERAL('\0'))
                    sLvItem.pszText[_tcslen(sLvItem.pszText) - 1] = XPR_STRING_LITERAL('\0');
            }
        }
    }

    if (XPR_TEST_BITS(sLvItem.mask, LVIF_IMAGE))
    {
        sLvItem.iImage = 23;
    }
}

xpr_bool_t CfgAdvStateDlg::PreTranslateMessage(MSG* pMsg)
{
    if (pMsg->message == WM_KEYDOWN)
    {
        if (pMsg->hwnd == mListCtrl.m_hWnd)
        {
            switch (pMsg->wParam)
            {
            case VK_DELETE:
                {
                    OnInit();
                    return XPR_TRUE;
                }

            case 'a':
            case 'A':
                {
                    if (GetKeyState(VK_CONTROL) < 0)
                    {
                        xpr_sint_t i;
                        xpr_sint_t sCount;

                        sCount = mListCtrl.GetItemCount();
                        for (i = 0; i < sCount; ++i)
                            mListCtrl.SetItemState(i, LVIS_SELECTED, LVIS_SELECTED);

                        return XPR_TRUE;
                    }
                }
            }
        }
    }

    return CDialog::PreTranslateMessage(pMsg);
}

void CfgAdvStateDlg::OnInit(void)
{
    if (mListCtrl.GetSelectedCount() <= 0)
    {
        const xpr_tchar_t *sMsg = theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.advanced.dialog_state.msg.select"));
        MessageBox(sMsg, XPR_NULL, MB_ICONSTOP | MB_OK);
        return;
    }

    const xpr_tchar_t *sMsg = theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.advanced.dialog_state.msg.confirm_init"));
    xpr_sint_t sMsgId = MessageBox(sMsg, XPR_NULL, MB_ICONQUESTION | MB_YESNO);
    if (sMsgId != IDYES)
        return;

    POSITION sPosition = mListCtrl.GetFirstSelectedItemPosition();
    while (XPR_IS_NOT_NULL(sPosition))
    {
        xpr_sint_t sIndex = mListCtrl.GetNextSelectedItem(sPosition);
        if (sIndex < 0)
            continue;

        xpr_uint_t sType = (xpr_uint_t)mListCtrl.GetItemData(sIndex);
        DlgStateMgr::instance().clear(sType);
    }
}

void CfgAdvStateDlg::OnInitAll(void)
{
    const xpr_tchar_t *sMsg = theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.advanced.dialog_state.msg.confirm_init_all"));
    xpr_sint_t sMsgId = MessageBox(sMsg, XPR_NULL, MB_ICONQUESTION | MB_YESNO);
    if (sMsgId != IDYES)
        return;

    DlgStateMgr::instance().clearAll();
}
