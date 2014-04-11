//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "file_combine_dlg.h"
#include "file_combine.h"

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

FileCombineDlg::FileCombineDlg()
    : super(IDD_FILE_COMBINE, XPR_NULL)
    , mFileCombine(XPR_NULL)
    , mDlgState(XPR_NULL)
{
    mIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

FileCombineDlg::~FileCombineDlg(void)
{
    XPR_SAFE_DELETE(mFileCombine);

    DESTROY_ICON(mIcon);
}

void FileCombineDlg::DoDataExchange(CDataExchange* pDX)
{
    super::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_COMBINE_PROGRESS, mProgressCtrl);
}

BEGIN_MESSAGE_MAP(FileCombineDlg, super)
    ON_WM_TIMER()
    ON_MESSAGE(WM_FINALIZE, OnFinalize)
    ON_BN_CLICKED(IDC_COMBINE_PATH_BROWSE,     OnPathBrowse)
    ON_BN_CLICKED(IDC_COMBINE_DEST_DIR_BROWSE, OnDestDirBrowse)
END_MESSAGE_MAP()

xpr_bool_t FileCombineDlg::OnInitDialog(void) 
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
    AddControl(IDC_COMBINE_PATH,            sizeResize, sizeNone);
    AddControl(IDC_COMBINE_PATH_BROWSE,     sizeRepos,  sizeNone,  XPR_FALSE);
    AddControl(IDC_COMBINE_CRC_FILE,        sizeResize, sizeNone);
    AddControl(IDC_COMBINE_DEST_DIR,        sizeResize, sizeNone);
    AddControl(IDC_COMBINE_DEST_DIR_BROWSE, sizeRepos,  sizeNone,  XPR_FALSE);
    AddControl(IDC_COMBINE_PROGRESS,        sizeResize, sizeNone);
    AddControl(IDC_COMBINE_STATUS,          sizeResize, sizeNone);
    AddControl(IDOK,                        sizeRepos,  sizeRepos, XPR_FALSE);
    AddControl(IDCANCEL,                    sizeRepos,  sizeRepos, XPR_FALSE);
    //------------------------------------------------------------

    ((CEdit *)GetDlgItem(IDC_COMBINE_PATH))->LimitText(XPR_MAX_PATH);
    ((CEdit *)GetDlgItem(IDC_COMBINE_DEST_DIR))->LimitText(XPR_MAX_PATH);

    SetDlgItemText(IDC_COMBINE_PATH, mPath.c_str());
    SetDlgItemText(IDC_COMBINE_DEST_DIR, mDestDir.c_str());
    setCrcFile();

    SetWindowText(gApp.loadString(XPR_STRING_LITERAL("popup.file_combine.title")));
    SetDlgItemText(IDC_COMBINE_LABEL_PATH,     gApp.loadString(XPR_STRING_LITERAL("popup.file_combine.label.path")));
    SetDlgItemText(IDC_COMBINE_LABEL_CRC_FILE, gApp.loadString(XPR_STRING_LITERAL("popup.file_combine.label.crc_file")));
    SetDlgItemText(IDC_COMBINE_LABEL_DEST_DIR, gApp.loadString(XPR_STRING_LITERAL("popup.file_combine.label.dest_dir")));
    SetDlgItemText(IDOK,                       gApp.loadString(XPR_STRING_LITERAL("popup.file_combine.button.combine")));
    SetDlgItemText(IDCANCEL,                   gApp.loadString(XPR_STRING_LITERAL("popup.file_combine.button.close")));

    setStatus(gApp.loadString(XPR_STRING_LITERAL("popup.file_combine.status.ready")));

    mDlgState = DlgStateManager::instance().getDlgState(XPR_STRING_LITERAL("FileCombine"));
    if (XPR_IS_NOT_NULL(mDlgState))
    {
        mDlgState->setDialog(this, XPR_TRUE);
        mDlgState->load();
    }

    return XPR_TRUE;
}

void FileCombineDlg::setPath(const xpr_tchar_t *aPath)
{
    if (aPath != XPR_NULL)
        mPath = aPath;
}

void FileCombineDlg::setDestDir(const xpr_tchar_t *aDestDir)
{
    if (aDestDir != XPR_NULL)
        mDestDir = aDestDir;
}

