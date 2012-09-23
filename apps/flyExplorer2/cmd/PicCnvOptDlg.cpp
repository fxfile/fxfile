//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "PicCnvOptDlg.h"

#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

PicCnvOptDlg::PicCnvOptDlg(xpr_sint_t aFormat,
                           GFL_SAVE_PARAMS *aGflSaveParams,
                           xpr_bool_t *aLossless,
                           xpr_sint_t *aPaletteTransparent)
    : super(IDD_PIC_CONV_OPTION, XPR_NULL)
    , mLossless(aLossless)
    , mPaletteTransparent(aPaletteTransparent)
    , mGflSaveParams(aGflSaveParams)
    , mFormat(aFormat)
{
    memset(mIds, 0, sizeof(xpr_uint_t) * MAX_ID_GROUP_COUNT * MAX_ID_COUNT);
    mIds[1][0] = IDC_PIC_CONV_OPTION_CHECK_INTERLACED;
    mIds[2][0] = IDC_PIC_CONV_OPTION_CHECK_PALLETE_TRANSPARENT; mIds[2][1] = IDC_PIC_CONV_OPTION_PALLETE_TRANSPARENT; mIds[2][2] = IDC_PIC_CONV_OPTION_PALLETE_TRANSPARENT_SPIN;
    mIds[3][0] = IDC_PIC_CONV_OPTION_CHECK_PROGRESSIVE;
    mIds[4][0] = IDC_PIC_CONV_OPTION_CHECK_LOSSLESS;
    mIds[5][0] = IDC_PIC_CONV_OPTION_LABEL_QUALITY; mIds[5][1] = IDC_PIC_CONV_OPTION_QUALITY_SLIDER; mIds[5][2] = IDC_PIC_CONV_OPTION_QUALITY; mIds[5][3] = IDC_PIC_CONV_OPTION_QUALITY_SPIN;
    mIds[6][0] = IDC_PIC_CONV_OPTION_LABEL_COMPRESS; mIds[6][1] = IDC_PIC_CONV_OPTION_COMPRESS;
    mIds[7][0] = IDC_PIC_CONV_OPTION_LABEL_COMPRESS_LEVEL; mIds[7][1] = IDC_PIC_CONV_OPTION_COMPRESS_LEVEL; mIds[7][2] = IDC_PIC_CONV_OPTION_COMPRESS_LEVEL_SPIN;
    mIds[8][0] = IDC_PIC_CONV_OPTION_CHECK_RLE;
    mIds[9][0] = IDC_PIC_CONV_OPTION_LABEL_OFFSET; mIds[9][1] = IDC_PIC_CONV_OPTION_OFFSET;
    mIds[10][0] = IDC_PIC_CONV_OPTION_LABEL_CHANNEL_ORDER; mIds[10][1] = IDC_PIC_CONV_OPTION_CHANNEL_ORDER;
    mIds[11][0] = IDC_PIC_CONV_OPTION_LABEL_CHANNEL_TYPE; mIds[11][1] = IDC_PIC_CONV_OPTION_CHANNEL_TYPE;

    memset(mFormats, 0, sizeof(xpr_uint_t) * MAX_FORMAT_COUNT * MAX_FORMAT_CONTROL);
    mFormats[GIF][0]  = 1;
    mFormats[GIF][1]  = 2;
    mFormats[JPEG][0] = 3;
    mFormats[JPEG][1] = 4;
    mFormats[JPEG][2] = 5;
    mFormats[TIFF][0] = 6;
    mFormats[PNG][0]  = 1;
    mFormats[PNG][1]  = 2;
    mFormats[PNG][2]  = 7;
    mFormats[BMP][0]  = 8;
    mFormats[IFF][0]  = 8;
    mFormats[PCX][0]  = 8;
    mFormats[TGA][0]  = 8;
    mFormats[SOFT][0] = 8;
    mFormats[SGI][0]  = 8;
    mFormats[RAW][0]  = 10;
    mFormats[RAW][1]  = 11;
    mFormats[RAW][2]  = 9;
}

