//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "program_ass_set_dlg.h"

#include "../filter.h"
#include "../resource.h"

#include "gui/FileDialogST.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace fxfile
{
namespace cfg
{
ProgramAssSetDlg::ProgramAssSetDlg(ProgramAssItem *aProgramAssItem)
    : super(IDD_PROGRAM_ASS_SET, XPR_NULL)
    , mByExt(-1)
{
    if (XPR_IS_NOT_NULL(aProgramAssItem))
        mProgramAssItem = *aProgramAssItem;
}

ProgramAssSetDlg::~ProgramAssSetDlg(void)
{
    mNameSet.clear();
}

void ProgramAssSetDlg::DoDataExchange(CDataExchange* pDX)
{
    super::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_PROGRAM_ASS_SET_FILTER, mFilterComboBox);
}

BEGIN_MESSAGE_MAP(ProgramAssSetDlg, super)
    ON_BN_CLICKED(IDC_PROGRAM_ASS_SET_METHOD_EXT,     OnAssType)
    ON_BN_CLICKED(IDC_PROGRAM_ASS_SET_METHOD_FILTER,  OnAssType)
    ON_BN_CLICKED(IDC_PROGRAM_ASS_SET_PROGRAM_BROWSE, OnProgramBrowse)
    ON_BN_CLICKED(IDOK,                            OnOK)
    ON_CBN_SELCHANGE(IDC_PROGRAM_ASS_SET_FILTER, OnCbnSelchangeFilter)
END_MESSAGE_MAP()

xpr_bool_t ProgramAssSetDlg::OnInitDialog(void) 
{
    super::OnInitDialog();

    ((CEdit *)GetDlgItem(IDC_PROGRAM_ASS_SET_NAME))->LimitText(MAX_PROGRAM_ASS_NAME);
    ((CEdit *)GetDlgItem(IDC_PROGRAM_ASS_SET_EXTS))->LimitText(MAX_PROGRAM_ASS_EXTS);
    ((CEdit *)GetDlgItem(IDC_PROGRAM_ASS_SET_PROGRAM))->LimitText(XPR_MAX_PATH);

    FilterMgr &sFilterMgr = FilterMgr::instance();

    xpr_sint_t i;
    xpr_sint_t sIndex;
    xpr_sint_t sCount;
    FilterItem *sFilterItem;

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

    SetDlgItemText(IDC_PROGRAM_ASS_SET_NAME, mProgramAssItem.mName.c_str());

    if (mProgramAssItem.mMethod == ProgramAssMethodFilter)
        ((CButton *)GetDlgItem(IDC_PROGRAM_ASS_SET_METHOD_FILTER))->SetCheck(XPR_TRUE);
    else
        ((CButton *)GetDlgItem(IDC_PROGRAM_ASS_SET_METHOD_EXT))->SetCheck(XPR_TRUE);

    ((CButton *)GetDlgItem(IDC_PROGRAM_ASS_SET_TYPE_VIEWER))->SetCheck(XPR_TEST_BITS(mProgramAssItem.mType, ProgramAssTypeViewer));
    ((CButton *)GetDlgItem(IDC_PROGRAM_ASS_SET_TYPE_EDITOR))->SetCheck(XPR_TEST_BITS(mProgramAssItem.mType, ProgramAssTypeEditor));

    if (mProgramAssItem.mMethod == ProgramAssMethodFilter)
    {
        CString sFilterName;

        sCount = mFilterComboBox.GetCount();
        for (i = 0; i < sCount; ++i)
        {
            mFilterComboBox.GetLBText(i, sFilterName);
            if (_tcsicmp(mProgramAssItem.mFilterName.c_str(), sFilterName) == 0)
            {
                sFilterItem = (FilterItem *)mFilterComboBox.GetItemData(i);
                if (XPR_IS_NOT_NULL(sFilterItem))
                    SetDlgItemText(IDC_PROGRAM_ASS_SET_EXTS, sFilterItem->mExts.c_str());

                mFilterComboBox.SetCurSel(i);
                break;
            }
        }
    }
    else
    {
        SetDlgItemText(IDC_PROGRAM_ASS_SET_EXTS, mProgramAssItem.mExts.c_str());
    }

    SetDlgItemText(IDC_PROGRAM_ASS_SET_PROGRAM, mProgramAssItem.mPath.c_str());

    OnAssType();

    SetWindowText(gApp.loadString(XPR_STRING_LITERAL("popup.program_ass_set.title")));
    SetDlgItemText(IDC_PROGRAM_ASS_SET_LABEL_NAME,    gApp.loadString(XPR_STRING_LITERAL("popup.program_ass_set.label.name")));
    SetDlgItemText(IDC_PROGRAM_ASS_SET_GROUP_METHOD,  gApp.loadString(XPR_STRING_LITERAL("popup.program_ass_set.group.method")));
    SetDlgItemText(IDC_PROGRAM_ASS_SET_METHOD_EXT,    gApp.loadString(XPR_STRING_LITERAL("popup.program_ass_set.radio.method_extension")));
    SetDlgItemText(IDC_PROGRAM_ASS_SET_METHOD_FILTER, gApp.loadString(XPR_STRING_LITERAL("popup.program_ass_set.radio.method_filter")));
    SetDlgItemText(IDC_PROGRAM_ASS_SET_GROUP_TYPE,    gApp.loadString(XPR_STRING_LITERAL("popup.program_ass_set.group.type")));
    SetDlgItemText(IDC_PROGRAM_ASS_SET_TYPE_VIEWER,   gApp.loadString(XPR_STRING_LITERAL("popup.program_ass_set.check.type_viewer")));
    SetDlgItemText(IDC_PROGRAM_ASS_SET_TYPE_EDITOR,   gApp.loadString(XPR_STRING_LITERAL("popup.program_ass_set.check.type_editor")));
    SetDlgItemText(IDC_PROGRAM_ASS_SET_LABEL_FILTER,  gApp.loadString(XPR_STRING_LITERAL("popup.program_ass_set.label.filter")));
    SetDlgItemText(IDC_PROGRAM_ASS_SET_LABEL_EXTS,    gApp.loadString(XPR_STRING_LITERAL("popup.program_ass_set.label.extensions")));
    SetDlgItemText(IDC_PROGRAM_ASS_SET_LABEL_PROGRAM, gApp.loadString(XPR_STRING_LITERAL("popup.program_ass_set.label.program")));
    SetDlgItemText(IDOK,                              gApp.loadString(XPR_STRING_LITERAL("popup.common.button.ok")));
    SetDlgItemText(IDCANCEL,                          gApp.loadString(XPR_STRING_LITERAL("popup.common.button.cancel")));

    return XPR_TRUE;
}

void ProgramAssSetDlg::OnOK(void)
{
    // name
    xpr_tchar_t sName[MAX_PROGRAM_ASS_NAME + 1] = {0};
    GetDlgItemText(IDC_PROGRAM_ASS_SET_NAME, sName, MAX_PROGRAM_ASS_NAME);

    // association method
    xpr_uint_t sMethod = ProgramAssMethodExt;
    if (((CButton *)GetDlgItem(IDC_PROGRAM_ASS_SET_METHOD_FILTER))->GetCheck())
        sMethod = ProgramAssMethodFilter;

    CString sExts;
    CString sFilterName;
    if (mProgramAssItem.mMethod == ProgramAssMethodFilter)
    {
        xpr_sint_t sIndex = mFilterComboBox.GetCurSel();
        if (sIndex != CB_ERR)
            mFilterComboBox.GetLBText(sIndex, sFilterName);
    }
    else
    {
        GetDlgItemText(IDC_PROGRAM_ASS_SET_EXTS, sExts);
    }

    // assoication type
    xpr_uint_t sType = ProgramAssTypeNone;
    if (((CButton *)GetDlgItem(IDC_PROGRAM_ASS_SET_TYPE_VIEWER))->GetCheck()) sType |= ProgramAssTypeViewer;
    if (((CButton *)GetDlgItem(IDC_PROGRAM_ASS_SET_TYPE_EDITOR))->GetCheck()) sType |= ProgramAssTypeEditor;

    // path
    xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
    GetDlgItemText(IDC_PROGRAM_ASS_SET_PROGRAM, sPath, XPR_MAX_PATH);

    if (_tcslen(sName) <= 0)
    {
        const xpr_tchar_t *sMsg = gApp.loadString(XPR_STRING_LITERAL("popup.program_ass_set.msg.input_name"));
        MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);

        GetDlgItem(IDC_PROGRAM_ASS_SET_NAME)->SetFocus();
        return;
    }

    NameSet::iterator sIterator = mNameSet.find(sName);
    if (sIterator != mNameSet.end())
    {
        const xpr_tchar_t *sMsg = gApp.loadString(XPR_STRING_LITERAL("popup.program_ass_set.msg.duplicated_name"));
        MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);

        GetDlgItem(IDC_PROGRAM_ASS_SET_NAME)->SetFocus();
        return;
    }

    if (sType == ProgramAssTypeNone)
    {
        const xpr_tchar_t *sMsg = gApp.loadString(XPR_STRING_LITERAL("popup.program_ass_set.msg.select_type"));
        MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);

        GetDlgItem(IDC_PROGRAM_ASS_SET_TYPE_VIEWER)->SetFocus();
        return;
    }

    if (sMethod == ProgramAssMethodExt)
    {
        if (sExts.GetLength() <= 0)
        {
            const xpr_tchar_t *sMsg = gApp.loadString(XPR_STRING_LITERAL("popup.program_ass_set.msg.input_extension"));
            MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);

            GetDlgItem(IDC_PROGRAM_ASS_SET_EXTS)->SetFocus();
            return;
        }
    }

    if (sMethod == ProgramAssMethodFilter)
    {
        if (sFilterName.GetLength() <= 0)
        {
            const xpr_tchar_t *sMsg = gApp.loadString(XPR_STRING_LITERAL("popup.program_ass_set.msg.select_filter"));
            MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);

            GetDlgItem(IDC_PROGRAM_ASS_SET_FILTER)->SetFocus();
            return;
        }
    }

    if (_tcslen(sPath) <= 0)
    {
        const xpr_tchar_t *sMsg = gApp.loadString(XPR_STRING_LITERAL("popup.program_ass_set.msg.input_program"));
        MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);

        GetDlgItem(IDC_PROGRAM_ASS_SET_PROGRAM)->SetFocus();
        return;
    }

    mProgramAssItem.mName       = sName;
    mProgramAssItem.mMethod     = sMethod;
    mProgramAssItem.mExts       = sExts;
    mProgramAssItem.mFilterName = sFilterName;
    mProgramAssItem.mType       = sType;
    mProgramAssItem.mPath       = sPath;

    super::OnOK();
}

