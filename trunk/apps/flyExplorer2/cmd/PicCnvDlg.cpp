//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "PicCnvDlg.h"

#include "fxb/fxb_pic_conv.h"

#include "resource.h"
#include "DlgState.h"
#include "PicCnvOptDlg.h"
#include "PicCnvClrDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// user defined timer
enum
{
    TM_ID_STATUS = 100,
};

// user defined message
enum
{
    WM_FINALIZE  = WM_USER + 100,
    WM_PIC_COLOR = WM_USER + 101,
};

PicCnvDlg::PicCnvDlg(void)
    : super(IDD_PIC_CONV, XPR_NULL)
    , mPicConv(XPR_NULL)
    , mGflSaveParams(XPR_NULL)
    , mLossless(XPR_FALSE)
    , mPaletteTransparent(-1)
{
    mIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

PicCnvDlg::~PicCnvDlg(void)
{
    PicItem *sPicItem;
    PicDeque::iterator sIterator;

    sIterator = mPicDeque.begin();
    for (; sIterator != mPicDeque.end(); ++sIterator)
    {
        sPicItem = *sIterator;
        XPR_SAFE_DELETE(sPicItem);
    }

    mPicDeque.clear();

    DESTROY_ICON(mIcon);
}

void PicCnvDlg::DoDataExchange(CDataExchange* pDX)
{
    super::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_PIC_CONV_PROGRESS, mProgressCtrl);
    DDX_Control(pDX, IDC_PIC_CONV_LIST,     mListCtrl);
    DDX_Control(pDX, IDC_PIC_CONV_FORMAT,   mFormatComboBox);
}

BEGIN_MESSAGE_MAP(PicCnvDlg, super)
    ON_WM_TIMER()
    ON_WM_DESTROY()
    ON_CBN_SELCHANGE(IDC_PIC_CONV_FORMAT, OnSelchangeFormat)
    ON_NOTIFY(LVN_GETDISPINFO, IDC_PIC_CONV_LIST, OnGetdispinfoPcList)
    ON_BN_CLICKED(IDC_PIC_CONV_OPTION, OnOption)
    ON_MESSAGE(WM_PIC_COLOR, OnPicColor)
    ON_MESSAGE(WM_FINALIZE, OnFinalize)
END_MESSAGE_MAP()

