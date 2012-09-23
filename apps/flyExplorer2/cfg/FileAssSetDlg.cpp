//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "FileAssSetDlg.h"

#include "fxb/fxb_filter.h"

#include "rgc/FileDialogST.h"

#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

FileAssSetDlg::FileAssSetDlg(fxb::FileAssItem *aFileAssItem)
    : super(IDD_FILE_ASS_SET, XPR_NULL)
    , mByExt(-1)
{
    if (XPR_IS_NOT_NULL(aFileAssItem))
        mFileAssItem = *aFileAssItem;
}

FileAssSetDlg::~FileAssSetDlg(void)
{
    mNameSet.clear();
}

void FileAssSetDlg::DoDataExchange(CDataExchange* pDX)
{
    super::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_FILE_ASS_SET_FILTER, mFilterComboBox);
}

BEGIN_MESSAGE_MAP(FileAssSetDlg, super)
    ON_BN_CLICKED(IDC_FILE_ASS_SET_METHOD_EXT,     OnAssType)
    ON_BN_CLICKED(IDC_FILE_ASS_SET_METHOD_FILTER,  OnAssType)
    ON_BN_CLICKED(IDC_FILE_ASS_SET_PROGRAM_BROWSE, OnProgramBrowse)
    ON_BN_CLICKED(IDOK,                            OnOK)
    ON_CBN_SELCHANGE(IDC_FILE_ASS_SET_FILTER, OnCbnSelchangeFilter)
END_MESSAGE_MAP()

xpr_bool_t FileAssSetDlg::OnInitDialog(void) 
{
    super::OnInitDialog();

    ((CEdit *)GetDlgItem(IDC_FILE_ASS_SET_NAME))->LimitText(MAX_FILE_ASS_NAME);
    ((CEdit *)GetDlgItem(IDC_FILE_ASS_SET_EXTS))->LimitText(MAX_FILE_ASS_EXTS);
    ((CEdit *)GetDlgItem(IDC_FILE_ASS_SET_PROGRAM))->LimitText(XPR_MAX_PATH);

    fxb::FilterMgr &sFilterMgr = fxb::FilterMgr::instance();

    xpr_sint_t i;
    xpr_sint_t sIndex;
    xpr_sint_t sCount;
    fxb::FilterItem *sFilterItem;

    sCount = sFilterMgr.getCount();
    for (i = 0; i < sCount; ++i)
    {
        sFilterItem = sFilterMgr.getFilter(i);
        if (XPR_IS_NULL(sFilterItem))
            continue;

        if (_tcsicmp(sFilterItem->mExts.c_str(), XPR_STRING_LITERAL("*")) == 0)
            continue;

        sIndex = mFilterComboBox.AddString(sFilterItem->mName.c_str());
        mFilterComboBox.SetItemData(sIndex, (DWORD_PTR)sFilterItem);
    }

    SetDlgItemText(IDC_FILE_ASS_SET_NAME, mFileAssItem.mName.c_str());

    if (mFileAssItem.mMethod == fxb::FileAssMethodFilter)
        ((CButton *)GetDlgItem(IDC_FILE_ASS_SET_METHOD_FILTER))->SetCheck(XPR_TRUE);
    else
        ((CButton *)GetDlgItem(IDC_FILE_ASS_SET_METHOD_EXT))->SetCheck(XPR_TRUE);

    ((CButton *)GetDlgItem(IDC_FILE_ASS_SET_TYPE_VIEWER))->SetCheck(XPR_TEST_BITS(mFileAssItem.mType, fxb::FileAssTypeViewer));
    ((CButton *)GetDlgItem(IDC_FILE_ASS_SET_TYPE_EDITOR))->SetCheck(XPR_TEST_BITS(mFileAssItem.mType, fxb::FileAssTypeEditor));

    if (mFileAssItem.mMethod == fxb::FileAssMethodFilter)
    {
        CString sFilterName;

        sCount = mFilterComboBox.GetCount();
        for (i = 0; i < sCount; ++i)
        {
            mFilterComboBox.GetLBText(i, sFilterName);
            if (_tcsicmp(mFileAssItem.mFilterName.c_str(), sFilterName) == 0)
            {
                sFilterItem = (fxb::FilterItem *)mFilterComboBox.GetItemData(i);
                if (XPR_IS_NOT_NULL(sFilterItem))
                    SetDlgItemText(IDC_FILE_ASS_SET_EXTS, sFilterItem->mExts.c_str());

                mFilterComboBox.SetCurSel(i);
                break;
            }
        }
    }
    else
    {
        SetDlgItemText(IDC_FILE_ASS_SET_EXTS, mFileAssItem.mExts.c_str());
    }

    SetDlgItemText(IDC_FILE_ASS_SET_PROGRAM, mFileAssItem.mPath.c_str());

    OnAssType();

    SetWindowText(theApp.loadString(XPR_STRING_LITERAL("popup.file_ass_set.title")));
    SetDlgItemText(IDC_FILE_ASS_SET_LABEL_NAME,    theApp.loadString(XPR_STRING_LITERAL("popup.file_ass_set.label.name")));
    SetDlgItemText(IDC_FILE_ASS_SET_GROUP_METHOD,  theApp.loadString(XPR_STRING_LITERAL("popup.file_ass_set.group.method")));
    SetDlgItemText(IDC_FILE_ASS_SET_METHOD_EXT,    theApp.loadString(XPR_STRING_LITERAL("popup.file_ass_set.radio.method_extension")));
    SetDlgItemText(IDC_FILE_ASS_SET_METHOD_FILTER, theApp.loadString(XPR_STRING_LITERAL("popup.file_ass_set.radio.method_filter")));
    SetDlgItemText(IDC_FILE_ASS_SET_GROUP_TYPE,    theApp.loadString(XPR_STRING_LITERAL("popup.file_ass_set.group.type")));
    SetDlgItemText(IDC_FILE_ASS_SET_TYPE_VIEWER,   theApp.loadString(XPR_STRING_LITERAL("popup.file_ass_set.check.type_viewer")));
    SetDlgItemText(IDC_FILE_ASS_SET_TYPE_EDITOR,   theApp.loadString(XPR_STRING_LITERAL("popup.file_ass_set.check.type_editor")));
    SetDlgItemText(IDC_FILE_ASS_SET_LABEL_FILTER,  theApp.loadString(XPR_STRING_LITERAL("popup.file_ass_set.label.filter")));
    SetDlgItemText(IDC_FILE_ASS_SET_LABEL_EXTS,    theApp.loadString(XPR_STRING_LITERAL("popup.file_ass_set.label.extensions")));
    SetDlgItemText(IDC_FILE_ASS_SET_LABEL_PROGRAM, theApp.loadString(XPR_STRING_LITERAL("popup.file_ass_set.label.program")));
    SetDlgItemText(IDOK,                           theApp.loadString(XPR_STRING_LITERAL("popup.common.button.ok")));
    SetDlgItemText(IDCANCEL,                       theApp.loadString(XPR_STRING_LITERAL("popup.common.button.cancel")));

    return XPR_TRUE;
}

