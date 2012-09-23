//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "CfgExpFilterDlg.h"

#include "../fxb/fxb_sys_img_list.h"
#include "../Option.h"
#include "../resource.h"

#include "FilterSetDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CfgExpFilterDlg::CfgExpFilterDlg(void)
    : super(IDD_CFG_EXP_FILTER, XPR_NULL)
{
}

void CfgExpFilterDlg::DoDataExchange(CDataExchange* pDX)
{
    super::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_CFG_EXP_FILTER_LIST, mListCtrl);
}

BEGIN_MESSAGE_MAP(CfgExpFilterDlg, super)
    ON_BN_CLICKED(IDC_CFG_EXP_FILTER_ADD,              OnListAdd)
    ON_BN_CLICKED(IDC_CFG_EXP_FILTER_MODIFY,           OnListModify)
    ON_BN_CLICKED(IDC_CFG_EXP_FILTER_DELETE,           OnListDelete)
    ON_BN_CLICKED(IDC_CFG_EXP_FILTER_ITEM_UP,          OnItemUp)
    ON_BN_CLICKED(IDC_CFG_EXP_FILTER_ITEM_DOWN,        OnItemDown)
    ON_BN_CLICKED(IDC_CFG_EXP_FILTER_USE_COLOR,        OnUseColor)
    ON_BN_CLICKED(IDC_CFG_EXP_FILTER_TEXT_COLOR,       OnTextColor)
    ON_BN_CLICKED(IDC_CFG_EXP_FILTER_BACKGROUND_COLOR, OnBackgroundColor)
    ON_BN_CLICKED(IDC_CFG_EXP_FILTER_DEFAULT,          OnDefault)
    ON_NOTIFY(NM_CUSTOMDRAW,    IDC_CFG_EXP_FILTER_LIST, OnCustomDrawList)
    ON_NOTIFY(LVN_GETDISPINFO,  IDC_CFG_EXP_FILTER_LIST, OnGetdispinfoList)
    ON_NOTIFY(LVN_DELETEITEM,   IDC_CFG_EXP_FILTER_LIST, OnDeleteitemList)
    ON_NOTIFY(LVN_ITEMACTIVATE, IDC_CFG_EXP_FILTER_LIST, OnItemActivateList)
END_MESSAGE_MAP()