xpr_bool_t PicCnvDlg::OnInitDialog(void) 
{
    super::OnInitDialog();

    SetIcon(mIcon, XPR_TRUE);

    // CResizingDialog -------------------------------------------
    //HideSizeIcon();

    //sizeNone:     Don't resize at all  
    //sizeResize:   The control will be stretched in the appropriate direction 
    //sizeRepos:    The control will be moved in the appropriate direction 
    //sizeRelative: The control will be moved proportionally in the appropriate direction 
    AddControl(IDC_PIC_CONV_FORMAT,   sizeResize, sizeNone);
    AddControl(IDC_PIC_CONV_OPTION,   sizeRepos,  sizeNone,  XPR_FALSE);
    AddControl(IDC_PIC_CONV_KEEP_ORG, sizeRepos,  sizeNone);
    AddControl(IDC_PIC_CONV_LIST,     sizeResize, sizeResize);
    AddControl(IDC_PIC_CONV_PROGRESS, sizeResize, sizeRepos);
    AddControl(IDC_PIC_CONV_STATUS,   sizeResize, sizeRepos);
    AddControl(IDOK,                  sizeRepos,  sizeRepos, XPR_FALSE);
    AddControl(IDCANCEL,              sizeRepos,  sizeRepos, XPR_FALSE);
    //------------------------------------------------------------

    // format
    const xpr_char_t *sDescA;
    xpr_tchar_t sText[XPR_MAX_PATH + 1];
    xpr_tchar_t sDesc[0xff];
    xpr_tchar_t sName[0xff];
    xpr_size_t sOutputBytes;
    GFL_FORMAT_INFORMATION sGflFormatInfo;
    COMBOBOXEXITEM sComboBoxExItem = {0};
    sComboBoxExItem.mask = CBEIF_TEXT | CBEIF_LPARAM;

    xpr_sint_t i;
    xpr_sint_t sIndex = 0;
    xpr_sint_t sCount = gflGetNumberOfFormat();
    for (i = 0; i < sCount; ++i)
    {
        memset(&sGflFormatInfo, 0, sizeof(GFL_FORMAT_INFORMATION));
        gflGetFormatInformationByIndex(i, &sGflFormatInfo);// != GFL_NO_ERROR)
        if (gflFormatIsWritableByIndex(i))
        {
            sDescA = gflGetFormatDescriptionByIndex(i);

            sOutputBytes = 0xfe * sizeof(xpr_tchar_t);
            XPR_MBS_TO_TCS(sDescA, strlen(sDescA) * sizeof(xpr_char_t), sDesc, &sOutputBytes);
            sDesc[sOutputBytes / sizeof(xpr_tchar_t)] = 0;

            sOutputBytes = 0xfe * sizeof(xpr_tchar_t);
            XPR_MBS_TO_TCS(sGflFormatInfo.Name, strlen(sGflFormatInfo.Name) * sizeof(xpr_char_t), sName, &sOutputBytes);
            sName[sOutputBytes / sizeof(xpr_tchar_t)] = 0;

            _stprintf(sText, XPR_STRING_LITERAL("[%d] %s (%s)"), sIndex + 1, sDesc, sName);

            sComboBoxExItem.iItem = sIndex++;
            sComboBoxExItem.pszText = sText;
            sComboBoxExItem.lParam = i;
            mFormatComboBox.InsertItem(&sComboBoxExItem);
        }
    }
    mFormatComboBox.SetCurSel(0);

    // keep the original file
    ((CButton *)GetDlgItem(IDC_PIC_CONV_KEEP_ORG))->SetCheck(2);

    // image file list
    mListCtrl.SetExtendedStyle(mListCtrl.GetExtendedStyle() | LVS_EX_FULLROWSELECT);
    mListCtrl.InsertColumn(0, theApp.loadString(XPR_STRING_LITERAL("popup.pic_conv.list.column.no")),   LVCFMT_RIGHT,  35);
    mListCtrl.InsertColumn(1, theApp.loadString(XPR_STRING_LITERAL("popup.pic_conv.list.column.name")), LVCFMT_LEFT,  500);

    i = 0;
    LVITEM sLvItem = {0};
    PicDeque::iterator sIterator;
    PicItem *sPicItem;

    mListCtrl.SetRedraw(XPR_FALSE);

    sIterator = mPicDeque.begin();
    for (; sIterator != mPicDeque.end(); ++sIterator)
    {
        sPicItem = *sIterator;
        if (sPicItem == XPR_NULL)
            continue;

        sLvItem.mask     = LVIF_TEXT | LVIF_PARAM;
        sLvItem.iItem    = i++;
        sLvItem.iSubItem = 0;
        sLvItem.pszText  = LPSTR_TEXTCALLBACK;
        sLvItem.lParam   = (LPARAM)sPicItem;
        mListCtrl.InsertItem(&sLvItem);
    }

    mListCtrl.SetRedraw();

    // status
    sCount = mListCtrl.GetItemCount();
    mProgressCtrl.SetRange32(0, sCount);
    _stprintf(sText, theApp.loadFormatString(XPR_STRING_LITERAL("popup.pic_conv.status.count"), XPR_STRING_LITERAL("%d")), sCount);
    setStatus(sText);

    mGflSaveParams = new GFL_SAVE_PARAMS;
    gflGetDefaultSaveParams(mGflSaveParams);
    mGflSaveParams->Offset = 0;

    SetWindowText(theApp.loadString(XPR_STRING_LITERAL("popup.pic_conv.title")));
    SetDlgItemText(IDC_PIC_CONV_LABEL_FORMAT, theApp.loadString(XPR_STRING_LITERAL("popup.pic_conv.label.format")));
    SetDlgItemText(IDC_PIC_CONV_KEEP_ORG,     theApp.loadString(XPR_STRING_LITERAL("popup.pic_conv.check.keep_original")));
    SetDlgItemText(IDC_PIC_CONV_OPTION,       theApp.loadString(XPR_STRING_LITERAL("popup.pic_conv.button.option")));
    SetDlgItemText(IDOK,                      theApp.loadString(XPR_STRING_LITERAL("popup.pic_conv.button.convert")));
    SetDlgItemText(IDCANCEL,                  theApp.loadString(XPR_STRING_LITERAL("popup.pic_conv.button.close")));

    // Load Dialog State
    mState.setSection(XPR_STRING_LITERAL("PicConv"));
    mState.setDialog(this, XPR_TRUE);
    mState.setListCtrl(&mListCtrl);
    mState.load();

    return XPR_TRUE;
}

void PicCnvDlg::OnDestroy(void) 
{
    super::OnDestroy();

    if (mPicConv != XPR_NULL)
        mPicConv->Stop();

    XPR_SAFE_DELETE(mPicConv);

    XPR_SAFE_DELETE(mGflSaveParams);

    DESTROY_ICON(mIcon);

    // Save Dialog State
    mState.reset();
    mState.save();
}