xpr_bool_t FileCombineDlg::DestroyWindow(void) 
{
    if (mFileCombine != XPR_NULL)
        mFileCombine->Stop();

    if (XPR_IS_NOT_NULL(mDlgState))
    {
        mDlgState->reset();
        mDlgState->save();
    }

    return super::DestroyWindow();
}

void FileCombineDlg::setStatus(const xpr_tchar_t *aStatus)
{
    if (aStatus != XPR_NULL)
        SetDlgItemText(IDC_COMBINE_STATUS, aStatus);
}

void FileCombineDlg::enableWindow(xpr_bool_t aEnable)
{
    SetDlgItemText(IDOK,
        (aEnable == XPR_TRUE) ?
        gApp.loadString(XPR_STRING_LITERAL("popup.file_combine.button.combine")) : gApp.loadString(XPR_STRING_LITERAL("popup.file_combine.button.stop")));

    GetDlgItem(IDC_COMBINE_PATH)->EnableWindow(aEnable);
    GetDlgItem(IDC_COMBINE_PATH_BROWSE)->EnableWindow(aEnable);
    GetDlgItem(IDC_COMBINE_CRC_FILE)->EnableWindow(aEnable);
    GetDlgItem(IDC_COMBINE_DEST_DIR)->EnableWindow(aEnable);
    GetDlgItem(IDC_COMBINE_DEST_DIR_BROWSE)->EnableWindow(aEnable);
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
}

static xpr_sint_t CALLBACK BrowseCallbackProc(HWND hwnd, xpr_uint_t uMsg, LPARAM lParam, LPARAM dwData)
{
    if (uMsg == BFFM_INITIALIZED)
        ::SendMessage(hwnd, BFFM_SETSELECTION, XPR_FALSE, dwData);

    return 0;
}

void FileCombineDlg::OnPathBrowse(void)
{
    xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
    GetDlgItemText(IDC_COMBINE_PATH, sPath, XPR_MAX_PATH);

    xpr_tchar_t sFilter[0xff] = {0};
    _stprintf(sFilter, XPR_STRING_LITERAL("%s (*.*)\0*.*\0\0"), gApp.loadString(XPR_STRING_LITERAL("popup.common.file_dialog.filter.all")));

    CFileDialogST sFileDialog(XPR_TRUE, XPR_STRING_LITERAL("*.*"), sPath, OFN_HIDEREADONLY, sFilter, this);
    if (sFileDialog.DoModal() != IDOK)
        return;

    SetDlgItemText(IDC_COMBINE_PATH, sFileDialog.GetPathName());
    setCrcFile();
}

void FileCombineDlg::setCrcFile(void)
{
    xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
    GetDlgItemText(IDC_COMBINE_PATH, sPath, XPR_MAX_PATH);

    xpr_tchar_t szCrcFile[XPR_MAX_PATH + 1] = {0};

    xpr_tchar_t *sExt = (xpr_tchar_t *)GetFileExt(sPath);
    if (sExt != XPR_NULL)
    {
        xpr_sint_t sIndex = -1;
        _stscanf(sExt+1, XPR_STRING_LITERAL("%d"), &sIndex);

        if (sIndex != -1)
        {
            _tcscpy(sExt, XPR_STRING_LITERAL(".crc"));
            _tcscpy(szCrcFile, sPath);
        }
    }

    SetDlgItemText(IDC_COMBINE_CRC_FILE, szCrcFile);
}

void FileCombineDlg::OnDestDirBrowse(void)
{
    xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
    GetDlgItemText(IDC_COMBINE_DEST_DIR, sPath, XPR_MAX_PATH);

    LPITEMIDLIST sOldFullPidl = Path2Pidl(sPath);

    xpr_tchar_t sOldPath[XPR_MAX_PATH + 1] = {0};
    GetName(sOldFullPidl, SHGDN_FORPARSING, sOldPath);

    BROWSEINFO sBrowseInfo = {0};
    sBrowseInfo.hwndOwner = GetSafeHwnd();
    sBrowseInfo.ulFlags   = BIF_RETURNONLYFSDIRS;//BIF_USENEWUI;
    sBrowseInfo.lpszTitle = gApp.loadString(XPR_STRING_LITERAL("popup.file_combine.folder_browse.title"));
    sBrowseInfo.lpfn      = (BFFCALLBACK)BrowseCallbackProc;
    sBrowseInfo.lParam    = (LPARAM)sOldFullPidl;
    LPITEMIDLIST sFullPidl = ::SHBrowseForFolder(&sBrowseInfo);
    if (sFullPidl != XPR_NULL)
    {
        xpr_tchar_t sNewPath[XPR_MAX_PATH + 1] = {0};
        GetName(sFullPidl, SHGDN_FORPARSING, sNewPath);

        if (_tcsicmp(sOldPath, sNewPath) != 0)
            SetDlgItemText(IDC_COMBINE_DEST_DIR, sNewPath);
    }

    COM_FREE(sFullPidl);
    COM_FREE(sOldFullPidl);
}

