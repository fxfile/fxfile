//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "file_split_dlg.h"
#include "file_split.h"

#include "gui/FileDialogST.h"

#include "resource.h"
#include "dlg_state.h"
#include "dlg_state_manager.h"
#include "size_format.h"

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

FileSplitDlg::FileSplitDlg(void)
    : super(IDD_FILE_SPLIT, XPR_NULL)
    , mFileSplit(XPR_NULL)
    , mSetSplitSizeMode(XPR_FALSE)
    , mSetSplitCoutMode(XPR_FALSE)
    , mDlgState(XPR_NULL)
{
    mIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

FileSplitDlg::~FileSplitDlg(void)
{
    XPR_SAFE_DELETE(mFileSplit);

    DESTROY_ICON(mIcon);
}

void FileSplitDlg::DoDataExchange(CDataExchange* pDX)
{
    super::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_FILE_SPLIT_PROGRESS,    mProgressCtrl);
    DDX_Control(pDX, IDC_FILE_SPLIT_SIZE_PRESET, mSplitSizePreset);
    DDX_Control(pDX, IDC_FILE_SPLIT_SIZE_UNIT,   mSplitSizeUnit);
}

BEGIN_MESSAGE_MAP(FileSplitDlg, super)
    ON_WM_TIMER()
    ON_MESSAGE(WM_FINALIZE, OnFinalize)
    ON_BN_CLICKED   (IDC_FILE_SPLIT_PATH_BROWSE,        OnPathBrowse)
    ON_BN_CLICKED   (IDC_FILE_SPLIT_OPTION_SPLIT_SIZE,  OnOptionSplit)
    ON_BN_CLICKED   (IDC_FILE_SPLIT_OPTION_SPLIT_COUNT, OnOptionSplit)
    ON_BN_CLICKED   (IDC_FILE_SPLIT_DEST_DIR_BROWSE,    OnDestDirBrowse)
    ON_CBN_SELCHANGE(IDC_FILE_SPLIT_SIZE_PRESET,        OnSelchangeSplitSizePreset)
    ON_EN_UPDATE    (IDC_FILE_SPLIT_SIZE,               OnUpdateSplitSize)
    ON_EN_UPDATE    (IDC_FILE_SPLIT_COUNT,              OnUpdateSplitCount)
END_MESSAGE_MAP()