ProgramAssItem *ProgramAssSetDlg::getProgramAssItem(void)
{
    return &mProgramAssItem;
}

void ProgramAssSetDlg::setProgramAssItem(ProgramAssItem *aProgramAssItem)
{
    if (XPR_IS_NOT_NULL(aProgramAssItem))
        mProgramAssItem = *aProgramAssItem;
}

void ProgramAssSetDlg::addName(const xpr_tchar_t *aName)
{
    if (XPR_IS_NOT_NULL(aName))
        mNameSet.insert(aName);
}

void ProgramAssSetDlg::OnAssType(void)
{
    xpr_bool_t sByExt = ((CButton *)GetDlgItem(IDC_PROGRAM_ASS_SET_METHOD_EXT))->GetCheck();

    mFilterComboBox.EnableWindow(!sByExt);
    GetDlgItem(IDC_PROGRAM_ASS_SET_EXTS)->EnableWindow(sByExt);

    if (XPR_IS_FALSE(sByExt))
        GetDlgItemText(IDC_PROGRAM_ASS_SET_EXTS, mExts, MAX_FILTER_EXTS);

    if (mByExt != -1 && mByExt != sByExt)
    {
        if (XPR_IS_TRUE(sByExt))
            SetDlgItemText(IDC_PROGRAM_ASS_SET_EXTS, mExts);
    }

    mByExt = sByExt;

    if (XPR_IS_FALSE(sByExt))
        OnCbnSelchangeFilter();
}

