//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "ParamExecDlg.h"

#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

ParamExecDlg::ParamExecDlg(void)
    : super(IDD_PARAM_EXEC, XPR_NULL)
{
    mPath[0] = 0;
}

void ParamExecDlg::DoDataExchange(CDataExchange* pDX)
{
    super::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_PARAM_EXEC_PARAMETER, mComboBox);
}

BEGIN_MESSAGE_MAP(ParamExecDlg, super)
END_MESSAGE_MAP()

void ParamExecDlg::setPath(const xpr_tchar_t *aPath)
{
    _tcscpy(mPath, aPath);
}

const xpr_tchar_t *ParamExecDlg::getParameter(void)
{
    return mParameter.c_str();
}

xpr_bool_t ParamExecDlg::OnInitDialog(void) 
{
    super::OnInitDialog();

    mState.setDialog(this);
    mState.setSection(XPR_STRING_LITERAL("ParamExec"));
    mState.setComboBoxList(XPR_STRING_LITERAL("Param"), mComboBox.GetDlgCtrlID());
    mState.load();

    if (mComboBox.GetCount() > 0)
        mComboBox.SetCurSel(0);

    SetDlgItemText(IDC_PARAM_EXEC_PATH, mPath);
    ((CEdit *)GetDlgItem(IDC_PARAM_EXEC_PATH))->LimitText(XPR_MAX_PARAM);

    SetWindowText(theApp.loadString(XPR_STRING_LITERAL("popup.param_exec.title")));
    SetDlgItemText(IDC_PARAM_EXEC_LABEL_PATH,      theApp.loadString(XPR_STRING_LITERAL("popup.param_exec.label.path")));
    SetDlgItemText(IDC_PARAM_EXEC_LABEL_INPUT,     theApp.loadString(XPR_STRING_LITERAL("popup.param_exec.label.input")));
    SetDlgItemText(IDC_PARAM_EXEC_LABEL_PARAMETER, theApp.loadString(XPR_STRING_LITERAL("popup.param_exec.label.parameter")));
    SetDlgItemText(IDOK,                           theApp.loadString(XPR_STRING_LITERAL("popup.common.button.ok")));
    SetDlgItemText(IDCANCEL,                       theApp.loadString(XPR_STRING_LITERAL("popup.common.button.cancel")));

    return XPR_TRUE;
}

void ParamExecDlg::OnOK(void) 
{
    xpr_tchar_t sParameter[XPR_MAX_PARAM + 1] = {0};
    mComboBox.GetWindowText(sParameter, XPR_MAX_PARAM);

    mParameter = sParameter;

    // Save Command
    DlgState::insertComboEditString(&mComboBox, XPR_FALSE);

    mState.reset();
    mState.save();

    super::OnOK();
}
