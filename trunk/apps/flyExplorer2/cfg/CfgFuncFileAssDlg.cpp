//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "CfgFuncFileAssDlg.h"

#include "../fxb/fxb_file_ass.h"
#include "../fxb/fxb_filter.h"
#include "../fxb/fxb_sys_img_list.h"

#include "../resource.h"

#include "FileAssSetDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CfgFuncFileAssDlg::CfgFuncFileAssDlg(void)
    : super(IDD_CFG_FUNC_FILE_ASS, XPR_NULL)
{
}

void CfgFuncFileAssDlg::DoDataExchange(CDataExchange* pDX)
{
    super::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_CFG_FUNC_FILE_ASS_LIST, mListCtrl);
}

BEGIN_MESSAGE_MAP(CfgFuncFileAssDlg, super)
    ON_NOTIFY(LVN_GETDISPINFO,  IDC_CFG_FUNC_FILE_ASS_LIST, OnGetdispinfoList)
    ON_NOTIFY(LVN_DELETEITEM,   IDC_CFG_FUNC_FILE_ASS_LIST, OnDeleteitemList)
    ON_NOTIFY(LVN_ITEMACTIVATE, IDC_CFG_FUNC_FILE_ASS_LIST, OnItemActivateList)
    ON_BN_CLICKED(IDC_CFG_FUNC_FILE_ASS_ITEM_ADD,    OnItemAdd)
    ON_BN_CLICKED(IDC_CFG_FUNC_FILE_ASS_ITEM_MODIFY, OnItemModify)
    ON_BN_CLICKED(IDC_CFG_FUNC_FILE_ASS_ITEM_DELETE, OnItemDelete)
    ON_BN_CLICKED(IDC_CFG_FUNC_FILE_ASS_ITEM_UP,     OnItemUp)
    ON_BN_CLICKED(IDC_CFG_FUNC_FILE_ASS_ITEM_DOWN,   OnItemDown)
    ON_BN_CLICKED(IDC_CFG_FUNC_FILE_ASS_DEFAULT,     OnDefault)
END_MESSAGE_MAP()