void ProgramAssSetDlg::OnCbnSelchangeFilter(void)
{
    xpr_sint_t sIndex = mFilterComboBox.GetCurSel();
    if (sIndex == CB_ERR)
        return;

    CString sFilterName;
    mFilterComboBox.GetLBText(sIndex, sFilterName);

    FilterItem *sFilterItem;
    sFilterItem = FilterMgr::instance().getFilterFromName(sFilterName);

    if (XPR_IS_NULL(sFilterItem))
        return;

    SetDlgItemText(IDC_PROGRAM_ASS_SET_EXTS, sFilterItem->mExts.c_str());
}

void ProgramAssSetDlg::OnProgramBrowse(void)
{
    xpr_tchar_t sFilter[0xff] = {0};
    _stprintf(sFilter, XPR_STRING_LITERAL("%s (*.exe)\0*.exe\0\0"), gApp.loadString(XPR_STRING_LITERAL("popup.common.file_dialog.filter.program")));
    CFileDialogST sFileDialog(XPR_TRUE, XPR_STRING_LITERAL("*.exe"), XPR_NULL, OFN_HIDEREADONLY, sFilter, this);
    if (sFileDialog.DoModal() != IDOK)
        return;

    SetDlgItemText(IDC_PROGRAM_ASS_SET_PROGRAM, sFileDialog.GetPathName());
}
} // namespace cfg
} // namespace fxfile
