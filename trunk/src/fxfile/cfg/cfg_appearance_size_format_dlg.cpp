//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "cfg_appearance_size_format_dlg.h"

#include "../option.h"
#include "../resource.h"
#include "size_format_dlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace fxfile
{
namespace cfg
{
CfgAppearanceSizeFormatDlg::CfgAppearanceSizeFormatDlg(void)
    : super(IDD_CFG_APPEARANCE_SIZE_FORMAT, XPR_NULL)
    , mSizeFormatDeque(XPR_NULL)
{
}

CfgAppearanceSizeFormatDlg::~CfgAppearanceSizeFormatDlg(void)
{
}

void CfgAppearanceSizeFormatDlg::DoDataExchange(CDataExchange* pDX)
{
    super::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CfgAppearanceSizeFormatDlg, super)
    ON_WM_DESTROY()
    ON_BN_CLICKED(IDC_CFG_SIZE_FORMAT_CUSTOM_FORMAT, OnCustomFormat)
END_MESSAGE_MAP()

xpr_bool_t CfgAppearanceSizeFormatDlg::OnInitDialog(void) 
{
    super::OnInitDialog();

    // disable apply button event
    addIgnoreApply(IDC_CFG_SIZE_FORMAT_CUSTOM_FORMAT);

    SetDlgItemText(IDC_CFG_SIZE_FORMAT_LABEL_SINGLE_ITEM_UNIT,    gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.size_format.label.single_item_unit")));
    SetDlgItemText(IDC_CFG_SIZE_FORMAT_LABEL_MULTIPLE_ITEMS_UNIT, gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.size_format.label.multiple_items_unit")));
    SetDlgItemText(IDC_CFG_SIZE_FORMAT_CUSTOM_FORMAT,             gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.size_format.button.custom_format")));

    return XPR_TRUE;
}

void CfgAppearanceSizeFormatDlg::OnDestroy(void)
{
    if (mSizeFormatDeque != XPR_NULL)
    {
        SizeFormatDeque::iterator sIterator;
        SizeFormat::Item *sItem;

        sIterator = mSizeFormatDeque->begin();
        for (; sIterator != mSizeFormatDeque->end(); ++sIterator)
        {
            sItem = *sIterator;
            XPR_SAFE_DELETE(sItem);
        }

        mSizeFormatDeque->clear();
        XPR_SAFE_DELETE(mSizeFormatDeque);
    }

    super::OnDestroy();
}

void CfgAppearanceSizeFormatDlg::copyListFromSizeFormat(void)
{
    if (mSizeFormatDeque != XPR_NULL)
        return;

    mSizeFormatDeque = new SizeFormatDeque;

    SizeFormat &sSizeFormat = SizeFormat::instance();

    xpr_size_t i;
    xpr_size_t sCount;
    SizeFormat::Item *sItem;
    SizeFormat::Item *sItem2;

    sCount = sSizeFormat.getCount();
    for (i = 0; i < sCount; ++i)
    {
        sItem = sSizeFormat.getItem(i);
        if (sItem == XPR_NULL)
            continue;

        sItem2 = new SizeFormat::Item;
        *sItem2 = *sItem;

        mSizeFormatDeque->push_back(sItem2);
    }
}

void CfgAppearanceSizeFormatDlg::copySizeFormatFromList(void)
{
    if (mSizeFormatDeque == XPR_NULL)
        return;

    SizeFormat &sSizeFormat = SizeFormat::instance();
    sSizeFormat.clear();

    SizeFormatDeque::iterator sIterator;
    SizeFormat::Item *sItem;

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

void CfgAppearanceSizeFormatDlg::onInit(const Option::Config &aConfig)
{
    xpr_sint_t i;
    xpr_sint_t sIndex;
    const xpr_tchar_t *sText;
    CComboBox *sComboBox;

    sComboBox = (CComboBox *)GetDlgItem(IDC_CFG_SIZE_FORMAT_SINGLE_ITEM_UNIT);
    for (i = SIZE_UNIT_DEFAULT; i <= SIZE_UNIT_TB; ++i)
    {
        sText = SizeFormat::getDefUnitText(i);
        if (sText == XPR_NULL)
            continue;

        sIndex = sComboBox->AddString(sText);
        sComboBox->SetItemData(sIndex, (DWORD_PTR)i);

        if (i == aConfig.mSingleSelFileSizeUnit)
            sComboBox->SetCurSel(sIndex);
    }

    sComboBox = (CComboBox *)GetDlgItem(IDC_CFG_SIZE_FORMAT_MULTIPLE_ITEMS_UNIT);
    for (i = SIZE_UNIT_DEFAULT; i <= SIZE_UNIT_TB; ++i)
    {
        sText = SizeFormat::getDefUnitText(i);
        if (sText == XPR_NULL)
            continue;

        sIndex = sComboBox->AddString(sText);
        sComboBox->SetItemData(sIndex, (DWORD_PTR)i);

        if (i == aConfig.mMultiSelFileSizeUnit)
            sComboBox->SetCurSel(sIndex);
    }

    copyListFromSizeFormat();
}

void CfgAppearanceSizeFormatDlg::onApply(Option::Config &aConfig)
{
    xpr_sint_t sCurSel;
    CComboBox *sComboBox;

    aConfig.mSingleSelFileSizeUnit = SIZE_UNIT_DEFAULT;
    sComboBox = (CComboBox *)GetDlgItem(IDC_CFG_SIZE_FORMAT_SINGLE_ITEM_UNIT);
    sCurSel = sComboBox->GetCurSel();
    if (sCurSel != CB_ERR)
        aConfig.mSingleSelFileSizeUnit = (xpr_sint_t)sComboBox->GetItemData(sCurSel);

    aConfig.mMultiSelFileSizeUnit = SIZE_UNIT_DEFAULT;
    sComboBox = (CComboBox *)GetDlgItem(IDC_CFG_SIZE_FORMAT_MULTIPLE_ITEMS_UNIT);
    sCurSel = sComboBox->GetCurSel();
    if (sCurSel != CB_ERR)
        aConfig.mMultiSelFileSizeUnit = (xpr_sint_t)sComboBox->GetItemData(sCurSel);

    copySizeFormatFromList();
}

void CfgAppearanceSizeFormatDlg::OnCustomFormat(void)
{
    copyListFromSizeFormat();

    SizeFormatDlg sDlg;
    sDlg.setSizeFormatList(mSizeFormatDeque);
    if (sDlg.DoModal() != IDOK)
        return;

    setModified();
}
} // namespace cfg
} // namespace fxfile
