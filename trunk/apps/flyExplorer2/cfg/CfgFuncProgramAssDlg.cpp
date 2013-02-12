//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "CfgFuncProgramAssDlg.h"

#include "../fxb/fxb_program_ass.h"
#include "../fxb/fxb_filter.h"
#include "../fxb/fxb_sys_img_list.h"

#include "../resource.h"

#include "ProgramAssSetDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CfgFuncProgramAssDlg::CfgFuncProgramAssDlg(void)
    : super(IDD_CFG_FUNC_PROGRAM_ASS, XPR_NULL)
{
}

void CfgFuncProgramAssDlg::DoDataExchange(CDataExchange* pDX)
{
    super::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_CFG_PROGRAM_ASS_LIST, mListCtrl);
}

BEGIN_MESSAGE_MAP(CfgFuncProgramAssDlg, super)
    ON_NOTIFY(LVN_GETDISPINFO,  IDC_CFG_PROGRAM_ASS_LIST, OnGetdispinfoList)
    ON_NOTIFY(LVN_DELETEITEM,   IDC_CFG_PROGRAM_ASS_LIST, OnDeleteitemList)
    ON_NOTIFY(LVN_ITEMACTIVATE, IDC_CFG_PROGRAM_ASS_LIST, OnItemActivateList)
    ON_BN_CLICKED(IDC_CFG_PROGRAM_ASS_ITEM_ADD,    OnItemAdd)
    ON_BN_CLICKED(IDC_CFG_PROGRAM_ASS_ITEM_MODIFY, OnItemModify)
    ON_BN_CLICKED(IDC_CFG_PROGRAM_ASS_ITEM_DELETE, OnItemDelete)
    ON_BN_CLICKED(IDC_CFG_PROGRAM_ASS_ITEM_UP,     OnItemUp)
    ON_BN_CLICKED(IDC_CFG_PROGRAM_ASS_ITEM_DOWN,   OnItemDown)
    ON_BN_CLICKED(IDC_CFG_PROGRAM_ASS_DEFAULT,     OnDefault)
END_MESSAGE_MAP()

