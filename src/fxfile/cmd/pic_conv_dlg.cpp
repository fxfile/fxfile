//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "pic_conv_dlg.h"
#include "pic_conv.h"

#include "resource.h"
#include "dlg_state.h"
#include "dlg_state_manager.h"
#include "pic_conv_opt_dlg.h"
#include "pic_conv_color_dlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace fxfile
{
namespace cmd
{
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

PicConvDlg::PicConvDlg(void)
    : super(IDD_PIC_CONV, XPR_NULL)
    , mPicConv(XPR_NULL)
    , mGflSaveParams(XPR_NULL)
    , mLossless(XPR_FALSE)
    , mPaletteTransparent(-1)
    , mDlgState(XPR_NULL)
{
    mIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

PicConvDlg::~PicConvDlg(void)
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

void PicConvDlg::DoDataExchange(CDataExchange* pDX)
{
    super::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_PIC_CONV_PROGRESS, mProgressCtrl);
    DDX_Control(pDX, IDC_PIC_CONV_LIST,     mListCtrl);
    DDX_Control(pDX, IDC_PIC_CONV_FORMAT,   mFormatComboBox);
}

BEGIN_MESSAGE_MAP(PicConvDlg, super)
    ON_WM_TIMER()
    ON_WM_DESTROY()
    ON_CBN_SELCHANGE(IDC_PIC_CONV_FORMAT, OnSelchangeFormat)
    ON_NOTIFY(LVN_GETDISPINFO, IDC_PIC_CONV_LIST, OnGetdispinfoPcList)
    ON_BN_CLICKED(IDC_PIC_CONV_OPTION, OnOption)
    ON_MESSAGE(WM_PIC_COLOR, OnPicColor)
    ON_MESSAGE(WM_FINALIZE, OnFinalize)
END_MESSAGE_MAP()

xpr_bool_t PicConvDlg::OnInitDialog(void) 
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
    xpr_size_t sInputBytes;
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

            sInputBytes = strlen(sDescA) * sizeof(xpr_char_t);
            sOutputBytes = 0xfe * sizeof(xpr_tchar_t);
            XPR_MBS_TO_TCS(sDescA, &sInputBytes, sDesc, &sOutputBytes);
            sDesc[sOutputBytes / sizeof(xpr_tchar_t)] = 0;

            sInputBytes = strlen(sGflFormatInfo.Name) * sizeof(xpr_char_t);
            sOutputBytes = 0xfe * sizeof(xpr_tchar_t);
            XPR_MBS_TO_TCS(sGflFormatInfo.Name, &sInputBytes, sName, &sOutputBytes);
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
    mListCtrl.InsertColumn(0, gApp.loadString(XPR_STRING_LITERAL("popup.pic_conv.list.column.no")),   LVCFMT_RIGHT,  35);
    mListCtrl.InsertColumn(1, gApp.loadString(XPR_STRING_LITERAL("popup.pic_conv.list.column.name")), LVCFMT_LEFT,  500);

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
    _stprintf(sText, gApp.loadFormatString(XPR_STRING_LITERAL("popup.pic_conv.status.count"), XPR_STRING_LITERAL("%d")), sCount);
    setStatus(sText);

    mGflSaveParams = new GFL_SAVE_PARAMS;
    gflGetDefaultSaveParams(mGflSaveParams);
    mGflSaveParams->Offset = 0;

    SetWindowText(gApp.loadString(XPR_STRING_LITERAL("popup.pic_conv.title")));
    SetDlgItemText(IDC_PIC_CONV_LABEL_FORMAT, gApp.loadString(XPR_STRING_LITERAL("popup.pic_conv.label.format")));
    SetDlgItemText(IDC_PIC_CONV_KEEP_ORG,     gApp.loadString(XPR_STRING_LITERAL("popup.pic_conv.check.keep_original")));
    SetDlgItemText(IDC_PIC_CONV_OPTION,       gApp.loadString(XPR_STRING_LITERAL("popup.pic_conv.button.option")));
    SetDlgItemText(IDOK,                      gApp.loadString(XPR_STRING_LITERAL("popup.pic_conv.button.convert")));
    SetDlgItemText(IDCANCEL,                  gApp.loadString(XPR_STRING_LITERAL("popup.pic_conv.button.close")));

    mDlgState = DlgStateManager::instance().getDlgState(XPR_STRING_LITERAL("PicConv"));
    if (XPR_IS_NOT_NULL(mDlgState))
    {
        mDlgState->setDialog(this, XPR_TRUE);
        mDlgState->setListCtrl(XPR_STRING_LITERAL("List"), mListCtrl.GetDlgCtrlID());
        mDlgState->load();
    }

    return XPR_TRUE;
}

void PicConvDlg::OnDestroy(void) 
{
    super::OnDestroy();

    if (mPicConv != XPR_NULL)
        mPicConv->Stop();

    XPR_SAFE_DELETE(mPicConv);

    XPR_SAFE_DELETE(mGflSaveParams);

    DESTROY_ICON(mIcon);

    if (XPR_IS_NOT_NULL(mDlgState))
    {
        mDlgState->reset();
        mDlgState->save();
    }
}

xpr_bool_t PicConvDlg::addPath(const xpr::tstring &aPath)
{
    xpr_uint64_t sFileSize = GetFileSize(aPath);
    if (sFileSize >= (xpr_uint64_t)kuint32max)
        return XPR_FALSE;

    xpr_char_t sPathA[XPR_MAX_PATH + 1];
    xpr_size_t sInputBytes;
    xpr_size_t sOutputBytes;

    sInputBytes = aPath.length() * sizeof(xpr_tchar_t);
    sOutputBytes = XPR_MAX_PATH * sizeof(xpr_char_t);
    XPR_TCS_TO_MBS(aPath.c_str(), &sInputBytes, sPathA, &sOutputBytes);
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

void PicConvDlg::OnSelchangeFormat(void) 
{
    // get default save parameters
    gflGetDefaultSaveParams(mGflSaveParams);
    mGflSaveParams->Offset = 0;

    xpr_sint_t sFormatIndex = (xpr_sint_t)mFormatComboBox.GetItemData(mFormatComboBox.GetCurSel());
    mGflSaveParams->FormatIndex = sFormatIndex;
}

void PicConvDlg::OnOption(void) 
{
    xpr_sint_t sFormat = mFormatComboBox.GetCurSel();

    PicConvOptDlg sDlg(sFormat, mGflSaveParams, &mLossless, &mPaletteTransparent);
    if (sDlg.isFormatOption())
        sDlg.DoModal();
}

void PicConvDlg::enableWindow(xpr_bool_t aEnable)
{
    SetDlgItemText(IDOK,
        (aEnable == XPR_TRUE) ? gApp.loadString(XPR_STRING_LITERAL("popup.pic_conv.button.convert")) : gApp.loadString(XPR_STRING_LITERAL("popup.pic_conv.button.stop")));

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

void PicConvDlg::setStatus(const xpr_tchar_t *aStatusText)
{
    if (aStatusText != XPR_NULL)
        SetDlgItemText(IDC_PIC_CONV_STATUS, aStatusText);
}

void PicConvDlg::OnOK(void)
{
    if (mPicConv != XPR_NULL)
    {
        if (mPicConv->getStatus() == PicConv::StatusConverting)
        {
            mPicConv->Stop();
            return;
        }
    }

    if (mListCtrl.GetItemCount() <= 0)
    {
        const xpr_tchar_t *sMsg = gApp.loadString(XPR_STRING_LITERAL("popup.pic_conv.msg.none"));
        MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);
        return;
    }

    if (mFormatComboBox.GetCurSel() < 0)
    {
        const xpr_tchar_t *sMsg = gApp.loadString(XPR_STRING_LITERAL("popup.pic_conv.msg.select_format"));
        MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);
        return;
    }

    xpr_sint_t sFormatIndex = (xpr_sint_t)mFormatComboBox.GetItemData(mFormatComboBox.GetCurSel());
    mGflSaveParams->FormatIndex = sFormatIndex;

    xpr_bool_t sKeepOrg = ((CButton *)GetDlgItem(IDC_PIC_CONV_KEEP_ORG))->GetCheck();

    XPR_SAFE_DELETE(mPicConv);

    PicConvColorDlg::initStaticVar();

    mPicConv = new PicConv;
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

LRESULT PicConvDlg::OnFinalize(WPARAM wParam, LPARAM lParam)
{
    mPicConv->Stop();

    KillTimer(TM_ID_STATUS);
    enableWindow(XPR_TRUE);

    PicConv::Status sStatus;
    xpr_size_t sProcessedCount = 0;
    xpr_size_t sSucceededCount = 0;
    sStatus = mPicConv->getStatus(&sProcessedCount, &sSucceededCount);

    mProgressCtrl.SetPos((xpr_sint_t)sProcessedCount);

    xpr_tchar_t sStatusText[0xff] = {0};

    switch (sStatus)
    {
    case PicConv::StatusConvertCompleted:
        {
            xpr_size_t sCount = mPicConv->getCount();

            _stprintf(sStatusText, gApp.loadFormatString(XPR_STRING_LITERAL("popup.pic_conv.status.completed"), XPR_STRING_LITERAL("%d,%d,%d")), sCount, sSucceededCount, sCount-sSucceededCount);
            setStatus(sStatusText);

            super::OnOK();
            break;
        }

    case PicConv::StatusStopped:
        {
            _stprintf(sStatusText, gApp.loadFormatString(XPR_STRING_LITERAL("popup.pic_conv.status.stoped"), XPR_STRING_LITERAL("%d")), sProcessedCount);
            setStatus(sStatusText);
            break;
        }
    }

    return 0;
}

LRESULT PicConvDlg::OnPicColor(WPARAM wParam, LPARAM lParam)
{
    xpr_tchar_t *sPath = (xpr_tchar_t *)wParam;
    if (sPath == XPR_NULL)
        return 0;

    xpr_bool_t sAllApply = XPR_FALSE;
    GFL_MODE sMode = -1;
    GFL_MODE_PARAMS sModeParams = -1;

    PicConvColorDlg sDlg(sPath);
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

void PicConvDlg::OnGetdispinfoPcList(NMHDR* pNMHDR, LRESULT* pResult) 
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

void PicConvDlg::OnTimer(UINT_PTR nIDEvent)
{
    if (nIDEvent == TM_ID_STATUS)
    {
        xpr_size_t sCount;
        xpr_size_t sProcessedCount;

        sCount = mPicConv->getCount();
        mPicConv->getStatus(&sProcessedCount);

        xpr_tchar_t sStatusText[0xff] = {0};
        _stprintf(sStatusText, gApp.loadFormatString(XPR_STRING_LITERAL("popup.pic_conv.status.in_progress"), XPR_STRING_LITERAL("%d,%d,%d")), sProcessedCount, sCount, (xpr_sint_t)((xpr_float_t)sProcessedCount / (xpr_float_t)(sCount) * 100 + 0.5));

        SetDlgItemText(IDC_ATTR_STATUS, sStatusText);
        mProgressCtrl.SetPos((xpr_sint_t)sProcessedCount);
    }

    super::OnTimer(nIDEvent);
}
} // namespace cmd
} // namespace fxfile
