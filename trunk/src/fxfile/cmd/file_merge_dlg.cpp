//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "file_merge_dlg.h"
#include "file_merge.h"

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

FileMergeDlg::FileMergeDlg(void)
    : CResizingDialog(IDD_FILE_MERGE, XPR_NULL)
    , mFileMerge(XPR_NULL)
    , mDlgState(XPR_NULL)
{
    mIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

FileMergeDlg::~FileMergeDlg(void)
{
    XPR_SAFE_DELETE(mFileMerge);

    mPathDeque.clear();

    DESTROY_ICON(mIcon);
}

void FileMergeDlg::DoDataExchange(CDataExchange* pDX)
{
    CResizingDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_MERGE_PROGRESS, mProgressCtrl);
    DDX_Control(pDX, IDC_MERGE_LIST,     mListCtrl);
}

BEGIN_MESSAGE_MAP(FileMergeDlg, CResizingDialog)
    ON_WM_TIMER()
    ON_MESSAGE(WM_FINALIZE, OnFinalize)
    ON_BN_CLICKED(IDC_MERGE_ITEM_UP,          OnItemUp)
    ON_BN_CLICKED(IDC_MERGE_ITEM_DOWN,        OnItemDown)
    ON_BN_CLICKED(IDC_MERGE_DEST_FILE_BROWSE, OnDestFileBrowse)
END_MESSAGE_MAP()

xpr_bool_t FileMergeDlg::OnInitDialog(void) 
{
    CResizingDialog::OnInitDialog();

    SetIcon(mIcon, XPR_TRUE);

    // CResizingDialog -------------------------------------------
    //HideSizeIcon();

    //sizeNone:     Don't resize at all  
    //sizeResize:   The control will be stretched in the appropriate direction 
    //sizeRepos:    The control will be moved in the appropriate direction 
    //sizeRelative: The control will be moved proportionally in the appropriate direction 
    AddControl(IDC_MERGE_PATH,             sizeResize, sizeNone);
    AddControl(IDC_MERGE_LIST,             sizeResize, sizeResize);
    AddControl(IDC_MERGE_ITEM_UP,          sizeNone,   sizeRepos, XPR_FALSE);
    AddControl(IDC_MERGE_ITEM_DOWN,        sizeNone,   sizeRepos, XPR_FALSE);
    AddControl(IDC_MERGE_DEST_FILE,        sizeResize, sizeNone);
    AddControl(IDC_MERGE_DEST_FILE_BROWSE, sizeRepos,  sizeNone,  XPR_FALSE);
    AddControl(IDC_MERGE_PROGRESS,         sizeResize, sizeRepos);
    AddControl(IDC_MERGE_STATUS,           sizeResize, sizeRepos);
    AddControl(IDOK,                       sizeRepos,  sizeRepos, XPR_FALSE);
    AddControl(IDCANCEL,                   sizeRepos,  sizeRepos, XPR_FALSE);
    //------------------------------------------------------------

    ((CEdit *)GetDlgItem(IDC_MERGE_DEST_FILE))->LimitText(XPR_MAX_PATH);

    SetDlgItemText(IDC_MERGE_PATH, mDir.c_str());
    SetDlgItemText(IDC_MERGE_DEST_FILE, mDestPath.c_str());

    mListCtrl.SetExtendedStyle(mListCtrl.GetExtendedStyle() | LVS_EX_FULLROWSELECT);
    mListCtrl.InsertColumn(0, gApp.loadString(XPR_STRING_LITERAL("popup.file_merge.list.column.no")),   LVCFMT_RIGHT,  35, -1);
    mListCtrl.InsertColumn(1, gApp.loadString(XPR_STRING_LITERAL("popup.file_merge.list.column.file")), LVCFMT_LEFT,  305, -1);

    xpr_sint_t i;
    PathDeque::iterator sIterator;
    xpr_tchar_t sText[0xff];
    xpr::string sPath;

    sIterator = mPathDeque.begin();
    for (i = 0; sIterator != mPathDeque.end(); ++sIterator, ++i)
    {
        sPath = sIterator->c_str();

        _stprintf(sText, XPR_STRING_LITERAL("%d"), i+1);
        mListCtrl.InsertItem(i, sText);

        sPath = sPath.substr(sPath.rfind(XPR_STRING_LITERAL('\\'))+1);
        mListCtrl.SetItem(i, 1, LVIF_TEXT, sPath.c_str(), -1, 0, 0, 0);
    }

    xpr_tchar_t sStatusText[0xff] = {0};
    _stprintf(sStatusText, gApp.loadFormatString(XPR_STRING_LITERAL("popup.file_merge.status.count"), XPR_STRING_LITERAL("%d")), mPathDeque.size());
    setStatus(sStatusText);

    SetWindowText(gApp.loadString(XPR_STRING_LITERAL("popup.file_merge.title")));
    SetDlgItemText(IDC_MERGE_LABEL_PATH,      gApp.loadString(XPR_STRING_LITERAL("popup.file_merge.label.path")));
    SetDlgItemText(IDC_MERGE_LABEL_FILES,     gApp.loadString(XPR_STRING_LITERAL("popup.file_merge.label.files_to_merge_in_order")));
    SetDlgItemText(IDC_MERGE_LABEL_DEST_FILE, gApp.loadString(XPR_STRING_LITERAL("popup.file_merge.label.dest_file")));
    SetDlgItemText(IDOK,                      gApp.loadString(XPR_STRING_LITERAL("popup.file_merge.button.merge")));
    SetDlgItemText(IDCANCEL,                  gApp.loadString(XPR_STRING_LITERAL("popup.file_merge.button.close")));

    mDlgState = DlgStateManager::instance().getDlgState(XPR_STRING_LITERAL("FileMerge"));
    if (XPR_IS_NOT_NULL(mDlgState))
    {
        mDlgState->setDialog(this, XPR_TRUE);
        mDlgState->setListCtrl(XPR_STRING_LITERAL("List"), mListCtrl.GetDlgCtrlID());
        mDlgState->load();
    }

    return XPR_TRUE;
}