void FileCombineDlg::OnOK(void) 
{
    if (mFileCombine != XPR_NULL)
    {
        if (mFileCombine->GetStatus() == FileCombine::StatusCombining)
        {
            mFileCombine->Stop();
            return;
        }
    }

    xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
    xpr_tchar_t sDestDir[XPR_MAX_PATH + 1] = {0};
    GetDlgItemText(IDC_COMBINE_PATH, sPath, XPR_MAX_PATH);
    GetDlgItemText(IDC_COMBINE_DEST_DIR, sDestDir, XPR_MAX_PATH);

    if (IsExistFile(sPath) == XPR_FALSE)
    {
        const xpr_tchar_t *sMsg = gApp.loadString(XPR_STRING_LITERAL("popup.file_combine.msg.not_exist"));
        MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);

        GetDlgItem(IDC_COMBINE_PATH)->SetFocus();
        return;
    }

    if (IsExistFile(sDestDir) == XPR_FALSE)
    {
        const xpr_tchar_t *sMsg = gApp.loadString(XPR_STRING_LITERAL("popup.file_combine.msg.dest_dir_not_exist"));
        MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);

        GetDlgItem(IDC_COMBINE_DEST_DIR)->SetFocus();
        return;
    }

    xpr_tchar_t sDestPath[XPR_MAX_PATH + 1] = {0};
    _tcscpy(sDestPath, sDestDir);
    if (sDestPath[_tcslen(sDestPath)-1] != XPR_STRING_LITERAL('\\'))
        _tcscat(sDestPath, XPR_STRING_LITERAL("\\"));

    xpr_tchar_t *sSplit = _tcsrchr(sPath, XPR_STRING_LITERAL('\\'));
    if (sSplit != XPR_NULL)
        _tcscat(sDestPath, sSplit+1);

    xpr_sint_t sIndex = -1;

    xpr_tchar_t *sExt = (xpr_tchar_t *)GetFileExt(sDestPath);
    if (sExt != XPR_NULL)
    {
        _stscanf(sExt+1, XPR_STRING_LITERAL("%d"), &sIndex);
        if (sIndex != -1)
            *sExt = XPR_STRING_LITERAL('\0');
    }

    if (sIndex == -1)
    {
        const xpr_tchar_t *sMsg = gApp.loadString(XPR_STRING_LITERAL("popup.file_combine.msg.no_combine"));
        MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);

        GetDlgItem(IDC_COMBINE_PATH)->SetFocus();
        return;
    }

    if (IsExistFile(sDestPath) == XPR_TRUE)
    {
        xpr_tchar_t sMsg[0xff] = {0};
        _stprintf(sMsg, gApp.loadFormatString(XPR_STRING_LITERAL("popup.file_combine.msg.question_overwrite"), XPR_STRING_LITERAL("%s")), sDestPath);
        if (MessageBox(sMsg, XPR_NULL, MB_YESNO | MB_ICONQUESTION) == IDNO)
            return;
    }

    XPR_SAFE_DELETE(mFileCombine);

    mFileCombine = new FileCombine;
    mFileCombine->setOwner(m_hWnd, WM_FINALIZE);

    mFileCombine->setPath(sPath);
    mFileCombine->setDestDir(sDestDir);

    if (mFileCombine->Start())
    {
        xpr_sint_t sCount = 0;

        xpr_tchar_t *sExt = (xpr_tchar_t *)GetFileExt(sPath);
        if (sExt != XPR_NULL)
        {
            xpr_sint_t i = -1;
            _stscanf(sExt+1, XPR_STRING_LITERAL("%d"), &i);
            *sExt = XPR_STRING_LITERAL('\0');

            if (i >= 1)
            {
                xpr_tchar_t sCombinePath[XPR_MAX_PATH + 1];

                for (; ; ++i)
                {
                    _stprintf(sCombinePath, XPR_STRING_LITERAL("%s.%03d"), sPath, i);
                    if (IsExistFile(sCombinePath) == XPR_FALSE)
                        break;

                    ++sCount;
                }
            }
        }

        enableWindow(XPR_FALSE);
        SetTimer(TM_ID_STATUS, 100, XPR_NULL);

        mProgressCtrl.SetRange32(0, sCount);
        mProgressCtrl.SetPos(0);
    }
    else
    {
        switch (mFileCombine->GetStatus())
        {
        case FileCombine::StatusFileNotExist:
            {
                const xpr_tchar_t *sMsg = gApp.loadString(XPR_STRING_LITERAL("popup.file_combine.status.not_exist"));
                MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);

                GetDlgItem(IDC_COMBINE_PATH)->SetFocus();
                break;
            }

        case FileCombine::StatusDestDirNotExist:
            {
                const xpr_tchar_t *sMsg = gApp.loadString(XPR_STRING_LITERAL("popup.file_combine.status.dest_dir_not_exist"));
                MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);

                GetDlgItem(IDC_COMBINE_DEST_DIR)->SetFocus();
                break;
            }
        }
    }
}