xpr_bool_t FileSplitDlg::OnInitDialog(void) 
{
    super::OnInitDialog();

    SetIcon(mIcon, XPR_TRUE);

    // CResizingDialog -------------------------------------------
    //HideSizeIcon();

    AllowSizing(sizeResize, sizeNone);

    //sizeNone:     Don't resize at all  
    //sizeResize:   The control will be stretched in the appropriate direction 
    //sizeRepos:    The control will be moved in the appropriate direction 
    //sizeRelative: The control will be moved proportionally in the appropriate direction 
    AddControl(IDC_FILE_SPLIT_PATH,            sizeResize, sizeNone);
    AddControl(IDC_FILE_SPLIT_DEST_DIR,        sizeResize, sizeNone);
    AddControl(IDC_FILE_SPLIT_PATH_BROWSE,     sizeRepos,  sizeNone, XPR_FALSE);
    AddControl(IDC_FILE_SPLIT_DEST_DIR_BROWSE, sizeRepos,  sizeNone, XPR_FALSE);
    AddControl(IDC_FILE_SPLIT_OPTION,          sizeResize, sizeNone, XPR_FALSE);
    AddControl(IDC_FILE_SPLIT_PROGRESS,        sizeResize, sizeNone);
    AddControl(IDC_FILE_SPLIT_STATUS,          sizeResize, sizeNone);
    AddControl(IDOK,                           sizeRepos,  sizeRepos, XPR_FALSE);
    AddControl(IDCANCEL,                       sizeRepos,  sizeRepos, XPR_FALSE);
    //------------------------------------------------------------

    xpr_sint_t sIndex;

    ((CEdit *)GetDlgItem(IDC_FILE_SPLIT_PATH))->LimitText(XPR_MAX_PATH);
    ((CEdit *)GetDlgItem(IDC_FILE_SPLIT_DEST_DIR))->LimitText(XPR_MAX_PATH);
    ((CEdit *)GetDlgItem(IDC_FILE_SPLIT_COUNT))->LimitText(kuint32max);

#define FXP_ADD_SPLIT_SIZE_PRESET(text, size) \
    sIndex = mSplitSizePreset.AddString(text); mSplitSizePreset.SetItemData(sIndex, size)

    FXP_ADD_SPLIT_SIZE_PRESET(XPR_STRING_LITERAL("1 MB"),            1*1024*1024);
    FXP_ADD_SPLIT_SIZE_PRESET(XPR_STRING_LITERAL("1.2 MB (5.25\")"), 1182720);
    FXP_ADD_SPLIT_SIZE_PRESET(XPR_STRING_LITERAL("1.44 MB (3.5\")"), 1440000);
    FXP_ADD_SPLIT_SIZE_PRESET(XPR_STRING_LITERAL("5 MB"),            5*1024*1024);
    FXP_ADD_SPLIT_SIZE_PRESET(XPR_STRING_LITERAL("10 MB"),           10*1024*1024);
    FXP_ADD_SPLIT_SIZE_PRESET(XPR_STRING_LITERAL("20 MB"),           20*1024*1024);
    FXP_ADD_SPLIT_SIZE_PRESET(XPR_STRING_LITERAL("50 MB"),           50*1024*1024);
    FXP_ADD_SPLIT_SIZE_PRESET(XPR_STRING_LITERAL("100 MB"),          100*1024*1024);
    FXP_ADD_SPLIT_SIZE_PRESET(XPR_STRING_LITERAL("200 MB"),          200*1024*1024);
    FXP_ADD_SPLIT_SIZE_PRESET(XPR_STRING_LITERAL("500 MB"),          500*1024*1024);
    FXP_ADD_SPLIT_SIZE_PRESET(XPR_STRING_LITERAL("650 MB (CD-R)"),   650*1024*1024);
    FXP_ADD_SPLIT_SIZE_PRESET(XPR_STRING_LITERAL("700 MB (CD-R)"),   700*1024*1024);
    FXP_ADD_SPLIT_SIZE_PRESET(gApp.loadString(XPR_STRING_LITERAL("popup.file_split.size_preset.user_defined")), 0);
    mSplitSizePreset.SetCurSel(2);

#define FXP_ADD_SPLIT_SIZE_UNIT(text, unit) \
    sIndex = mSplitSizeUnit.AddString(text); mSplitSizeUnit.SetItemData(sIndex, unit)

    FXP_ADD_SPLIT_SIZE_UNIT(gApp.loadString(XPR_STRING_LITERAL("common.size.byte")), 1);
    FXP_ADD_SPLIT_SIZE_UNIT(XPR_STRING_LITERAL("KB"), 1*1024);
    FXP_ADD_SPLIT_SIZE_UNIT(XPR_STRING_LITERAL("MB"), 1*1024*1024);
    FXP_ADD_SPLIT_SIZE_UNIT(XPR_STRING_LITERAL("GB"), 1*1024*1024*1024);
    mSplitSizeUnit.SetCurSel(2);

    SetDlgItemText(IDC_FILE_SPLIT_PATH, mPath.c_str());
    SetDlgItemText(IDC_FILE_SPLIT_DEST_DIR, mDestDir.c_str());
    ((CButton *)GetDlgItem(IDC_FILE_SPLIT_WITH_CRC))->SetCheck(XPR_TRUE);
    ((CButton *)GetDlgItem(IDC_FILE_SPLIT_OPTION_SPLIT_SIZE))->SetCheck(XPR_TRUE);
    ((CButton *)GetDlgItem(IDC_FILE_SPLIT_OPTION_SPLIT_COUNT))->SetCheck(XPR_FALSE);

    SetWindowText(gApp.loadString(XPR_STRING_LITERAL("popup.file_split.title")));
    SetDlgItemText(IDC_FILE_SPLIT_LABEL_PATH,         gApp.loadString(XPR_STRING_LITERAL("popup.file_split.label.path")));
    SetDlgItemText(IDC_FILE_SPLIT_LABEL_DEST_DIR,     gApp.loadString(XPR_STRING_LITERAL("popup.file_split.label.dest_dir")));
    SetDlgItemText(IDC_FILE_SPLIT_GROUP_OPTION,       gApp.loadString(XPR_STRING_LITERAL("popup.file_split.group.option")));
    SetDlgItemText(IDC_FILE_SPLIT_OPTION_SPLIT_SIZE,  gApp.loadString(XPR_STRING_LITERAL("popup.file_split.radio.split_size")));
    SetDlgItemText(IDC_FILE_SPLIT_OPTION_SPLIT_COUNT, gApp.loadString(XPR_STRING_LITERAL("popup.file_split.radio.split_count")));
    SetDlgItemText(IDC_FILE_SPLIT_WITH_CRC,           gApp.loadString(XPR_STRING_LITERAL("popup.file_split.check.with_crc")));
    SetDlgItemText(IDC_FILE_SPLIT_DELETE_ORG,         gApp.loadString(XPR_STRING_LITERAL("popup.file_split.check.delete_orignal_file")));
    SetDlgItemText(IDC_FILE_SPLIT_CREATE_BATCH_FILE,  gApp.loadString(XPR_STRING_LITERAL("popup.file_split.check.create_batch_file")));
    SetDlgItemText(IDOK,                              gApp.loadString(XPR_STRING_LITERAL("popup.file_split.button.split")));
    SetDlgItemText(IDCANCEL,                          gApp.loadString(XPR_STRING_LITERAL("popup.file_split.button.close")));

    setFileSizeText();
    setStatus(gApp.loadString(XPR_STRING_LITERAL("popup.file_split.status.ready")));

    // Load Dialog State
    mDlgState = DlgStateManager::instance().getDlgState(XPR_STRING_LITERAL("FileSplit"));
    if (XPR_IS_NOT_NULL(mDlgState))
    {
        mDlgState->setDialog(this, XPR_TRUE);
        mDlgState->setComboBox(XPR_STRING_LITERAL("SplitSizeTemplate"),  IDC_FILE_SPLIT_SIZE_PRESET);
        mDlgState->setComboBox(XPR_STRING_LITERAL("SplitSizeUnit"),      IDC_FILE_SPLIT_SIZE_UNIT);
        mDlgState->setEditCtrl(XPR_STRING_LITERAL("SplitSize"),          IDC_FILE_SPLIT_SIZE);
        mDlgState->setCheckBox(XPR_STRING_LITERAL("CrcFile"),            IDC_FILE_SPLIT_WITH_CRC);
        mDlgState->setCheckBox(XPR_STRING_LITERAL("DeleteOrgignalFile"), IDC_FILE_SPLIT_DELETE_ORG);
        mDlgState->setCheckBox(XPR_STRING_LITERAL("BatchFile"),          IDC_FILE_SPLIT_CREATE_BATCH_FILE);
        mDlgState->load();
    }

    OnOptionSplit();
    OnSelchangeSplitSizePreset();

    return XPR_TRUE;
}