xpr_bool_t CfgExpFilterDlg::OnInitDialog(void) 
{
    super::OnInitDialog();

    mListCtrl.SetExtendedStyle(mListCtrl.GetExtendedStyle() | LVS_EX_FULLROWSELECT);
    mListCtrl.InsertColumn(0, theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.explorer_window.filter.list.column.name")),      LVCFMT_LEFT, 100, -1);
    mListCtrl.InsertColumn(1, theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.explorer_window.filter.list.column.extension")), LVCFMT_LEFT, 245, -1);

    mListCtrl.SetImageList(&fxb::SysImgListMgr::instance().mCusImgList16, LVSIL_SMALL);

    xpr_tchar_t sText[0xff] = {0};
    _stprintf(sText, theApp.loadFormatString(XPR_STRING_LITERAL("popup.cfg.body.explorer_window.filter.label.max_filter"), XPR_STRING_LITERAL("%d")), MAX_FILTER);
    SetDlgItemText(IDC_CFG_EXP_FILTER_LABEL_MAX_FILTER, sText);

    fxb::FilterMgr &sFilterMgr = fxb::FilterMgr::instance();

    fxb::FilterItem *sFilterItem;
    fxb::FilterItem *sFilterItem2;

    xpr_sint_t i;
    xpr_sint_t sIndex;
    xpr_sint_t sCount;
    LVITEM sLvItem = {0};

    sIndex = 0;

    sCount = sFilterMgr.getCount();
    for (i = 0; i < sCount; ++i)
    {
        sFilterItem = sFilterMgr.getFilter(i);
        if (sFilterItem == XPR_NULL)
            continue;

        sFilterItem2 = new fxb::FilterItem;
        *sFilterItem2 = *sFilterItem;

        addFilterItem(sFilterItem2);

        sIndex++;
    }

    xpr_bool_t sEnable = XPR_FALSE;
    if (gOpt->mExplorerFilterColorType != FILTER_COLOR_TYPE_NONE)
    {
        ((CButton *)GetDlgItem(IDC_CFG_EXP_FILTER_USE_COLOR))->SetCheck(XPR_TRUE);
        sEnable = XPR_TRUE;
    }

    GetDlgItem(IDC_CFG_EXP_FILTER_TEXT_COLOR)->EnableWindow(sEnable);
    GetDlgItem(IDC_CFG_EXP_FILTER_BACKGROUND_COLOR)->EnableWindow(sEnable);

    if (XPR_IS_TRUE(sEnable))
    {
        if (gOpt->mExplorerFilterColorType == FILTER_COLOR_TYPE_TEXT)
            ((CButton *)GetDlgItem(IDC_CFG_EXP_FILTER_TEXT_COLOR))->SetCheck(XPR_TRUE);
        else
            ((CButton *)GetDlgItem(IDC_CFG_EXP_FILTER_BACKGROUND_COLOR))->SetCheck(XPR_TRUE);
    }

    SetDlgItemText(IDC_CFG_EXP_FILTER_USE_COLOR,        theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.explorer_window.filter.check.use_color")));
    SetDlgItemText(IDC_CFG_EXP_FILTER_TEXT_COLOR,       theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.explorer_window.filter.radio.text_color")));
    SetDlgItemText(IDC_CFG_EXP_FILTER_BACKGROUND_COLOR, theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.explorer_window.filter.radio.background_color")));
    SetDlgItemText(IDC_CFG_EXP_FILTER_LABEL_LIST,       theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.explorer_window.filter.label.list")));
    SetDlgItemText(IDC_CFG_EXP_FILTER_DEFAULT,          theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.explorer_window.filter.button.default")));
    SetDlgItemText(IDC_CFG_EXP_FILTER_ADD,              theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.explorer_window.filter.button.add")));
    SetDlgItemText(IDC_CFG_EXP_FILTER_MODIFY,           theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.explorer_window.filter.button.modify")));
    SetDlgItemText(IDC_CFG_EXP_FILTER_DELETE,           theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.explorer_window.filter.button.delete")));

    return XPR_TRUE;
}

void CfgExpFilterDlg::OnGetdispinfoList(NMHDR* pNMHDR, LRESULT* pResult)
{
    LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
    *pResult = 0;

    LPLVITEM sLvItem = &pDispInfo->item;

    fxb::FilterItem *sFilterItem = (fxb::FilterItem *)sLvItem->lParam;
    if (sFilterItem == XPR_NULL)
        return;

    if (XPR_TEST_BITS(sLvItem->mask, LVIF_TEXT))
    {
        switch (sLvItem->iSubItem)
        {
        case 0: _tcscpy(sLvItem->pszText, sFilterItem->mName.c_str()); break;
        case 1: _tcscpy(sLvItem->pszText, sFilterItem->mExts.c_str()); break;
        }
    }

    if (XPR_TEST_BITS(sLvItem->mask, LVIF_IMAGE))
    {
        sLvItem->iImage = sFilterItem->mIconIndex;
    }
}

void CfgExpFilterDlg::OnCustomDrawList(NMHDR* pNMHDR, LRESULT* pResult) 
{
    NMLVCUSTOMDRAW* pLVCD = reinterpret_cast<NMLVCUSTOMDRAW *>(pNMHDR);
    *pResult = CDRF_DODEFAULT;

    if (pLVCD->nmcd.dwDrawStage == CDDS_PREPAINT)
    {
        *pResult = CDRF_NOTIFYITEMDRAW;
    }
    else if (pLVCD->nmcd.dwDrawStage == CDDS_ITEMPREPAINT)
    {
        xpr_sint_t sIndex = (xpr_sint_t)pLVCD->nmcd.dwItemSpec;
        fxb::FilterItem *sFilterItem = (fxb::FilterItem *)pLVCD->nmcd.lItemlParam;
        if (sFilterItem != XPR_NULL)
            pLVCD->clrText = sFilterItem->mColor;

        *pResult = CDRF_DODEFAULT;
    }
}

void CfgExpFilterDlg::OnListAdd(void) 
{
    xpr_sint_t i;
    xpr_sint_t sCount;
    fxb::FilterItem *sFilterItem;

    sCount = mListCtrl.GetItemCount();
    if (sCount > MAX_FILTER)
    {
        xpr_tchar_t sMsg[0xff] = {0};
        _stprintf(sMsg, theApp.loadFormatString(XPR_STRING_LITERAL("popup.cfg.body.explorer_window.filter.msg.excess_max_count"), XPR_STRING_LITERAL("%d")), MAX_FILTER);
        MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);
        return;
    }

    FilterSetDlg sDlg;

    for (i = 0; i < sCount; ++i)
    {
        sFilterItem = (fxb::FilterItem *)mListCtrl.GetItemData(i);
        if (sFilterItem == XPR_NULL)
            continue;

        sDlg.addName(sFilterItem->mName.c_str());
    }

    if (sDlg.DoModal() != IDOK)
        return;

    sFilterItem = new fxb::FilterItem;
    *sFilterItem = *sDlg.getFilter();

    xpr_sint_t sIndex;
    sIndex = addFilterItem(sFilterItem);

    selectItem(sIndex);

    setModified();
}

void CfgExpFilterDlg::OnListModify(void) 
{
    xpr_sint_t i;
    xpr_sint_t sIndex;
    xpr_sint_t sCount;
    fxb::FilterItem *sFilterItem;
    fxb::FilterItem *sFilterItem2;

    sIndex = mListCtrl.GetSelectionMark();
    if (sIndex < 0)
        return;

    sFilterItem = (fxb::FilterItem *)mListCtrl.GetItemData(sIndex);
    if (sFilterItem == XPR_NULL)
        return;

    FilterSetDlg sDlg(sFilterItem);

    sCount = mListCtrl.GetItemCount();
    for (i = 0; i < sCount; ++i)
    {
        if (i == sIndex)
            continue;

        sFilterItem2 = (fxb::FilterItem *)mListCtrl.GetItemData(i);
        if (sFilterItem2 == XPR_NULL)
            continue;

        sDlg.addName(sFilterItem2->mName.c_str());
    }

    if (sDlg.DoModal() != IDOK)
        return;

    *sFilterItem = *sDlg.getFilter();

    mListCtrl.Invalidate(XPR_FALSE);
    mListCtrl.UpdateWindow();

    selectItem(sIndex);

    setModified();
}

void CfgExpFilterDlg::OnListDelete(void) 
{
    xpr_sint_t sIndex = mListCtrl.GetSelectionMark();
    if (sIndex < 0)
        return;

    if (sIndex == 0 || sIndex == 1)
        return;

    const xpr_tchar_t *sMsg = theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.explorer_window.filter.msg.confirm_delete"));
    xpr_sint_t sMsgId = MessageBox(sMsg, XPR_NULL, MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2);
    if (sMsgId == IDNO)
        return;

    mListCtrl.DeleteItem(sIndex);

    xpr_sint_t sCount = mListCtrl.GetItemCount();
    if (sIndex == sCount)
        sIndex = sCount-1;

    selectItem(sIndex);

    setModified();
}

void CfgExpFilterDlg::OnItemUp(void) 
{
    xpr_sint_t sIndex = mListCtrl.GetSelectionMark();
    if (sIndex < 0)
        return;

    if (sIndex == 0 || sIndex == 1 || sIndex == 2)
        return;

    mListCtrl.SetRedraw(XPR_FALSE);

    fxb::FilterItem *sFilterItem = (fxb::FilterItem *)mListCtrl.GetItemData(sIndex);
    mListCtrl.SetItemData(sIndex, 0);
    mListCtrl.DeleteItem(sIndex);
    sIndex--;

    insertFilterItem(sIndex, sFilterItem);
    selectItem(sIndex);

    mListCtrl.SetRedraw();

    setModified();
}

void CfgExpFilterDlg::OnItemDown(void) 
{
    xpr_sint_t sIndex = mListCtrl.GetSelectionMark();
    if (sIndex < 0)
        return;

    xpr_sint_t sCount = mListCtrl.GetItemCount();
    if (sIndex == 0 || sIndex == 1 || sIndex == (sCount-1))
        return;

    mListCtrl.SetRedraw(XPR_FALSE);

    fxb::FilterItem *sFilterItem = (fxb::FilterItem *)mListCtrl.GetItemData(sIndex);
    mListCtrl.SetItemData(sIndex, 0);
    mListCtrl.DeleteItem(sIndex);
    sIndex++;

    insertFilterItem(sIndex, sFilterItem);
    selectItem(sIndex);

    mListCtrl.SetRedraw();

    setModified();
}

xpr_sint_t CfgExpFilterDlg::insertFilterItem(xpr_sint_t aIndex, fxb::FilterItem *aFilterItem)
{
    LVITEM sLvItem = {0};
    sLvItem.mask     = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
    sLvItem.iItem    = aIndex;
    sLvItem.iSubItem = 0;
    sLvItem.iImage   = I_IMAGECALLBACK;
    sLvItem.pszText  = LPSTR_TEXTCALLBACK;
    sLvItem.lParam   = (LPARAM)aFilterItem;

    return mListCtrl.InsertItem(&sLvItem);
}

xpr_sint_t CfgExpFilterDlg::addFilterItem(fxb::FilterItem *aFilterItem)
{
    xpr_sint_t sIndex;
    sIndex = mListCtrl.GetItemCount();

    return insertFilterItem(sIndex, aFilterItem);
}

void CfgExpFilterDlg::selectItem(xpr_sint_t aIndex)
{
    mListCtrl.SetFocus();
    mListCtrl.EnsureVisible(aIndex, XPR_TRUE);
    mListCtrl.SetSelectionMark(aIndex);
    mListCtrl.SetItemState(aIndex, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
}

void CfgExpFilterDlg::OnDeleteitemList(NMHDR* pNMHDR, LRESULT* pResult) 
{
    NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
    *pResult = 0;

    fxb::FilterItem *sFilterItem = (fxb::FilterItem *)pNMListView->lParam;
    XPR_SAFE_DELETE(sFilterItem);
}

void CfgExpFilterDlg::OnItemActivateList(NMHDR* pNMHDR, LRESULT* pResult)
{
    NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
    *pResult = 0;

    OnListModify();
}

xpr_bool_t CfgExpFilterDlg::PreTranslateMessage(MSG* pMsg) 
{
    if (pMsg->message == WM_KEYDOWN && pMsg->hwnd == GetDlgItem(IDC_CFG_EXP_FILTER_LIST)->m_hWnd)
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
            OnListAdd();
            return XPR_TRUE;
            break;

        case VK_RETURN:
            if (GetKeyState(VK_CONTROL) < 0)
            {
                OnListModify();
                return XPR_TRUE;
            }
            break;

        case VK_DELETE:
            OnListDelete();
            return XPR_TRUE;
            break;
        }
    }

    return super::PreTranslateMessage(pMsg);
}

void CfgExpFilterDlg::OnUseColor(void) 
{
    setModified();

    xpr_bool_t sEnable = ((CButton *)GetDlgItem(IDC_CFG_EXP_FILTER_USE_COLOR))->GetCheck();
    GetDlgItem(IDC_CFG_EXP_FILTER_TEXT_COLOR)->EnableWindow(sEnable);
    GetDlgItem(IDC_CFG_EXP_FILTER_BACKGROUND_COLOR)->EnableWindow(sEnable);
}

void CfgExpFilterDlg::OnApply(void)
{
    OptionMgr &sOptionMgr = OptionMgr::instance();
    fxb::FilterMgr &sFilterMgr = fxb::FilterMgr::instance();

    xpr_sint_t i;
    xpr_sint_t sCount;
    fxb::FilterItem *sFilterItem;
    fxb::FilterItem *sFilterItem2;

    sFilterMgr.clear();

    sCount = mListCtrl.GetItemCount();
    for (i = 0; i < sCount; ++i)
    {
        sFilterItem = (fxb::FilterItem *)mListCtrl.GetItemData(i);
        if (sFilterItem == XPR_NULL)
            continue;

        sFilterItem2 = new fxb::FilterItem;
        *sFilterItem2 = *sFilterItem;

        sFilterMgr.addFilter(sFilterItem2);
    }

    gOpt->mExplorerFilterColorType = FILTER_COLOR_TYPE_NONE;
    if (((CButton *)GetDlgItem(IDC_CFG_EXP_FILTER_USE_COLOR))->GetCheck())
    {
        if (((CButton *)GetDlgItem(IDC_CFG_EXP_FILTER_TEXT_COLOR))->GetCheck())
            gOpt->mExplorerFilterColorType = FILTER_COLOR_TYPE_TEXT;
        else if (((CButton *)GetDlgItem(IDC_CFG_EXP_FILTER_BACKGROUND_COLOR))->GetCheck())
            gOpt->mExplorerFilterColorType = FILTER_COLOR_TYPE_BKGND;
    }

    sOptionMgr.applyExplorerView(2, XPR_FALSE);
}

xpr_bool_t CfgExpFilterDlg::DestroyWindow(void)
{
    mImageList.DeleteImageList();

    return CDialog::DestroyWindow();
}

void CfgExpFilterDlg::OnTextColor(void) 
{
    setModified();
}

void CfgExpFilterDlg::OnBackgroundColor(void) 
{
    setModified();
}

void CfgExpFilterDlg::OnDefault(void)
{
    const xpr_tchar_t *sMsg = theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.explorer_window.filter.msg.confirm_init"));
    xpr_sint_t sMsgId = MessageBox(sMsg, XPR_NULL, MB_YESNO | MB_ICONQUESTION);
    if (sMsgId == IDNO)
        return;

    CWaitCursor sWaitCursor;

    mListCtrl.DeleteAllItems();

    fxb::Filter sFilter;
    sFilter.initDefault();

    xpr_sint_t i, sCount;
    fxb::FilterItem *sFilterItem;
    fxb::FilterItem *sFilterItem2;

    sCount = sFilter.getCount();
    for (i = 0; i < sCount; ++i)
    {
        sFilterItem = sFilter.getFilter(i);

        sFilterItem2 = new fxb::FilterItem;
        *sFilterItem2 = *sFilterItem;

        addFilterItem(sFilterItem2);
    }

    if (mListCtrl.GetItemCount() > 0)
        selectItem(0);

    mListCtrl.Invalidate(XPR_FALSE);
    mListCtrl.UpdateWindow();

    setModified();
}
