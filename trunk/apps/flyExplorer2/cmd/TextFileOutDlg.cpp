//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "TextFileOutDlg.h"

#include "fxb/fxb_file_list.h"

#include "rgc/FileDialogST.h"

#include "resource.h"

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

TextFileOutDlg::TextFileOutDlg(void)
    : super(IDD_FILE_LIST)
    , mTextFileOut(XPR_NULL)
{
}

TextFileOutDlg::~TextFileOutDlg(void)
{
    XPR_SAFE_DELETE(mTextFileOut);

    mPathDeque.clear();
}

void TextFileOutDlg::DoDataExchange(CDataExchange* pDX)
{
    super::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(TextFileOutDlg, super)
    ON_BN_CLICKED(IDC_FILE_LIST_PATH_BROWSE, OnPathBrowse)
    ON_BN_CLICKED(IDOK, OnOK)
    ON_MESSAGE(WM_FINALIZE, OnFinalize)
END_MESSAGE_MAP()

xpr_bool_t TextFileOutDlg::OnInitDialog(void) 
{
    super::OnInitDialog();

    HICON sIcon = fxb::GetFileExtIcon(XPR_STRING_LITERAL(".txt"), XPR_TRUE);
    setDesc(theApp.loadString(XPR_STRING_LITERAL("popup.file_list.label.top_desc")));
    setDescIcon(sIcon, XPR_TRUE);

    ((CButton *)GetDlgItem(IDC_FILE_LIST_BY_LINE))->SetCheck(XPR_TRUE);
    ((CButton *)GetDlgItem(IDC_FILE_LIST_ONLY_FILENAME))->SetCheck(XPR_TRUE);
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
    SetDlgItemText(IDC_FILE_LIST_ONLY_FILENAME, theApp.loadString(XPR_STRING_LITERAL("popup.file_list.check.only_filename")));
    SetDlgItemText(IDC_FILE_LIST_BY_LINE,       theApp.loadString(XPR_STRING_LITERAL("popup.file_list.check.by_line")));
    SetDlgItemText(IDC_FILE_LIST_WITH_EXT,      theApp.loadString(XPR_STRING_LITERAL("popup.file_list.check.with_extension")));
    SetDlgItemText(IDC_FILE_LIST_WITH_INFO,     theApp.loadString(XPR_STRING_LITERAL("popup.file_list.check.with_info")));
    SetDlgItemText(IDC_FILE_LIST_WITH_DIR,      theApp.loadString(XPR_STRING_LITERAL("popup.file_list.check.with_dir")));
    SetDlgItemText(IDC_FILE_LIST_SEPARATOR,     theApp.loadString(XPR_STRING_LITERAL("popup.file_list.check.separator")));

    // Load Dialog State
    mState.setSection(XPR_STRING_LITERAL("FileList"));
    mState.setDialog(this, XPR_TRUE);
    mState.setCheckBox(XPR_STRING_LITERAL("ByLine"),         IDC_FILE_LIST_BY_LINE);
    mState.setCheckBox(XPR_STRING_LITERAL("OnlyFileName"),   IDC_FILE_LIST_ONLY_FILENAME);
    mState.setCheckBox(XPR_STRING_LITERAL("WithExt"),        IDC_FILE_LIST_WITH_EXT);
    mState.setCheckBox(XPR_STRING_LITERAL("WithDir"),        IDC_FILE_LIST_WITH_DIR);
    mState.setCheckBox(XPR_STRING_LITERAL("WithInfo"),       IDC_FILE_LIST_WITH_INFO);
    mState.setCheckBox(XPR_STRING_LITERAL("Seperator"),      IDC_FILE_LIST_SEPARATOR);
    mState.setEditCtrl(XPR_STRING_LITERAL("Seperator Text"), IDC_FILE_LIST_SEPARATOR_TEXT);
    mState.load();

    return XPR_TRUE;
}

xpr_bool_t TextFileOutDlg::DestroyWindow(void)
{
    if (mTextFileOut != XPR_NULL)
        mTextFileOut->Stop();

    // Save Dialog State
    mState.reset();
    mState.save();

    return super::DestroyWindow();
}

void TextFileOutDlg::setTextFile(const xpr_tchar_t *aTextFile)
{
    if (aTextFile != XPR_NULL)
        mTextFile = aTextFile;
}

void TextFileOutDlg::addPath(const xpr_tchar_t *aPath)
{
    if (aPath != XPR_NULL)
        mPathDeque.push_back(aPath);
}

void TextFileOutDlg::OnPathBrowse(void) 
{
    xpr_tchar_t sFileType[0xff] = {0};
    _stprintf(sFileType, XPR_STRING_LITERAL("%s (*.txt)\0*.txt\0\0"), theApp.loadString(XPR_STRING_LITERAL("popup.file_list.file_dialog.filter.text_file")));

    CFileDialogST sFileDialog(XPR_FALSE, XPR_STRING_LITERAL("*.txt"), XPR_NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, sFileType, this);
    if (sFileDialog.DoModal() == IDOK)
        SetDlgItemText(IDC_PATH, sFileDialog.GetPathName());
}

void TextFileOutDlg::enableWindow(xpr_bool_t aEnable)
{
    xpr_uint_t sIds[] = {
        IDC_FILE_LIST_PATH,
        IDC_FILE_LIST_PATH_BROWSE,
        IDC_FILE_LIST_BY_LINE,
        IDC_FILE_LIST_ONLY_FILENAME,
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

void TextFileOutDlg::OnOK(void) 
{
    if (mTextFileOut != XPR_NULL)
    {
        if (mTextFileOut->getStatus() == fxb::TextFileOut::StatusCreating)
        {
            mTextFileOut->Stop();
            return;
        }
    }

    xpr_tchar_t sTextFile[XPR_MAX_PATH + 1] = {0};
    GetDlgItemText(IDC_FILE_LIST_PATH, sTextFile, XPR_MAX_PATH);

    xpr_bool_t sByLine    = ((CButton *)GetDlgItem(IDC_FILE_LIST_BY_LINE))->GetCheck();
    xpr_bool_t sOnlyFile  = ((CButton *)GetDlgItem(IDC_FILE_LIST_ONLY_FILENAME))->GetCheck();
    xpr_bool_t sFrontPath = ((CButton *)GetDlgItem(IDC_FILE_LIST_WITH_DIR))->GetCheck();
    xpr_bool_t sExtension = ((CButton *)GetDlgItem(IDC_FILE_LIST_WITH_EXT))->GetCheck();
    xpr_bool_t sAttribute = ((CButton *)GetDlgItem(IDC_FILE_LIST_WITH_INFO))->GetCheck();
    xpr_bool_t sSplitChar = ((CButton *)GetDlgItem(IDC_FILE_LIST_SEPARATOR))->GetCheck();

    xpr_tchar_t sText[MAX_DIV_TEXT + 1] = {0};
    xpr_tchar_t sDivText[MAX_DIV_TEXT + 1] = {0};
    GetDlgItemText(IDC_FILE_LIST_SEPARATOR_TEXT, sText, MAX_DIV_TEXT);
    fxb::ConvertStringToFormat(sText, sDivText);

    xpr_uint_t sFlags = 0;
    if (sByLine    == XPR_TRUE) sFlags |= fxb::TextFileOut::FlagsByLine;
    if (sOnlyFile  == XPR_TRUE) sFlags |= fxb::TextFileOut::FlagsOnlyFile;
    if (sFrontPath == XPR_TRUE) sFlags |= fxb::TextFileOut::FlagsWithDir;
    if (sExtension == XPR_TRUE) sFlags |= fxb::TextFileOut::FlagsExtension;
    if (sAttribute == XPR_TRUE) sFlags |= fxb::TextFileOut::FlagsAttribute;
    if (sSplitChar == XPR_TRUE) sFlags |= fxb::TextFileOut::FlagsSplitChar;

    XPR_SAFE_DELETE(mTextFileOut);

    mTextFileOut = new fxb::TextFileOut;
    mTextFileOut->setOwner(m_hWnd, WM_FINALIZE);

    mTextFileOut->setTextFile(sTextFile);
    mTextFileOut->setFlags(sFlags);
    mTextFileOut->setSplitChar(sDivText);

    PathDeque::iterator sIterator = mPathDeque.begin();
    for (; sIterator != mPathDeque.end(); ++sIterator)
        mTextFileOut->addPath(sIterator->c_str());

    if (mTextFileOut->Start())
    {
        enableWindow(XPR_FALSE);
    }
}

LRESULT TextFileOutDlg::OnFinalize(WPARAM wParam, LPARAM lParam)
{
    mTextFileOut->Stop();

    fxb::TextFileOut::Status sStatus;
    sStatus = mTextFileOut->getStatus();

    switch (sStatus)
    {
    case fxb::TextFileOut::StatusFailed:
        {
            const xpr_tchar_t *sMsg = theApp.loadString(XPR_STRING_LITERAL("popup.file_list.msg.not_created"));
            MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);
            break;
        }

    case fxb::TextFileOut::StatusCreateCompleted:
        {
            super::OnOK();
            break;
        }
    }

    return 0;
}