void FileSplitDlg::setPath(const xpr_tchar_t *aPath)
{
    if (aPath != XPR_NULL)
        mPath = aPath;
}

void FileSplitDlg::setDestDir(const xpr_tchar_t *aDestDir)
{
    if (aDestDir != XPR_NULL)
        mDestDir = aDestDir;
}

xpr_bool_t FileSplitDlg::DestroyWindow(void) 
{
    if (mFileSplit != XPR_NULL)
        mFileSplit->stop();

    if (XPR_IS_NOT_NULL(mDlgState))
    {
        mDlgState->reset();
        mDlgState->save();
    }

    return super::DestroyWindow();
}

void FileSplitDlg::setStatus(const xpr_tchar_t *aStatus)
{
    if (aStatus != XPR_NULL)
        SetDlgItemText(IDC_FILE_SPLIT_STATUS, aStatus);
}

void FileSplitDlg::enableWindow(xpr_bool_t aEnable)
{
    SetDlgItemText(IDOK, (aEnable == XPR_TRUE) ? gApp.loadString(XPR_STRING_LITERAL("popup.file_split.button.split")) : gApp.loadString(XPR_STRING_LITERAL("popup.file_split.button.stop")));

    GetDlgItem(IDC_FILE_SPLIT_PATH)->EnableWindow(aEnable);
    GetDlgItem(IDC_FILE_SPLIT_PATH_BROWSE)->EnableWindow(aEnable);
    GetDlgItem(IDC_FILE_SPLIT_DEST_DIR)->EnableWindow(aEnable);
    GetDlgItem(IDC_FILE_SPLIT_DEST_DIR_BROWSE)->EnableWindow(aEnable);
    GetDlgItem(IDC_FILE_SPLIT_OPTION_SPLIT_SIZE)->EnableWindow(aEnable);
    GetDlgItem(IDC_FILE_SPLIT_OPTION_SPLIT_COUNT)->EnableWindow(aEnable);
    GetDlgItem(IDC_FILE_SPLIT_SIZE_PRESET)->EnableWindow(aEnable);
    GetDlgItem(IDC_FILE_SPLIT_SIZE)->EnableWindow(aEnable);
    GetDlgItem(IDC_FILE_SPLIT_SIZE_UNIT)->EnableWindow(aEnable);
    GetDlgItem(IDC_FILE_SPLIT_COUNT)->EnableWindow(aEnable);
    GetDlgItem(IDC_FILE_SPLIT_WITH_CRC)->EnableWindow(aEnable);
    GetDlgItem(IDC_FILE_SPLIT_DELETE_ORG)->EnableWindow(aEnable);
    GetDlgItem(IDC_FILE_SPLIT_CREATE_BATCH_FILE)->EnableWindow(aEnable);
    GetDlgItem(IDCANCEL)->EnableWindow(aEnable);

    if (aEnable == XPR_TRUE)
        OnOptionSplit();

    // System Button
    CMenu *sMenu = GetSystemMenu(XPR_FALSE);
    if (sMenu != XPR_NULL)
    {
        xpr_uint_t sFlags = MF_BYCOMMAND;
        if (aEnable == XPR_FALSE)
            sFlags |= MF_GRAYED;
        sMenu->EnableMenuItem(SC_CLOSE, sFlags);
    }
}