xpr_bool_t PicCnvDlg::addPath(const std::tstring &aPath)
{
    xpr_uint64_t sFileSize = fxb::GetFileSize(aPath);
    if (sFileSize >= (xpr_uint64_t)kuint32max)
        return XPR_FALSE;

    xpr_char_t sPathA[XPR_MAX_PATH + 1];
    xpr_size_t sOutputBytes;

    sOutputBytes = XPR_MAX_PATH * sizeof(xpr_char_t);
    XPR_TCS_TO_MBS(aPath.c_str(), aPath.length() * sizeof(xpr_tchar_t), sPathA, &sOutputBytes);
    sPathA[sOutputBytes / sizeof(xpr_char_t)] = 0;

    GFL_FILE_INFORMATION gfi;
    GFL_ERROR gflError;

    gflError = gflGetFileInformation(sPathA, -1, &gfi);
    if (gflError != GFL_NO_ERROR)
        return XPR_FALSE;

    PicItem *sPicItem = new PicItem;
    sPicItem->mPath        = aPath;
    sPicItem->mFormatIndex = gfi.FormatIndex;
    mPicDeque.push_back(sPicItem);

    gflFreeFileInformation(&gfi);

    return XPR_TRUE;
}

void PicCnvDlg::OnSelchangeFormat(void) 
{
    // get default save parameters
    gflGetDefaultSaveParams(mGflSaveParams);
    mGflSaveParams->Offset = 0;

    xpr_sint_t sFormatIndex = (xpr_sint_t)mFormatComboBox.GetItemData(mFormatComboBox.GetCurSel());
    mGflSaveParams->FormatIndex = sFormatIndex;
}

void PicCnvDlg::OnOption(void) 
{
    xpr_sint_t nFormat = mFormatComboBox.GetCurSel();

    PicCnvOptDlg dlgPicCnvOpt(nFormat, mGflSaveParams, &mLossless, &mPaletteTransparent);
    if (dlgPicCnvOpt.isFormatOption())
        dlgPicCnvOpt.DoModal();
}

void PicCnvDlg::enableWindow(xpr_bool_t aEnable)
{
    SetDlgItemText(IDOK,
        (aEnable == XPR_TRUE) ? theApp.loadString(XPR_STRING_LITERAL("popup.pic_conv.button.convert")) : theApp.loadString(XPR_STRING_LITERAL("popup.pic_conv.button.stop")));

    GetDlgItem(IDC_PIC_CONV_FORMAT)->EnableWindow(aEnable);
    GetDlgItem(IDC_PIC_CONV_OPTION)->EnableWindow(aEnable);
    GetDlgItem(IDC_PIC_CONV_KEEP_ORG)->EnableWindow(aEnable);
    GetDlgItem(IDCANCEL)->EnableWindow(aEnable);

    // System Button
    CMenu *sMenu = GetSystemMenu(XPR_FALSE);
    if (sMenu != XPR_NULL)
    {
        xpr_uint_t sFlags = MF_BYCOMMAND;
        if (aEnable == XPR_FALSE) sFlags |= MF_GRAYED;
        sMenu->EnableMenuItem(SC_CLOSE, sFlags);
    }
}

void PicCnvDlg::setStatus(const xpr_tchar_t *aStatusText)
{
    if (aStatusText != XPR_NULL)
        SetDlgItemText(IDC_PIC_CONV_STATUS, aStatusText);
}

void PicCnvDlg::OnOK(void)
{
    if (mPicConv != XPR_NULL)
    {
        if (mPicConv->getStatus() == fxb::PicConv::StatusConverting)
        {
            mPicConv->Stop();
            return;
        }
    }

    if (mListCtrl.GetItemCount() <= 0)
    {
        const xpr_tchar_t *sMsg = theApp.loadString(XPR_STRING_LITERAL("popup.pic_conv.msg.none"));
        MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);
        return;
    }

    if (mFormatComboBox.GetCurSel() < 0)
    {
        const xpr_tchar_t *sMsg = theApp.loadString(XPR_STRING_LITERAL("popup.pic_conv.msg.select_format"));
        MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);
        return;
    }

    xpr_sint_t sFormatIndex = (xpr_sint_t)mFormatComboBox.GetItemData(mFormatComboBox.GetCurSel());
    mGflSaveParams->FormatIndex = sFormatIndex;

    xpr_bool_t sKeepOrg = ((CButton *)GetDlgItem(IDC_PIC_CONV_KEEP_ORG))->GetCheck();

    XPR_SAFE_DELETE(mPicConv);

    PicCnvClrDlg::initStaticVar();

    mPicConv = new fxb::PicConv;
    mPicConv->setOwner(m_hWnd, WM_FINALIZE, WM_PIC_COLOR);

    mPicConv->setSavParams(mGflSaveParams);
    mPicConv->setKeepOrg(sKeepOrg);

    xpr_sint_t i;
    xpr_sint_t sCount;
    PicItem *sPicItem;

    sCount = mListCtrl.GetItemCount();
    for (i = 0; i < sCount; ++i)
    {
        sPicItem = (PicItem *)mListCtrl.GetItemData(i);
        if (sPicItem == XPR_NULL)
            continue;

        mPicConv->addPath(sPicItem->mPath.c_str());
    }

    if (mPicConv->Start() == XPR_TRUE)
    {
        enableWindow(XPR_FALSE);
        SetTimer(TM_ID_STATUS, 100, XPR_NULL);
    }
}