void FileAssSetDlg::OnOK(void)
{
    // name
    xpr_tchar_t sName[MAX_FILE_ASS_NAME + 1] = {0};
    GetDlgItemText(IDC_FILE_ASS_SET_NAME, sName, MAX_FILE_ASS_NAME);

    // association method
    xpr_uint_t sMethod = fxb::FileAssMethodExt;
    if (((CButton *)GetDlgItem(IDC_FILE_ASS_SET_METHOD_FILTER))->GetCheck())
        sMethod = fxb::FileAssMethodFilter;

    CString sExts;
    CString sFilterName;
    if (mFileAssItem.mMethod == fxb::FileAssMethodFilter)
    {
        xpr_sint_t sIndex = mFilterComboBox.GetCurSel();
        if (sIndex != CB_ERR)
            mFilterComboBox.GetLBText(sIndex, sFilterName);
    }
    else
    {
        GetDlgItemText(IDC_FILE_ASS_SET_EXTS, sExts);
    }

    // assoication type
    xpr_uint_t sType = fxb::FileAssTypeNone;
    if (((CButton *)GetDlgItem(IDC_FILE_ASS_SET_TYPE_VIEWER))->GetCheck()) sType |= fxb::FileAssTypeViewer;
    if (((CButton *)GetDlgItem(IDC_FILE_ASS_SET_TYPE_EDITOR))->GetCheck()) sType |= fxb::FileAssTypeEditor;

    // path
    xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
    GetDlgItemText(IDC_FILE_ASS_SET_PROGRAM, sPath, XPR_MAX_PATH);

    if (_tcslen(sName) <= 0)
    {
        const xpr_tchar_t *sMsg = theApp.loadString(XPR_STRING_LITERAL("popup.file_ass_set.msg.input_name"));
        MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);

        GetDlgItem(IDC_FILE_ASS_SET_NAME)->SetFocus();
        return;
    }

    NameSet::iterator sIterator = mNameSet.find(sName);
    if (sIterator != mNameSet.end())
    {
        const xpr_tchar_t *sMsg = theApp.loadString(XPR_STRING_LITERAL("popup.file_ass_set.msg.duplicated_name"));
        MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);

        GetDlgItem(IDC_FILE_ASS_SET_NAME)->SetFocus();
        return;
    }

    if (sType == fxb::FileAssTypeNone)
    {
        const xpr_tchar_t *sMsg = theApp.loadString(XPR_STRING_LITERAL("popup.file_ass_set.msg.select_type"));
        MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);

        GetDlgItem(IDC_FILE_ASS_SET_TYPE_VIEWER)->SetFocus();
        return;
    }

    if (sMethod == fxb::FileAssMethodExt)
    {
        if (sExts.GetLength() <= 0)
        {
            const xpr_tchar_t *sMsg = theApp.loadString(XPR_STRING_LITERAL("popup.file_ass_set.msg.input_extension"));
            MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);

            GetDlgItem(IDC_FILE_ASS_SET_EXTS)->SetFocus();
            return;
        }
    }

    if (sMethod == fxb::FileAssMethodFilter)
    {
        if (sFilterName.GetLength() <= 0)
        {
            const xpr_tchar_t *sMsg = theApp.loadString(XPR_STRING_LITERAL("popup.file_ass_set.msg.select_filter"));
            MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);

            GetDlgItem(IDC_FILE_ASS_SET_FILTER)->SetFocus();
            return;
        }
    }

    if (_tcslen(sPath) <= 0)
    {
        const xpr_tchar_t *sMsg = theApp.loadString(XPR_STRING_LITERAL("popup.file_ass_set.msg.input_program"));
        MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);

        GetDlgItem(IDC_FILE_ASS_SET_PROGRAM)->SetFocus();
        return;
    }

    mFileAssItem.mName       = sName;
    mFileAssItem.mMethod     = sMethod;
    mFileAssItem.mExts       = sExts;
    mFileAssItem.mFilterName = sFilterName;
    mFileAssItem.mType       = sType;
    mFileAssItem.mPath       = sPath;

    super::OnOK();
}

