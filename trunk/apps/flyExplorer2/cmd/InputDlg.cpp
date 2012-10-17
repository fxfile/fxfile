//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "InputDlg.h"

#include "resource.h"
#include "DlgState.h"
#include "DlgStateMgr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

InputDlg::InputDlg(xpr_sint_t aDlgWidth)
    : super(IDD_INPUT, XPR_NULL)
    , mDlgWidth(aDlgWidth)
    , mFileRename(XPR_FALSE)
    , mCheckEmpty(XPR_FALSE)
    , mLimitText(XPR_MAX_PATH)
    , mDlgState(XPR_NULL)
{
    mIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
    mText[0] = 0;
}

InputDlg::~InputDlg(void)
{
    DESTROY_ICON(mIcon);
}

void InputDlg::DoDataExchange(CDataExchange* pDX)
{
    super::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(InputDlg, super)
    ON_WM_DESTROY()
END_MESSAGE_MAP()

void InputDlg::setProfile(const xpr_tchar_t *aProfile)
{
    if (aProfile == XPR_NULL)
        return;

    mProfile = aProfile;
}

void InputDlg::setTitle(const xpr_tchar_t *aTitle)
{
    if (aTitle == XPR_NULL)
        return;

    mTitle = aTitle;
}

void InputDlg::setDesc(const xpr_tchar_t *aDesc)
{
    if (aDesc == XPR_NULL)
        return;

    mDesc = aDesc;
}

void InputDlg::setText(const xpr_tchar_t *aText)
{
    if (aText == XPR_NULL)
        return;

    _tcscpy(mText, aText);
}

void InputDlg::setFileRename(xpr_bool_t aFileRename)
{
    mFileRename = aFileRename;
}

void InputDlg::setCheckEmpty(xpr_bool_t aCheckEmpty)
{
    mCheckEmpty = aCheckEmpty;
}

void InputDlg::setLimitText(xpr_sint_t aLimitText)
{
    if (aLimitText > 0)
        mLimitText = aLimitText;
}

const xpr_tchar_t *InputDlg::getText(void)
{
    return mText;
}

xpr_bool_t InputDlg::OnInitDialog(void) 
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
    AddControl(IDC_INPUT_LABEL_DESC, sizeResize, sizeNone);
    AddControl(IDC_INPUT_EDIT,       sizeResize, sizeNone);
    AddControl(IDOK,                 sizeRepos,  sizeRepos, XPR_FALSE);
    AddControl(IDCANCEL,             sizeRepos,  sizeRepos, XPR_FALSE);
    //------------------------------------------------------------

    if (mDlgWidth != -1)
    {
        CRect sRect;
        GetWindowRect(&sRect);

        sRect.right = sRect.left + mDlgWidth;

        MoveWindow(sRect);
        CenterWindow();
    }

    ((CEdit *)GetDlgItem(IDC_INPUT_EDIT))->LimitText(mLimitText);

    // TODO: Add extra initialization here
    if (mDesc.empty())
        mDesc = theApp.loadString(XPR_STRING_LITERAL("popup.input.label.desc"));

    SetWindowText(mTitle.c_str());
    SetDlgItemText(IDC_INPUT_LABEL_DESC, mDesc.c_str());
    SetDlgItemText(IDC_INPUT_EDIT, mText);

    // Load Dialog State
    if (mProfile.empty() == XPR_FALSE)
    {
        mDlgState = DlgStateMgr::instance().getDlgState(mProfile.c_str());
        if (XPR_IS_NOT_NULL(mDlgState))
        {
            mDlgState->setDialog(this, XPR_TRUE);
            mDlgState->load();
        }
    }

    SetDlgItemText(IDOK,     theApp.loadString(XPR_STRING_LITERAL("popup.common.button.ok")));
    SetDlgItemText(IDCANCEL, theApp.loadString(XPR_STRING_LITERAL("popup.common.button.cancel")));

    return XPR_TRUE;
}

void InputDlg::OnOK(void) 
{
    GetDlgItemText(IDC_INPUT_EDIT, mText, XPR_MAX_PATH);

    if (mFileRename == XPR_TRUE)
    {
        if (fxb::VerifyFileName(mText) == XPR_FALSE)
        {
            const xpr_tchar_t *sMsg = theApp.loadString(XPR_STRING_LITERAL("popup.input.msg.wrong_filename"));
            MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);
            return;
        }
    }

    if (mCheckEmpty == XPR_TRUE)
    {
        if (_tcslen(mText) <= 0)
        {
            const xpr_tchar_t *sMsg = theApp.loadString(XPR_STRING_LITERAL("popup.input.msg.empty_re-input"));
            MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONWARNING);
            return;
        }
    }

    super::OnOK();
}

void InputDlg::OnDestroy(void) 
{
    super::OnDestroy();

    DESTROY_ICON(mIcon);

    // Save Dialog State
    if (mProfile.empty() == XPR_FALSE)
    {
        if (XPR_IS_NOT_NULL(mDlgState))
        {
            mDlgState->reset();
            mDlgState->save();
        }
    }
}