void PicCnvOptDlg::DoDataExchange(CDataExchange* pDX)
{
    super::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(PicCnvOptDlg, super)
    ON_WM_HSCROLL()
    ON_EN_UPDATE(IDC_PIC_CONV_OPTION_QUALITY, OnUpdateQuality)
END_MESSAGE_MAP()

xpr_bool_t PicCnvOptDlg::OnInitDialog(void) 
{
    super::OnInitDialog();

    CComboBox *sComboBox;
    sComboBox = (CComboBox *)GetDlgItem(IDC_PIC_CONV_OPTION_COMPRESS);
    sComboBox->AddString(XPR_STRING_LITERAL("None"));
    sComboBox->AddString(XPR_STRING_LITERAL("Packbits"));
    sComboBox->AddString(XPR_STRING_LITERAL("LZW"));
    sComboBox->AddString(XPR_STRING_LITERAL("CCITT G3"));
    sComboBox->AddString(XPR_STRING_LITERAL("CCITT G3-2D"));
    sComboBox->AddString(XPR_STRING_LITERAL("CCITT G4"));

    sComboBox = (CComboBox *)GetDlgItem(IDC_PIC_CONV_OPTION_CHANNEL_ORDER);
    sComboBox->AddString(XPR_STRING_LITERAL("Interleaved"));
    sComboBox->AddString(XPR_STRING_LITERAL("Sequential"));
    sComboBox->AddString(XPR_STRING_LITERAL("Separate"));

    sComboBox = (CComboBox *)GetDlgItem(IDC_PIC_CONV_OPTION_CHANNEL_TYPE);
    sComboBox->AddString(XPR_STRING_LITERAL("Grayscale"));
    sComboBox->AddString(XPR_STRING_LITERAL("RGB"));
    sComboBox->AddString(XPR_STRING_LITERAL("BGR"));
    sComboBox->AddString(XPR_STRING_LITERAL("RGBA"));
    sComboBox->AddString(XPR_STRING_LITERAL("ABGR"));
    sComboBox->AddString(XPR_STRING_LITERAL("CMY"));
    sComboBox->AddString(XPR_STRING_LITERAL("CMYK"));

    //----------------------------------------------------------------------
    // initialize controls by GFL_SAVE_PARAMS
    //----------------------------------------------------------------------
    CComboBox *sComboBox2;
    CSliderCtrl *sSliderCtrl;
    CSpinButtonCtrl *sSpinCtrl;

    // Interlaced
    ((CButton *)GetDlgItem(IDC_PIC_CONV_OPTION_CHECK_INTERLACED))->SetCheck(mGflSaveParams->Interlaced ? 2 : 0);

    // Pallete Transparent
    ((CButton *)GetDlgItem(IDC_PIC_CONV_OPTION_CHECK_PALLETE_TRANSPARENT))->SetCheck(*mPaletteTransparent != -1 ? 2 : 0);
    SetDlgItemInt(IDC_PIC_CONV_OPTION_PALLETE_TRANSPARENT, 0);
    sSpinCtrl = (CSpinButtonCtrl *)GetDlgItem(IDC_PIC_CONV_OPTION_PALLETE_TRANSPARENT_SPIN);
    sSpinCtrl->SetRange(0, 255);
    sSpinCtrl->SetPos(*mPaletteTransparent);

    // Progressive
    ((CButton *)GetDlgItem(IDC_PIC_CONV_OPTION_CHECK_PROGRESSIVE))->SetCheck(mGflSaveParams->Progressive ? 2 : 0);

    // Lossless Compression
    ((CButton *)GetDlgItem(IDC_PIC_CONV_OPTION_CHECK_LOSSLESS))->SetCheck(*mLossless ? 2 : 0);

    // Quality
    sSliderCtrl = (CSliderCtrl *)GetDlgItem(IDC_PIC_CONV_OPTION_QUALITY_SLIDER);
    sSliderCtrl->SetRange(0, 100);
    sSliderCtrl->SetPos(mGflSaveParams->Quality);
    sSliderCtrl->SetTicFreq(10);
    SetDlgItemInt(IDC_PIC_CONV_OPTION_QUALITY, mGflSaveParams->Quality);
    sSpinCtrl = (CSpinButtonCtrl *)GetDlgItem(IDC_PIC_CONV_OPTION_QUALITY_SPIN);
    sSpinCtrl->SetRange(0, 100);
    sSpinCtrl->SetPos(mGflSaveParams->Quality);

    // Compression
    sComboBox2 = (CComboBox *)GetDlgItem(IDC_PIC_CONV_OPTION_COMPRESS);
    switch (mGflSaveParams->Compression)
    {
    case GFL_RLE:            sComboBox2->SetCurSel(1); break;
    case GFL_LZW:            sComboBox2->SetCurSel(2); break;
    case GFL_CCITT_FAX3:     sComboBox2->SetCurSel(3); break;
    case GFL_CCITT_FAX3_2D:  sComboBox2->SetCurSel(4); break;
    case GFL_CCITT_FAX4:     sComboBox2->SetCurSel(5); break;
    case GFL_NO_COMPRESSION:
    default:                 sComboBox2->SetCurSel(0); break;
    }
    sComboBox2->SetItemData(0, GFL_NO_COMPRESSION);
    sComboBox2->SetItemData(1, GFL_RLE);
    sComboBox2->SetItemData(2, GFL_LZW);
    sComboBox2->SetItemData(3, GFL_CCITT_FAX3);
    sComboBox2->SetItemData(4, GFL_CCITT_FAX3_2D);
    sComboBox2->SetItemData(5, GFL_CCITT_FAX4);

    // Compression Level
    SetDlgItemInt(IDC_PIC_CONV_OPTION_COMPRESS_LEVEL, mGflSaveParams->CompressionLevel);
    sSpinCtrl = (CSpinButtonCtrl *)GetDlgItem(IDC_PIC_CONV_OPTION_COMPRESS_LEVEL_SPIN);
    sSpinCtrl->SetRange(1, 7);
    sSpinCtrl->SetPos(mGflSaveParams->CompressionLevel);

    // RLE
    ((CButton *)GetDlgItem(IDC_PIC_CONV_OPTION_CHECK_RLE))->SetCheck(mGflSaveParams->Compression == GFL_RLE ? 2 : 0);

    // Offset
    SetDlgItemInt(IDC_PIC_CONV_OPTION_OFFSET, mGflSaveParams->Offset);

    // Channel Order
    sComboBox2 = (CComboBox *)GetDlgItem(IDC_PIC_CONV_OPTION_CHANNEL_ORDER);
    switch (mGflSaveParams->ChannelOrder)
    {
    case GFL_CORDER_SEQUENTIAL:  sComboBox2->SetCurSel(1); break;
    case GFL_CORDER_SEPARATE:    sComboBox2->SetCurSel(2); break;
    case GFL_CORDER_INTERLEAVED:
    default:                     sComboBox2->SetCurSel(0); break;
    }
    sComboBox2->SetItemData(0, GFL_CORDER_INTERLEAVED);
    sComboBox2->SetItemData(1, GFL_CORDER_SEQUENTIAL);
    sComboBox2->SetItemData(2, GFL_CORDER_SEPARATE);

    // Channel Type
    sComboBox2 = (CComboBox *)GetDlgItem(IDC_PIC_CONV_OPTION_CHANNEL_TYPE);
    switch (mGflSaveParams->ChannelType)
    {
    case GFL_CTYPE_GREYSCALE: sComboBox2->SetCurSel(0); break;
    case GFL_CTYPE_RGB:       sComboBox2->SetCurSel(1); break;
    case GFL_CTYPE_RGBA:      sComboBox2->SetCurSel(3); break;
    case GFL_CTYPE_ABGR:      sComboBox2->SetCurSel(4); break;
    case GFL_CTYPE_CMY:       sComboBox2->SetCurSel(5); break;
    case GFL_CTYPE_CMYK:      sComboBox2->SetCurSel(6); break;
    case GFL_CTYPE_BGR:
    default:                  sComboBox2->SetCurSel(2); break;
    }
    sComboBox2->SetItemData(0, GFL_CTYPE_GREYSCALE);
    sComboBox2->SetItemData(1, GFL_CTYPE_RGB);
    sComboBox2->SetItemData(2, GFL_CTYPE_BGR);
    sComboBox2->SetItemData(3, GFL_CTYPE_RGBA);
    sComboBox2->SetItemData(4, GFL_CTYPE_ABGR);
    sComboBox2->SetItemData(5, GFL_CTYPE_CMY);
    sComboBox2->SetItemData(6, GFL_CTYPE_CMYK);

    //----------------------------------------------------------------------
    // show/hide controls by format
    //----------------------------------------------------------------------

    CRect sRect, sRect2;
    CWnd *sWnd, *sWnd2;
    xpr_tchar_t sClassName[0xff];

    xpr_uint_t sId;
    xpr_sint_t i, j;
    xpr_sint_t sTop = 15;
    xpr_sint_t sIndex, sHeight;

    for (i = 0; i < MAX_FORMAT_CONTROL; ++i)
    {
        sIndex = mFormats[mFormat][i];
        if (sIndex == 0)
            break;

        sHeight = 0;
        for (j = 0; j < MAX_ID_COUNT; ++j)
        {
            sId = mIds[sIndex][j];
            if (sId == 0)
                break;

            sWnd = GetDlgItem(sId);
            sWnd->ShowWindow(SW_SHOW);
            sWnd->GetWindowRect(&sRect);
            ScreenToClient(&sRect);
            sWnd->SetWindowPos(XPR_NULL, sRect.left, sTop, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
            if (sHeight < sRect.Height())
                sHeight = sRect.Height();
            sHeight += 7;

            if (j == 1)
            {
                sWnd2 = GetDlgItem(mIds[sIndex][0]);
                GetClassName(sWnd2->m_hWnd, sClassName, 0xfe);
                if (_tcsicmp(sClassName, XPR_STRING_LITERAL("STATIC")) == 0 || _tcsicmp(sClassName, XPR_STRING_LITERAL("BUTTON")) == 0)
                {
                    sWnd2->GetWindowRect(&sRect2);
                    ScreenToClient(&sRect2);
                    sWnd2->SetWindowPos(XPR_NULL, sRect2.left, sTop+(sRect.Height()-sRect2.Height())/2, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
                }
            }
        }
        sTop += sHeight;
    }
    sTop += 20;

    sWnd = GetDlgItem(IDOK);
    sWnd->GetWindowRect(&sRect);
    ScreenToClient(&sRect);
    sWnd->SetWindowPos(XPR_NULL, sRect.left, sTop, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

    sWnd = GetDlgItem(IDCANCEL);
    sWnd->GetWindowRect(&sRect);
    ScreenToClient(&sRect);
    sWnd->SetWindowPos(XPR_NULL, sRect.left, sTop, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

    sTop += sRect.Height();
    GetClientRect(&sRect);
    GetWindowRect(&sRect2);
    sHeight = sRect2.Height()-sRect.Height()+sTop+7;

    SetWindowPos(XPR_NULL, 0, 0, sRect2.Width(), sHeight, SWP_NOMOVE | SWP_NOZORDER);
    CenterWindow();

    SetWindowText(theApp.loadString(XPR_STRING_LITERAL("popup.pic_conv_option.title")));
    SetDlgItemText(IDC_PIC_CONV_OPTION_CHECK_INTERLACED,          theApp.loadString(XPR_STRING_LITERAL("popup.pic_conv_option.check.interlaced")));
    SetDlgItemText(IDC_PIC_CONV_OPTION_CHECK_PALLETE_TRANSPARENT, theApp.loadString(XPR_STRING_LITERAL("popup.pic_conv_option.check.pallete_transparent")));
    SetDlgItemText(IDC_PIC_CONV_OPTION_CHECK_PROGRESSIVE,         theApp.loadString(XPR_STRING_LITERAL("popup.pic_conv_option.check.progressive")));
    SetDlgItemText(IDC_PIC_CONV_OPTION_CHECK_LOSSLESS,            theApp.loadString(XPR_STRING_LITERAL("popup.pic_conv_option.check.lossless")));
    SetDlgItemText(IDC_PIC_CONV_OPTION_LABEL_QUALITY,             theApp.loadString(XPR_STRING_LITERAL("popup.pic_conv_option.label.quality")));
    SetDlgItemText(IDC_PIC_CONV_OPTION_LABEL_COMPRESS,            theApp.loadString(XPR_STRING_LITERAL("popup.pic_conv_option.label.compress")));
    SetDlgItemText(IDC_PIC_CONV_OPTION_LABEL_COMPRESS_LEVEL,      theApp.loadString(XPR_STRING_LITERAL("popup.pic_conv_option.label.compress_level")));
    SetDlgItemText(IDC_PIC_CONV_OPTION_CHECK_RLE,                 theApp.loadString(XPR_STRING_LITERAL("popup.pic_conv_option.check.rle")));
    SetDlgItemText(IDC_PIC_CONV_OPTION_LABEL_OFFSET,              theApp.loadString(XPR_STRING_LITERAL("popup.pic_conv_option.label.offset")));
    SetDlgItemText(IDC_PIC_CONV_OPTION_LABEL_CHANNEL_ORDER,       theApp.loadString(XPR_STRING_LITERAL("popup.pic_conv_option.label.channel_order")));
    SetDlgItemText(IDC_PIC_CONV_OPTION_LABEL_CHANNEL_TYPE,        theApp.loadString(XPR_STRING_LITERAL("popup.pic_conv_option.label.channel_type")));
    SetDlgItemText(IDOK,                                          theApp.loadString(XPR_STRING_LITERAL("popup.common.button.ok")));
    SetDlgItemText(IDCANCEL,                                      theApp.loadString(XPR_STRING_LITERAL("popup.common.button.cancel")));

    return XPR_TRUE;
}

void PicCnvOptDlg::OnHScroll(xpr_uint_t nSBCode, xpr_uint_t nPos, CScrollBar* pScrollBar) 
{
    if (pScrollBar->GetDlgCtrlID() == IDC_PIC_CONV_OPTION_QUALITY_SLIDER)
    {
        CSliderCtrl *sSliderCtrl = (CSliderCtrl *)pScrollBar;
        SetDlgItemInt(IDC_PIC_CONV_OPTION_QUALITY, sSliderCtrl->GetPos());
    }

    super::OnHScroll(nSBCode, nPos, pScrollBar);
}

void PicCnvOptDlg::OnUpdateQuality(void) 
{
    CSliderCtrl *sSliderCtrl = (CSliderCtrl *)GetDlgItem(IDC_PIC_CONV_OPTION_QUALITY_SLIDER);
    xpr_sint_t sPos = GetDlgItemInt(IDC_PIC_CONV_OPTION_QUALITY);
    if (sPos < 0 || sPos > 255)
    {
        sPos = 70;
        SetDlgItemInt(IDC_PIC_CONV_OPTION_QUALITY, sPos);
    }
    sSliderCtrl->SetPos(sPos);
}

void PicCnvOptDlg::OnOK(void)
{
    CComboBox *sComboBox;

    switch (mFormat)
    {
    case GIF:
        mGflSaveParams->Interlaced = ((CButton *)GetDlgItem(IDC_PIC_CONV_OPTION_CHECK_INTERLACED))->GetCheck();
        if (((CButton *)GetDlgItem(IDC_PIC_CONV_OPTION_CHECK_PALLETE_TRANSPARENT))->GetCheck())
            *mPaletteTransparent = GetDlgItemInt(IDC_PIC_CONV_OPTION_PALLETE_TRANSPARENT);
        else
            *mPaletteTransparent = -1;
        break;

    case JPEG:
        mGflSaveParams->Progressive = ((CButton *)GetDlgItem(IDC_PIC_CONV_OPTION_CHECK_PROGRESSIVE))->GetCheck();
        mGflSaveParams->Quality = GetDlgItemInt(IDC_PIC_CONV_OPTION_QUALITY);
        *mLossless = ((CButton *)GetDlgItem(IDC_PIC_CONV_OPTION_CHECK_LOSSLESS))->GetCheck();
        break;

    case TIFF:
        sComboBox = (CComboBox *)GetDlgItem(IDC_PIC_CONV_OPTION_COMPRESS);
        mGflSaveParams->Compression = (xpr_ushort_t)sComboBox->GetItemData(sComboBox->GetCurSel());
        break;

    case PNG:
        mGflSaveParams->Interlaced = ((CButton *)GetDlgItem(IDC_PIC_CONV_OPTION_CHECK_INTERLACED))->GetCheck();
        if (((CButton *)GetDlgItem(IDC_PIC_CONV_OPTION_CHECK_PALLETE_TRANSPARENT))->GetCheck())
            *mPaletteTransparent = GetDlgItemInt(IDC_PIC_CONV_OPTION_PALLETE_TRANSPARENT);
        else
            *mPaletteTransparent = -1;
        mGflSaveParams->CompressionLevel = GetDlgItemInt(IDC_PIC_CONV_OPTION_COMPRESS_LEVEL);
        break;

    case BMP:
    case IFF:
    case PCX:
    case TGA:
    case SOFT:
    case SGI:
        mGflSaveParams->Compression = ((CButton *)GetDlgItem(IDC_PIC_CONV_OPTION_CHECK_RLE))->GetCheck() ? GFL_RLE : GFL_NO_COMPRESSION;
        break;

    case RAW:
        mGflSaveParams->Offset = GetDlgItemInt(IDC_PIC_CONV_OPTION_OFFSET);
        sComboBox = (CComboBox *)GetDlgItem(IDC_PIC_CONV_OPTION_CHANNEL_ORDER);
        mGflSaveParams->ChannelOrder = (xpr_ushort_t)sComboBox->GetItemData(sComboBox->GetCurSel());
        sComboBox = (CComboBox *)GetDlgItem(IDC_PIC_CONV_OPTION_CHANNEL_TYPE);
        mGflSaveParams->ChannelType = (xpr_ushort_t)sComboBox->GetItemData(sComboBox->GetCurSel());
        break;
    }

    super::OnOK();
}