static xpr_sint_t CALLBACK BrowseCallbackProc(HWND hwnd, xpr_uint_t uMsg, LPARAM lParam, LPARAM dwData)
{
    if (uMsg == BFFM_INITIALIZED)
        ::SendMessage(hwnd, BFFM_SETSELECTION, XPR_FALSE, dwData);

    return 0;
}

void FileSplitDlg::OnPathBrowse(void)
{
    xpr_tchar_t sFilter[0xff] = {0};
    _stprintf(sFilter, XPR_STRING_LITERAL("%s (*.*)\0*.*\0\0"), gApp.loadString(XPR_STRING_LITERAL("popup.common.file_dialog.filter.all")));
    CFileDialogST sFileDialog(XPR_TRUE, XPR_STRING_LITERAL("*.*"), XPR_NULL, OFN_HIDEREADONLY, sFilter, this);
    if (sFileDialog.DoModal() != IDOK)
        return;

    SetDlgItemText(IDC_FILE_SPLIT_PATH, sFileDialog.GetPathName());
    setFileSizeText();
}

void FileSplitDlg::OnDestDirBrowse(void)
{
    xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
    GetDlgItemText(IDC_FILE_SPLIT_DEST_DIR, sPath, XPR_MAX_PATH);

    LPITEMIDLIST sOldFullPidl = Path2Pidl(sPath);

    xpr_tchar_t sOldPath[XPR_MAX_PATH + 1] = {0};
    GetName(sOldFullPidl, SHGDN_FORPARSING, sOldPath);

    BROWSEINFO sBrowseInfo = {0};
    sBrowseInfo.hwndOwner = GetSafeHwnd();
    sBrowseInfo.ulFlags   = BIF_RETURNONLYFSDIRS;//BIF_USENEWUI;
    sBrowseInfo.lpszTitle = gApp.loadString(XPR_STRING_LITERAL("popup.file_split.folder_browse.title"));
    sBrowseInfo.lpfn      = (BFFCALLBACK)BrowseCallbackProc;
    sBrowseInfo.lParam    = (LPARAM)sOldFullPidl;
    LPITEMIDLIST sFullPidl = ::SHBrowseForFolder(&sBrowseInfo);
    if (sFullPidl != XPR_NULL)
    {
        xpr_tchar_t sNewPath[XPR_MAX_PATH + 1] = {0};
        GetName(sFullPidl, SHGDN_FORPARSING, sNewPath);

        if (_tcsicmp(sOldPath, sNewPath))
            SetDlgItemText(IDC_FILE_SPLIT_DEST_DIR, sNewPath);
    }

    COM_FREE(sFullPidl);
    COM_FREE(sOldFullPidl);
}

