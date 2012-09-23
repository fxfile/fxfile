//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "CfgExpDispDlg.h"

#include "../fxb/fxb_size_format.h"
#include "../rgc/FileDialogST.h"

#include "../Option.h"
#include "../resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CfgExpDispDlg::CfgExpDispDlg(void)
    : super(IDD_CFG_EXP_DISP, XPR_NULL)
{
}

void CfgExpDispDlg::DoDataExchange(CDataExchange* pDX)
{
    super::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CfgExpDispDlg, super)
    ON_BN_CLICKED(IDC_CFG_EXP_DISP_CUSTOM_ICON_16_PATH_BROWSE, OnCustomIcon16x16Browse)
    ON_BN_CLICKED(IDC_CFG_EXP_DISP_CUSTOM_ICON_32_PATH_BROWSE, OnCustomIcon32x32Browse)
END_MESSAGE_MAP()

xpr_bool_t CfgExpDispDlg::OnInitDialog(void) 
{
    super::OnInitDialog();

    xpr_sint_t i;
    xpr_sint_t sIndex;
    const xpr_tchar_t *sText;
    CComboBox *sComboBox;

    sComboBox = (CComboBox *)GetDlgItem(IDC_CFG_EXP_DISP_SHOW_LIST_TYPE);
    sComboBox->AddString(theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.explorer_window.display.list_type.default")));
    sComboBox->AddString(theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.explorer_window.display.list_type.only_folder")));
    sComboBox->AddString(theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.explorer_window.display.list_type.only_file")));

    sComboBox = (CComboBox *)GetDlgItem(IDC_CFG_EXP_DISP_ITEM_TEXT_CASE);
    sComboBox->AddString(theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.explorer_window.display.case_sensitivity.original")));
    sComboBox->AddString(theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.explorer_window.display.case_sensitivity.upper_case")));
    sComboBox->AddString(theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.explorer_window.display.case_sensitivity.lower_case")));

    sComboBox = (CComboBox *)GetDlgItem(IDC_CFG_EXP_DISP_SIZE_UNIT);
    for (i = SIZE_UNIT_DEFAULT; i <= SIZE_UNIT_TB; ++i)
    {
        sText = fxb::SizeFormat::getDefUnitText(i);
        if (sText == XPR_NULL)
            continue;

        sIndex = sComboBox->AddString(sText);
        sComboBox->SetItemData(sIndex, (DWORD_PTR)i);

        if (i == gOpt->mExplorerSizeUnit)
            sComboBox->SetCurSel(sIndex);
    }

    ((CButton *)GetDlgItem(IDC_CFG_EXP_DISP_SHOW_GRID_LINES     ))->SetCheck(gOpt->mGridLines ? 1 : 0);
    ((CButton *)GetDlgItem(IDC_CFG_EXP_DISP_FULL_ROW_SELECTION  ))->SetCheck(gOpt->mFullRowSelect ? 1 : 0);
    ((CButton *)GetDlgItem(IDC_CFG_EXP_DISP_SHOW_PARENT_FOLDER  ))->SetCheck(gOpt->mExplorerParentFolder);
    ((CButton *)GetDlgItem(IDC_CFG_EXP_DISP_SHOW_EACH_DRIVE     ))->SetCheck(gOpt->mExplorerShowDrive);
    ((CButton *)GetDlgItem(IDC_CFG_EXP_DISP_SHOW_DRIVE_ITEM     ))->SetCheck(gOpt->mExplorerShowDriveItem);
    ((CButton *)GetDlgItem(IDC_CFG_EXP_DISP_SHOW_DRIVE_USAGE    ))->SetCheck(gOpt->mExplorerShowDriveSize);
    ((CButton *)GetDlgItem(IDC_CFG_EXP_DISP_SHOW_24_HOUR_FORMAT ))->SetCheck(gOpt->mExplorer24HourTime);
    ((CButton *)GetDlgItem(IDC_CFG_EXP_DISP_SHOW_2_DIGITS_YEAR  ))->SetCheck(gOpt->mExplorer2YearDate);
    ((CButton *)GetDlgItem(IDC_CFG_EXP_DISP_USE_CUSTOM_ICON     ))->SetCheck(gOpt->mExplorerCustomIcon);

    SetDlgItemText(IDC_CFG_EXP_DISP_CUSTOM_ICON_16_PATH, gOpt->mExplorerCustomIconFile[0]);
    SetDlgItemText(IDC_CFG_EXP_DISP_CUSTOM_ICON_32_PATH, gOpt->mExplorerCustomIconFile[1]);
    ::SHAutoComplete(GetDlgItem(IDC_CFG_EXP_DISP_CUSTOM_ICON_16_PATH)->m_hWnd, SHACF_FILESYSTEM);
    ::SHAutoComplete(GetDlgItem(IDC_CFG_EXP_DISP_CUSTOM_ICON_32_PATH)->m_hWnd, SHACF_FILESYSTEM);

    ((CEdit *)GetDlgItem(IDC_CFG_EXP_DISP_CUSTOM_ICON_16_PATH))->LimitText(XPR_MAX_PATH);
    ((CEdit *)GetDlgItem(IDC_CFG_EXP_DISP_CUSTOM_ICON_32_PATH))->LimitText(XPR_MAX_PATH);

    ((CComboBox *)GetDlgItem(IDC_CFG_EXP_DISP_SHOW_LIST_TYPE))->SetCurSel(gOpt->mExplorerListType);
    ((CComboBox *)GetDlgItem(IDC_CFG_EXP_DISP_ITEM_TEXT_CASE))->SetCurSel(gOpt->mExplorerNameCaseType);

    SetDlgItemText(IDC_CFG_EXP_DISP_SHOW_PARENT_FOLDER,   theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.explorer_window.display.check.show_parent_folder")));
    SetDlgItemText(IDC_CFG_EXP_DISP_SHOW_EACH_DRIVE,      theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.explorer_window.display.check.show_each_drive")));
    SetDlgItemText(IDC_CFG_EXP_DISP_SHOW_DRIVE_USAGE,     theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.explorer_window.display.check.show_drive_usage")));
    SetDlgItemText(IDC_CFG_EXP_DISP_SHOW_DRIVE_ITEM,      theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.explorer_window.display.check.show_drive_item")));
    SetDlgItemText(IDC_CFG_EXP_DISP_SHOW_24_HOUR_FORMAT,  theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.explorer_window.display.check.show_24-hour_format")));
    SetDlgItemText(IDC_CFG_EXP_DISP_SHOW_2_DIGITS_YEAR,   theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.explorer_window.display.check.show_2_digits_year")));
    SetDlgItemText(IDC_CFG_EXP_DISP_SHOW_GRID_LINES,      theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.explorer_window.display.check.show_grid_lines")));
    SetDlgItemText(IDC_CFG_EXP_DISP_FULL_ROW_SELECTION,   theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.explorer_window.display.check.full_row_selection")));
    SetDlgItemText(IDC_CFG_EXP_DISP_USE_CUSTOM_ICON,      theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.explorer_window.display.check.use_custom_icon")));
    SetDlgItemText(IDC_CFG_EXP_DISP_LABEL_SHOW_LIST_TYPE, theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.explorer_window.display.label.show_list_type")));
    SetDlgItemText(IDC_CFG_EXP_DISP_LABEL_TEXT_NAME_CASE, theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.explorer_window.display.label.case_sensitivity")));
    SetDlgItemText(IDC_CFG_EXP_DISP_LABEL_SIZE_UNIT,      theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.explorer_window.display.label.size_display_unit")));

    return XPR_TRUE;
}

void CfgExpDispDlg::OnApply(void)
{
    OptionMgr &sOptionMgr = OptionMgr::instance();

    gOpt->mGridLines             = ((CButton *)GetDlgItem(IDC_CFG_EXP_DISP_SHOW_GRID_LINES     ))->GetCheck();
    gOpt->mFullRowSelect         = ((CButton *)GetDlgItem(IDC_CFG_EXP_DISP_FULL_ROW_SELECTION  ))->GetCheck();
    gOpt->mExplorerParentFolder  = ((CButton *)GetDlgItem(IDC_CFG_EXP_DISP_SHOW_PARENT_FOLDER  ))->GetCheck();
    gOpt->mExplorerShowDrive     = ((CButton *)GetDlgItem(IDC_CFG_EXP_DISP_SHOW_EACH_DRIVE     ))->GetCheck();
    gOpt->mExplorerShowDriveItem = ((CButton *)GetDlgItem(IDC_CFG_EXP_DISP_SHOW_DRIVE_ITEM     ))->GetCheck();
    gOpt->mExplorerShowDriveSize = ((CButton *)GetDlgItem(IDC_CFG_EXP_DISP_SHOW_DRIVE_USAGE    ))->GetCheck();
    gOpt->mExplorer24HourTime    = ((CButton *)GetDlgItem(IDC_CFG_EXP_DISP_SHOW_24_HOUR_FORMAT ))->GetCheck();
    gOpt->mExplorer2YearDate     = ((CButton *)GetDlgItem(IDC_CFG_EXP_DISP_SHOW_2_DIGITS_YEAR  ))->GetCheck();
    gOpt->mExplorerCustomIcon    = ((CButton *)GetDlgItem(IDC_CFG_EXP_DISP_USE_CUSTOM_ICON     ))->GetCheck();

    GetDlgItemText(IDC_CFG_EXP_DISP_CUSTOM_ICON_16_PATH, gOpt->mExplorerCustomIconFile[0], XPR_MAX_PATH);
    GetDlgItemText(IDC_CFG_EXP_DISP_CUSTOM_ICON_32_PATH, gOpt->mExplorerCustomIconFile[1], XPR_MAX_PATH);

    xpr_sint_t sCurSel;
    CComboBox *sComboBox;

    gOpt->mExplorerSizeUnit = SIZE_UNIT_DEFAULT;
    sComboBox = (CComboBox *)GetDlgItem(IDC_CFG_EXP_DISP_SIZE_UNIT);
    sCurSel = sComboBox->GetCurSel();
    if (sCurSel != CB_ERR)
        gOpt->mExplorerSizeUnit = (xpr_sint_t)sComboBox->GetItemData(sCurSel);

    gOpt->mExplorerListType     = ((CComboBox *)GetDlgItem(IDC_CFG_EXP_DISP_SHOW_LIST_TYPE))->GetCurSel();
    gOpt->mExplorerNameCaseType = ((CComboBox *)GetDlgItem(IDC_CFG_EXP_DISP_ITEM_TEXT_CASE))->GetCurSel();

    sOptionMgr.applyFolderCtrl(2, XPR_FALSE);
    sOptionMgr.applyExplorerView(2, XPR_FALSE);
}

xpr_bool_t CfgExpDispDlg::OnCommand(WPARAM wParam, LPARAM lParam) 
{
    xpr_uint_t sNotifyMsg = HIWORD(wParam);
    xpr_uint_t sId = LOWORD(wParam);

    if (sNotifyMsg == BN_CLICKED)
    {
        switch (sId)
        {
        case IDC_CFG_EXP_DISP_SHOW_GRID_LINES:
        case IDC_CFG_EXP_DISP_FULL_ROW_SELECTION:
        case IDC_CFG_EXP_DISP_SHOW_PARENT_FOLDER:
        case IDC_CFG_EXP_DISP_SHOW_ATTR_ARHS_STYLE:
        case IDC_CFG_EXP_DISP_SHOW_EACH_DRIVE:
        case IDC_CFG_EXP_DISP_SHOW_DRIVE_ITEM:
        case IDC_CFG_EXP_DISP_SHOW_DRIVE_USAGE:
        case IDC_CFG_EXP_DISP_SHOW_24_HOUR_FORMAT:
        case IDC_CFG_EXP_DISP_SHOW_2_DIGITS_YEAR:
        case IDC_CFG_EXP_DISP_USE_CUSTOM_ICON:
            setModified();
            break;
        }
    }
    else if (sNotifyMsg == EN_UPDATE)
    {
        switch (sId)
        {
        case IDC_CFG_EXP_DISP_CUSTOM_ICON_16_PATH:
        case IDC_CFG_EXP_DISP_CUSTOM_ICON_32_PATH:
            setModified();
            break;
        }
    }
    else if (sNotifyMsg == CBN_SELCHANGE)
    {
        switch (sId)
        {
        case IDC_CFG_EXP_DISP_SHOW_LIST_TYPE:
        case IDC_CFG_EXP_DISP_SIZE_UNIT:
        case IDC_CFG_EXP_DISP_ITEM_TEXT_CASE:
            setModified();
            break;
        }
    }

    return super::OnCommand(wParam, lParam);
}

xpr_bool_t CfgExpDispDlg::loadImage(xpr_sint_t aWidth, xpr_sint_t aHeight, std::tstring &aPath, xpr_bool_t &aSatisfied)
{
    aSatisfied = XPR_FALSE;

    xpr_tchar_t sFilter[0xff] = {0};
    _stprintf(sFilter, XPR_STRING_LITERAL("%s (*.bmp)\0*.bmp\0%s (*.*)\0*.*\0\0"), theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.explorer_window.display.file_dialog.filter.16bit_bitmap_file")), theApp.loadString(XPR_STRING_LITERAL("popup.common.file_dialog.filter.all")));
    CFileDialogST sFileDialog(XPR_TRUE, XPR_STRING_LITERAL("*.bmp"), XPR_NULL, OFN_HIDEREADONLY, sFilter, this);
    if (sFileDialog.DoModal() != IDOK)
        return XPR_FALSE;

    aPath = sFileDialog.GetPathName();

    CFile sFile;
    BITMAPINFOHEADER sBitmapInfoHeader = {0};
    if (sFile.Open(aPath.c_str(), CFile::modeRead | CFile::typeBinary))
    {
        sFile.Seek(sizeof(BITMAPFILEHEADER), CFile::begin);
        sFile.Read(&sBitmapInfoHeader, sizeof(BITMAPINFOHEADER));
        sFile.Close();

        if (sBitmapInfoHeader.biBitCount == 16 && (sBitmapInfoHeader.biWidth % aWidth) == 0 && sBitmapInfoHeader.biHeight == aHeight)
        {
            aSatisfied = XPR_TRUE;
        }
    }

    return XPR_TRUE;
}

void CfgExpDispDlg::OnCustomIcon16x16Browse(void) 
{
    xpr_bool_t sSatisfied = XPR_FALSE;
    std::tstring sPath;

    if (loadImage(16, 16, sPath, sSatisfied) == XPR_FALSE)
        return;

    if (sSatisfied == XPR_FALSE)
    {
        xpr_tchar_t sMsg[XPR_MAX_PATH + 0xff + 1] = {0};
        _stprintf(sMsg, theApp.loadFormatString(XPR_STRING_LITERAL("popup.cfg.body.explorer_window.display.msg.not_bitmap"), XPR_STRING_LITERAL("%s")), sPath.c_str());
        MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);
        return;
    }

    SetDlgItemText(IDC_CFG_EXP_DISP_CUSTOM_ICON_16_PATH, sPath.c_str());
    setModified();
}

void CfgExpDispDlg::OnCustomIcon32x32Browse(void) 
{
    xpr_bool_t sSatisfied = XPR_FALSE;
    std::tstring sPath;

    if (loadImage(32, 32, sPath, sSatisfied) == XPR_FALSE)
        return;

    if (sSatisfied == XPR_FALSE)
    {
        xpr_tchar_t sMsg[XPR_MAX_PATH + 0xff + 1] = {0};
        _stprintf(sMsg, theApp.loadFormatString(XPR_STRING_LITERAL("popup.cfg.body.explorer_window.display.msg.not_bitmap"), XPR_STRING_LITERAL("%s")), sPath.c_str());
        MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);
        return;
    }

    SetDlgItemText(IDC_CFG_EXP_DISP_CUSTOM_ICON_32_PATH, sPath.c_str());
    setModified();
}
