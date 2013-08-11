//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "crc_verify_dlg.h"
#include "crc_verify.h"

#include "crc_checksum.h"

#include "gui/FileDialogST.h"

#include "resource.h"
#include "dlg_state.h"
#include "dlg_state_manager.h"

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
    WM_FINALIZE = WM_USER + 100,
};

CrcVerifyDlg::CrcVerifyDlg(const xpr_tchar_t *aDir)
    : super(IDD_CRC_VERIFY, XPR_NULL)
    , mCrcVerify(new CrcVerify)
    , mEnable(XPR_TRUE)
    , mDlgState(XPR_NULL)
{
    mIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

    if (aDir != XPR_NULL)
        mDir = aDir;
}

CrcVerifyDlg::~CrcVerifyDlg(void)
{
    XPR_SAFE_DELETE(mCrcVerify);

    mFileDeque.clear();

    DESTROY_ICON(mIcon);
}

void CrcVerifyDlg::DoDataExchange(CDataExchange* pDX)
{
    super::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_CRCVERIFY_PROGRESS, mProgressCtrl);
    DDX_Control(pDX, IDC_CRCVERIFY_LIST,     mListCtrl);
}

BEGIN_MESSAGE_MAP(CrcVerifyDlg, super)
    ON_WM_TIMER()
    ON_MESSAGE(WM_FINALIZE, OnFinalize)
    ON_BN_CLICKED(IDOK, OnOK)
END_MESSAGE_MAP()