fxb::FileAssItem *FileAssSetDlg::getFileAssItem(void)
{
    return &mFileAssItem;
}

void FileAssSetDlg::setFileAssItem(fxb::FileAssItem *aFileAssItem)
{
    if (XPR_IS_NOT_NULL(aFileAssItem))
        mFileAssItem = *aFileAssItem;
}

void FileAssSetDlg::addName(const xpr_tchar_t *aName)
{
    if (XPR_IS_NOT_NULL(aName))
        mNameSet.insert(aName);
}

void FileAssSetDlg::OnAssType(void)
{
    xpr_bool_t sByExt = ((CButton *)GetDlgItem(IDC_FILE_ASS_SET_METHOD_EXT))->GetCheck();

    mFilterComboBox.EnableWindow(!sByExt);
    GetDlgItem(IDC_FILE_ASS_SET_EXTS)->EnableWindow(sByExt);

    if (XPR_IS_FALSE(sByExt))
        GetDlgItemText(IDC_FILE_ASS_SET_EXTS, mExts, MAX_FILTER_EXTS);

    if (mByExt != -1 && mByExt != sByExt)
    {
        if (XPR_IS_TRUE(sByExt))
            SetDlgItemText(IDC_FILE_ASS_SET_EXTS, mExts);
    }

    mByExt = sByExt;

    if (XPR_IS_FALSE(sByExt))
        OnCbnSelchangeFilter();
}

void FileAssSetDlg::OnCbnSelchangeFilter(void)
{
    xpr_sint_t sIndex = mFilterComboBox.GetCurSel();
    if (sIndex == CB_ERR)
        return;

    CString sFilterName;
    mFilterComboBox.GetLBText(sIndex, sFilterName);

    fxb::FilterItem *sFilterItem;
    sFilterItem = fxb::FilterMgr::instance().getFilterFromName(sFilterName);

    if (XPR_IS_NULL(sFilterItem))
        return;

    SetDlgItemText(IDC_FILE_ASS_SET_EXTS, sFilterItem->mExts.c_str());
}

void FileAssSetDlg::OnProgramBrowse(void)
{
    xpr_tchar_t sFilter[0xff] = {0};
    _stprintf(sFilter, XPR_STRING_LITERAL("%s (*.exe)\0*.exe\0\0"), theApp.loadString(XPR_STRING_LITERAL("popup.common.file_dialog.filter.program")));
    CFileDialogST sFileDialog(XPR_TRUE, XPR_STRING_LITERAL("*.exe"), XPR_NULL, OFN_HIDEREADONLY, sFilter, this);
    if (sFileDialog.DoModal() != IDOK)
        return;

    SetDlgItemText(IDC_FILE_ASS_SET_PROGRAM, sFileDialog.GetPathName());
}
