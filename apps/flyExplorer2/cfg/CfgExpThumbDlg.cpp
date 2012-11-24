//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "CfgExpThumbDlg.h"

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

CfgExpThumbDlg::CfgExpThumbDlg(void)
    : super(IDD_CFG_EXP_THUMB, XPR_NULL)
{
}

void CfgExpThumbDlg::DoDataExchange(CDataExchange* pDX)
{
    super::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_CFG_EXP_THUMB_PRIORITY,        mPriorityComboBox);
    DDX_Control(pDX, IDC_CFG_EXP_THUMB_SUPPORTED_IMAGE, mImageFormatComboBox);
}

BEGIN_MESSAGE_MAP(CfgExpThumbDlg, super)
    ON_BN_CLICKED(IDC_CFG_EXP_THUMB_CACHE_CLEAN, OnCacheClean)
    ON_BN_CLICKED(IDC_CFG_EXP_THUMB_CACHE_INIT,  OnCacheInit)
END_MESSAGE_MAP()

xpr_bool_t CfgExpThumbDlg::OnInitDialog(void) 
{
    super::OnInitDialog();

    CComboBox *sComboBox;
    sComboBox = (CComboBox *)GetDlgItem(IDC_CFG_EXP_THUMB_PRIORITY);
    sComboBox->AddString(theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.explorer_window.thumbnail.priority.low")));
    sComboBox->AddString(theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.explorer_window.thumbnail.priority.normal")));
    sComboBox->AddString(theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.explorer_window.thumbnail.priority.high")));

    CSpinButtonCtrl *sSpinCtrl;
    sSpinCtrl = (CSpinButtonCtrl *)GetDlgItem(IDC_CFG_EXP_THUMB_WIDTH_SPIN);
    sSpinCtrl->SetRange(MIN_THUMB_SIZE, MAX_THUMB_SIZE);
    sSpinCtrl = (CSpinButtonCtrl *)GetDlgItem(IDC_CFG_EXP_THUMB_HEIGHT_SPIN);
    sSpinCtrl->SetRange(MIN_THUMB_SIZE, MAX_THUMB_SIZE);

    SetDlgItemInt(IDC_CFG_EXP_THUMB_WIDTH,  gOpt->mThumbnailWidth);
    SetDlgItemInt(IDC_CFG_EXP_THUMB_HEIGHT, gOpt->mThumbnailHeight);
    ((CButton *)GetDlgItem(IDC_CFG_EXP_THUMB_CACHE))->SetCheck(gOpt->mThumbnailSaveCache);
    ((CButton *)GetDlgItem(IDC_CFG_EXP_THUMB_LOAD_BY_EXT))->SetCheck(gOpt->mThumbnailLoadByExt);
    mPriorityComboBox.SetCurSel(gOpt->mThumbnailPriority);

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

    _stprintf(sText, theApp.loadFormatString(XPR_STRING_LITERAL("popup.cfg.body.explorer_window.thumbnail.label.limit_size"), XPR_STRING_LITERAL("%d,%d")), MIN_THUMB_SIZE, MAX_THUMB_SIZE);
    SetDlgItemText(IDC_CFG_EXP_THUMB_LIMIT_SIZE, sText);

    _stprintf(sText, theApp.loadFormatString(XPR_STRING_LITERAL("popup.cfg.body.explorer_window.thumbnail.label.supported_image_count"), XPR_STRING_LITERAL("%d")), sCount);
    SetDlgItemText(IDC_CFG_EXP_THUMB_LABEL_SUPPORTED_IMAGE_COUNT, sText);

    SetDlgItemText(IDC_CFG_EXP_THUMB_LABEL_WIDTH,           theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.explorer_window.thumbnail.label.width")));
    SetDlgItemText(IDC_CFG_EXP_THUMB_LABEL_HEIGHT,          theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.explorer_window.thumbnail.label.height")));
    SetDlgItemText(IDC_CFG_EXP_THUMB_LABEL_PRIORITY,        theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.explorer_window.thumbnail.label.priority")));
    SetDlgItemText(IDC_CFG_EXP_THUMB_LABEL_CAUTION,         theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.explorer_window.thumbnail.label.caution")));
    SetDlgItemText(IDC_CFG_EXP_THUMB_LABEL_SUPPORTED_IMAGE, theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.explorer_window.thumbnail.label.supported_image")));
    SetDlgItemText(IDC_CFG_EXP_THUMB_LOAD_BY_EXT,           theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.explorer_window.thumbnail.check.load_by_extension")));
    SetDlgItemText(IDC_CFG_EXP_THUMB_CACHE,                 theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.explorer_window.thumbnail.check.cache")));
    SetDlgItemText(IDC_CFG_EXP_THUMB_CACHE_CLEAN,           theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.explorer_window.thumbnail.button.cache_clean")));
    SetDlgItemText(IDC_CFG_EXP_THUMB_CACHE_INIT,            theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.explorer_window.thumbnail.button.cache_initalization")));

    return XPR_TRUE;
}

void CfgExpThumbDlg::OnApply(void)
{
    OptionMgr &sOptionMgr = OptionMgr::instance();

    gOpt->mThumbnailWidth     = GetDlgItemInt(IDC_CFG_EXP_THUMB_WIDTH);
    gOpt->mThumbnailHeight    = GetDlgItemInt(IDC_CFG_EXP_THUMB_HEIGHT);
    gOpt->mThumbnailSaveCache = ((CButton *)GetDlgItem(IDC_CFG_EXP_THUMB_CACHE))->GetCheck();
    gOpt->mThumbnailPriority  = mPriorityComboBox.GetCurSel();
    gOpt->mThumbnailLoadByExt = ((CButton *)GetDlgItem(IDC_CFG_EXP_THUMB_LOAD_BY_EXT))->GetCheck();

    if (gOpt->mThumbnailWidth  < MIN_THUMB_SIZE) gOpt->mThumbnailWidth  = MIN_THUMB_SIZE;
    if (gOpt->mThumbnailWidth  > MAX_THUMB_SIZE) gOpt->mThumbnailWidth  = MAX_THUMB_SIZE;
    if (gOpt->mThumbnailHeight < MIN_THUMB_SIZE) gOpt->mThumbnailHeight = MIN_THUMB_SIZE;
    if (gOpt->mThumbnailHeight > MAX_THUMB_SIZE) gOpt->mThumbnailHeight = MAX_THUMB_SIZE;

    sOptionMgr.applyExplorerView(5);
}

xpr_bool_t CfgExpThumbDlg::OnCommand(WPARAM wParam, LPARAM lParam) 
{
    xpr_uint_t sNotifyMsg = HIWORD(wParam);
    xpr_uint_t sId = LOWORD(wParam);

    if (sNotifyMsg == BN_CLICKED)
    {
        switch (sId)
        {
        case IDC_CFG_EXP_THUMB_CACHE:
        case IDC_CFG_EXP_THUMB_LOAD_BY_EXT:
            setModified();
            break;
        }
    }
    else if (sNotifyMsg == EN_UPDATE)
    {
        switch (sId)
        {
        case IDC_CFG_EXP_THUMB_HEIGHT:
        case IDC_CFG_EXP_THUMB_WIDTH:
            setModified();
            break;
        }
    }
    else if (sNotifyMsg == CBN_SELCHANGE)
    {
        switch (sId)
        {
        case IDC_CFG_EXP_THUMB_PRIORITY:
            setModified();
            break;
        }
    }

    return super::OnCommand(wParam, lParam);
}

void CfgExpThumbDlg::OnCacheClean(void) 
{
    CWaitCursor sWaitCursor;

    fxb::Thumbnail::instance().verifyThumb();

    const xpr_tchar_t *sMsg = theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.explorer_window.thumbnail.msg.completed_clean"));
    MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONINFORMATION);
}

void CfgExpThumbDlg::OnCacheInit(void) 
{
    const xpr_tchar_t *sMsg;

    sMsg = theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.explorer_window.thumbnail.msg.warning_initialize"));
    xpr_sint_t sMsgId = MessageBox(sMsg, XPR_NULL, MB_YESNO | MB_ICONQUESTION);
    if (sMsgId != IDYES)
        return;

    CWaitCursor aWaitCursor;

    fxb::Thumbnail::instance().initThumb();

    sMsg = theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.explorer_window.thumbnail.msg.completed_initialize"));
    MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONINFORMATION);
}