LRESULT PicCnvDlg::OnFinalize(WPARAM wParam, LPARAM lParam)
{
    mPicConv->Stop();

    KillTimer(TM_ID_STATUS);
    enableWindow(XPR_TRUE);

    fxb::PicConv::Status sStatus;
    xpr_size_t sProcessedCount = 0;
    xpr_size_t sSucceededCount = 0;
    sStatus = mPicConv->getStatus(&sProcessedCount, &sSucceededCount);

    mProgressCtrl.SetPos((xpr_sint_t)sProcessedCount);

    xpr_tchar_t sStatusText[0xff] = {0};

    switch (sStatus)
    {
    case fxb::PicConv::StatusConvertCompleted:
        {
            xpr_size_t sCount = mPicConv->getCount();

            _stprintf(sStatusText, theApp.loadFormatString(XPR_STRING_LITERAL("popup.pic_conv.status.completed"), XPR_STRING_LITERAL("%d,%d,%d")), sCount, sSucceededCount, sCount-sSucceededCount);
            setStatus(sStatusText);

            super::OnOK();
            break;
        }

    case fxb::PicConv::StatusStopped:
        {
            _stprintf(sStatusText, theApp.loadFormatString(XPR_STRING_LITERAL("popup.pic_conv.status.stoped"), XPR_STRING_LITERAL("%d")), sProcessedCount);
            setStatus(sStatusText);
            break;
        }
    }

    return 0;
}

LRESULT PicCnvDlg::OnPicColor(WPARAM wParam, LPARAM lParam)
{
    xpr_tchar_t *sPath = (xpr_tchar_t *)wParam;
    if (sPath == XPR_NULL)
        return 0;

    xpr_bool_t sAllApply = XPR_FALSE;
    GFL_MODE sMode = -1;
    GFL_MODE_PARAMS sModeParams = -1;

    PicCnvClrDlg sDlg(sPath);
    xpr_sintptr_t sId = sDlg.DoModal();
    if (sId == IDOK)
    {
        sAllApply = sDlg.isAllApply();
        sDlg.getColorMode(sMode, sModeParams);
    }
    else
    {
        mPicConv->StopEvent();
    }

    mPicConv->setColorMode(sAllApply, sMode, sModeParams);

    XPR_SAFE_DELETE_ARRAY(sPath);

    return 0;
}

void PicCnvDlg::OnGetdispinfoPcList(NMHDR* pNMHDR, LRESULT* pResult) 
{
    LV_DISPINFO *sDispInfo = (LV_DISPINFO *)pNMHDR;
    *pResult = 0;

    LVITEM &sLvItem = sDispInfo->item;
    PicItem *sPicItem = (PicItem *)sDispInfo->item.lParam;

    if (sPicItem == XPR_NULL)
        return;

    if (XPR_TEST_BITS(sLvItem.mask, LVIF_TEXT))
    {
        switch (sLvItem.iSubItem)
        {
        case 0:
            _stprintf(sLvItem.pszText, XPR_STRING_LITERAL("%d"), sLvItem.iItem + 1);
            break;

        case 1:
            _tcscpy(sLvItem.pszText, sPicItem->mPath.c_str());
            break;
        }
    }

    sLvItem.mask |= LVIF_DI_SETITEM;
}

void PicCnvDlg::OnTimer(UINT_PTR nIDEvent)
{
    if (nIDEvent == TM_ID_STATUS)
    {
        xpr_size_t sCount;
        xpr_size_t sProcessedCount;

        sCount = mPicConv->getCount();
        mPicConv->getStatus(&sProcessedCount);

        xpr_tchar_t sStatusText[0xff] = {0};
        _stprintf(sStatusText, theApp.loadFormatString(XPR_STRING_LITERAL("popup.pic_conv.status.in_progress"), XPR_STRING_LITERAL("%d,%d,%d")), sProcessedCount, sCount, (xpr_sint_t)((xpr_float_t)sProcessedCount / (xpr_float_t)(sCount) * 100 + 0.5));

        SetDlgItemText(IDC_ATTR_STATUS, sStatusText);
        mProgressCtrl.SetPos((xpr_sint_t)sProcessedCount);
    }

    super::OnTimer(nIDEvent);
}