xpr_bool_t CfgFuncProgramAssDlg::OnInitDialog(void) 
{
    super::OnInitDialog();

    // disable apply button event
    addIgnoreApply(IDC_CFG_PROGRAM_ASS_ITEM_ADD);
    addIgnoreApply(IDC_CFG_PROGRAM_ASS_ITEM_MODIFY);
    addIgnoreApply(IDC_CFG_PROGRAM_ASS_ITEM_DELETE);
    addIgnoreApply(IDC_CFG_PROGRAM_ASS_ITEM_UP);
    addIgnoreApply(IDC_CFG_PROGRAM_ASS_ITEM_DOWN);
    addIgnoreApply(IDC_CFG_PROGRAM_ASS_DEFAULT);

    mListCtrl.SetExtendedStyle(mListCtrl.GetExtendedStyle() | LVS_EX_FULLROWSELECT);
    mListCtrl.InsertColumn(0, theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.function.program_ass.list.column.name")),      LVCFMT_LEFT, 100);
    mListCtrl.InsertColumn(1, theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.function.program_ass.list.column.extension")), LVCFMT_LEFT, 100);
    mListCtrl.InsertColumn(2, theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.function.program_ass.list.column.program")),   LVCFMT_LEFT, 200);

    mListCtrl.SetImageList(&fxb::SysImgListMgr::instance().mSysImgList16, LVSIL_SMALL);

    xpr_tchar_t sText[0xff] = {0};
    _stprintf(sText, theApp.loadFormatString(XPR_STRING_LITERAL("popup.cfg.body.function.program_ass.label.limit_count"), XPR_STRING_LITERAL("%d")), MAX_PROGRAM_ASS);
    SetDlgItemText(IDC_CFG_PROGRAM_ASS_LABEL_LIMIT_COUNT, sText);

    SetDlgItemText(IDC_CFG_PROGRAM_ASS_LABEL_LIST,   theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.function.program_ass.label.list")));
    SetDlgItemText(IDC_CFG_PROGRAM_ASS_DEFAULT,      theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.function.program_ass.button.default")));
    SetDlgItemText(IDC_CFG_PROGRAM_ASS_ITEM_ADD,     theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.function.program_ass.button.add")));
    SetDlgItemText(IDC_CFG_PROGRAM_ASS_ITEM_MODIFY,  theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.function.program_ass.button.modify")));
    SetDlgItemText(IDC_CFG_PROGRAM_ASS_ITEM_DELETE,  theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.function.program_ass.button.delete")));

    return XPR_TRUE;
}

void CfgFuncProgramAssDlg::onInit(Option::Config &aConfig)
{
    fxb::ProgramAssMgr &sProgramAssMgr = fxb::ProgramAssMgr::instance();

    fxb::ProgramAssItem *sProgramAssItem;
    fxb::ProgramAssItem *sProgramAssItem2;

    xpr_sint_t sIndex;
    xpr_size_t i, sCount;
    LVITEM sLvItem = {0};

    sIndex = 0;

    sCount = sProgramAssMgr.getCount();
    for (i = 0; i < sCount; ++i)
    {
        sProgramAssItem = sProgramAssMgr.getItem(i);
        if (XPR_IS_NULL(sProgramAssItem))
            continue;

        sProgramAssItem2 = new fxb::ProgramAssItem;
        *sProgramAssItem2 = *sProgramAssItem;

        addProgramAssItem(sProgramAssItem2);

        sIndex++;
    }
}

void CfgFuncProgramAssDlg::onApply(Option::Config &aConfig)
{
    fxb::ProgramAssMgr &sProgramAssMgr = fxb::ProgramAssMgr::instance();

    xpr_sint_t i;
    xpr_sint_t sCount;
    fxb::ProgramAssItem *sProgramAssItem;
    fxb::ProgramAssItem *sProgramAssItem2;

    sProgramAssMgr.clear();

    sCount = mListCtrl.GetItemCount();
    for (i = 0; i < sCount; ++i)
    {
        sProgramAssItem = (fxb::ProgramAssItem *)mListCtrl.GetItemData(i);
        if (XPR_IS_NULL(sProgramAssItem))
            continue;

        sProgramAssItem2 = new fxb::ProgramAssItem;
        *sProgramAssItem2 = *sProgramAssItem;

        sProgramAssMgr.addItem(sProgramAssItem2);
    }
}

xpr_sint_t CfgFuncProgramAssDlg::insertProgramAssItem(xpr_sint_t aIndex, fxb::ProgramAssItem *aProgramAssItem)
{
    LVITEM sLvItem = {0};
    sLvItem.mask     = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
    sLvItem.iItem    = aIndex;
    sLvItem.iSubItem = 0;
    sLvItem.iImage   = I_IMAGECALLBACK;
    sLvItem.pszText  = LPSTR_TEXTCALLBACK;
    sLvItem.lParam   = (LPARAM)aProgramAssItem;

    return mListCtrl.InsertItem(&sLvItem);
}

xpr_sint_t CfgFuncProgramAssDlg::addProgramAssItem(fxb::ProgramAssItem *aProgramAssItem)
{
    xpr_sint_t sIndex;
    sIndex = mListCtrl.GetItemCount();

    return insertProgramAssItem(sIndex, aProgramAssItem);
}

void CfgFuncProgramAssDlg::selectItem(xpr_sint_t aIndex)
{
    mListCtrl.SetFocus();
    mListCtrl.EnsureVisible(aIndex, XPR_TRUE);
    mListCtrl.SetSelectionMark(aIndex);
    mListCtrl.SetItemState(aIndex, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
}

void CfgFuncProgramAssDlg::OnItemAdd(void)
{
    xpr_sint_t i;
    xpr_sint_t sCount;
    fxb::ProgramAssItem *sProgramAssItem;

    sCount = mListCtrl.GetItemCount();
    if (sCount > MAX_PROGRAM_ASS)
    {
        xpr_tchar_t sMsg[0xff] = {0};
        _stprintf(sMsg, theApp.loadFormatString(XPR_STRING_LITERAL("popup.cfg.body.function.program_ass.msg.excess_max_count"), XPR_STRING_LITERAL("%d")), MAX_PROGRAM_ASS);
        MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);
        return;
    }

    ProgramAssSetDlg sDlg;

    for (i = 0; i < sCount; ++i)
    {
        sProgramAssItem = (fxb::ProgramAssItem *)mListCtrl.GetItemData(i);
        if (XPR_IS_NULL(sProgramAssItem))
            continue;

        sDlg.addName(sProgramAssItem->mName.c_str());
    }

    if (sDlg.DoModal() != IDOK)
        return;

    sProgramAssItem = new fxb::ProgramAssItem;
    *sProgramAssItem = *sDlg.getProgramAssItem();

    xpr_sint_t sIndex;
    sIndex = addProgramAssItem(sProgramAssItem);

    selectItem(sIndex);

    setModified();
}

void CfgFuncProgramAssDlg::OnItemModify(void)
{
    xpr_sint_t i;
    xpr_sint_t sIndex;
    xpr_sint_t sCount;
    fxb::ProgramAssItem *sProgramAssItem;
    fxb::ProgramAssItem *sProgramAssItem2;

    sIndex = mListCtrl.GetSelectionMark();
    if (sIndex < 0)
        return;

    sProgramAssItem = (fxb::ProgramAssItem *)mListCtrl.GetItemData(sIndex);
    if (XPR_IS_NULL(sProgramAssItem))
        return;

    ProgramAssSetDlg sDlg(sProgramAssItem);

    sCount = mListCtrl.GetItemCount();
    for (i = 0; i < sCount; ++i)
    {
        if (i == sIndex)
            continue;

        sProgramAssItem2 = (fxb::ProgramAssItem *)mListCtrl.GetItemData(i);
        if (!sProgramAssItem2)
            continue;

        sDlg.addName(sProgramAssItem2->mName.c_str());
    }

    if (sDlg.DoModal() != IDOK)
        return;

    *sProgramAssItem = *sDlg.getProgramAssItem();

    mListCtrl.Invalidate(XPR_FALSE);
    mListCtrl.UpdateWindow();

    selectItem(sIndex);

    setModified();
}

void CfgFuncProgramAssDlg::OnItemDelete(void)
{
    xpr_sint_t sIndex = mListCtrl.GetSelectionMark();
    if (sIndex < 0)
        return;

    if (sIndex == 0 || sIndex == 1)
        return;

    const xpr_tchar_t *sMsg = theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.function.program_ass.msg.confirm_delete"));
    xpr_sint_t sMsgId = MessageBox(sMsg, XPR_NULL, MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2);
    if (sMsgId == IDNO)
        return;

    mListCtrl.DeleteItem(sIndex);

    xpr_sint_t sCount = mListCtrl.GetItemCount();
    if (sIndex == sCount)
        sIndex = sCount - 1;

    selectItem(sIndex);

    setModified();
}

void CfgFuncProgramAssDlg::OnItemUp(void)
{
    xpr_sint_t sIndex = mListCtrl.GetSelectionMark();
    if (sIndex < 0)
        return;

    if (sIndex == 0 || sIndex == 1 || sIndex == 2)
        return;

    mListCtrl.SetRedraw(XPR_FALSE);

    fxb::ProgramAssItem *sProgramAssItem = (fxb::ProgramAssItem *)mListCtrl.GetItemData(sIndex);
    mListCtrl.SetItemData(sIndex, XPR_NULL);
    mListCtrl.DeleteItem(sIndex);
    sIndex--;

    insertProgramAssItem(sIndex, sProgramAssItem);
    selectItem(sIndex);

    mListCtrl.SetRedraw();

    setModified();
}

void CfgFuncProgramAssDlg::OnItemDown(void)
{
    xpr_sint_t sIndex = mListCtrl.GetSelectionMark();
    if (sIndex < 0)
        return;

    xpr_sint_t sCount = mListCtrl.GetItemCount();
    if (sIndex == 0 || sIndex == 1 || sIndex == (sCount - 1))
        return;

    mListCtrl.SetRedraw(XPR_FALSE);

    fxb::ProgramAssItem *sProgramAssItem = (fxb::ProgramAssItem *)mListCtrl.GetItemData(sIndex);
    mListCtrl.SetItemData(sIndex, XPR_NULL);
    mListCtrl.DeleteItem(sIndex);
    sIndex++;

    insertProgramAssItem(sIndex, sProgramAssItem);
    selectItem(sIndex);

    mListCtrl.SetRedraw();

    setModified();
}

void CfgFuncProgramAssDlg::OnGetdispinfoList(NMHDR* pNMHDR, LRESULT* pResult)
{
    LV_DISPINFO *pDispInfo = (LV_DISPINFO*)pNMHDR;
    *pResult = 0;

    LVITEM &sLvItem = pDispInfo->item;

    fxb::ProgramAssItem *sProgramAssItem = (fxb::ProgramAssItem *)sLvItem.lParam;
    if (XPR_IS_NULL(sProgramAssItem))
        return;

    if (XPR_TEST_BITS(sLvItem.mask, LVIF_TEXT))
    {
        switch (sLvItem.iSubItem)
        {
        case 0:
            {
                _tcscpy(sLvItem.pszText, sProgramAssItem->mName.c_str());
                break;
            }
        case 1:
            {
                if (sProgramAssItem->mMethod == fxb::ProgramAssMethodFilter)
                {
                    fxb::FilterItem *lpFilterItem;
                    lpFilterItem = fxb::FilterMgr::instance().getFilterFromName(sProgramAssItem->mFilterName);

                    if (lpFilterItem)
                        _tcscpy(sLvItem.pszText, lpFilterItem->mExts.c_str());
                }
                else
                {
                    _tcscpy(sLvItem.pszText, sProgramAssItem->mExts.c_str());
                }
                break;
            }
        case 2:
            {
                _tcscpy(sLvItem.pszText, sProgramAssItem->mPath.c_str());
                break;
            }
        }
    }

    if (XPR_TEST_BITS(sLvItem.mask, LVIF_IMAGE))
    {
        std::tstring sPath;
        fxb::GetEnvRealPath(sProgramAssItem->mPath, sPath);

        sLvItem.iImage = fxb::GetItemIconIndex(sPath.c_str());
    }
}

void CfgFuncProgramAssDlg::OnDeleteitemList(NMHDR* pNMHDR, LRESULT* pResult) 
{
    NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
    *pResult = 0;

    fxb::ProgramAssItem *sProgramAssItem = (fxb::ProgramAssItem *)pNMListView->lParam;
    XPR_SAFE_DELETE(sProgramAssItem);
}

void CfgFuncProgramAssDlg::OnItemActivateList(NMHDR* pNMHDR, LRESULT* pResult)
{
    NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
    *pResult = 0;

    OnItemModify();
}

xpr_bool_t CfgFuncProgramAssDlg::PreTranslateMessage(MSG* pMsg) 
{
    if (pMsg->message == WM_KEYDOWN && pMsg->hwnd == GetDlgItem(IDC_CFG_PROGRAM_ASS_LIST)->m_hWnd)
    {
        switch (pMsg->wParam)
        {
        case VK_UP:
            if (GetKeyState(VK_CONTROL) < 0)
            {
                OnItemUp();
                return XPR_TRUE;
            }
            break;

        case VK_DOWN:
            if (GetKeyState(VK_CONTROL) < 0)
            {
                OnItemDown();
                return XPR_TRUE;
            }
            break;

        case VK_INSERT:
            OnItemAdd();
            return XPR_TRUE;
            break;

        case VK_RETURN:
            if (GetKeyState(VK_CONTROL) < 0)
            {
                OnItemModify();
                return XPR_TRUE;
            }
            break;

        case VK_DELETE:
            OnItemDelete();
            return XPR_TRUE;
            break;
        }
    }

    return super::PreTranslateMessage(pMsg);
}

void CfgFuncProgramAssDlg::OnDefault(void)
{
    const xpr_tchar_t *sMsg = theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.function.program_ass.msg.confirm_default"));
    xpr_sint_t sMsgId = MessageBox(sMsg, XPR_NULL, MB_YESNO | MB_ICONQUESTION);
    if (sMsgId == IDNO)
        return;

    CWaitCursor sWaitCursor;

    mListCtrl.DeleteAllItems();

    fxb::ProgramAss sProgramAss;
    sProgramAss.initDefault();

    xpr_size_t i, sCount;
    fxb::ProgramAssItem *sProgramAssItem;
    fxb::ProgramAssItem *sProgramAssItem2;

    sCount = sProgramAss.getCount();
    for (i = 0; i < sCount; ++i)
    {
        sProgramAssItem = sProgramAss.getItem(i);

        sProgramAssItem2 = new fxb::ProgramAssItem;
        *sProgramAssItem2 = *sProgramAssItem;

        addProgramAssItem(sProgramAssItem2);
    }

    if (mListCtrl.GetItemCount() > 0)
        selectItem(0);

    mListCtrl.Invalidate(XPR_FALSE);
    mListCtrl.UpdateWindow();

    setModified();
}