xpr_bool_t CfgFuncFileAssDlg::OnInitDialog(void) 
{
    super::OnInitDialog();

    mListCtrl.SetExtendedStyle(mListCtrl.GetExtendedStyle() | LVS_EX_FULLROWSELECT);
    mListCtrl.InsertColumn(0, theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.function.view_edit.list.column.name")),       LVCFMT_LEFT, 100);
    mListCtrl.InsertColumn(1, theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.function.view_edit.list.column.extentsion")), LVCFMT_LEFT, 100);
    mListCtrl.InsertColumn(2, theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.function.view_edit.list.column.program")),    LVCFMT_LEFT, 200);

    mListCtrl.SetImageList(&fxb::SysImgListMgr::instance().mSysImgList16, LVSIL_SMALL);

    xpr_tchar_t sText[0xff] = {0};
    _stprintf(sText, theApp.loadFormatString(XPR_STRING_LITERAL("popup.cfg.body.function.view_edit.label.limit_count"), XPR_STRING_LITERAL("%d")), MAX_FILE_ASS);
    SetDlgItemText(IDC_CFG_FUNC_FILE_ASS_LABEL_LIMIT_COUNT, sText);

    fxb::FileAssMgr &sFileAssMgr = fxb::FileAssMgr::instance();

    fxb::FileAssItem *sFileAssItem;
    fxb::FileAssItem *sFileAssItem2;

    xpr_sint_t sIndex;
    xpr_size_t i, sCount;
    LVITEM sLvItem = {0};

    sIndex = 0;

    sCount = sFileAssMgr.getCount();
    for (i = 0; i < sCount; ++i)
    {
        sFileAssItem = sFileAssMgr.getItem(i);
        if (XPR_IS_NULL(sFileAssItem))
            continue;

        sFileAssItem2 = new fxb::FileAssItem;
        *sFileAssItem2 = *sFileAssItem;

        addFileAssItem(sFileAssItem2);

        sIndex++;
    }

    SetDlgItemText(IDC_CFG_FUNC_FILE_ASS_LABEL_LIST,   theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.function.view_edit.label.list")));
    SetDlgItemText(IDC_CFG_FUNC_FILE_ASS_DEFAULT,      theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.function.view_edit.button.default")));
    SetDlgItemText(IDC_CFG_FUNC_FILE_ASS_ITEM_ADD,     theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.function.view_edit.button.add")));
    SetDlgItemText(IDC_CFG_FUNC_FILE_ASS_ITEM_MODIFY,  theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.function.view_edit.button.modify")));
    SetDlgItemText(IDC_CFG_FUNC_FILE_ASS_ITEM_DELETE,  theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.function.view_edit.button.delete")));

    return XPR_TRUE;
}

void CfgFuncFileAssDlg::OnApply(void)
{
    OptionMgr &sOptionMgr = OptionMgr::instance();
    fxb::FileAssMgr &sFileAssMgr = fxb::FileAssMgr::instance();

    xpr_sint_t i;
    xpr_sint_t sCount;
    fxb::FileAssItem *sFileAssItem;
    fxb::FileAssItem *sFileAssItem2;

    sFileAssMgr.clear();

    sCount = mListCtrl.GetItemCount();
    for (i = 0; i < sCount; ++i)
    {
        sFileAssItem = (fxb::FileAssItem *)mListCtrl.GetItemData(i);
        if (XPR_IS_NULL(sFileAssItem))
            continue;

        sFileAssItem2 = new fxb::FileAssItem;
        *sFileAssItem2 = *sFileAssItem;

        sFileAssMgr.addItem(sFileAssItem2);
    }

    sOptionMgr.applyExplorerView(2, XPR_FALSE);
}

xpr_bool_t CfgFuncFileAssDlg::OnCommand(WPARAM wParam, LPARAM lParam) 
{
    xpr_uint_t sNotifyMsg = HIWORD(wParam);
    xpr_uint_t sId = LOWORD(wParam);

    if (sNotifyMsg == BN_CLICKED)
    {
    }

    return super::OnCommand(wParam, lParam);
}

xpr_sint_t CfgFuncFileAssDlg::insertFileAssItem(xpr_sint_t aIndex, fxb::FileAssItem *aFileAssItem)
{
    LVITEM sLvItem = {0};
    sLvItem.mask     = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
    sLvItem.iItem    = aIndex;
    sLvItem.iSubItem = 0;
    sLvItem.iImage   = I_IMAGECALLBACK;
    sLvItem.pszText  = LPSTR_TEXTCALLBACK;
    sLvItem.lParam   = (LPARAM)aFileAssItem;

    return mListCtrl.InsertItem(&sLvItem);
}

xpr_sint_t CfgFuncFileAssDlg::addFileAssItem(fxb::FileAssItem *aFileAssItem)
{
    xpr_sint_t sIndex;
    sIndex = mListCtrl.GetItemCount();

    return insertFileAssItem(sIndex, aFileAssItem);
}

void CfgFuncFileAssDlg::selectItem(xpr_sint_t aIndex)
{
    mListCtrl.SetFocus();
    mListCtrl.EnsureVisible(aIndex, XPR_TRUE);
    mListCtrl.SetSelectionMark(aIndex);
    mListCtrl.SetItemState(aIndex, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
}

void CfgFuncFileAssDlg::OnItemAdd(void)
{
    xpr_sint_t i;
    xpr_sint_t sCount;
    fxb::FileAssItem *sFileAssItem;

    sCount = mListCtrl.GetItemCount();
    if (sCount > MAX_FILE_ASS)
    {
        xpr_tchar_t sMsg[0xff] = {0};
        _stprintf(sMsg, theApp.loadFormatString(XPR_STRING_LITERAL("popup.cfg.body.function.view_edit.msg.excess_max_count"), XPR_STRING_LITERAL("%d")), MAX_FILE_ASS);
        MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);
        return;
    }

    FileAssSetDlg sDlg;

    for (i = 0; i < sCount; ++i)
    {
        sFileAssItem = (fxb::FileAssItem *)mListCtrl.GetItemData(i);
        if (XPR_IS_NULL(sFileAssItem))
            continue;

        sDlg.addName(sFileAssItem->mName.c_str());
    }

    if (sDlg.DoModal() != IDOK)
        return;

    sFileAssItem = new fxb::FileAssItem;
    *sFileAssItem = *sDlg.getFileAssItem();

    xpr_sint_t sIndex;
    sIndex = addFileAssItem(sFileAssItem);

    selectItem(sIndex);

    setModified();
}

void CfgFuncFileAssDlg::OnItemModify(void)
{
    xpr_sint_t i;
    xpr_sint_t sIndex;
    xpr_sint_t sCount;
    fxb::FileAssItem *sFileAssItem;
    fxb::FileAssItem *sFileAssItem2;

    sIndex = mListCtrl.GetSelectionMark();
    if (sIndex < 0)
        return;

    sFileAssItem = (fxb::FileAssItem *)mListCtrl.GetItemData(sIndex);
    if (XPR_IS_NULL(sFileAssItem))
        return;

    FileAssSetDlg sDlg(sFileAssItem);

    sCount = mListCtrl.GetItemCount();
    for (i = 0; i < sCount; ++i)
    {
        if (i == sIndex)
            continue;

        sFileAssItem2 = (fxb::FileAssItem *)mListCtrl.GetItemData(i);
        if (!sFileAssItem2)
            continue;

        sDlg.addName(sFileAssItem2->mName.c_str());
    }

    if (sDlg.DoModal() != IDOK)
        return;

    *sFileAssItem = *sDlg.getFileAssItem();

    mListCtrl.Invalidate(XPR_FALSE);
    mListCtrl.UpdateWindow();

    selectItem(sIndex);

    setModified();
}

void CfgFuncFileAssDlg::OnItemDelete(void)
{
    xpr_sint_t sIndex = mListCtrl.GetSelectionMark();
    if (sIndex < 0)
        return;

    if (sIndex == 0 || sIndex == 1)
        return;

    const xpr_tchar_t *sMsg = theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.function.view_edit.msg.confirm_delete"));
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

void CfgFuncFileAssDlg::OnItemUp(void)
{
    xpr_sint_t sIndex = mListCtrl.GetSelectionMark();
    if (sIndex < 0)
        return;

    if (sIndex == 0 || sIndex == 1 || sIndex == 2)
        return;

    mListCtrl.SetRedraw(XPR_FALSE);

    fxb::FileAssItem *sFileAssItem = (fxb::FileAssItem *)mListCtrl.GetItemData(sIndex);
    mListCtrl.SetItemData(sIndex, XPR_NULL);
    mListCtrl.DeleteItem(sIndex);
    sIndex--;

    insertFileAssItem(sIndex, sFileAssItem);
    selectItem(sIndex);

    mListCtrl.SetRedraw();

    setModified();
}

void CfgFuncFileAssDlg::OnItemDown(void)
{
    xpr_sint_t sIndex = mListCtrl.GetSelectionMark();
    if (sIndex < 0)
        return;

    xpr_sint_t sCount = mListCtrl.GetItemCount();
    if (sIndex == 0 || sIndex == 1 || sIndex == (sCount - 1))
        return;

    mListCtrl.SetRedraw(XPR_FALSE);

    fxb::FileAssItem *sFileAssItem = (fxb::FileAssItem *)mListCtrl.GetItemData(sIndex);
    mListCtrl.SetItemData(sIndex, XPR_NULL);
    mListCtrl.DeleteItem(sIndex);
    sIndex++;

    insertFileAssItem(sIndex, sFileAssItem);
    selectItem(sIndex);

    mListCtrl.SetRedraw();

    setModified();
}

void CfgFuncFileAssDlg::OnGetdispinfoList(NMHDR* pNMHDR, LRESULT* pResult)
{
    LV_DISPINFO *pDispInfo = (LV_DISPINFO*)pNMHDR;
    *pResult = 0;

    LVITEM &sLvItem = pDispInfo->item;

    fxb::FileAssItem *sFileAssItem = (fxb::FileAssItem *)sLvItem.lParam;
    if (XPR_IS_NULL(sFileAssItem))
        return;

    if (XPR_TEST_BITS(sLvItem.mask, LVIF_TEXT))
    {
        switch (sLvItem.iSubItem)
        {
        case 0:
            {
                _tcscpy(sLvItem.pszText, sFileAssItem->mName.c_str());
                break;
            }
        case 1:
            {
                if (sFileAssItem->mMethod == fxb::FileAssMethodFilter)
                {
                    fxb::FilterItem *lpFilterItem;
                    lpFilterItem = fxb::FilterMgr::instance().getFilterFromName(sFileAssItem->mFilterName);

                    if (lpFilterItem)
                        _tcscpy(sLvItem.pszText, lpFilterItem->mExts.c_str());
                }
                else
                {
                    _tcscpy(sLvItem.pszText, sFileAssItem->mExts.c_str());
                }
                break;
            }
        case 2:
            {
                _tcscpy(sLvItem.pszText, sFileAssItem->mPath.c_str());
                break;
            }
        }
    }

    if (XPR_TEST_BITS(sLvItem.mask, LVIF_IMAGE))
    {
        std::tstring sPath;
        fxb::GetEnvRealPath(sFileAssItem->mPath, sPath);

        sLvItem.iImage = fxb::GetItemIconIndex(sPath.c_str());
    }
}

void CfgFuncFileAssDlg::OnDeleteitemList(NMHDR* pNMHDR, LRESULT* pResult) 
{
    NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
    *pResult = 0;

    fxb::FileAssItem *sFileAssItem = (fxb::FileAssItem *)pNMListView->lParam;
    XPR_SAFE_DELETE(sFileAssItem);
}

void CfgFuncFileAssDlg::OnItemActivateList(NMHDR* pNMHDR, LRESULT* pResult)
{
    NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
    *pResult = 0;

    OnItemModify();
}

xpr_bool_t CfgFuncFileAssDlg::PreTranslateMessage(MSG* pMsg) 
{
    if (pMsg->message == WM_KEYDOWN && pMsg->hwnd == GetDlgItem(IDC_CFG_FUNC_FILE_ASS_LIST)->m_hWnd)
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

void CfgFuncFileAssDlg::OnDefault(void)
{
    const xpr_tchar_t *sMsg = theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.function.view_edit.msg.confirm_default"));
    xpr_sint_t sMsgId = MessageBox(sMsg, XPR_NULL, MB_YESNO | MB_ICONQUESTION);
    if (sMsgId == IDNO)
        return;

    CWaitCursor sWaitCursor;

    mListCtrl.DeleteAllItems();

    fxb::FileAss sFileAss;
    sFileAss.initDefault();

    xpr_size_t i, sCount;
    fxb::FileAssItem *sFileAssItem;
    fxb::FileAssItem *sFileAssItem2;

    sCount = sFileAss.getCount();
    for (i = 0; i < sCount; ++i)
    {
        sFileAssItem = sFileAss.getItem(i);

        sFileAssItem2 = new fxb::FileAssItem;
        *sFileAssItem2 = *sFileAssItem;

        addFileAssItem(sFileAssItem2);
    }

    if (mListCtrl.GetItemCount() > 0)
        selectItem(0);

    mListCtrl.Invalidate(XPR_FALSE);
    mListCtrl.UpdateWindow();

    setModified();
}