void FileCombineDlg::OnTimer(UINT_PTR nIDEvent)
{
    if (nIDEvent == TM_ID_STATUS)
    {
        xpr_size_t sCombined = 0;
        mFileCombine->GetStatus(&sCombined);

        xpr_tchar_t sStatusText[0xff] = {0};
        _stprintf(sStatusText, gApp.loadFormatString(XPR_STRING_LITERAL("popup.file_combine.status.in_progress"), XPR_STRING_LITERAL("%d")), sCombined);
        setStatus(sStatusText);

        mProgressCtrl.SetPos((xpr_sint_t)sCombined);
    }

    super::OnTimer(nIDEvent);
}

LRESULT FileCombineDlg::OnFinalize(WPARAM wParam, LPARAM lParam)
{
    mFileCombine->Stop();

    KillTimer(TM_ID_STATUS);
    enableWindow(XPR_TRUE);

    xpr_size_t sCombined = 0;
    FileCombine::Status sStatus;
    sStatus = mFileCombine->GetStatus(&sCombined);

    mProgressCtrl.SetPos((xpr_sint_t)sCombined);

    switch (sStatus)
    {
    case FileCombine::StatusNotCombine:
        {
            const xpr_tchar_t *sMsg = gApp.loadString(XPR_STRING_LITERAL("popup.file_combine.msg.no_combine"));
            MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);
            break;
        }

    case FileCombine::StatusNotWritable:
        {
            const xpr_tchar_t *sMsg = gApp.loadString(XPR_STRING_LITERAL("popup.file_combine.msg.not_created"));
            MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);
            break;
        }

    case FileCombine::StatusCombineCompleted:
        {
            xpr_tchar_t sStatusText[0xff] = {0};
            _stprintf(sStatusText, gApp.loadFormatString(XPR_STRING_LITERAL("popup.file_combine.status.completed"), XPR_STRING_LITERAL("%d")), sCombined);
            setStatus(sStatusText);
            break;
        }

    case FileCombine::StatusStopped:
        {
            xpr_tchar_t sStatusText[0xff] = {0};
            _stprintf(sStatusText, gApp.loadFormatString(XPR_STRING_LITERAL("popup.file_combine.status.stoped"), XPR_STRING_LITERAL("%d")), sCombined);
            setStatus(sStatusText);
            break;
        }

    case FileCombine::StatusInvalidCrcCode:
        {
            xpr_tchar_t sStatusText[0xff] = {0};
            _stprintf(sStatusText, gApp.loadFormatString(XPR_STRING_LITERAL("popup.file_combine.status.invalid_crc_code"), XPR_STRING_LITERAL("%d")), sCombined);
            setStatus(sStatusText);
            break;
        }
    }

    return 0;
}
} // namespace cmd
} // namespace fxfile
