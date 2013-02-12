//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "CfgAppearanceThumbnailDlg.h"

#include "../fxb/fxb_thumbnail.h"

#include "../Option.h"
#include "../resource.h"
#include "../ExplorerCtrl.h"

#include "gfl/libgfl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CfgAppearanceThumbnailDlg::CfgAppearanceThumbnailDlg(void)
    : super(IDD_CFG_APPEARANCE_THUMBNAIL, XPR_NULL)
{
}

void CfgAppearanceThumbnailDlg::DoDataExchange(CDataExchange* pDX)
{
    super::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_CFG_THUMBNAIL_PRIORITY,        mPriorityComboBox);
    DDX_Control(pDX, IDC_CFG_THUMBNAIL_SUPPORTED_IMAGE, mImageFormatComboBox);
}

BEGIN_MESSAGE_MAP(CfgAppearanceThumbnailDlg, super)
    ON_BN_CLICKED(IDC_CFG_THUMBNAIL_CACHE_CLEAN, OnCacheClean)
    ON_BN_CLICKED(IDC_CFG_THUMBNAIL_CACHE_INIT,  OnCacheInit)
END_MESSAGE_MAP()

xpr_bool_t CfgAppearanceThumbnailDlg::OnInitDialog(void) 
{
    super::OnInitDialog();

    // disable apply button event
    addIgnoreApply(IDC_CFG_THUMBNAIL_SUPPORTED_IMAGE);
    addIgnoreApply(IDC_CFG_THUMBNAIL_CACHE_CLEAN);
    addIgnoreApply(IDC_CFG_THUMBNAIL_CACHE_INIT);

    CComboBox *sComboBox;
    sComboBox = (CComboBox *)GetDlgItem(IDC_CFG_THUMBNAIL_PRIORITY);
    sComboBox->AddString(theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearnace.thumbnail.priority.low")));
    sComboBox->AddString(theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearnace.thumbnail.priority.normal")));
    sComboBox->AddString(theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearnace.thumbnail.priority.high")));

    CSpinButtonCtrl *sSpinCtrl;
    sSpinCtrl = (CSpinButtonCtrl *)GetDlgItem(IDC_CFG_THUMBNAIL_WIDTH_SPIN);
    sSpinCtrl->SetRange(MIN_THUMB_SIZE, MAX_THUMB_SIZE);
    sSpinCtrl = (CSpinButtonCtrl *)GetDlgItem(IDC_CFG_THUMBNAIL_HEIGHT_SPIN);
    sSpinCtrl->SetRange(MIN_THUMB_SIZE, MAX_THUMB_SIZE);

    xpr_sint_t i, sCount;
    xpr_tchar_t sText[0xff] = {0};
    xpr_tchar_t sDesc[0xff] = {0};
    xpr_tchar_t sExt[0xff] = {0};
    GFL_FORMAT_INFORMATION sGflFormatInformation;
    xpr_size_t sInputBytes;
    xpr_size_t sOutputBytes;

    sCount = gflGetNumberOfFormat();
    for (i = 0; i < sCount; ++i)
    {
        memset(&sGflFormatInformation, 0, sizeof(GFL_FORMAT_INFORMATION));
        if (gflGetFormatInformationByIndex(i, &sGflFormatInformation) != GFL_NO_ERROR)
            continue;

        sInputBytes = strlen(sGflFormatInformation.Description) * sizeof(xpr_char_t);
        sOutputBytes = 0xfe * sizeof(xpr_tchar_t);
        XPR_MBS_TO_TCS(sGflFormatInformation.Description, &sInputBytes, sDesc, &sOutputBytes);
        sDesc[sOutputBytes / sizeof(xpr_tchar_t)] = 0;

        sInputBytes = strlen(sGflFormatInformation.Extension[0]) * sizeof(xpr_char_t);
        sOutputBytes = 0xfe * sizeof(xpr_tchar_t);
        XPR_MBS_TO_TCS(sGflFormatInformation.Extension[0], &sInputBytes, sExt, &sOutputBytes);
        sExt[sOutputBytes / sizeof(xpr_tchar_t)] = 0;

        _stprintf(sText, XPR_STRING_LITERAL("%s (*.%s)"), sDesc, sExt);
        mImageFormatComboBox.AddString(sText);
    }

    _stprintf(sText, theApp.loadFormatString(XPR_STRING_LITERAL("popup.cfg.body.appearnace.thumbnail.label.limit_size"), XPR_STRING_LITERAL("%d,%d")), MIN_THUMB_SIZE, MAX_THUMB_SIZE);
    SetDlgItemText(IDC_CFG_THUMBNAIL_LIMIT_SIZE, sText);

    _stprintf(sText, theApp.loadFormatString(XPR_STRING_LITERAL("popup.cfg.body.appearnace.thumbnail.label.supported_image_count"), XPR_STRING_LITERAL("%d")), sCount);
    SetDlgItemText(IDC_CFG_THUMBNAIL_LABEL_SUPPORTED_IMAGE_COUNT, sText);

    SetDlgItemText(IDC_CFG_THUMBNAIL_LABEL_WIDTH,           theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearnace.thumbnail.label.width")));
    SetDlgItemText(IDC_CFG_THUMBNAIL_LABEL_HEIGHT,          theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearnace.thumbnail.label.height")));
    SetDlgItemText(IDC_CFG_THUMBNAIL_LABEL_PRIORITY,        theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearnace.thumbnail.label.priority")));
    SetDlgItemText(IDC_CFG_THUMBNAIL_LABEL_CAUTION,         theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearnace.thumbnail.label.caution")));
    SetDlgItemText(IDC_CFG_THUMBNAIL_LABEL_SUPPORTED_IMAGE, theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearnace.thumbnail.label.supported_image")));
    SetDlgItemText(IDC_CFG_THUMBNAIL_LOAD_BY_EXT,           theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearnace.thumbnail.check.load_by_extension")));
    SetDlgItemText(IDC_CFG_THUMBNAIL_CACHE,                 theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearnace.thumbnail.check.cache")));
    SetDlgItemText(IDC_CFG_THUMBNAIL_CACHE_CLEAN,           theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearnace.thumbnail.button.cache_clean")));
    SetDlgItemText(IDC_CFG_THUMBNAIL_CACHE_INIT,            theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearnace.thumbnail.button.cache_initalization")));

    return XPR_TRUE;
}

void CfgAppearanceThumbnailDlg::onInit(Option::Config &aConfig)
{
    SetDlgItemInt(IDC_CFG_THUMBNAIL_WIDTH,  aConfig.mThumbnailWidth);
    SetDlgItemInt(IDC_CFG_THUMBNAIL_HEIGHT, aConfig.mThumbnailHeight);
    ((CButton *)GetDlgItem(IDC_CFG_THUMBNAIL_CACHE))->SetCheck(aConfig.mThumbnailSaveCache);
    ((CButton *)GetDlgItem(IDC_CFG_THUMBNAIL_LOAD_BY_EXT))->SetCheck(aConfig.mThumbnailLoadByExt);
    mPriorityComboBox.SetCurSel(aConfig.mThumbnailPriority);
}

void CfgAppearanceThumbnailDlg::onApply(Option::Config &aConfig)
{
    aConfig.mThumbnailWidth     = GetDlgItemInt(IDC_CFG_THUMBNAIL_WIDTH);
    aConfig.mThumbnailHeight    = GetDlgItemInt(IDC_CFG_THUMBNAIL_HEIGHT);
    aConfig.mThumbnailSaveCache = ((CButton *)GetDlgItem(IDC_CFG_THUMBNAIL_CACHE))->GetCheck();
    aConfig.mThumbnailPriority  = mPriorityComboBox.GetCurSel();
    aConfig.mThumbnailLoadByExt = ((CButton *)GetDlgItem(IDC_CFG_THUMBNAIL_LOAD_BY_EXT))->GetCheck();

    if (aConfig.mThumbnailWidth  < MIN_THUMB_SIZE) aConfig.mThumbnailWidth  = MIN_THUMB_SIZE;
    if (aConfig.mThumbnailWidth  > MAX_THUMB_SIZE) aConfig.mThumbnailWidth  = MAX_THUMB_SIZE;
    if (aConfig.mThumbnailHeight < MIN_THUMB_SIZE) aConfig.mThumbnailHeight = MIN_THUMB_SIZE;
    if (aConfig.mThumbnailHeight > MAX_THUMB_SIZE) aConfig.mThumbnailHeight = MAX_THUMB_SIZE;
}

void CfgAppearanceThumbnailDlg::OnCacheClean(void) 
{
    CWaitCursor sWaitCursor;

    fxb::Thumbnail::instance().verifyThumb();

    const xpr_tchar_t *sMsg = theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearnace.thumbnail.msg.completed_clean"));
    MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONINFORMATION);
}

void CfgAppearanceThumbnailDlg::OnCacheInit(void) 
{
    const xpr_tchar_t *sMsg;

    sMsg = theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearnace.thumbnail.msg.warning_initialize"));
    xpr_sint_t sMsgId = MessageBox(sMsg, XPR_NULL, MB_YESNO | MB_ICONQUESTION);
    if (sMsgId != IDYES)
        return;

    CWaitCursor aWaitCursor;

    fxb::Thumbnail::instance().initThumb();

    sMsg = theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.appearnace.thumbnail.msg.completed_initialize"));
    MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONINFORMATION);
}