void FileMergeDlg::setDir(const xpr_tchar_t *aDir)
{
    if (aDir != XPR_NULL)
        mDir = aDir;
}

void FileMergeDlg::addPath(const xpr_tchar_t *aPath)
{
    if (aPath != XPR_NULL)
        mPathDeque.push_back(aPath);
}

void FileMergeDlg::setDestPath(const xpr_tchar_t *aDestPath)
{
    if (aDestPath != XPR_NULL)
        mDestPath = aDestPath;
}

xpr_bool_t FileMergeDlg::DestroyWindow(void) 
{
    if (mFileMerge != XPR_NULL)
        mFileMerge->stop();

    if (XPR_IS_NOT_NULL(mDlgState))
    {
        mDlgState->reset();
        mDlgState->save();
    }

    return CResizingDialog::DestroyWindow();
}

void FileMergeDlg::setStatus(const xpr_tchar_t *sStatus)
{
    if (sStatus != XPR_NULL)
        SetDlgItemText(IDC_MERGE_STATUS, sStatus);
}

void FileMergeDlg::OnItemUp(void) 
{
    if (mListCtrl.GetSelectedCount() <= 0)
        return;

    xpr_sint_t sIndex = mListCtrl.GetSelectionMark();
    if (sIndex == 0)
        return;

    xpr_tchar_t sItem1[XPR_MAX_PATH + 1];
    xpr_tchar_t sItem2[XPR_MAX_PATH + 1];
    mListCtrl.GetItemText(sIndex, 1, sItem1, XPR_MAX_PATH);
    mListCtrl.GetItemText(sIndex-1, 1, sItem2, XPR_MAX_PATH);

    mListCtrl.SetItemText(sIndex, 1, sItem2);
    mListCtrl.SetItemText(sIndex-1, 1, sItem1);

    mListCtrl.SetFocus();
    mListCtrl.EnsureVisible(sIndex-1, XPR_TRUE);
    mListCtrl.SetSelectionMark(sIndex-1);
    mListCtrl.SetItemState(sIndex-1, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
}

void FileMergeDlg::OnItemDown(void) 
{
    if (mListCtrl.GetSelectedCount() <= 0)
        return;

    xpr_sint_t sIndex = mListCtrl.GetSelectionMark();
    if (sIndex == (mListCtrl.GetItemCount()-1))
        return;

    xpr_tchar_t sItem1[XPR_MAX_PATH + 1];
    xpr_tchar_t sItem2[XPR_MAX_PATH + 1];
    mListCtrl.GetItemText(sIndex, 1, sItem1, XPR_MAX_PATH);
    mListCtrl.GetItemText(sIndex+1, 1, sItem2, XPR_MAX_PATH);

    mListCtrl.SetItemText(sIndex, 1, sItem2);
    mListCtrl.SetItemText(sIndex+1, 1, sItem1);

    mListCtrl.SetFocus();
    mListCtrl.EnsureVisible(sIndex+1, XPR_TRUE);
    mListCtrl.SetSelectionMark(sIndex+1);
    mListCtrl.SetItemState(sIndex+1, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
}

void FileMergeDlg::enableWindow(xpr_bool_t aEnable)
{
    SetDlgItemText(IDOK,
        (aEnable == XPR_TRUE) ?
        gApp.loadString(XPR_STRING_LITERAL("popup.file_merge.button.merge")) : gApp.loadString(XPR_STRING_LITERAL("popup.file_merge.button.stop")));

    GetDlgItem(IDC_MERGE_PATH)->EnableWindow(aEnable);
    GetDlgItem(IDC_MERGE_LIST)->EnableWindow(aEnable);
    GetDlgItem(IDC_MERGE_DEST_FILE)->EnableWindow(aEnable);
    GetDlgItem(IDC_MERGE_DEST_FILE_BROWSE)->EnableWindow(aEnable);
    GetDlgItem(IDC_MERGE_ITEM_UP)->EnableWindow(aEnable);
    GetDlgItem(IDC_MERGE_ITEM_DOWN)->EnableWindow(aEnable);
    GetDlgItem(IDCANCEL)->EnableWindow(aEnable);

    // System Button
    CMenu *sMenu = GetSystemMenu(XPR_FALSE);
    if (sMenu != XPR_FALSE)
    {
        xpr_uint_t sFlags = MF_BYCOMMAND;
        if (aEnable == XPR_FALSE)
            sFlags |= MF_GRAYED;
        sMenu->EnableMenuItem(SC_CLOSE, sFlags);
    }
}

void FileMergeDlg::OnDestFileBrowse(void)
{
    xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
    GetDlgItemText(IDC_MERGE_DEST_FILE, sPath, XPR_MAX_PATH);

    xpr_tchar_t sFilter[0xff] = {0};
    _stprintf(sFilter, XPR_STRING_LITERAL("%s (*.*)\0*.*\0\0"), gApp.loadString(XPR_STRING_LITERAL("popup.common.file_dialog.filter.all")));

    CFileDialogST sFileDialog(XPR_TRUE, XPR_STRING_LITERAL("*.*"), sPath, OFN_HIDEREADONLY, sFilter, this);
    if (sFileDialog.DoModal() != IDOK)
        return;

    SetDlgItemText(IDC_MERGE_DEST_FILE, sFileDialog.GetPathName());
}

void FileMergeDlg::OnOK(void) 
{
    if (mFileMerge != XPR_NULL)
    {
        if (mFileMerge->getStatus() == FileMerge::StatusMerging)
        {
            mFileMerge->stop();
            return;
        }
    }

    xpr_tchar_t sDestFile[XPR_MAX_PATH + 1] = {0};
    GetDlgItemText(IDC_MERGE_DEST_FILE, sDestFile, XPR_MAX_PATH);

    if (IsExistFile(sDestFile) == XPR_TRUE)
    {
        if (IsFileSystemFolder(sDestFile) == XPR_TRUE)
        {
            const xpr_tchar_t *sMsg = gApp.loadString(XPR_STRING_LITERAL("popup.file_merge.msg.input"));
            MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);

            GetDlgItem(IDC_MERGE_DEST_FILE)->SetFocus();
            return;
        }

        xpr_tchar_t sMsg[0xff] = {0};
        _stprintf(sMsg, gApp.loadFormatString(XPR_STRING_LITERAL("popup.file_merge.msg.question_overwrite"), XPR_STRING_LITERAL("%s")), sDestFile);
        if (MessageBox(sMsg, XPR_NULL, MB_YESNO | MB_ICONQUESTION) == IDNO)
        {
            GetDlgItem(IDC_MERGE_DEST_FILE)->SetFocus();
            return;
        }
    }

    XPR_SAFE_DELETE(mFileMerge);

    mFileMerge = new FileMerge;
    mFileMerge->setOwner(m_hWnd, WM_FINALIZE);

    mFileMerge->setDestPath(sDestFile);

    PathDeque::iterator sIterator = mPathDeque.begin();
    for (; sIterator != mPathDeque.end(); ++sIterator)
        mFileMerge->addPath(sIterator->c_str());

    if (mFileMerge->start() == XPR_TRUE)
    {
        enableWindow(XPR_FALSE);
        SetTimer(TM_ID_STATUS, 100, XPR_NULL);

        mProgressCtrl.SetRange32(0, mListCtrl.GetItemCount());
        mProgressCtrl.SetPos(0);
    }
    else
    {
        switch (mFileMerge->getStatus())
        {
        case FileMerge::StatusDestDirNotExist:
            {
                xpr_tchar_t sMsg[0xff] = {0};
                _stprintf(sMsg, gApp.loadString(XPR_STRING_LITERAL("popup.file_merge.msg.dest_file_not_exist")), sDestFile);
                MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);

                GetDlgItem(IDC_MERGE_DEST_FILE)->SetFocus();
                break;
            }
        }
    }
}

