//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "size_format_dlg.h"

#include "option.h"
#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace fxfile
{
namespace cfg
{
SizeFormatDlg::SizeFormatDlg(void)
    : super(IDD_SIZE_FORMAT, XPR_NULL)
    , mSizeFormatDeque(XPR_NULL)
{
}

SizeFormatDlg::~SizeFormatDlg(void)
{
}

void SizeFormatDlg::DoDataExchange(CDataExchange* pDX)
{
    super::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_SIZE_FORMAT_LIST, mListCtrl);
    DDX_Control(pDX, IDC_SIZE_FORMAT_UNIT, mUnitComboBox);
}

BEGIN_MESSAGE_MAP(SizeFormatDlg, super)
    ON_NOTIFY(LVN_GETDISPINFO, IDC_SIZE_FORMAT_LIST, OnLvnGetdispinfoList)
    ON_NOTIFY(LVN_ITEMCHANGED, IDC_SIZE_FORMAT_LIST, OnLvnItemchangedList)
    ON_NOTIFY(LVN_DELETEITEM,  IDC_SIZE_FORMAT_LIST, OnLvnDeleteitemList)
    ON_BN_CLICKED(IDC_SIZE_FORMAT_ADD,       &SizeFormatDlg::OnAdd)
    ON_BN_CLICKED(IDC_SIZE_FORMAT_MODIFY,    &SizeFormatDlg::OnModify)
    ON_BN_CLICKED(IDC_SIZE_FORMAT_DELETE,    &SizeFormatDlg::OnDelete)
    ON_BN_CLICKED(IDC_SIZE_FORMAT_ITEM_UP,   &SizeFormatDlg::OnItemUp)
    ON_BN_CLICKED(IDC_SIZE_FORMAT_ITEM_DOWN, &SizeFormatDlg::OnItemDown)
END_MESSAGE_MAP()

xpr_bool_t SizeFormatDlg::OnInitDialog(void) 
{
    super::OnInitDialog();

    DWORD sExtendedStyle = mListCtrl.GetExtendedStyle();
    sExtendedStyle |= LVS_EX_FULLROWSELECT;
    mListCtrl.SetExtendedStyle(sExtendedStyle);

    mListCtrl.InsertColumn(0, theApp.loadString(XPR_STRING_LITERAL("popup.size_format.list.column.no")),        LVCFMT_RIGHT,  35, -1);
    mListCtrl.InsertColumn(1, theApp.loadString(XPR_STRING_LITERAL("popup.size_format.list.column.condition")), LVCFMT_LEFT,  100, -1);
    mListCtrl.InsertColumn(2, theApp.loadString(XPR_STRING_LITERAL("popup.size_format.list.column.unit")),      LVCFMT_RIGHT,  70, -1);
    mListCtrl.InsertColumn(3, theApp.loadString(XPR_STRING_LITERAL("popup.size_format.list.column.display")),   LVCFMT_LEFT,  100, -1);

    ((CEdit *)GetDlgItem(IDC_SIZE_FORMAT_UNIT_SIZE))->LimitText(100);
    ((CEdit *)GetDlgItem(IDC_SIZE_FORMAT_UNIT_DISP))->LimitText(MAX_SIZE_FORMAT_DISP);
    ((CEdit *)GetDlgItem(IDC_SIZE_FORMAT_DECIMAL_PLACE))->LimitText(1);

    ((CSpinButtonCtrl*)GetDlgItem(IDC_SIZE_FORMAT_DECIMAL_PLACE_SPIN))->SetRange32(MIN_SIZE_FORMAT_DECIAML_DIGIT, MAX_SIZE_FORMAT_DECIAML_DIGIT);

    xpr_sint_t i;
    xpr_sint_t sIndex;
    const xpr_tchar_t *sText;

    for (i = SIZE_UNIT_BYTE; i <= SIZE_UNIT_TB; ++i)
    {
        sText = SizeFormat::getDefUnitText(i);
        if (sText == XPR_NULL)
            continue;

        sIndex = mUnitComboBox.AddString(sText);
        mUnitComboBox.SetItemData(sIndex, (DWORD_PTR)i);
    }

    SizeFormatDeque::iterator sIterator;
    SizeFormat::Item *sItem;
    SizeFormat::Item *sItem2;

    sIterator = mSizeFormatDeque->begin();
    for (; sIterator != mSizeFormatDeque->end(); ++sIterator)
    {
        sItem = *sIterator;
        if (sItem == XPR_NULL)
            continue;

        sItem2 = new SizeFormat::Item;
        *sItem2 = *sItem;

        addSizeFormat(sItem2);
    }

    if (mListCtrl.GetItemCount() > 0)
        selectItem(0);

    SetWindowText(theApp.loadString(XPR_STRING_LITERAL("popup.size_format.title")));
    SetDlgItemText(IDC_SIZE_FORMAT_LABEL_LIST,             theApp.loadString(XPR_STRING_LITERAL("popup.size_format.label.list")));
    SetDlgItemText(IDC_SIZE_FORMAT_ADD,                    theApp.loadString(XPR_STRING_LITERAL("popup.size_format.button.add")));
    SetDlgItemText(IDC_SIZE_FORMAT_MODIFY,                 theApp.loadString(XPR_STRING_LITERAL("popup.size_format.button.modify")));
    SetDlgItemText(IDC_SIZE_FORMAT_DELETE,                 theApp.loadString(XPR_STRING_LITERAL("popup.size_format.button.delete")));
    SetDlgItemText(IDC_SIZE_FORMAT_CONDITION,              theApp.loadString(XPR_STRING_LITERAL("popup.size_format.group.condition")));
    SetDlgItemText(IDC_SIZE_FORMAT_LABEL_UNIT,             theApp.loadString(XPR_STRING_LITERAL("popup.size_format.label.unit")));
    SetDlgItemText(IDC_SIZE_FORMAT_LABEL_MORE,             theApp.loadString(XPR_STRING_LITERAL("popup.size_format.label.more")));
    SetDlgItemText(IDC_SIZE_FORMAT_SIZE_DISP,              theApp.loadString(XPR_STRING_LITERAL("popup.size_format.label.display")));
    SetDlgItemText(IDC_SIZE_FORMAT_LABEL_DECIMAL_PLACE,    theApp.loadString(XPR_STRING_LITERAL("popup.size_format.label.decimal_place")));
    SetDlgItemText(IDC_SIZE_FORMAT_DEFAULT_DECIMAL_PLACE,  theApp.loadString(XPR_STRING_LITERAL("popup.size_format.default_decimal_place")));
    SetDlgItemText(IDC_SIZE_FORMAT_CUSTOM_DECIMAL_PLACE,   theApp.loadString(XPR_STRING_LITERAL("popup.size_format.custom_decimal_place")));
    SetDlgItemText(IDC_SIZE_FORMAT_ROUND_OFF,              theApp.loadString(XPR_STRING_LITERAL("popup.size_format.round_off")));
    SetDlgItemText(IDOK,                                   theApp.loadString(XPR_STRING_LITERAL("popup.common.button.ok")));
    SetDlgItemText(IDCANCEL,                               theApp.loadString(XPR_STRING_LITERAL("popup.common.button.cancel")));

    return XPR_TRUE;
}

void SizeFormatDlg::OnOK(void) 
{
    SizeFormatDeque::iterator sIterator;
    SizeFormat::Item *sItem;

    sIterator = mSizeFormatDeque->begin();
    for (; sIterator != mSizeFormatDeque->end(); ++sIterator)
    {
        sItem = *sIterator;
        if (sItem == XPR_NULL)
            continue;

        XPR_SAFE_DELETE(sItem);
    }

    mSizeFormatDeque->clear();

    xpr_sint_t i;
    xpr_sint_t sCount;

    sCount = mListCtrl.GetItemCount();
    for (i = 0; i < sCount; ++i)
    {
        sItem = (SizeFormat::Item *)mListCtrl.GetItemData(i);
        if (sItem == XPR_NULL)
            continue;

        mSizeFormatDeque->push_back(sItem);

        mListCtrl.SetItemData(i, XPR_NULL);
    }

    super::OnOK();
}

void SizeFormatDlg::setSizeFormatList(SizeFormatDeque *aSizeFormatDeque)
{
    mSizeFormatDeque = aSizeFormatDeque;
}

void SizeFormatDlg::OnLvnGetdispinfoList(NMHDR *pNMHDR, LRESULT *pResult)
{
    NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
    *pResult = 0;

    SizeFormat::Item *sItem = (SizeFormat::Item *)pDispInfo->item.lParam;
    if (sItem == XPR_NULL)
        return;

    LVITEM &sLvItem = pDispInfo->item;

    if (XPR_TEST_BITS(sLvItem.mask, LVIF_TEXT))
    {
        sLvItem.pszText[0] = XPR_STRING_LITERAL('\0');

        switch (sLvItem.iSubItem)
        {
        case 0:
            {
                _stprintf(sLvItem.pszText, XPR_STRING_LITERAL("%d"), sLvItem.iItem+1);
                break;
            }

        case 1:
            {
                SizeFormat::getFormatedNumber(sItem->mSize, sLvItem.pszText, sLvItem.cchTextMax);
                break;
            }

        case 2:
            {
                const xpr_tchar_t *sText = SizeFormat::getDefUnitText(sItem->mSizeUnit);
                if (sText != XPR_NULL)
                    _tcscpy(sLvItem.pszText, sText);
                break;
            }

        case 3:
            {
                _tcscpy(sLvItem.pszText, sItem->mText.c_str());
                break;
            }
        }
    }
}

void SizeFormatDlg::OnLvnItemchangedList(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
    *pResult = 0;

    if (!XPR_TEST_BITS(pNMLV->uChanged, LVIF_STATE))
        return;

    if (XPR_TEST_BITS(pNMLV->uNewState, LVIS_SELECTED))
    {
        SizeFormat::Item *sItem = (SizeFormat::Item *)pNMLV->lParam;
        if (sItem != XPR_NULL)
        {
            xpr_tchar_t sUnitSizeText[0xff] = {0};
            _stprintf(sUnitSizeText, XPR_STRING_LITERAL("%I64u"), sItem->mSize);

            SetDlgItemText(IDC_SIZE_FORMAT_UNIT_SIZE, sUnitSizeText);

            xpr_sint_t i;
            xpr_sint_t sCount;
            xpr_sint_t sUnit;
            xpr_sint_t sCurSel = -1;

            sCount = mUnitComboBox.GetCount();
            for (i = 0; i < sCount; ++i)
            {
                sUnit = (xpr_sint_t)mUnitComboBox.GetItemData(i);
                if (sUnit == sItem->mSizeUnit)
                {
                    sCurSel = i;
                    break;
                }
            }

            mUnitComboBox.SetCurSel(sCurSel);

            SetDlgItemText(IDC_SIZE_FORMAT_UNIT_DISP, sItem->mText.c_str());
            SetDlgItemInt(IDC_SIZE_FORMAT_DECIMAL_PLACE, sItem->mDecimalPlace);
            ((CButton *)GetDlgItem(IDC_SIZE_FORMAT_DEFAULT_DECIMAL_PLACE))->SetCheck(sItem->mDefaultDecimalPlace);
            ((CButton *)GetDlgItem(IDC_SIZE_FORMAT_CUSTOM_DECIMAL_PLACE))->SetCheck(!sItem->mDefaultDecimalPlace);
            ((CButton *)GetDlgItem(IDC_SIZE_FORMAT_ROUND_OFF))->SetCheck(sItem->mRoundOff);
        }
    }
}

void SizeFormatDlg::OnLvnDeleteitemList(NMHDR* pNMHDR, LRESULT* pResult) 
{
    NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
    *pResult = 0;

    SizeFormat::Item *sItem = (SizeFormat::Item *)pNMListView->lParam;
    XPR_SAFE_DELETE(sItem);
}

xpr_sint_t SizeFormatDlg::addSizeFormat(SizeFormat::Item *aItem, xpr_sint_t aIndex)
{
    if (aItem == XPR_NULL)
        return -1;

    LVITEM sLvItem = {0};
    sLvItem.mask       = LVIF_TEXT | LVIF_PARAM;
    sLvItem.iItem      = (aIndex != -1) ? aIndex : mListCtrl.GetItemCount();
    sLvItem.iSubItem   = 0;
    sLvItem.pszText    = LPSTR_TEXTCALLBACK;
    sLvItem.cchTextMax = XPR_MAX_PATH;
    sLvItem.lParam     = (LPARAM)aItem;

    return mListCtrl.InsertItem(&sLvItem);
}

void SizeFormatDlg::selectItem(xpr_sint_t aIndex)
{
    mListCtrl.SetFocus();
    mListCtrl.EnsureVisible(aIndex, XPR_TRUE);
    mListCtrl.SetSelectionMark(aIndex);
    mListCtrl.SetItemState(aIndex, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
}

void SizeFormatDlg::OnAdd(void)
{
    if (mListCtrl.GetItemCount() >= MAX_SIZE_FORMAT)
    {
        xpr_tchar_t sMsg[0xff] = {0};
        _stprintf(sMsg, theApp.loadFormatString(XPR_STRING_LITERAL("popup.size_format.msg.excess_max_count"), XPR_STRING_LITERAL("%d")), MAX_SIZE_FORMAT);
        MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);
        return;
    }

    xpr_tchar_t sUnitSizeText[0xff] = {0};
    xpr_tchar_t sUnitDispText[0xff] = {0};
    GetDlgItemText(IDC_SIZE_FORMAT_UNIT_SIZE, sUnitSizeText, 0xfe);
    GetDlgItemText(IDC_SIZE_FORMAT_UNIT_DISP, sUnitDispText, 0xfe);

    xpr_bool_t sDefaultDecimalPlace = ((CButton *)GetDlgItem(IDC_SIZE_FORMAT_DEFAULT_DECIMAL_PLACE))->GetCheck();
    xpr_sint_t sDecimalPlace        = GetDlgItemInt(IDC_SIZE_FORMAT_DECIMAL_PLACE);
    xpr_bool_t sRoundOff            = ((CButton *)GetDlgItem(IDC_SIZE_FORMAT_ROUND_OFF))->GetCheck();

    xpr_sint_t sCurSel = mUnitComboBox.GetCurSel();
    if (sCurSel == CB_ERR)
    {
        const xpr_tchar_t *sMsg = theApp.loadString(XPR_STRING_LITERAL("popup.size_format.msg.select_unit"));
        MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);
        mUnitComboBox.SetFocus();
        return;
    }

    xpr_sint_t sSizeUnit = (xpr_sint_t)mUnitComboBox.GetItemData(sCurSel);

    SizeFormat::Item *sItem = new SizeFormat::Item;
    _stscanf(sUnitSizeText, XPR_STRING_LITERAL("%I64u"), &sItem->mSize);
    sItem->mSizeUnit            = sSizeUnit;
    sItem->mText                = sUnitDispText;
    sItem->mDefaultDecimalPlace = sDefaultDecimalPlace;
    sItem->mDecimalPlace        = sDecimalPlace;
    sItem->mRoundOff            = sRoundOff;

    xpr_sint_t sIndex = addSizeFormat(sItem);
    if (sIndex >= 0)
        selectItem(sIndex);
}

