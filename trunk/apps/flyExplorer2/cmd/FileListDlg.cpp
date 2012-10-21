//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "FileListDlg.h"

#include "fxb/fxb_file_list.h"

#include "rgc/FileDialogST.h"

#include "resource.h"
#include "DlgState.h"
#include "DlgStateMgr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define MAX_DIV_TEXT 30

// user defined message
enum
{
    WM_FINALIZE = WM_USER + 100,
};

FileListDlg::FileListDlg(void)
    : super(IDD_FILE_LIST)
    , mFileList(XPR_NULL)
    , mDlgState(XPR_NULL)
{
}

FileListDlg::~FileListDlg(void)
{
    XPR_SAFE_DELETE(mFileList);

    mPathDeque.clear();
}

void FileListDlg::DoDataExchange(CDataExchange* pDX)
{
    super::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(FileListDlg, super)
    ON_BN_CLICKED(IDC_FILE_LIST_PATH_BROWSE, OnPathBrowse)
    ON_BN_CLICKED(IDOK, OnOK)
    ON_MESSAGE(WM_FINALIZE, OnFinalize)
END_MESSAGE_MAP()

xpr_bool_t FileListDlg::OnInitDialog(void) 
{
    super::OnInitDialog();

    HICON sIcon = fxb::GetFileExtIcon(XPR_STRING_LITERAL(".txt"), XPR_TRUE);
    setDesc(theApp.loadString(XPR_STRING_LITERAL("popup.file_list.label.top_desc")));
    setDescIcon(sIcon, XPR_TRUE);

    ((CButton *)GetDlgItem(IDC_FILE_LIST_BY_LINE))->SetCheck(XPR_TRUE);
    ((CButton *)GetDlgItem(IDC_FILE_LIST_ONLY_FILE))->SetCheck(XPR_TRUE);
    ((CButton *)GetDlgItem(IDC_FILE_LIST_WITH_EXT))->SetCheck(XPR_TRUE);
    ((CButton *)GetDlgItem(IDC_FILE_LIST_WITH_DIR))->SetCheck(XPR_FALSE);
    ((CButton *)GetDlgItem(IDC_FILE_LIST_WITH_INFO))->SetCheck(XPR_FALSE);
    ((CButton *)GetDlgItem(IDC_FILE_LIST_SEPARATOR))->SetCheck(XPR_FALSE);
    SetDlgItemText(IDC_FILE_LIST_PATH, mTextFile.c_str());

    ((CEdit *)GetDlgItem(IDC_FILE_LIST_SEPARATOR_TEXT))->LimitText(MAX_DIV_TEXT);
    ((CEdit *)GetDlgItem(IDC_FILE_LIST_PATH))->LimitText(XPR_MAX_PATH);

    SetWindowText(theApp.loadString(XPR_STRING_LITERAL("popup.file_list.title")));
    SetDlgItemText(IDC_FILE_LIST_LABEL_PATH,    theApp.loadString(XPR_STRING_LITERAL("popup.file_list.label.path")));
    SetDlgItemText(IDC_FILE_LIST_PATH_BROWSE,   theApp.loadString(XPR_STRING_LITERAL("popup.file_list.button.path_browse")));
    SetDlgItemText(IDC_FILE_LIST_GROUP_OPTION,  theApp.loadString(XPR_STRING_LITERAL("popup.file_list.group.option")));
    SetDlgItemText(IDC_FILE_LIST_ONLY_FILE,     theApp.loadString(XPR_STRING_LITERAL("popup.file_list.check.only_file")));
    SetDlgItemText(IDC_FILE_LIST_BY_LINE,       theApp.loadString(XPR_STRING_LITERAL("popup.file_list.check.by_line")));
    SetDlgItemText(IDC_FILE_LIST_WITH_EXT,      theApp.loadString(XPR_STRING_LITERAL("popup.file_list.check.with_extension")));
    SetDlgItemText(IDC_FILE_LIST_WITH_INFO,     theApp.loadString(XPR_STRING_LITERAL("popup.file_list.check.with_info")));
    SetDlgItemText(IDC_FILE_LIST_WITH_DIR,      theApp.loadString(XPR_STRING_LITERAL("popup.file_list.check.with_dir")));
    SetDlgItemText(IDC_FILE_LIST_SEPARATOR,     theApp.loadString(XPR_STRING_LITERAL("popup.file_list.check.separator")));

    mDlgState = DlgStateMgr::instance().getDlgState(XPR_STRING_LITERAL("FileList"));
    if (XPR_IS_NOT_NULL(mDlgState))
    {
        mDlgState->setDialog(this, XPR_TRUE);
        mDlgState->setCheckBox(XPR_STRING_LITERAL("ByLine"),         IDC_FILE_LIST_BY_LINE);
        mDlgState->setCheckBox(XPR_STRING_LITERAL("OnlyFile"),       IDC_FILE_LIST_ONLY_FILE);
        mDlgState->setCheckBox(XPR_STRING_LITERAL("WithExt"),        IDC_FILE_LIST_WITH_EXT);
        mDlgState->setCheckBox(XPR_STRING_LITERAL("WithDir"),        IDC_FILE_LIST_WITH_DIR);
        mDlgState->setCheckBox(XPR_STRING_LITERAL("WithInfo"),       IDC_FILE_LIST_WITH_INFO);
        mDlgState->setCheckBox(XPR_STRING_LITERAL("Seperator"),      IDC_FILE_LIST_SEPARATOR);
        mDlgState->setEditCtrl(XPR_STRING_LITERAL("Seperator Text"), IDC_FILE_LIST_SEPARATOR_TEXT);
        mDlgState->load();
    }

    return XPR_TRUE;
}

xpr_bool_t FileListDlg::DestroyWindow(void)
{
    if (mFileList != XPR_NULL)
        mFileList->Stop();

    if (XPR_IS_NOT_NULL(mDlgState))
    {
        mDlgState->reset();
        mDlgState->save();
    }

    return super::DestroyWindow();
}

void FileListDlg::setTextFile(const xpr_tchar_t *aTextFile)
{
    if (aTextFile != XPR_NULL)
        mTextFile = aTextFile;
}

void FileListDlg::addPath(const xpr_tchar_t *aPath)
{
    if (aPath != XPR_NULL)
        mPathDeque.push_back(aPath);
}

void FileListDlg::OnPathBrowse(void) 
{
    xpr_tchar_t sFileType[0xff] = {0};
    _stprintf(sFileType, XPR_STRING_LITERAL("%s (*.txt)\0*.txt\0\0"), theApp.loadString(XPR_STRING_LITERAL("popup.file_list.file_dialog.filter.text_file")));

    CFileDialogST sFileDialog(XPR_FALSE, XPR_STRING_LITERAL("*.txt"), XPR_NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, sFileType, this);
    if (sFileDialog.DoModal() == IDOK)
        SetDlgItemText(IDC_PATH, sFileDialog.GetPathName());
}

void FileListDlg::enableWindow(xpr_bool_t aEnable)
{
    xpr_uint_t sIds[] = {
        IDC_FILE_LIST_PATH,
        IDC_FILE_LIST_PATH_BROWSE,
        IDC_FILE_LIST_BY_LINE,
        IDC_FILE_LIST_ONLY_FILE,
        IDC_FILE_LIST_WITH_DIR,
        IDC_FILE_LIST_WITH_EXT,
        IDC_FILE_LIST_WITH_INFO,
        IDC_FILE_LIST_SEPARATOR,
        IDCANCEL,
        0
    };

    xpr_sint_t i;
    CWnd *sWnd;

    for (i = 0; sIds[i]; ++i)
    {
        sWnd = GetDlgItem(sIds[i]);
        if (sWnd != XPR_NULL)
            sWnd->EnableWindow(aEnable);
    }

    GetDlgItem(IDOK)->SetWindowText((aEnable == XPR_TRUE) ? theApp.loadString(XPR_STRING_LITERAL("popup.common.button.ok")) : theApp.loadString(XPR_STRING_LITERAL("popup.file_list.button.stop")));
}

void FileListDlg::OnOK(void) 
{
    if (mFileList != XPR_NULL)
    {
        if (mFileList->getStatus() == fxb::FileList::StatusCreating)
        {
            mFileList->Stop();
            return;
        }
    }

    xpr_tchar_t sTextFile[XPR_MAX_PATH + 1] = {0};
    GetDlgItemText(IDC_FILE_LIST_PATH, sTextFile, XPR_MAX_PATH);

    xpr_bool_t sByLine    = ((CButton *)GetDlgItem(IDC_FILE_LIST_BY_LINE))->GetCheck();
    xpr_bool_t sOnlyFile  = ((CButton *)GetDlgItem(IDC_FILE_LIST_ONLY_FILE))->GetCheck();
    xpr_bool_t sFrontPath = ((CButton *)GetDlgItem(IDC_FILE_LIST_WITH_DIR))->GetCheck();
    xpr_bool_t sExtension = ((CButton *)GetDlgItem(IDC_FILE_LIST_WITH_EXT))->GetCheck();
    xpr_bool_t sAttribute = ((CButton *)GetDlgItem(IDC_FILE_LIST_WITH_INFO))->GetCheck();
    xpr_bool_t sSplitChar = ((CButton *)GetDlgItem(IDC_FILE_LIST_SEPARATOR))->GetCheck();

    xpr_tchar_t sText[MAX_DIV_TEXT + 1] = {0};
    xpr_tchar_t sDivText[MAX_DIV_TEXT + 1] = {0};
    GetDlgItemText(IDC_FILE_LIST_SEPARATOR_TEXT, sText, MAX_DIV_TEXT);
    fxb::ConvertStringToFormat(sText, sDivText);

    xpr_uint_t sFlags = 0;
    if (sByLine    == XPR_TRUE) sFlags |= fxb::FileList::FlagsByLine;
    if (sOnlyFile  == XPR_TRUE) sFlags |= fxb::FileList::FlagsOnlyFile;
    if (sFrontPath == XPR_TRUE) sFlags |= fxb::FileList::FlagsWithDir;
    if (sExtension == XPR_TRUE) sFlags |= fxb::FileList::FlagsExtension;
    if (sAttribute == XPR_TRUE) sFlags |= fxb::FileList::FlagsAttribute;
    if (sSplitChar == XPR_TRUE) sFlags |= fxb::FileList::FlagsSplitChar;

    XPR_SAFE_DELETE(mFileList);

    mFileList = new fxb::FileList;
    mFileList->setOwner(m_hWnd, WM_FINALIZE);

    mFileList->setTextFile(sTextFile);
    mFileList->setFlags(sFlags);
    mFileList->setSplitChar(sDivText);

    PathDeque::iterator sIterator = mPathDeque.begin();
    for (; sIterator != mPathDeque.end(); ++sIterator)
        mFileList->addPath(sIterator->c_str());

    if (mFileList->Start())
    {
        enableWindow(XPR_FALSE);
    }
}

LRESULT FileListDlg::OnFinalize(WPARAM wParam, LPARAM lParam)
{
    mFileList->Stop();

    fxb::FileList::Status sStatus;
    sStatus = mFileList->getStatus();

    switch (sStatus)
    {
    case fxb::FileList::StatusFailed:
        {
            const xpr_tchar_t *sMsg = theApp.loadString(XPR_STRING_LITERAL("popup.file_list.msg.not_created"));
            MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);
            break;
        }

    case fxb::FileList::StatusCreateCompleted:
        {
            super::OnOK();
            break;
        }
    }

    return 0;
}
