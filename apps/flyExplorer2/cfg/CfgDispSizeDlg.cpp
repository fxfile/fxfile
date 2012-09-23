//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "CfgDispSizeDlg.h"

#include "../Option.h"
#include "../resource.h"
#include "SizeFormatDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CfgDispSizeDlg::CfgDispSizeDlg(void)
    : super(IDD_CFG_DISP_SIZE, XPR_NULL)
    , mSizeFormatDeque(XPR_NULL)
{
}

CfgDispSizeDlg::~CfgDispSizeDlg(void)
{
}

void CfgDispSizeDlg::DoDataExchange(CDataExchange* pDX)
{
    super::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CfgDispSizeDlg, super)
    ON_BN_CLICKED(IDC_CFG_DISP_SIZE_CUSTOM_FORMAT, OnCustomFormat)
END_MESSAGE_MAP()

xpr_bool_t CfgDispSizeDlg::OnInitDialog(void) 
{
    super::OnInitDialog();

    xpr_sint_t i;
    xpr_sint_t sIndex;
    const xpr_tchar_t *sText;
    CComboBox *sComboBox;

    sComboBox = (CComboBox *)GetDlgItem(IDC_CFG_DISP_SIZE_SINGLE_ITEM_UNIT);
    for (i = SIZE_UNIT_DEFAULT; i <= SIZE_UNIT_TB; ++i)
    {
        sText = fxb::SizeFormat::getDefUnitText(i);
        if (sText == XPR_NULL)
            continue;

        sIndex = sComboBox->AddString(sText);
        sComboBox->SetItemData(sIndex, (DWORD_PTR)i);

        if (i == gOpt->mSingleSelFileSizeUnit)
            sComboBox->SetCurSel(sIndex);
    }

    sComboBox = (CComboBox *)GetDlgItem(IDC_CFG_DISP_SIZE_MULTIPLE_ITEMS_UNIT);
    for (i = SIZE_UNIT_DEFAULT; i <= SIZE_UNIT_TB; ++i)
    {
        sText = fxb::SizeFormat::getDefUnitText(i);
        if (sText == XPR_NULL)
            continue;

        sIndex = sComboBox->AddString(sText);
        sComboBox->SetItemData(sIndex, (DWORD_PTR)i);

        if (i == gOpt->mMultiSelFileSizeUnit)
            sComboBox->SetCurSel(sIndex);
    }

    copyListFromSizeFormat();

    SetDlgItemText(IDC_CFG_DISP_SIZE_LABEL_SINGLE_ITEM_UNIT,    theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.display.size.label.single_item_unit")));
    SetDlgItemText(IDC_CFG_DISP_SIZE_LABEL_MULTIPLE_ITEMS_UNIT, theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.display.size.label.multiple_items_unit")));
    SetDlgItemText(IDC_CFG_DISP_SIZE_CUSTOM_FORMAT,             theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.display.size.button.custom_format")));

    return XPR_TRUE;
}

xpr_bool_t CfgDispSizeDlg::DestroyWindow(void)
{
    if (mSizeFormatDeque != XPR_NULL)
    {
        SizeFormatDeque::iterator sIterator;
        fxb::SizeFormat::Item *sItem;

        sIterator = mSizeFormatDeque->begin();
        for (; sIterator != mSizeFormatDeque->end(); ++sIterator)
        {
            sItem = *sIterator;
            XPR_SAFE_DELETE(sItem);
        }

        mSizeFormatDeque->clear();
        XPR_SAFE_DELETE(mSizeFormatDeque);
    }

    return CDialog::DestroyWindow();
}

void CfgDispSizeDlg::copyListFromSizeFormat(void)
{
    if (mSizeFormatDeque != XPR_NULL)
        return;

    mSizeFormatDeque = new SizeFormatDeque;

    fxb::SizeFormat &sSizeFormat = fxb::SizeFormat::instance();

    xpr_size_t i;
    xpr_size_t sCount;
    fxb::SizeFormat::Item *sItem;
    fxb::SizeFormat::Item *sItem2;

    sCount = sSizeFormat.getCount();
    for (i = 0; i < sCount; ++i)
    {
        sItem = sSizeFormat.getItem(i);
        if (sItem == XPR_NULL)
            continue;

        sItem2 = new fxb::SizeFormat::Item;
        *sItem2 = *sItem;

        mSizeFormatDeque->push_back(sItem2);
    }
}

void CfgDispSizeDlg::copySizeFormatFromList(void)
{
    if (mSizeFormatDeque == XPR_NULL)
        return;

    fxb::SizeFormat &sSizeFormat = fxb::SizeFormat::instance();
    sSizeFormat.clear();

    SizeFormatDeque::iterator sIterator;
    fxb::SizeFormat::Item *sItem;

    sIterator = mSizeFormatDeque->begin();
    for (; sIterator != mSizeFormatDeque->end(); ++sIterator)
    {
        sItem = *sIterator;
        if (sItem == XPR_NULL)
            continue;

        sSizeFormat.addItem(sItem);
    }

    mSizeFormatDeque->clear();
    XPR_SAFE_DELETE(mSizeFormatDeque);
}

void CfgDispSizeDlg::OnApply(void)
{
    OptionMgr &sOptionMgr = OptionMgr::instance();

    xpr_sint_t sCurSel;
    CComboBox *sComboBox;

    gOpt->mSingleSelFileSizeUnit = SIZE_UNIT_DEFAULT;
    sComboBox = (CComboBox *)GetDlgItem(IDC_CFG_DISP_SIZE_SINGLE_ITEM_UNIT);
    sCurSel = sComboBox->GetCurSel();
    if (sCurSel != CB_ERR)
        gOpt->mSingleSelFileSizeUnit = (xpr_sint_t)sComboBox->GetItemData(sCurSel);

    gOpt->mMultiSelFileSizeUnit = SIZE_UNIT_DEFAULT;
    sComboBox = (CComboBox *)GetDlgItem(IDC_CFG_DISP_SIZE_MULTIPLE_ITEMS_UNIT);
    sCurSel = sComboBox->GetCurSel();
    if (sCurSel != CB_ERR)
        gOpt->mMultiSelFileSizeUnit = (xpr_sint_t)sComboBox->GetItemData(sCurSel);

    copySizeFormatFromList();

    sOptionMgr.applyFolderCtrl(2, XPR_FALSE);
    sOptionMgr.applyExplorerView(2, XPR_FALSE);
}

xpr_bool_t CfgDispSizeDlg::OnCommand(WPARAM wParam, LPARAM lParam) 
{
    xpr_uint_t sNotifyMsg = HIWORD(wParam);
    xpr_uint_t sId = LOWORD(wParam);

    if (sNotifyMsg == BN_CLICKED)
    {
    }
    else if (sNotifyMsg == CBN_SELCHANGE)
    {
        switch (sId)
        {
        case IDC_CFG_DISP_SIZE_SINGLE_ITEM_UNIT:
        case IDC_CFG_DISP_SIZE_MULTIPLE_ITEMS_UNIT:
            setModified();
            break;
        }
    }

    return super::OnCommand(wParam, lParam);
}

void CfgDispSizeDlg::OnCustomFormat(void)
{
    copyListFromSizeFormat();

    SizeFormatDlg sDlg;
    sDlg.setSizeFormatList(mSizeFormatDeque);
    if (sDlg.DoModal() != IDOK)
        return;

    setModified();
}