void SizeFormatDlg::OnModify(void)
{
    xpr_sint_t sIndex = mListCtrl.GetSelectionMark();
    if (sIndex < 0)
        return;

    SizeFormat::Item *sItem = (SizeFormat::Item *)mListCtrl.GetItemData(sIndex);
    if (sItem == XPR_NULL)
        return;

    xpr_sint_t sCurSel = mUnitComboBox.GetCurSel();
    if (sCurSel == CB_ERR)
    {
        const xpr_tchar_t *sMsg = theApp.loadString(XPR_STRING_LITERAL("popup.size_format.msg.select_unit"));
        MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);
        mUnitComboBox.SetFocus();
        return;
    }

    xpr_sint_t sSizeUnit = (xpr_sint_t)mUnitComboBox.GetItemData(sCurSel);

    xpr_tchar_t sUnitSizeText[0xff] = {0};
    xpr_tchar_t sUnitDispText[0xff] = {0};
    GetDlgItemText(IDC_SIZE_FORMAT_UNIT_SIZE, sUnitSizeText, 0xfe);
    GetDlgItemText(IDC_SIZE_FORMAT_UNIT_DISP, sUnitDispText, 0xfe);

    xpr_bool_t sDefaultDecimalPlace = ((CButton *)GetDlgItem(IDC_SIZE_FORMAT_DEFAULT_DECIMAL_PLACE))->GetCheck();
    xpr_sint_t sDecimalPlace        = GetDlgItemInt(IDC_SIZE_FORMAT_DECIMAL_PLACE);
    xpr_bool_t sRoundOff            = ((CButton *)GetDlgItem(IDC_SIZE_FORMAT_ROUND_OFF))->GetCheck();

    _stscanf(sUnitSizeText, XPR_STRING_LITERAL("%I64u"), &sItem->mSize);
    sItem->mSizeUnit            = sSizeUnit;
    sItem->mText                = sUnitDispText;
    sItem->mDefaultDecimalPlace = sDefaultDecimalPlace;
    sItem->mDecimalPlace        = sDecimalPlace;
    sItem->mRoundOff            = sRoundOff;

    mListCtrl.Invalidate(XPR_FALSE);
    mListCtrl.UpdateWindow();
}

