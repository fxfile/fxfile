//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "cfg_appearance_file_list_dlg.h"

#include "gui/FileDialogST.h"

#include "../size_format.h"
#include "../option.h"
#include "../resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace fxfile
{
namespace cfg
{
CfgAppearanceFileListDlg::CfgAppearanceFileListDlg(void)
    : super(IDD_CFG_APPEARANCE_FILE_LIST, XPR_NULL)
{
}

void CfgAppearanceFileListDlg::DoDataExchange(CDataExchange* pDX)
{
    super::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CfgAppearanceFileListDlg, super)
    ON_BN_CLICKED(IDC_CFG_FILE_LIST_CUSTOM_ICON_16_PATH_BROWSE, OnCustomIcon16x16Browse)
    ON_BN_CLICKED(IDC_CFG_FILE_LIST_CUSTOM_ICON_32_PATH_BROWSE, OnCustomIcon32x32Browse)
END_MESSAGE_MAP()

xpr_bool_t CfgAppearanceFileListDlg::OnInitDialog(void) 
{
    super::OnInitDialog();

    // disable apply button event
    addIgnoreApply(IDC_CFG_FILE_LIST_CUSTOM_ICON_16_PATH_BROWSE);
    addIgnoreApply(IDC_CFG_FILE_LIST_CUSTOM_ICON_32_PATH_BROWSE);

    CComboBox *sComboBox;

    sComboBox = (CComboBox *)GetDlgItem(IDC_CFG_FILE_LIST_SHOW_LIST_TYPE);
    sComboBox->AddString(gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.file_list.list_type.default")));
    sComboBox->AddString(gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.file_list.list_type.only_folder")));
    sComboBox->AddString(gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.file_list.list_type.only_file")));

    sComboBox = (CComboBox *)GetDlgItem(IDC_CFG_FILE_LIST_ITEM_TEXT_CASE);
    sComboBox->AddString(gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.file_list.case_sensitivity.original")));
    sComboBox->AddString(gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.file_list.case_sensitivity.upper_case")));
    sComboBox->AddString(gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.file_list.case_sensitivity.lower_case")));

    ::SHAutoComplete(GetDlgItem(IDC_CFG_FILE_LIST_CUSTOM_ICON_16_PATH)->m_hWnd, SHACF_FILESYSTEM);
    ::SHAutoComplete(GetDlgItem(IDC_CFG_FILE_LIST_CUSTOM_ICON_32_PATH)->m_hWnd, SHACF_FILESYSTEM);

    ((CEdit *)GetDlgItem(IDC_CFG_FILE_LIST_CUSTOM_ICON_16_PATH))->LimitText(XPR_MAX_PATH);
    ((CEdit *)GetDlgItem(IDC_CFG_FILE_LIST_CUSTOM_ICON_32_PATH))->LimitText(XPR_MAX_PATH);

    SetDlgItemText(IDC_CFG_FILE_LIST_SHOW_PARENT_FOLDER,   gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.file_list.check.show_parent_folder")));
    SetDlgItemText(IDC_CFG_FILE_LIST_SHOW_EACH_DRIVE,      gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.file_list.check.show_each_drive")));
    SetDlgItemText(IDC_CFG_FILE_LIST_SHOW_DRIVE_USAGE,     gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.file_list.check.show_drive_usage")));
    SetDlgItemText(IDC_CFG_FILE_LIST_SHOW_DRIVE_ITEM,      gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.file_list.check.show_drive_item")));
    SetDlgItemText(IDC_CFG_FILE_LIST_SHOW_24_HOUR_FORMAT,  gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.file_list.check.show_24-hour_format")));
    SetDlgItemText(IDC_CFG_FILE_LIST_SHOW_2_DIGITS_YEAR,   gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.file_list.check.show_2_digits_year")));
    SetDlgItemText(IDC_CFG_FILE_LIST_SHOW_GRID_LINES,      gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.file_list.check.show_grid_lines")));
    SetDlgItemText(IDC_CFG_FILE_LIST_FULL_ROW_SELECTION,   gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.file_list.check.full_row_selection")));
    SetDlgItemText(IDC_CFG_FILE_LIST_USE_CUSTOM_ICON,      gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.file_list.check.use_custom_icon")));
    SetDlgItemText(IDC_CFG_FILE_LIST_LABEL_SHOW_LIST_TYPE, gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.file_list.label.show_list_type")));
    SetDlgItemText(IDC_CFG_FILE_LIST_LABEL_TEXT_NAME_CASE, gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.file_list.label.case_sensitivity")));
    SetDlgItemText(IDC_CFG_FILE_LIST_LABEL_SIZE_UNIT,      gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.file_list.label.size_display_unit")));

    return XPR_TRUE;
}

void CfgAppearanceFileListDlg::onInit(const Option::Config &aConfig)
{
    xpr_sint_t i;
    xpr_sint_t sIndex;
    const xpr_tchar_t *sText;
    CComboBox *sComboBox;

    sComboBox = (CComboBox *)GetDlgItem(IDC_CFG_FILE_LIST_SIZE_UNIT);
    for (i = SIZE_UNIT_DEFAULT; i <= SIZE_UNIT_TB; ++i)
    {
        sText = SizeFormat::getDefUnitText(i);
        if (sText == XPR_NULL)
            continue;

        sIndex = sComboBox->AddString(sText);
        sComboBox->SetItemData(sIndex, (DWORD_PTR)i);

        if (i == aConfig.mExplorerSizeUnit)
            sComboBox->SetCurSel(sIndex);
    }

    ((CButton *)GetDlgItem(IDC_CFG_FILE_LIST_SHOW_GRID_LINES     ))->SetCheck(aConfig.mGridLines ? 1 : 0);
    ((CButton *)GetDlgItem(IDC_CFG_FILE_LIST_FULL_ROW_SELECTION  ))->SetCheck(aConfig.mFullRowSelect ? 1 : 0);
    ((CButton *)GetDlgItem(IDC_CFG_FILE_LIST_SHOW_PARENT_FOLDER  ))->SetCheck(aConfig.mExplorerParentFolder);
    ((CButton *)GetDlgItem(IDC_CFG_FILE_LIST_SHOW_EACH_DRIVE     ))->SetCheck(aConfig.mExplorerShowDrive);
    ((CButton *)GetDlgItem(IDC_CFG_FILE_LIST_SHOW_DRIVE_ITEM     ))->SetCheck(aConfig.mExplorerShowDriveItem);
    ((CButton *)GetDlgItem(IDC_CFG_FILE_LIST_SHOW_DRIVE_USAGE    ))->SetCheck(aConfig.mExplorerShowDriveSize);
    ((CButton *)GetDlgItem(IDC_CFG_FILE_LIST_SHOW_24_HOUR_FORMAT ))->SetCheck(aConfig.mExplorer24HourTime);
    ((CButton *)GetDlgItem(IDC_CFG_FILE_LIST_SHOW_2_DIGITS_YEAR  ))->SetCheck(aConfig.mExplorer2YearDate);
    ((CButton *)GetDlgItem(IDC_CFG_FILE_LIST_USE_CUSTOM_ICON     ))->SetCheck(aConfig.mExplorerCustomIcon);

    SetDlgItemText(IDC_CFG_FILE_LIST_CUSTOM_ICON_16_PATH, aConfig.mExplorerCustomIconFile[0]);
    SetDlgItemText(IDC_CFG_FILE_LIST_CUSTOM_ICON_32_PATH, aConfig.mExplorerCustomIconFile[1]);

    ((CComboBox *)GetDlgItem(IDC_CFG_FILE_LIST_SHOW_LIST_TYPE))->SetCurSel(aConfig.mExplorerListType);
    ((CComboBox *)GetDlgItem(IDC_CFG_FILE_LIST_ITEM_TEXT_CASE))->SetCurSel(aConfig.mExplorerNameCaseType);
}

void CfgAppearanceFileListDlg::onApply(Option::Config &aConfig)
{
    aConfig.mGridLines             = ((CButton *)GetDlgItem(IDC_CFG_FILE_LIST_SHOW_GRID_LINES     ))->GetCheck();
    aConfig.mFullRowSelect         = ((CButton *)GetDlgItem(IDC_CFG_FILE_LIST_FULL_ROW_SELECTION  ))->GetCheck();
    aConfig.mExplorerParentFolder  = ((CButton *)GetDlgItem(IDC_CFG_FILE_LIST_SHOW_PARENT_FOLDER  ))->GetCheck();
    aConfig.mExplorerShowDrive     = ((CButton *)GetDlgItem(IDC_CFG_FILE_LIST_SHOW_EACH_DRIVE     ))->GetCheck();
    aConfig.mExplorerShowDriveItem = ((CButton *)GetDlgItem(IDC_CFG_FILE_LIST_SHOW_DRIVE_ITEM     ))->GetCheck();
    aConfig.mExplorerShowDriveSize = ((CButton *)GetDlgItem(IDC_CFG_FILE_LIST_SHOW_DRIVE_USAGE    ))->GetCheck();
    aConfig.mExplorer24HourTime    = ((CButton *)GetDlgItem(IDC_CFG_FILE_LIST_SHOW_24_HOUR_FORMAT ))->GetCheck();
    aConfig.mExplorer2YearDate     = ((CButton *)GetDlgItem(IDC_CFG_FILE_LIST_SHOW_2_DIGITS_YEAR  ))->GetCheck();
    aConfig.mExplorerCustomIcon    = ((CButton *)GetDlgItem(IDC_CFG_FILE_LIST_USE_CUSTOM_ICON     ))->GetCheck();

    GetDlgItemText(IDC_CFG_FILE_LIST_CUSTOM_ICON_16_PATH, aConfig.mExplorerCustomIconFile[0], XPR_MAX_PATH);
    GetDlgItemText(IDC_CFG_FILE_LIST_CUSTOM_ICON_32_PATH, aConfig.mExplorerCustomIconFile[1], XPR_MAX_PATH);

    xpr_sint_t sCurSel;
    CComboBox *sComboBox;

    aConfig.mExplorerSizeUnit = SIZE_UNIT_DEFAULT;
    sComboBox = (CComboBox *)GetDlgItem(IDC_CFG_FILE_LIST_SIZE_UNIT);
    sCurSel = sComboBox->GetCurSel();
    if (sCurSel != CB_ERR)
        aConfig.mExplorerSizeUnit = (xpr_sint_t)sComboBox->GetItemData(sCurSel);

    aConfig.mExplorerListType     = ((CComboBox *)GetDlgItem(IDC_CFG_FILE_LIST_SHOW_LIST_TYPE))->GetCurSel();
    aConfig.mExplorerNameCaseType = ((CComboBox *)GetDlgItem(IDC_CFG_FILE_LIST_ITEM_TEXT_CASE))->GetCurSel();
}

xpr_bool_t CfgAppearanceFileListDlg::loadImage(xpr_sint_t aWidth, xpr_sint_t aHeight, xpr::tstring &aPath, xpr_bool_t &aSatisfied)
{
    aSatisfied = XPR_FALSE;

    xpr_tchar_t sFilter[0xff] = {0};
    _stprintf(sFilter, XPR_STRING_LITERAL("%s (*.bmp)\0*.bmp\0%s (*.*)\0*.*\0\0"), gApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearance.file_list.file_dialog.filter.16bit_bitmap_file")), gApp.loadString(XPR_STRING_LITERAL("popup.common.file_dialog.filter.all")));
    CFileDialogST sFileDialog(XPR_TRUE, XPR_STRING_LITERAL("*.bmp"), XPR_NULL, OFN_HIDEREADONLY, sFilter, this);
    if (sFileDialog.DoModal() != IDOK)
        return XPR_FALSE;

    aPath = sFileDialog.GetPathName();

    xpr_rcode_t sRcode;
    xpr_ssize_t sRead;
    xpr::FileIo sFileIo;

    sRcode = sFileIo.open(aPath.c_str(), xpr::FileIo::OpenModeReadOnly);
    if (XPR_RCODE_IS_SUCCESS(sRcode))
    {
        BITMAPINFOHEADER sBitmapInfoHeader = {0};

        sRcode = sFileIo.seekFromBegin(sizeof(BITMAPFILEHEADER));
        sRcode = sFileIo.read(&sBitmapInfoHeader, sizeof(BITMAPINFOHEADER), &sRead);
        sFileIo.close();

        if (sBitmapInfoHeader.biBitCount == 16 && (sBitmapInfoHeader.biWidth % aWidth) == 0 && sBitmapInfoHeader.biHeight == aHeight)
        {
            aSatisfied = XPR_TRUE;
        }
    }

    return XPR_TRUE;
}

void CfgAppearanceFileListDlg::OnCustomIcon16x16Browse(void) 
{
    xpr_bool_t sSatisfied = XPR_FALSE;
    xpr::tstring sPath;

    if (loadImage(16, 16, sPath, sSatisfied) == XPR_FALSE)
        return;

    if (sSatisfied == XPR_FALSE)
    {
        xpr_tchar_t sMsg[XPR_MAX_PATH + 0xff + 1] = {0};
        _stprintf(sMsg, gApp.loadFormatString(XPR_STRING_LITERAL("popup.cfg.body.appearance.file_list.msg.not_bitmap"), XPR_STRING_LITERAL("%s")), sPath.c_str());
        MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);
        return;
    }

    SetDlgItemText(IDC_CFG_FILE_LIST_CUSTOM_ICON_16_PATH, sPath.c_str());
    setModified();
}

void CfgAppearanceFileListDlg::OnCustomIcon32x32Browse(void) 
{
    xpr_bool_t sSatisfied = XPR_FALSE;
    xpr::tstring sPath;

    if (loadImage(32, 32, sPath, sSatisfied) == XPR_FALSE)
        return;

    if (sSatisfied == XPR_FALSE)
    {
        xpr_tchar_t sMsg[XPR_MAX_PATH + 0xff + 1] = {0};
        _stprintf(sMsg, gApp.loadFormatString(XPR_STRING_LITERAL("popup.cfg.body.appearance.file_list.msg.not_bitmap"), XPR_STRING_LITERAL("%s")), sPath.c_str());
        MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);
        return;
    }

    SetDlgItemText(IDC_CFG_FILE_LIST_CUSTOM_ICON_32_PATH, sPath.c_str());
    setModified();
}
} // namespace cfg
} // namespace fxfile
