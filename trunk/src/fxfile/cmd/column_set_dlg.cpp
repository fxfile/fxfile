//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "column_set_dlg.h"

#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace fxfile
{
namespace cmd
{
ColumnSetDlg::ColumnSetDlg(void)
    : super(IDD_COLUMN_SET, XPR_NULL)
    , mShellFolder2(XPR_NULL)
    , mAvgCharWidth(6)
{
}

ColumnSetDlg::~ColumnSetDlg(void)
{
    ColumnItemData *sColumnItemData;

    {
        ColumnDeque::iterator sIterator;

        sIterator = mListDeque.begin();
        for (; sIterator != mListDeque.end(); ++sIterator)
        {
            sColumnItemData = *sIterator;
            XPR_SAFE_DELETE(sColumnItemData);
        }

        sIterator = mUseListDeque.begin();
        for (; sIterator != mUseListDeque.end(); ++sIterator)
        {
            sColumnItemData = *sIterator;
            XPR_SAFE_DELETE(sColumnItemData);
        }
    }

    {
        ColumnMap::iterator sIterator;

        sIterator = mListMap.begin();
        for (; sIterator != mListMap.end(); ++sIterator)
        {
            sColumnItemData = sIterator->second;
            XPR_SAFE_DELETE(sColumnItemData);
        }
    }

    mListDeque.clear();
    mUseListDeque.clear();
    mListMap.clear();

    COM_RELEASE(mShellFolder2);
}

void ColumnSetDlg::DoDataExchange(CDataExchange* pDX)
{
    super::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_COLUMN_LIST, mListCtrl);
}

BEGIN_MESSAGE_MAP(ColumnSetDlg, super)
    ON_NOTIFY(LVN_GETDISPINFO, IDC_COLUMN_LIST, OnLvnGetdispinfoList)
    ON_NOTIFY(LVN_ITEMCHANGED, IDC_COLUMN_LIST, OnLvnItemchangedList)
    ON_BN_CLICKED(IDC_COLUMN_ITEM_UP,       OnItemUp)
    ON_BN_CLICKED(IDC_COLUMN_ITEM_DOWN,     OnItemDown)
    ON_BN_CLICKED(IDC_COLUMN_ITEM_SHOW,     OnItemShow)
    ON_BN_CLICKED(IDC_COLUMN_ITEM_HIDE,     OnItemHide)
    ON_BN_CLICKED(IDC_COLUMN_ITEM_SHOW_ALL, OnItemShowAll)
    ON_BN_CLICKED(IDC_COLUMN_ITEM_HIDE_ALL, OnItemHideAll)
    ON_BN_CLICKED(IDOK,                     OnOK)
END_MESSAGE_MAP()

xpr_bool_t ColumnSetDlg::OnInitDialog(void) 
{
    super::OnInitDialog();

    DWORD sExtendedStyle = mListCtrl.GetExtendedStyle();
    sExtendedStyle |= LVS_EX_FULLROWSELECT;
    sExtendedStyle |= LVS_EX_CHECKBOXES;
    mListCtrl.SetExtendedStyle(sExtendedStyle);

    mListCtrl.InsertColumn(0, gApp.loadString(XPR_STRING_LITERAL("popup.column_set.list.column.name")),  LVCFMT_LEFT,  170, -1);
    mListCtrl.InsertColumn(1, gApp.loadString(XPR_STRING_LITERAL("popup.column_set.list.column.width")), LVCFMT_RIGHT,  70, -1);

    if (mShellFolder2 != XPR_NULL)
    {
        xpr_sint_t i;
        HRESULT sResult;
        xpr_tchar_t sName[0xff] = {0};
        SHELLDETAILS sShellDetails;
        SHCOLUMNID sShColumnId;
        ColumnId sColumnId;
        ColumnItemData *sColumnItemData;
        ColumnMap::iterator sIterator;

        for (i = 0; ; ++i)
        {
            sResult = mShellFolder2->MapColumnToSCID(i, &sShColumnId);
            if (FAILED(sResult))
                break;

            sResult = mShellFolder2->GetDetailsOf(XPR_NULL, i, &sShellDetails);
            if (FAILED(sResult))
                break;

            sResult = ::StrRetToBuf(&sShellDetails.str, XPR_NULL, sName, 0xfe);
            if (FAILED(sResult))
                continue;

            if (_tcslen(sName) == 0)
                continue;

            sColumnId.mFormatId.fromBuffer((const xpr_byte_t *)&sShColumnId.fmtid);
            sColumnId.mPropertyId = sShColumnId.pid;

            sIterator = mListMap.find(sColumnId);
            if (sIterator != mListMap.end())
                continue;

            sColumnItemData = new ColumnItemData;
            sColumnItemData->mUse        = XPR_FALSE;
            sColumnItemData->mWidth      = sShellDetails.cxChar * mAvgCharWidth;
            sColumnItemData->mAlign      = sShellDetails.fmt;
            sColumnItemData->mName       = sName;
            sColumnItemData->mFormatId.fromBuffer((const xpr_byte_t *)&sShColumnId.fmtid);
            sColumnItemData->mPropertyId = sShColumnId.pid;

            mListDeque.push_back(sColumnItemData);
        }
    }

    ColumnItemData *sColumnItemData;
    ColumnDeque::iterator sIterator;

    sIterator = mListDeque.begin();
    for (; sIterator != mListDeque.end(); ++sIterator)
    {
        sColumnItemData = *sIterator;
        if (!sColumnItemData)
            continue;

        addList(sColumnItemData);
    }

    mListMap.clear();

    SetWindowText(gApp.loadString(XPR_STRING_LITERAL("popup.column_set.title")));
    SetDlgItemText(IDC_COLUMN_LABEL_DESC,    gApp.loadString(XPR_STRING_LITERAL("popup.column_set.label.desc")));
    SetDlgItemText(IDC_COLUMN_LABEL_LIST,    gApp.loadString(XPR_STRING_LITERAL("popup.column_set.label.list")));
    SetDlgItemText(IDC_COLUMN_LABEL_WIDTH,   gApp.loadString(XPR_STRING_LITERAL("popup.column_set.label.width")));
    SetDlgItemText(IDC_COLUMN_ITEM_UP,       gApp.loadString(XPR_STRING_LITERAL("popup.column_set.button.move_up")));
    SetDlgItemText(IDC_COLUMN_ITEM_DOWN,     gApp.loadString(XPR_STRING_LITERAL("popup.column_set.button.move_down")));
    SetDlgItemText(IDC_COLUMN_ITEM_SHOW,     gApp.loadString(XPR_STRING_LITERAL("popup.column_set.button.show")));
    SetDlgItemText(IDC_COLUMN_ITEM_HIDE,     gApp.loadString(XPR_STRING_LITERAL("popup.column_set.button.hide")));
    SetDlgItemText(IDC_COLUMN_ITEM_SHOW_ALL, gApp.loadString(XPR_STRING_LITERAL("popup.column_set.button.show_all")));
    SetDlgItemText(IDC_COLUMN_ITEM_HIDE_ALL, gApp.loadString(XPR_STRING_LITERAL("popup.column_set.button.hide_all")));
    SetDlgItemText(IDOK,                     gApp.loadString(XPR_STRING_LITERAL("popup.common.button.ok")));
    SetDlgItemText(IDCANCEL,                 gApp.loadString(XPR_STRING_LITERAL("popup.common.button.cancel")));

    return XPR_TRUE;
}

void ColumnSetDlg::OnOK(void)
{
    xpr_sint_t sIndex;
    ColumnItemData *sColumnItemData;

    sIndex = mListCtrl.GetSelectionMark();
    if (sIndex >= 0)
    {
        sColumnItemData = (ColumnItemData *)mListCtrl.GetItemData(sIndex);
        if (sColumnItemData != XPR_NULL)
        {
            sColumnItemData->mWidth = GetDlgItemInt(IDC_COLUMN_WIDTH, XPR_FALSE);

            mListCtrl.Invalidate(XPR_FALSE);
            mListCtrl.UpdateWindow();
        }
    }

    mListDeque.clear();
    mUseListDeque.clear();

    xpr_sint_t i;
    xpr_sint_t sCount;

    sCount = mListCtrl.GetItemCount();
    for (i = 0; i < sCount; ++i)
    {
        sColumnItemData = (ColumnItemData *)mListCtrl.GetItemData(i);
        if (sColumnItemData == XPR_NULL)
            continue;

        sColumnItemData->mUse = (i == 0) ? XPR_TRUE : mListCtrl.GetCheck(i);

        mUseListDeque.push_back(sColumnItemData);
    }

    ColumnDeque::iterator sIterator;

    sIterator = mUseListDeque.begin();
    while (sIterator != mUseListDeque.end())
    {
        sColumnItemData = *sIterator;
        if (sColumnItemData != XPR_NULL)
        {
            if (XPR_IS_FALSE(sColumnItemData->mUse))
            {
                XPR_SAFE_DELETE(sColumnItemData);

                sIterator = mUseListDeque.erase(sIterator);
                continue;
            }
        }

        ++sIterator;
    }

    super::OnOK();
}

void ColumnSetDlg::setShellFolder2(LPSHELLFOLDER2 aShellFolder2)
{
    mShellFolder2 = aShellFolder2;
    mShellFolder2->AddRef();
}

void ColumnSetDlg::setAvgCharWidth(xpr_sint_t aAvgCharWidth)
{
    mAvgCharWidth = aAvgCharWidth;
}

void ColumnSetDlg::setColumnList(ColumnDataList &aColumnList)
{
    ColumnItemData *sColumnItemData;

    ColumnDataList::iterator sIterator = aColumnList.begin();
    while (sIterator != aColumnList.end())
    {
        sColumnItemData = *sIterator;

        ColumnId sColumnId;
        sColumnId.mFormatId   = sColumnItemData->mFormatId;
        sColumnId.mPropertyId = sColumnItemData->mPropertyId;

        if (mListMap.find(sColumnId) != mListMap.end())
        {
            XPR_SAFE_DELETE(sColumnItemData);

            sIterator = aColumnList.erase(sIterator);
            continue;
        }

        mListDeque.push_back(sColumnItemData);
        mListMap[sColumnId] = sColumnItemData;

        ++sIterator;
    }

    aColumnList.clear();
}

xpr_size_t ColumnSetDlg::getUseColumnCount(void)
{
    return mUseListDeque.size();
}

void ColumnSetDlg::getUseColumnList(ColumnDataList &aUseColumnList)
{
    ColumnDeque::iterator sIterator;
    ColumnItemData *sColumnItemData;

    sIterator = mUseListDeque.begin();
    for (; sIterator != mUseListDeque.end(); ++sIterator)
    {
        sColumnItemData = *sIterator;
        aUseColumnList.push_back(sColumnItemData);
    }

    mUseListDeque.clear();
}

xpr_sint_t ColumnSetDlg::addList(ColumnItemData *aColumnItemData)
{
    if (aColumnItemData == XPR_NULL)
        return -1;

    xpr_sint_t sIndex = mListCtrl.GetItemCount();

    return insertList(sIndex, aColumnItemData);
}

xpr_sint_t ColumnSetDlg::insertList(xpr_sint_t aIndex, ColumnItemData *aColumnItemData)
{
    if (aColumnItemData == XPR_NULL)
        return -1;

    if (aIndex != -1 && !XPR_IS_RANGE(0, aIndex, mListCtrl.GetItemCount()))
        return -1;

    LVITEM sLvItem = {0};
    sLvItem.mask        = LVIF_TEXT | LVIF_PARAM;
    sLvItem.iItem       = aIndex;
    sLvItem.iSubItem    = 0;
    sLvItem.pszText     = LPSTR_TEXTCALLBACK;
    sLvItem.cchTextMax  = XPR_MAX_PATH;
    sLvItem.lParam      = (LPARAM)aColumnItemData;

    aIndex = mListCtrl.InsertItem(&sLvItem);
    if (aIndex < 0)
        return -1;

    mListCtrl.SetCheck(aIndex, aColumnItemData->mUse);

    return aIndex;
}

void ColumnSetDlg::selectItem(xpr_sint_t aIndex)
{
    mListCtrl.SetFocus();
    mListCtrl.EnsureVisible(aIndex, XPR_TRUE);
    mListCtrl.SetSelectionMark(aIndex);
    mListCtrl.SetItemState(aIndex, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
}

void ColumnSetDlg::OnLvnGetdispinfoList(NMHDR *pNMHDR, LRESULT *pResult)
{
    NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
    *pResult = 0;

    ColumnItemData *sColumnItemData = (ColumnItemData *)pDispInfo->item.lParam;
    if (sColumnItemData == XPR_NULL)
        return;

    LVITEM &sLvItem = pDispInfo->item;

    if (XPR_TEST_BITS(sLvItem.mask, LVIF_TEXT))
    {
        switch (sLvItem.iSubItem)
        {
        case 0:
            {
                _tcscpy(sLvItem.pszText, sColumnItemData->mName.c_str());
                if (XPR_IS_FALSE(sColumnItemData->mFormatId.none()))
                    _tcscat(sLvItem.pszText, XPR_STRING_LITERAL(" *"));
                break;
            }

        case 1:
            {
                _stprintf(sLvItem.pszText, XPR_STRING_LITERAL("%d"), sColumnItemData->mWidth);
                break;
            }
        }
    }
}

void ColumnSetDlg::OnLvnItemchangedList(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
    *pResult = 0;

    if (!XPR_TEST_BITS(pNMLV->uChanged, LVIF_STATE))
        return;

    ColumnItemData *sColumnItemData;

    if (XPR_TEST_BITS(pNMLV->uOldState, LVIS_SELECTED))
    {
        xpr_sint_t sIndex = pNMLV->iItem;
        if (pNMLV->uNewState != 0)
        {
            sIndex = mListCtrl.GetSelectionMark();
            if (sIndex == pNMLV->iItem)
                sIndex = -1;
        }

        if (sIndex >= 0)
        {
            sColumnItemData = (ColumnItemData *)mListCtrl.GetItemData(sIndex);
            if (sColumnItemData != XPR_NULL)
            {
                sColumnItemData->mWidth = GetDlgItemInt(IDC_COLUMN_WIDTH, XPR_FALSE);

                mListCtrl.Invalidate(XPR_FALSE);
                mListCtrl.UpdateWindow();
            }
        }
    }

    if (XPR_TEST_BITS(pNMLV->uNewState, LVIS_SELECTED))
    {
        sColumnItemData = (ColumnItemData *)pNMLV->lParam;
        if (sColumnItemData != XPR_NULL)
        {
            SetDlgItemInt(IDC_COLUMN_WIDTH, sColumnItemData->mWidth);
        }
    }

    // pin name column
    if (XPR_ANY_BITS(pNMLV->uNewState, LVIS_STATEIMAGEMASK))
    {
        xpr_bool_t sUncheck = (pNMLV->uNewState & LVIS_STATEIMAGEMASK) == INDEXTOSTATEIMAGEMASK(1);
        if (sUncheck == XPR_TRUE)
        {
            if (pNMLV->iItem == 0)
                mListCtrl.SetCheck(0, XPR_TRUE);
        }
    }
}

void ColumnSetDlg::OnItemUp(void)
{
    xpr_sint_t sIndex = mListCtrl.GetSelectionMark();
    if (sIndex < 0)
        return;

    if (sIndex == 0 || sIndex == 1)
        return;

    mListCtrl.SetRedraw(XPR_FALSE);

    ColumnItemData *sColumnItemData = (ColumnItemData *)mListCtrl.GetItemData(sIndex);
    if (sColumnItemData != XPR_NULL)
    {
        sColumnItemData->mWidth = GetDlgItemInt(IDC_COLUMN_WIDTH, XPR_FALSE);
        sColumnItemData->mUse   = mListCtrl.GetCheck(sIndex);
    }

    sColumnItemData = (ColumnItemData *)mListCtrl.GetItemData(sIndex);
    mListCtrl.SetItemData(sIndex, XPR_NULL);
    mListCtrl.DeleteItem(sIndex);
    sIndex--;

    insertList(sIndex, sColumnItemData);
    selectItem(sIndex);

    mListCtrl.SetRedraw();
}

void ColumnSetDlg::OnItemDown(void)
{
    xpr_sint_t sIndex = mListCtrl.GetSelectionMark();
    if (sIndex < 0)
        return;

    xpr_sint_t sCount = mListCtrl.GetItemCount();
    if (sIndex == 0 || sIndex == (sCount-1))
        return;

    mListCtrl.SetRedraw(XPR_FALSE);

    ColumnItemData *sColumnItemData = (ColumnItemData *)mListCtrl.GetItemData(sIndex);
    if (sColumnItemData != XPR_NULL)
    {
        sColumnItemData->mWidth = GetDlgItemInt(IDC_COLUMN_WIDTH, XPR_FALSE);
        sColumnItemData->mUse   = mListCtrl.GetCheck(sIndex);
    }

    sColumnItemData = (ColumnItemData *)mListCtrl.GetItemData(sIndex);
    mListCtrl.SetItemData(sIndex, XPR_NULL);
    mListCtrl.DeleteItem(sIndex);
    sIndex++;

    insertList(sIndex, sColumnItemData);
    selectItem(sIndex);

    mListCtrl.SetRedraw();
}

void ColumnSetDlg::OnItemShow(void)
{
    xpr_sint_t sIndex = mListCtrl.GetSelectionMark();
    if (sIndex >= 0)
        mListCtrl.SetCheck(sIndex, XPR_TRUE);
}

void ColumnSetDlg::OnItemHide(void)
{
    xpr_sint_t sIndex = mListCtrl.GetSelectionMark();
    if (sIndex >= 1)
        mListCtrl.SetCheck(sIndex, XPR_FALSE);
}

void ColumnSetDlg::OnItemShowAll(void)
{
    xpr_sint_t i;
    xpr_sint_t sCount;

    sCount = mListCtrl.GetItemCount();
    for (i = 0; i < sCount; ++i)
        mListCtrl.SetCheck(i, XPR_TRUE);
}

void ColumnSetDlg::OnItemHideAll(void)
{
    xpr_sint_t i;
    xpr_sint_t sCount;

    sCount = mListCtrl.GetItemCount();
    for (i = 1; i < sCount; ++i)
        mListCtrl.SetCheck(i, XPR_FALSE);
}

xpr_bool_t ColumnSetDlg::PreTranslateMessage(MSG* pMsg) 
{
    if (pMsg->message == WM_KEYDOWN && pMsg->hwnd == mListCtrl.m_hWnd)
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
        }
    }

    return super::PreTranslateMessage(pMsg);
}
} // namespace cmd
} // namespace fxfile