void SizeFormatDlg::OnDelete(void)
{
    xpr_sint_t sIndex = mListCtrl.GetSelectionMark();
    if (sIndex < 0)
        return;

    const xpr_tchar_t *sMsg = theApp.loadString(XPR_STRING_LITERAL("popup.size_format.msg.confirm_delete"));
    xpr_sint_t sMsgId = MessageBox(sMsg, XPR_NULL, MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2);
    if (sMsgId == IDNO)
        return;

    mListCtrl.DeleteItem(sIndex);

    mListCtrl.Invalidate(XPR_FALSE);
    mListCtrl.UpdateWindow();
}

void SizeFormatDlg::OnItemUp(void)
{
    xpr_sint_t sIndex = mListCtrl.GetSelectionMark();
    if (sIndex < 0)
        return;

    if (sIndex == 0)
        return;

    mListCtrl.SetRedraw(XPR_FALSE);

    SizeFormat::Item *sItem = (SizeFormat::Item *)mListCtrl.GetItemData(sIndex);
    mListCtrl.SetItemData(sIndex, XPR_NULL);
    mListCtrl.DeleteItem(sIndex);
    sIndex--;

    addSizeFormat(sItem, sIndex);
    selectItem(sIndex);

    mListCtrl.SetRedraw();
}

void SizeFormatDlg::OnItemDown(void)
{
    xpr_sint_t sIndex = mListCtrl.GetSelectionMark();
    if (sIndex < 0)
        return;

    xpr_sint_t sCount = mListCtrl.GetItemCount();
    if (sIndex == (sCount-1))
        return;

    mListCtrl.SetRedraw(XPR_FALSE);

    SizeFormat::Item *sItem = (SizeFormat::Item *)mListCtrl.GetItemData(sIndex);
    mListCtrl.SetItemData(sIndex, XPR_NULL);
    mListCtrl.DeleteItem(sIndex);
    sIndex++;

    addSizeFormat(sItem, sIndex);
    selectItem(sIndex);

    mListCtrl.SetRedraw();
}

xpr_bool_t SizeFormatDlg::PreTranslateMessage(MSG* pMsg) 
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
} // namespace cfg
} // namespace fxfile
