//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "crc_create_dlg.h"
#include "crc_create.h"

#include "crc_checksum.h"
#include "resource.h"
#include "dlg_state.h"
#include "dlg_state_manager.h"

#include "gui/FileDialogST.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace fxfile
{
namespace cmd
{
namespace
{
// user timer
enum
{
    TM_ID_STATUS = 100,
};

// user message
enum
{
    WM_FINALIZE = WM_USER + 100,
};
} // namespace anonymous

CrcCreateDlg::CrcCreateDlg(void)
    : super(IDD_CRC_CREATE, XPR_NULL)
    , mCrcCreate(XPR_NULL)
    , mEnable(XPR_TRUE)
    , mDlgState(XPR_NULL)
{
    mIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

    mPath[0] = 0;
    mSaveDefaultPath[0] = 0;
}

CrcCreateDlg::~CrcCreateDlg(void)
{
    XPR_SAFE_DELETE(mCrcCreate);

    mListDeque.clear();

    DESTROY_ICON(mIcon);
}

void CrcCreateDlg::DoDataExchange(CDataExchange* pDX)
{
    super::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_CRCCREATE_PROGRESS, mProgressCtrl);
    DDX_Control(pDX, IDC_CRCCREATE_LIST,     mListCtrl);
    DDX_Control(pDX, IDC_CRCCREATE_METHOD,   mMethodWnd);
}

BEGIN_MESSAGE_MAP(CrcCreateDlg, super)
    ON_WM_TIMER()
    ON_MESSAGE(WM_FINALIZE, OnFinalize)
    ON_CBN_SELCHANGE(IDC_CRCCREATE_METHOD, OnSelchangeMethod)
    ON_BN_CLICKED   (IDC_CRCCREATE_BROWSE, OnBrowse)
    ON_BN_CLICKED   (IDC_CRCCREATE_EACH,   OnEach)
    ON_BN_CLICKED   (IDOK,                 OnOK)
END_MESSAGE_MAP()

xpr_bool_t CrcCreateDlg::OnInitDialog(void) 
{
    super::OnInitDialog();

    SetIcon(mIcon, XPR_TRUE);

    // CResizingDialog -------------------------------------------
    //HideSizeIcon();

    //sizeNone:     Don't resize at all  
    //sizeResize:   The control will be stretched in the appropriate direction 
    //sizeRepos:    The control will be moved in the appropriate direction 
    //sizeRelative: The control will be moved proportionally in the appropriate direction 
    AddControl(IDC_CRCCREATE_PATH,     sizeResize, sizeNone);
    AddControl(IDC_CRCCREATE_BROWSE,   sizeRepos,  sizeNone, XPR_FALSE);

    AddControl(IDC_CRCCREATE_PROGRESS, sizeResize, sizeRepos);
    AddControl(IDC_CRCCREATE_STATUS,   sizeResize, sizeRepos);
    AddControl(IDC_CRCCREATE_LIST,     sizeResize, sizeResize);

    AddControl(IDOK,                   sizeRepos,  sizeRepos, XPR_FALSE);
    AddControl(IDCANCEL,               sizeRepos,  sizeRepos, XPR_FALSE);
    //------------------------------------------------------------

    CComboBox *sComboBox;
    sComboBox = (CComboBox *)GetDlgItem(IDC_CRCCREATE_METHOD);
    sComboBox->AddString(XPR_STRING_LITERAL("SFV"));
    sComboBox->AddString(XPR_STRING_LITERAL("MD5"));

    mMethodWnd.SetCurSel(0);
    SetDlgItemText(IDC_CRCCREATE_PATH, mSaveDefaultPath);

    mListCtrl.SetExtendedStyle(mListCtrl.GetExtendedStyle() | LVS_EX_FULLROWSELECT);
    mListCtrl.InsertColumn(0, gApp.loadString(XPR_STRING_LITERAL("popup.crc_create.list.column.no")),   LVCFMT_RIGHT,  35, -1);
    mListCtrl.InsertColumn(1, gApp.loadString(XPR_STRING_LITERAL("popup.crc_create.list.column.file")), LVCFMT_LEFT,  290, -1);

    xpr_sint_t i;
    LVITEM sLvItem = {0};
    xpr_tchar_t sText[0xff] = {0};

    PathDeque::iterator sIterator = mListDeque.begin();
    for (i = 0; sIterator != mListDeque.end(); ++sIterator, ++i)
    {
        _stprintf(sText, XPR_STRING_LITERAL("%d"), i+1);
        sLvItem.mask     = LVIF_TEXT;
        sLvItem.iItem    = i;
        sLvItem.iSubItem = 0;
        sLvItem.pszText  = sText;
        mListCtrl.InsertItem(&sLvItem);

        sLvItem.mask     = LVIF_TEXT;
        sLvItem.iItem    = i;
        sLvItem.iSubItem = 1;
        sLvItem.pszText  = (xpr_tchar_t *)sIterator->c_str();
        mListCtrl.SetItem(&sLvItem);
    }

    xpr_size_t sCount = mListDeque.size();

    mProgressCtrl.SetRange32(0, (xpr_sint_t)sCount);

    xpr_tchar_t sStatusText[0xff] = {0};
    _stprintf(sStatusText, gApp.loadFormatString(XPR_STRING_LITERAL("popup.crc_create.status.count"), XPR_STRING_LITERAL("%d")), sCount);
    setStatus(sStatusText);

    ((CEdit *)GetDlgItem(IDC_CRCCREATE_PATH))->LimitText(XPR_MAX_PATH);

    SetWindowText(gApp.loadString(XPR_STRING_LITERAL("popup.crc_create.title")));
    SetDlgItemText(IDC_CRCCREATE_LABEL_PATH,   gApp.loadString(XPR_STRING_LITERAL("popup.crc_create.label.path")));
    SetDlgItemText(IDC_CRCCREATE_LABEL_METHOD, gApp.loadString(XPR_STRING_LITERAL("popup.crc_create.label.method")));
    SetDlgItemText(IDC_CRCCREATE_EACH,         gApp.loadString(XPR_STRING_LITERAL("popup.crc_create.check.each")));
    SetDlgItemText(IDOK,                       gApp.loadString(XPR_STRING_LITERAL("popup.crc_create.button.create")));
    SetDlgItemText(IDCANCEL,                   gApp.loadString(XPR_STRING_LITERAL("popup.crc_create.button.close")));

    mDlgState = DlgStateManager::instance().getDlgState(XPR_STRING_LITERAL("CrcCreate"));
    if (XPR_IS_NOT_NULL(mDlgState))
    {
        mDlgState->setDialog(this, XPR_TRUE);
        mDlgState->setListCtrl(XPR_STRING_LITERAL("List"), mListCtrl.GetDlgCtrlID());
        mDlgState->load();
    }

    return XPR_TRUE;
}

xpr_bool_t CrcCreateDlg::DestroyWindow(void) 
{
    if (XPR_IS_NOT_NULL(mDlgState))
    {
        mDlgState->reset();
        mDlgState->save();
    }

    return super::DestroyWindow();
}

void CrcCreateDlg::addPath(const xpr_tchar_t *aPath)
{
    if (aPath != XPR_NULL)
        mListDeque.push_back(aPath);
}

void CrcCreateDlg::saveDefaultPath(const xpr_tchar_t *aPath)
{
    if (aPath != XPR_NULL)
        _tcscpy(mSaveDefaultPath, aPath);
}

void CrcCreateDlg::OnBrowse(void) 
{
    const xpr_tchar_t *sFileType = XPR_STRING_LITERAL("Simple File Verification (*.sfv)\0*.sfv\0Message Digest 5 (*.md5)\0*.md5\0\0");

    CFileDialogST sFileDialog(XPR_FALSE, XPR_STRING_LITERAL("*.sfv"), XPR_NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, sFileType, this);
    if (sFileDialog.DoModal() != IDOK)
        return;

    xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
    _tcscpy(sPath, sFileDialog.GetPathName());

    xpr_sint_t sMethod = 0;
    if (IsEqualFileExt(sPath, XPR_STRING_LITERAL(".md5")))
        sMethod = 1;

    mMethodWnd.SetCurSel(sMethod);

    SetDlgItemText(IDC_CRCCREATE_PATH, sPath);
}

void CrcCreateDlg::enableWindow(xpr_bool_t aEnable)
{
    SetDlgItemText(IDOK,
                   (aEnable == XPR_TRUE) ?
                   gApp.loadString(XPR_STRING_LITERAL("popup.crc_create.button.create")) :
                   gApp.loadString(XPR_STRING_LITERAL("popup.crc_create.button.stop")));

    GetDlgItem(IDC_CRCCREATE_PATH)->EnableWindow(aEnable);
    GetDlgItem(IDC_CRCCREATE_BROWSE)->EnableWindow(aEnable);
    GetDlgItem(IDC_CRCCREATE_METHOD)->EnableWindow(aEnable);
    GetDlgItem(IDC_CRCCREATE_EACH)->EnableWindow(aEnable);
    GetDlgItem(IDC_CRCCREATE_LIST)->EnableWindow(aEnable);
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

void CrcCreateDlg::setStatus(const xpr_tchar_t *aStatus)
{
    SetDlgItemText(IDC_CRCCREATE_STATUS, aStatus);
}

void CrcCreateDlg::OnSelchangeMethod(void) 
{
    xpr_sint_t sMethod = mMethodWnd.GetCurSel();

    xpr_tchar_t sExt[100] = {0};
    if (sMethod == 0) _tcscpy(sExt, XPR_STRING_LITERAL(".sfv"));
    else              _tcscpy(sExt, XPR_STRING_LITERAL(".md5"));

    xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
    GetDlgItemText(IDC_CRCCREATE_PATH, sPath, XPR_MAX_PATH);

    xpr_tchar_t *sSplit = _tcsrchr(sPath, '\\');
    if (sSplit != XPR_NULL)
    {
        sSplit++;
        sSplit = (xpr_tchar_t *)GetFileExt(sSplit);
        if (sSplit != XPR_NULL)
            _tcscpy(sSplit, sExt);
    }

    SetDlgItemText(IDC_CRCCREATE_PATH, sPath);
}

void CrcCreateDlg::OnEach(void) 
{
    xpr_bool_t sEach = ((CButton *)GetDlgItem(IDC_CRCCREATE_EACH))->GetCheck();
    GetDlgItem(IDC_CRCCREATE_PATH)->EnableWindow(!sEach);
    GetDlgItem(IDC_CRCCREATE_BROWSE)->EnableWindow(!sEach);
}

void CrcCreateDlg::OnOK(void) 
{
    if (mCrcCreate != XPR_NULL)
    {
        if (mCrcCreate->getStatus() == CrcCreate::StatusCreating)
        {
            mCrcCreate->stop();
            return;
        }
    }

    xpr_bool_t sEach;
    xpr_sint_t sMethod;
    xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};

    sEach = ((CButton *)GetDlgItem(IDC_CRCCREATE_EACH))->GetCheck();
    sMethod = mMethodWnd.GetCurSel();
    GetDlgItemText(IDC_CRCCREATE_PATH, sPath, XPR_MAX_PATH);

    XPR_SAFE_DELETE(mCrcCreate);

    mCrcCreate = new CrcCreate;
    mCrcCreate->setOwner(m_hWnd, WM_FINALIZE);

    mCrcCreate->setEach(sEach);
    mCrcCreate->setMethod(sMethod);
    mCrcCreate->setPath(sPath);

    PathDeque::iterator sIterator = mListDeque.begin();
    for (; sIterator != mListDeque.end(); ++sIterator)
        mCrcCreate->addPath(*sIterator);

    if (mCrcCreate->start() == XPR_TRUE)
    {
        enableWindow(XPR_FALSE);
        SetTimer(TM_ID_STATUS, 100, XPR_NULL);
    }
}

LRESULT CrcCreateDlg::OnFinalize(WPARAM wParam, LPARAM lParam)
{
    mCrcCreate->stop();

    KillTimer(TM_ID_STATUS);
    enableWindow(XPR_TRUE);

    xpr_size_t sProcessedCount = 0;
    xpr_size_t sSucceededCount = 0;
    CrcCreate::Status sStatus;
    sStatus = mCrcCreate->getStatus(&sProcessedCount, &sSucceededCount);

    mProgressCtrl.SetPos((xpr_sint_t)sProcessedCount);

    switch (sStatus)
    {
    case CrcCreate::StatusFailed:
        {
            const xpr_tchar_t *sMsg = gApp.loadString(XPR_STRING_LITERAL("popup.crc_create.msg.failed"));
            MessageBox(sMsg);
            break;
        }

    case CrcCreate::StatusCreateCompleted:
        {
            const xpr_tchar_t *sStatusText = gApp.loadString(XPR_STRING_LITERAL("popup.crc_create.status.completed"));
            setStatus(sStatusText);

            super::OnOK();
            break;
        }

    case CrcCreate::StatusStopped:
        {
            xpr_tchar_t sStatusText[0xff] = {0};
            _stprintf(sStatusText, gApp.loadFormatString(XPR_STRING_LITERAL("popup.crc_create.status.stoped"), XPR_STRING_LITERAL("%d")), sProcessedCount);
            setStatus(sStatusText);
            break;
        }
    }

    return 0;
}

void CrcCreateDlg::OnCancel(void) 
{
    if (mEnable == XPR_FALSE)
        return;

    super::OnCancel();
}

void CrcCreateDlg::OnTimer(UINT_PTR nIDEvent)
{
    if (nIDEvent == TM_ID_STATUS)
    {
        xpr_size_t sCount;
        xpr_size_t sProcessedCount;

        sCount = mCrcCreate->getCount();
        mCrcCreate->getStatus(&sProcessedCount);

        xpr_tchar_t sStatusText[0xff] = {0};
        _stprintf(sStatusText, gApp.loadFormatString(XPR_STRING_LITERAL("popup.crc_create.status.in_progress"), XPR_STRING_LITERAL("%d,%d,%d")), sProcessedCount, sCount, (xpr_sint_t)((xpr_float_t)sProcessedCount / (xpr_float_t)sCount * 100.0));

        setStatus(sStatusText);
        mProgressCtrl.SetPos((xpr_sint_t)sProcessedCount);
    }

    super::OnTimer(nIDEvent);
}
} // namespace cmd
} // namespace fxfile
