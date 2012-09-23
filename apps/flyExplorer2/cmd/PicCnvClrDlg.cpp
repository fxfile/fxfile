//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "PicCnvClrDlg.h"

#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

xpr_sint_t PicCnvClrDlg::mIndex = 2;
xpr_sint_t PicCnvClrDlg::mCurSel = 6;

PicCnvClrDlg::PicCnvClrDlg(const xpr_tchar_t *aPath)
    : super(IDD_PIC_CONV_COLOR, XPR_NULL)
{
    mAllApply = XPR_FALSE;
    _tcscpy(mPath, aPath);
}

void PicCnvClrDlg::DoDataExchange(CDataExchange* pDX)
{
    super::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(PicCnvClrDlg, super)
    ON_BN_CLICKED(IDC_PIC_CONV_COLOR_APPLY_ALL,       OnApplyAll)
    ON_BN_CLICKED(IDC_PIC_CONV_COLOR_RADIO_BINARY,    OnRadioBinary)
    ON_BN_CLICKED(IDC_PIC_CONV_COLOR_RADIO_GRAYSCALE, OnRadioGrayscale)
    ON_BN_CLICKED(IDC_PIC_CONV_COLOR_RADIO_COLOR,     OnRadioColor)
    ON_BN_CLICKED(IDC_PIC_CONV_COLOR_RADIO_TRUECOLOR, OnRadioTruecolor)
END_MESSAGE_MAP()

void PicCnvClrDlg::initStaticVar(void)
{
    mIndex = 2;
    mCurSel = 6;
}

xpr_bool_t PicCnvClrDlg::OnInitDialog(void) 
{
    super::OnInitDialog();

    CComboBox *sComboBox;
    sComboBox = (CComboBox *)GetDlgItem(IDC_PIC_CONV_COLOR_BINARY);
    sComboBox->AddString(XPR_STRING_LITERAL("Pattern dithering"));
    sComboBox->AddString(XPR_STRING_LITERAL("HalfTone 45 dithering"));
    sComboBox->AddString(XPR_STRING_LITERAL("HalfTone 90 dithering"));
    sComboBox->AddString(XPR_STRING_LITERAL("Floyd-Steinberg dithering"));

    sComboBox = (CComboBox *)GetDlgItem(IDC_PIC_CONV_COLOR_GRAYSCALE);
    sComboBox->AddString(XPR_STRING_LITERAL("4"));
    sComboBox->AddString(XPR_STRING_LITERAL("8"));
    sComboBox->AddString(XPR_STRING_LITERAL("16"));
    sComboBox->AddString(XPR_STRING_LITERAL("32"));
    sComboBox->AddString(XPR_STRING_LITERAL("64"));
    sComboBox->AddString(XPR_STRING_LITERAL("128"));
    sComboBox->AddString(XPR_STRING_LITERAL("256"));

    sComboBox = (CComboBox *)GetDlgItem(IDC_PIC_CONV_COLOR_COLOR);
    sComboBox->AddString(XPR_STRING_LITERAL("8"));
    sComboBox->AddString(XPR_STRING_LITERAL("16"));
    sComboBox->AddString(XPR_STRING_LITERAL("32"));
    sComboBox->AddString(XPR_STRING_LITERAL("64"));
    sComboBox->AddString(XPR_STRING_LITERAL("128"));
    sComboBox->AddString(XPR_STRING_LITERAL("216"));
    sComboBox->AddString(XPR_STRING_LITERAL("256"));

    xpr_sint_t i;
    xpr_uint_t sRadioIds[4];
    xpr_uint_t sIds[3];

    sRadioIds[0] = IDC_PIC_CONV_COLOR_RADIO_BINARY;
    sRadioIds[1] = IDC_PIC_CONV_COLOR_RADIO_GRAYSCALE;
    sRadioIds[2] = IDC_PIC_CONV_COLOR_RADIO_COLOR;
    sRadioIds[3] = IDC_PIC_CONV_COLOR_RADIO_TRUECOLOR;

    sIds[0] = IDC_PIC_CONV_COLOR_BINARY;
    sIds[1] = IDC_PIC_CONV_COLOR_GRAYSCALE;
    sIds[2] = IDC_PIC_CONV_COLOR_COLOR;

    for (i = 0; i < 3; ++i)
        ((CComboBox *)GetDlgItem(sIds[i]))->SetCurSel(i ? 6 : 0);

    SetDlgItemText(IDC_PIC_CONV_COLOR_PATH, mPath);
    ((CButton *)GetDlgItem(sRadioIds[mIndex]))->SetCheck(2);
    if (mIndex < 3)
        ((CComboBox *)GetDlgItem(sIds[mIndex]))->SetCurSel(mCurSel);
    enableWindow(mIndex);

    SetWindowText(theApp.loadString(XPR_STRING_LITERAL("popup.pic_conv_color.title")));
    SetDlgItemText(IDC_PIC_CONV_COLOR_RADIO_BINARY,    theApp.loadString(XPR_STRING_LITERAL("popup.pic_conv_color.radio.binary")));
    SetDlgItemText(IDC_PIC_CONV_COLOR_RADIO_GRAYSCALE, theApp.loadString(XPR_STRING_LITERAL("popup.pic_conv_color.radio.grayscale")));
    SetDlgItemText(IDC_PIC_CONV_COLOR_RADIO_COLOR,     theApp.loadString(XPR_STRING_LITERAL("popup.pic_conv_color.radio.color")));
    SetDlgItemText(IDC_PIC_CONV_COLOR_RADIO_TRUECOLOR, theApp.loadString(XPR_STRING_LITERAL("popup.pic_conv_color.radio.true_color")));
    SetDlgItemText(IDC_PIC_CONV_COLOR_APPLY_ALL,       theApp.loadString(XPR_STRING_LITERAL("popup.pic_conv_color.button.apply_all")));
    SetDlgItemText(IDOK,                               theApp.loadString(XPR_STRING_LITERAL("popup.pic_conv_color.button.apply")));
    SetDlgItemText(IDCANCEL,                           theApp.loadString(XPR_STRING_LITERAL("popup.pic_conv_color.button.cancel")));

    return XPR_TRUE;
}

void PicCnvClrDlg::getColorMode(GFL_MODE &aGflMode, GFL_MODE_PARAMS &aGflModeParams)
{
    aGflMode = mGflMode;
    aGflModeParams = mGflModeParams;
}

void PicCnvClrDlg::enableWindow(xpr_sint_t aIndex)
{
    xpr_sint_t i;
    xpr_bool_t sEnable;
    xpr_uint_t sIds[3] = {IDC_PIC_CONV_COLOR_BINARY, IDC_PIC_CONV_COLOR_GRAYSCALE, IDC_PIC_CONV_COLOR_COLOR};
    for (i = 0; i < 3; ++i)
    {
        sEnable = (i == aIndex) ? XPR_TRUE : XPR_FALSE;
        GetDlgItem(sIds[i])->EnableWindow(sEnable);
    }
}

void PicCnvClrDlg::OnRadioBinary(void) 
{
    enableWindow(0);
}

void PicCnvClrDlg::OnRadioGrayscale(void) 
{
    enableWindow(1);
}

void PicCnvClrDlg::OnRadioColor(void) 
{
    enableWindow(2);
}

void PicCnvClrDlg::OnRadioTruecolor(void) 
{
    enableWindow(3);
}

void PicCnvClrDlg::preOK(void)
{
    xpr_ushort_t sMode[10][10];
    xpr_ushort_t sModeParams[10];
    memset(sMode, 0, sizeof(sMode));
    memset(sModeParams, 0, sizeof(sModeParams));

    sMode[0][0] = GFL_MODE_TO_BINARY;
    sMode[1][0] = GFL_MODE_TO_4GREY;
    sMode[1][1] = GFL_MODE_TO_8GREY;
    sMode[1][2] = GFL_MODE_TO_16GREY;
    sMode[1][3] = GFL_MODE_TO_32GREY;
    sMode[1][4] = GFL_MODE_TO_64GREY;
    sMode[1][5] = GFL_MODE_TO_128GREY;
    sMode[1][6] = GFL_MODE_TO_216GREY;
    sMode[1][7] = GFL_MODE_TO_256GREY;
    sMode[2][0] = GFL_MODE_TO_8COLORS;
    sMode[2][1] = GFL_MODE_TO_16COLORS;
    sMode[2][2] = GFL_MODE_TO_32COLORS;
    sMode[2][3] = GFL_MODE_TO_64COLORS;
    sMode[2][4] = GFL_MODE_TO_128GREY;
    sMode[2][5] = GFL_MODE_TO_216COLORS;
    sMode[2][6] = GFL_MODE_TO_256COLORS;
    sMode[3][0] = GFL_MODE_TO_RGB;
    sMode[3][1] = GFL_MODE_TO_RGBA;
    sMode[3][2] = GFL_MODE_TO_BGR;
    sMode[3][3] = GFL_MODE_TO_ABGR;
    sMode[3][4] = GFL_MODE_TO_BGRA;

    sModeParams[0] = GFL_MODE_NO_DITHER;
    sModeParams[1] = GFL_MODE_ADAPTIVE;
    sModeParams[2] = GFL_MODE_PATTERN_DITHER;
    sModeParams[3] = GFL_MODE_HALTONE45_DITHER;
    sModeParams[4] = GFL_MODE_HALTONE90_DITHER;
    sModeParams[5] = GFL_MODE_FLOYD_STEINBERG;

    CComboBox *sComboBox = XPR_NULL;
    switch (GetCheckedRadioButton(IDC_PIC_CONV_COLOR_RADIO_BINARY, IDC_PIC_CONV_COLOR_RADIO_TRUECOLOR))
    {
    case IDC_PIC_CONV_COLOR_RADIO_BINARY:    sComboBox = (CComboBox *)GetDlgItem(IDC_PIC_CONV_COLOR_BINARY);    mIndex = 0; break;
    case IDC_PIC_CONV_COLOR_RADIO_GRAYSCALE: sComboBox = (CComboBox *)GetDlgItem(IDC_PIC_CONV_COLOR_GRAYSCALE); mIndex = 1; break;
    case IDC_PIC_CONV_COLOR_RADIO_COLOR:     sComboBox = (CComboBox *)GetDlgItem(IDC_PIC_CONV_COLOR_COLOR);     mIndex = 2; break;
    case IDC_PIC_CONV_COLOR_RADIO_TRUECOLOR:                                                                    mIndex = 3; break;
    }
    mCurSel = (sComboBox != XPR_NULL) ? sComboBox->GetCurSel() : 2;
    mGflMode = sMode[mIndex][mCurSel];
    if (mIndex == 0)
        mGflModeParams = sModeParams[mCurSel+2];
    else
        mGflModeParams = sModeParams[0];
}

void PicCnvClrDlg::OnOK(void) 
{
    preOK();

    super::OnOK();
}

void PicCnvClrDlg::OnApplyAll(void) 
{
    mAllApply = XPR_TRUE;
    preOK();

    super::OnOK();
}