void FileSplitDlg::OnOptionSplit(void)
{
    xpr_bool_t bSize = ((CButton *)GetDlgItem(IDC_FILE_SPLIT_OPTION_SPLIT_SIZE))->GetCheck();

    GetDlgItem(IDC_FILE_SPLIT_SIZE_PRESET)->EnableWindow(bSize);
    GetDlgItem(IDC_FILE_SPLIT_SIZE)->EnableWindow(bSize);
    GetDlgItem(IDC_FILE_SPLIT_SIZE_UNIT)->EnableWindow(bSize);
    GetDlgItem(IDC_FILE_SPLIT_COUNT)->EnableWindow(!bSize);
}

xpr_uint64_t FileSplitDlg::getFileSize(void)
{
    xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
    GetDlgItemText(IDC_FILE_SPLIT_PATH, sPath, XPR_MAX_PATH);

    return GetFileSize(sPath);
}

void FileSplitDlg::setFileSizeText(void)
{
    xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
    GetDlgItemText(IDC_FILE_SPLIT_PATH, sPath, XPR_MAX_PATH);

    xpr_uint64_t sFileSize = GetFileSize(sPath);

    xpr_tchar_t sFormatedFileSize[0xff] = {0};
    SizeFormat::getFormatedNumber(sFileSize, sFormatedFileSize, 0xfe);

    xpr_tchar_t sFileSizeText[0xff] = {0};
    _stprintf(sFileSizeText, XPR_STRING_LITERAL("%s: %s %s"), gApp.loadString(XPR_STRING_LITERAL("popup.file_split.label.file_size")), sFormatedFileSize, gApp.loadString(XPR_STRING_LITERAL("common.size.byte")));
    SetDlgItemText(IDC_FILE_SPLIT_LABEL_FILE_SIZE, sFileSizeText);
}

xpr_uint64_t FileSplitDlg::getSplitSize(void)
{
    xpr_uint64_t sSplitSize = 0;

    xpr_tchar_t szText[0xff] = {0};
    GetDlgItemText(IDC_FILE_SPLIT_SIZE, szText, 0xfe);
    _stscanf(szText, XPR_STRING_LITERAL("%I64u"), &sSplitSize);

    xpr_sint_t i;
    xpr_sint_t sSizeUnit;

    sSizeUnit = mSplitSizeUnit.GetCurSel();
    for (i = 0; i < sSizeUnit; ++i)
        sSplitSize *= (xpr_uint64_t)1024;

    return sSplitSize;
}

xpr_size_t FileSplitDlg::getSplitCount(void)
{
    return GetDlgItemInt(IDC_FILE_SPLIT_COUNT, XPR_NULL, XPR_FALSE);
}