xpr_bool_t CrcVerifyDlg::OnInitDialog(void) 
{
    super::OnInitDialog();

    SetIcon(mIcon, XPR_TRUE);

    // CResizingDialog -------------------------------------------
    //HideSizeIcon();

    //sizeNone     Don't resize at all  
    //sizeResize   The control will be stretched in the appropriate direction 
    //sizeRepos    The control will be moved in the appropriate direction 
    //sizeRelative The control will be moved proportionally in the appropriate direction 
    AddControl(IDC_CRCVERIFY_PATH,     sizeResize, sizeNone);

    AddControl(IDC_CRCVERIFY_PROGRESS, sizeResize, sizeRepos);
    AddControl(IDC_CRCVERIFY_STATUS,   sizeResize, sizeRepos);
    AddControl(IDC_CRCVERIFY_LIST,     sizeResize, sizeResize);

    AddControl(IDOK,                   sizeRepos,  sizeRepos, XPR_FALSE);
    AddControl(IDCANCEL,               sizeRepos,  sizeRepos, XPR_FALSE);
    //------------------------------------------------------------

    SetDlgItemText(IDC_CRCVERIFY_PATH, mDir.c_str());

    mListCtrl.SetExtendedStyle(mListCtrl.GetExtendedStyle() | LVS_EX_FULLROWSELECT);
    mListCtrl.InsertColumn(0, theApp.loadString(XPR_STRING_LITERAL("popup.crc_verify.list.column.no")),             LVCFMT_RIGHT,  35, -1);
    mListCtrl.InsertColumn(1, theApp.loadString(XPR_STRING_LITERAL("popup.crc_verify.list.column.file")),           LVCFMT_LEFT,  150, -1);
    mListCtrl.InsertColumn(2, theApp.loadString(XPR_STRING_LITERAL("popup.crc_verify.list.column.file_to_verify")), LVCFMT_LEFT,  150, -1);
    mListCtrl.InsertColumn(3, theApp.loadString(XPR_STRING_LITERAL("popup.crc_verify.list.column.crc_code")),       LVCFMT_LEFT,   90, -1);
    mListCtrl.InsertColumn(4, theApp.loadString(XPR_STRING_LITERAL("popup.crc_verify.list.column.result")),         LVCFMT_LEFT,   50, -1);

    FileDeque::iterator sIterator = mFileDeque.begin();
    for (; sIterator != mFileDeque.end(); ++sIterator)
        mCrcVerify->addCrcPath(sIterator->c_str());
    mFileDeque.clear();

    mCrcVerify->init();
    mCrcVerify->setOwner(m_hWnd, WM_FINALIZE);

    xpr_sint_t sIndex;
    LVITEM sLvItem = {0};
    xpr_tchar_t sText[XPR_MAX_PATH + 1] = {0};
    const xpr_tchar_t *sSplit;

    xpr_size_t i, j;
    xpr_size_t sCrcCount;
    xpr_size_t sVerifyCount;
    const xpr_tchar_t *sCrcPath;
    CrcVerify::VerifyFile *sVerifyFile;
    xpr_tchar_t sCrcCode[0xff] = {0};
    xpr_size_t sInputBytes;
    xpr_size_t sOutputBytes;

    sIndex = 0;

    sCrcCount = mCrcVerify->getCrcCount();
    for (i = 0; i < sCrcCount; ++i)
    {
        sCrcPath = mCrcVerify->getCrcPath(i);
        if (sCrcPath == XPR_NULL)
            continue;

        sVerifyCount = mCrcVerify->getVerifyCount(i);
        for (j = 0; j < sVerifyCount; ++j)
        {
            sVerifyFile = mCrcVerify->getVerifyFile(i, j);
            if (sVerifyFile == XPR_NULL)
                continue;

            _stprintf(sText, XPR_STRING_LITERAL("%d"), sIndex+1);
            sLvItem.mask     = LVIF_TEXT | LVIF_PARAM;
            sLvItem.iItem    = sIndex;
            sLvItem.iSubItem = 0;
            sLvItem.pszText  = sText;
            sLvItem.lParam   = (LPARAM)sVerifyFile;
            mListCtrl.InsertItem(&sLvItem);

            sSplit = _tcsrchr(sCrcPath, XPR_STRING_LITERAL('\\'));
            if (sSplit != XPR_NULL)
                _tcscpy(sText, sSplit+1);

            sLvItem.mask     = LVIF_TEXT;
            sLvItem.iItem    = sIndex;
            sLvItem.iSubItem = 1;
            sLvItem.pszText  = (xpr_tchar_t *)sText;
            mListCtrl.SetItem(&sLvItem);

            sSplit = _tcsrchr(sVerifyFile->mPath.c_str(), XPR_STRING_LITERAL('\\'));
            if (sSplit != XPR_NULL)
                _tcscpy(sText, sSplit+1);

            sLvItem.mask     = LVIF_TEXT;
            sLvItem.iItem    = sIndex;
            sLvItem.iSubItem = 2;
            sLvItem.pszText  = (xpr_tchar_t *)sText;
            mListCtrl.SetItem(&sLvItem);

            sInputBytes = strlen(sVerifyFile->mCrcCode);
            sOutputBytes = 0xff * sizeof(xpr_tchar_t);
            XPR_MBS_TO_TCS(sVerifyFile->mCrcCode, &sInputBytes, sCrcCode, &sOutputBytes);
            sCrcCode[sOutputBytes / sizeof(xpr_tchar_t)] = 0;

            sLvItem.mask     = LVIF_TEXT;
            sLvItem.iItem    = sIndex;
            sLvItem.iSubItem = 3;
            sLvItem.pszText  = sCrcCode;
            mListCtrl.SetItem(&sLvItem);

            ++sIndex;
        }
    }

    xpr_sint_t sCount = mListCtrl.GetItemCount();

    mProgressCtrl.SetRange32(0, sCount);
    mProgressCtrl.SetPos(0);

    xpr_tchar_t sStatusText[0xff] = {0};
    _stprintf(sStatusText, theApp.loadFormatString(XPR_STRING_LITERAL("popup.crc_verify.status.count"), XPR_STRING_LITERAL("%d")), sCount);
    setStatus(sStatusText);

    SetWindowText(theApp.loadString(XPR_STRING_LITERAL("popup.crc_verify.title")));
    SetDlgItemText(IDC_CRCVERIFY_LABEL_PATH, theApp.loadString(XPR_STRING_LITERAL("popup.crc_verify.label.path")));
    SetDlgItemText(IDC_CRCVERIFY_LABEL_LIST, theApp.loadString(XPR_STRING_LITERAL("popup.crc_verify.label.list")));
    SetDlgItemText(IDOK,                     theApp.loadString(XPR_STRING_LITERAL("popup.crc_verify.button.verify")));
    SetDlgItemText(IDCANCEL,                 theApp.loadString(XPR_STRING_LITERAL("popup.crc_verify.button.close")));

    mDlgState = DlgStateManager::instance().getDlgState(XPR_STRING_LITERAL("CrcVerify"));
    if (XPR_IS_NOT_NULL(mDlgState))
    {
        mDlgState->setDialog(this, XPR_TRUE);
        mDlgState->setListCtrl(XPR_STRING_LITERAL("List"), mListCtrl.GetDlgCtrlID());
        mDlgState->load();
    }

    return XPR_TRUE;
}

void CrcVerifyDlg::setStatus(const xpr_tchar_t *aStatus)
{
    SetDlgItemText(IDC_CRCVERIFY_STATUS, aStatus);
}

void CrcVerifyDlg::addPath(const xpr_tchar_t *aPath)
{
    if (aPath == XPR_NULL)
        return;

    mFileDeque.push_back(aPath);
}

void CrcVerifyDlg::enableWindow(xpr_bool_t aEnable)
{
    SetDlgItemText(IDOK,
        (aEnable == XPR_TRUE) ?
        theApp.loadString(XPR_STRING_LITERAL("popup.crc_verify.button.verify")) : theApp.loadString(XPR_STRING_LITERAL("popup.crc_verify.button.stop")));

    GetDlgItem(IDC_CRCVERIFY_LIST)->EnableWindow(aEnable);
    GetDlgItem(IDCANCEL)->EnableWindow(aEnable);

    // System Button
    CMenu *sMenu = GetSystemMenu(XPR_FALSE);
    if (sMenu != XPR_NULL)
    {
        xpr_uint_t sFlags = MF_BYCOMMAND;
        if (aEnable == XPR_FALSE)
            sFlags |= MF_GRAYED;
        sMenu->EnableMenuItem(SC_CLOSE, sFlags);
    }

    mEnable = aEnable;
}