void FileMergeDlg::OnTimer(UINT_PTR nIDEvent)
{
    if (nIDEvent == TM_ID_STATUS)
    {
        xpr_sint_t sCount = mListCtrl.GetItemCount();

        xpr_size_t sMergedCount = 0;
        mFileMerge->getStatus(&sMergedCount);

        xpr_tchar_t sStatusText[0xff] = {0};
        _stprintf(sStatusText, gApp.loadFormatString(XPR_STRING_LITERAL("popup.file_merge.status.in_progress"), XPR_STRING_LITERAL("%d,%d,%d")), sMergedCount, sCount, (xpr_sint_t)((xpr_float_t)sMergedCount / (xpr_float_t)sCount * 100.0));
        setStatus(sStatusText);

        mProgressCtrl.SetPos((xpr_sint_t)sMergedCount);
    }

    CResizingDialog::OnTimer(nIDEvent);
}

LRESULT FileMergeDlg::OnFinalize(WPARAM wParam, LPARAM lParam)
{
    mFileMerge->stop();

    KillTimer(TM_ID_STATUS);
    enableWindow(XPR_TRUE);

    xpr_size_t sMergedCount = 0;
    FileMerge::Status sStatus;
    sStatus = mFileMerge->getStatus(&sMergedCount);

    mProgressCtrl.SetPos((xpr_sint_t)sMergedCount);

    switch (sStatus)
    {
    case FileMerge::StatusNotWritable:
        {
            const xpr_tchar_t *sMsg = gApp.loadString(XPR_STRING_LITERAL("popup.file_merge.msg.not_created"));
            MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);
            break;
        }

    case FileMerge::StatusMergeCompleted:
        {
            xpr_tchar_t sStatusText[0xff] = {0};
            _stprintf(sStatusText, gApp.loadFormatString(XPR_STRING_LITERAL("popup.file_merge.status.completed"), XPR_STRING_LITERAL("%d")), sMergedCount);
            setStatus(sStatusText);
            break;
        }

    case FileMerge::StatusStopped:
        {
            xpr_tchar_t sStatusText[0xff] = {0};
            _stprintf(sStatusText, gApp.loadFormatString(XPR_STRING_LITERAL("popup.file_merge.status.stoped"), XPR_STRING_LITERAL("%d")), sMergedCount);
            setStatus(sStatusText);
            break;
        }
    }

    return 0;
}

xpr_bool_t FileMergeDlg::PreTranslateMessage(MSG* pMsg)
{
    if (pMsg->message == WM_KEYDOWN && pMsg->hwnd == mListCtrl.m_hWnd)
    {
        switch (pMsg->wParam)
        {
        case VK_UP:
            if (GetKeyState(VK_CONTROL) < 0)
            {
                OnItemUp();
                return XPR_TRUE;
            }
            break;

        case VK_DOWN:
            if (GetKeyState(VK_CONTROL) < 0)
            {
                OnItemDown();
                return XPR_TRUE;
            }
            break;
        }
    }

    return CResizingDialog::PreTranslateMessage(pMsg);
}
} // namespace cmd
} // namespace fxfile