void FileSplitDlg::OnOK(void) 
{
    if (mFileSplit != XPR_NULL)
    {
        if (mFileSplit->getStatus() == FileSplit::StatusSplitting)
        {
            mFileSplit->stop();
            return;
        }
    }

    xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
    xpr_tchar_t sDestDir[XPR_MAX_PATH + 1] = {0};

    GetDlgItemText(IDC_FILE_SPLIT_PATH, sPath, XPR_MAX_PATH);
    GetDlgItemText(IDC_FILE_SPLIT_DEST_DIR, sDestDir, XPR_MAX_PATH);

    if (IsExistFile(sPath) == XPR_FALSE)
    {
        const xpr_tchar_t *sMsg = gApp.loadString(XPR_STRING_LITERAL("popup.file_split.msg.not_exist"));
        MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);

        GetDlgItem(IDC_FILE_SPLIT_PATH)->SetFocus();
        return;
    }

    if (IsExistFile(sDestDir) == XPR_FALSE)
    {
        const xpr_tchar_t *sMsg = gApp.loadString(XPR_STRING_LITERAL("popup.file_split.msg.dest_dir_not_exist"));
        MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);

        GetDlgItem(IDC_FILE_SPLIT_DEST_DIR)->SetFocus();
        return;
    }

    xpr_tchar_t sDestPath[XPR_MAX_PATH + 1] = {0};
    _tcscpy(sDestPath, sDestDir);
    if (sDestPath[_tcslen(sDestPath)-1] != XPR_STRING_LITERAL('\\'))
        _tcscat(sDestPath, XPR_STRING_LITERAL("\\"));

    xpr_tchar_t *sSplit = _tcsrchr(sPath, XPR_STRING_LITERAL('\\'));
    if (sSplit != XPR_NULL)
        _tcscat(sDestPath, sSplit+1);

    _tcscat(sDestPath, XPR_STRING_LITERAL(".001"));

    if (IsExistFile(sDestPath) == XPR_TRUE)
    {
        xpr_tchar_t sMsg[XPR_MAX_PATH * 2] = {0};
        _stprintf(sMsg, gApp.loadFormatString(XPR_STRING_LITERAL("popup.file_split.msg.question_overwrite"), XPR_STRING_LITERAL("%s")), sDestPath);
        if (MessageBox(sMsg, XPR_NULL, MB_YESNO | MB_ICONQUESTION) == IDNO)
            return;
    }

    xpr_uint64_t sSplitSize = getSplitSize();
    xpr_size_t sSplitCount = getSplitCount();
    xpr_bool_t sCrcFile = ((CButton *)GetDlgItem(IDC_FILE_SPLIT_WITH_CRC))->GetCheck();

    xpr_uint_t sFlags = 0;
    if (sCrcFile == XPR_TRUE) sFlags |= FileSplit::FlagCrcFile;
    sFlags |= FileSplit::FlagBatFile;

    XPR_SAFE_DELETE(mFileSplit);

    mFileSplit = new FileSplit;
    mFileSplit->setOwner(m_hWnd, WM_FINALIZE);

    mFileSplit->setPath(sPath, sDestDir);
    mFileSplit->setSplitSize(sSplitSize);
    mFileSplit->setFlags(sFlags);

    if (mFileSplit->start() == XPR_TRUE)
    {
        enableWindow(XPR_FALSE);
        SetTimer(TM_ID_STATUS, 100, XPR_NULL);

        mProgressCtrl.SetRange32(0, (xpr_sint_t)sSplitCount);
        mProgressCtrl.SetPos(0);
    }
    else
    {
        FileSplit::Status sStatus = mFileSplit->getStatus();

        switch (sStatus)
        {
        case FileSplit::StatusFileNotExist:
            {
                const xpr_tchar_t *sMsg = gApp.loadString(XPR_STRING_LITERAL("popup.file_split.msg.not_exist"));
                MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);

                GetDlgItem(IDC_FILE_SPLIT_PATH)->SetFocus();
                break;
            }

        case FileSplit::StatusDestDirNotExist:
            {
                const xpr_tchar_t *sMsg = gApp.loadString(XPR_STRING_LITERAL("popup.file_split.msg.dest_dir_not_exist"));
                MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);

                GetDlgItem(IDC_FILE_SPLIT_DEST_DIR)->SetFocus();
                break;
            }
        }
    }
}

void FileSplitDlg::OnTimer(UINT_PTR nIDEvent)
{
    if (nIDEvent == TM_ID_STATUS)
    {
        xpr_size_t sSplitedCount = 0;
        mFileSplit->getStatus(&sSplitedCount);

        xpr_tchar_t sStatusText[0xff] = {0};
        _stprintf(sStatusText, gApp.loadFormatString(XPR_STRING_LITERAL("popup.file_split.status.in_progress"), XPR_STRING_LITERAL("%d")), sSplitedCount);
        setStatus(sStatusText);

        mProgressCtrl.SetPos((xpr_sint_t)sSplitedCount);
    }

    super::OnTimer(nIDEvent);
}