xpr_bool_t CrcVerifyDlg::DestroyWindow(void) 
{
    if (XPR_IS_NOT_NULL(mDlgState))
    {
        mDlgState->reset();
        mDlgState->save();
    }

    return super::DestroyWindow();
}

void CrcVerifyDlg::OnOK(void) 
{
    if (mCrcVerify != XPR_NULL)
    {
        if (mCrcVerify->getStatus() == CrcVerify::StatusVerifying)
        {
            mCrcVerify->Stop();
            return;
        }
    }

    xpr_sint_t sCount = mListCtrl.GetItemCount();
    if (sCount <= 0)
    {
        const xpr_tchar_t *sMsg = theApp.loadString(XPR_STRING_LITERAL("popup.crc_verify.msg.none"));
        MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);
        return;
    }

    if (mCrcVerify->Start())
    {
        enableWindow(XPR_FALSE);
        SetTimer(TM_ID_STATUS, 100, XPR_NULL);
    }
}

LRESULT CrcVerifyDlg::OnFinalize(WPARAM wParam, LPARAM lParam)
{
    DWORD sResult = (DWORD)wParam;

    mCrcVerify->Stop();

    KillTimer(TM_ID_STATUS);
    enableWindow(XPR_TRUE);

    xpr_sint_t sCount = mListCtrl.GetItemCount();

    {
        mListCtrl.SetRedraw(XPR_FALSE);

        xpr_sint_t i;
        const xpr_tchar_t *sText;
        CrcVerify::VerifyFile *sVerifyFile;

        for (i = 0; i < sCount; ++i)
        {
            sVerifyFile = (CrcVerify::VerifyFile *)mListCtrl.GetItemData(i);
            sText = XPR_NULL;

            switch (sVerifyFile->mResult)
            {
            case CrcVerify::VerifyResultFailed:     sText = theApp.loadString(XPR_STRING_LITERAL("popup.crc_verify.list.result_read_failed")); break;
            case CrcVerify::VerifyResultNotEqualed: sText = theApp.loadString(XPR_STRING_LITERAL("popup.crc_verify.list.result_not_matched")); break;
            case CrcVerify::VerifyResultEqualed:    sText = theApp.loadString(XPR_STRING_LITERAL("popup.crc_verify.list.result_success"));     break;
            }

            if (sText != XPR_NULL)
                mListCtrl.SetItemText(i, 4, sText);
        }

        mListCtrl.SetRedraw();
    }

    CrcVerify::Status sStatus;
    sStatus = mCrcVerify->getStatus();

    xpr_size_t sProcessedCount = 0;
    xpr_size_t sNotEqualedCount = 0;
    xpr_size_t sFailedCount = 0;
    mCrcVerify->getResult(&sProcessedCount, &sNotEqualedCount, &sFailedCount);

    mProgressCtrl.SetPos((xpr_sint_t)sProcessedCount);

    switch (sStatus)
    {
    case CrcVerify::StatusVerifyCompleted:
        {
            xpr_tchar_t sStatusText[0xff] = {0};
            _stprintf(sStatusText, theApp.loadFormatString(XPR_STRING_LITERAL("popup.crc_verify.status.completed"), XPR_STRING_LITERAL("%d,%d,%d,%d")), sCount, sCount - (sFailedCount + sNotEqualedCount), sNotEqualedCount, sFailedCount);
            setStatus(sStatusText);

            GetDlgItem(IDOK)->EnableWindow(XPR_FALSE);
            break;
        }

    case CrcVerify::StatusStopped:
        {
            xpr_tchar_t sStatusText[0xff] = {0};
            _stprintf(sStatusText, theApp.loadFormatString(XPR_STRING_LITERAL("popup.crc_verify.status.stoped"), XPR_STRING_LITERAL("%d,%d,%d,%d")), sProcessedCount, sProcessedCount - (sFailedCount + sNotEqualedCount), sNotEqualedCount, sFailedCount);
            setStatus(sStatusText);
            break;
        }
    }

    return 0;
}

void CrcVerifyDlg::OnCancel(void) 
{
    if (mEnable == XPR_FALSE)
        return;

    super::OnCancel();
}

void CrcVerifyDlg::OnTimer(UINT_PTR nIDEvent)
{
    if (nIDEvent == TM_ID_STATUS)
    {
        xpr_sint_t sCount = mListCtrl.GetItemCount();

        xpr_size_t sProcessedCount = 0;
        mCrcVerify->getResult(&sProcessedCount);

        mProgressCtrl.SetPos((xpr_sint_t)sProcessedCount);

        xpr_tchar_t sStatusText[0xff] = {0};
        _stprintf(sStatusText, theApp.loadFormatString(XPR_STRING_LITERAL("popup.crc_verify.status.in_progress"), XPR_STRING_LITERAL("%d,%d,%d")), sProcessedCount, sCount, (xpr_sint_t)((xpr_float_t)sProcessedCount / (xpr_float_t)sCount * 100.0));
        setStatus(sStatusText);
    }

    super::OnTimer(nIDEvent);
}
} // namespace cmd
} // namespace fxfile