LRESULT FileSplitDlg::OnFinalize(WPARAM wParam, LPARAM lParam)
{
    mFileSplit->stop();

    KillTimer(TM_ID_STATUS);
    enableWindow(XPR_TRUE);

    xpr_size_t sSplitedCount = 0;
    FileSplit::Status sStatus;
    sStatus = mFileSplit->getStatus(&sSplitedCount);

    mProgressCtrl.SetPos((xpr_sint_t)sSplitedCount);

    switch (sStatus)
    {
    case FileSplit::StatusNotReadable:
        {
            const xpr_tchar_t *sMsg = gApp.loadString(XPR_STRING_LITERAL("popup.file_split.msg.read_failed"));
            MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);
            break;
        }

    case FileSplit::StatusNotWritable:
        {
            const xpr_tchar_t *sMsg = gApp.loadString(XPR_STRING_LITERAL("popup.file_split.msg.not_create"));
            MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);
            break;
        }

    case FileSplit::StatusSplitCompleted:
        {
            xpr_tchar_t sStatusText[0xff] = {0};
            _stprintf(sStatusText, gApp.loadFormatString(XPR_STRING_LITERAL("popup.file_split.status.completed"), XPR_STRING_LITERAL("%d")), sSplitedCount);
            setStatus(sStatusText);
            break;
        }

    case FileSplit::StatusStopped:
        {
            xpr_tchar_t sStatusText[0xff] = {0};
            _stprintf(sStatusText, gApp.loadFormatString(XPR_STRING_LITERAL("popup.file_split.status.stoped"), XPR_STRING_LITERAL("%d")), sSplitedCount);
            setStatus(sStatusText);
            break;
        }
    }

    return 0;
}

void FileSplitDlg::OnSelchangeSplitSizePreset(void)
{
    xpr_sint_t sCurSel = mSplitSizePreset.GetCurSel();
    if (sCurSel == CB_ERR)
        return;

    xpr_sint64_t sSplitSize = (xpr_sint64_t)mSplitSizePreset.GetItemData(sCurSel);

    xpr_bool_t sUserDefined = (sSplitSize == 0);
    GetDlgItem(IDC_FILE_SPLIT_SIZE)->EnableWindow(sUserDefined);
    mSplitSizeUnit.EnableWindow(sUserDefined);

    if (sUserDefined == XPR_FALSE)
    {
        xpr_tchar_t sSplitSizeText[XPR_MAX_PATH + 1] = {0};
        _stprintf(sSplitSizeText, XPR_STRING_LITERAL("%I64d"), sSplitSize);

        SetDlgItemText(IDC_FILE_SPLIT_SIZE, sSplitSizeText);
        mSplitSizeUnit.SetCurSel(0);
    }
}

void FileSplitDlg::OnUpdateSplitSize(void)
{
    if (mSetSplitSizeMode == XPR_TRUE)
        return;

    xpr_uint64_t sFileSize = getFileSize();
    xpr_uint64_t sSplitSize = getSplitSize();

    xpr_size_t sSplitCount = 0;
    if (sSplitSize > 0)
    {
        sSplitCount = (xpr_size_t)(sFileSize / sSplitSize);
        if (sFileSize % sSplitSize)
            sSplitCount++;
    }

    xpr_tchar_t sSplitCountText[XPR_MAX_PATH + 1] = {0};
    _stprintf(sSplitCountText, XPR_STRING_LITERAL("%zd"), sSplitCount);

    mSetSplitCoutMode = XPR_TRUE;
    SetDlgItemText(IDC_FILE_SPLIT_COUNT, sSplitCountText);
    mSetSplitCoutMode = XPR_FALSE;
}

void FileSplitDlg::OnUpdateSplitCount(void)
{
    if (mSetSplitCoutMode == XPR_TRUE)
        return;

    xpr_uint64_t sFileSize = getFileSize();
    xpr_size_t sSplitCount = getSplitCount();

    xpr_uint64_t sSplitSize = 0;
    if (sSplitCount > 0)
    {
        sSplitSize = sFileSize / (xpr_uint64_t)sSplitCount;
        if (sFileSize % (xpr_uint64_t)sSplitCount)
            sSplitSize++;
    }

    xpr_tchar_t sSplitSizeText[XPR_MAX_PATH + 1] = {0};
    _stprintf(sSplitSizeText, XPR_STRING_LITERAL("%I64u"), sSplitSize);

    mSetSplitSizeMode = XPR_TRUE;
    SetDlgItemText(IDC_FILE_SPLIT_SIZE, sSplitSizeText);
    mSetSplitSizeMode = XPR_FALSE;

    mSplitSizeUnit.SetCurSel(0);
    mSplitSizePreset.SetCurSel(mSplitSizePreset.GetCount()-1);
}
} // namespace cmd
} // namespace fxfile
