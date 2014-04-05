//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "batch_rename_edit_dlg.h"

#include "winapi_ex.h"
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
BatchRenameEditDlg::BatchRenameEditDlg(void)
    : super(IDD_BATCH_RENAME_EDIT, XPR_NULL)
    , mDlgState(XPR_NULL)
{
    mIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

    mSyncScroll = XPR_FALSE;

    mOldEditScroll = XPR_FALSE;
    mNewEditScroll = XPR_FALSE;

    mPrevOldVert = 0;
    mPrevNewVert = 0;
}

BatchRenameEditDlg::~BatchRenameEditDlg(void)
{
    xpr_tchar_t *sFilename;
    std::deque<xpr_tchar_t *>::iterator sIterator;

    sIterator = mOldList.begin();
    for (; sIterator != mOldList.end(); ++sIterator)
    {
        sFilename = *sIterator;
        XPR_SAFE_DELETE_ARRAY(sFilename);
    }

    sIterator = mNewList.begin();
    for (; sIterator != mNewList.end(); ++sIterator)
    {
        sFilename = *sIterator;
        XPR_SAFE_DELETE_ARRAY(sFilename);
    }

    sIterator = mEditedList.begin();
    for (; sIterator != mEditedList.end(); ++sIterator)
    {
        sFilename = *sIterator;
        XPR_SAFE_DELETE_ARRAY(sFilename);
    }

    mOldList.clear();
    mNewList.clear();
    mEditedList.clear();

    DESTROY_ICON(mIcon);
}

void BatchRenameEditDlg::DoDataExchange(CDataExchange* pDX)
{
    super::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_RENAME_EDIT_NEW_NAME_EDIT, mNewEditCtrl);
    DDX_Control(pDX, IDC_RENAME_EDIT_OLD_NAME_EDIT, mOldEditCtrl);
}

BEGIN_MESSAGE_MAP(BatchRenameEditDlg, super)
    ON_WM_SIZE()
    ON_WM_DESTROY()
    ON_EN_VSCROLL(IDC_RENAME_EDIT_OLD_NAME_EDIT, OnVscrollSrnOldEdit)
    ON_EN_VSCROLL(IDC_RENAME_EDIT_NEW_NAME_EDIT, OnVscrollSrnNewEdit)
    ON_BN_CLICKED(IDC_RENAME_EDIT_SYNC_SCROLL, OnSyncScroll)
END_MESSAGE_MAP()

void BatchRenameEditDlg::add(const xpr_tchar_t *aOldFilename, const xpr_tchar_t *aNewFilenmae)
{
    xpr_tchar_t *sFilename;

    sFilename = new xpr_tchar_t[_tcslen(aOldFilename)+1];
    _tcscpy(sFilename, aOldFilename);
    mOldList.push_back(sFilename);

    sFilename = new xpr_tchar_t[_tcslen(aNewFilenmae)+1];
    _tcscpy(sFilename, aNewFilenmae);
    mNewList.push_back(sFilename);
}

xpr_size_t BatchRenameEditDlg::getNewCount(void)
{
    return mEditedList.size();
}

const xpr_tchar_t *BatchRenameEditDlg::getNewName(xpr_size_t aIndex)
{
    if (!FXFILE_STL_IS_INDEXABLE(aIndex, mEditedList))
        return XPR_NULL;

    return mEditedList[aIndex];
}

xpr_bool_t BatchRenameEditDlg::OnInitDialog(void) 
{
    super::OnInitDialog();

    SetIcon(mIcon, XPR_TRUE);

    // CResizingDialog -------------------------------------------
    //HideSizeIcon();

    //sizeNone:     Don't resize at all  
    //sizeResize:   The control will be stretched in the appropriate direction 
    //sizeRepos:    The control will be moved in the appropriate direction 
    //sizeRelative: The control will be moved proportionally in the appropriate direction 
    AddControl(IDC_RENAME_EDIT_LABEL_ARROW, sizeRelative, sizeRelative);
    AddControl(IDC_RENAME_EDIT_SYNC_SCROLL, sizeNone,     sizeRepos);

    AddControl(IDOK,                        sizeRepos,    sizeRepos, XPR_FALSE);
    AddControl(IDCANCEL,                    sizeRepos,    sizeRepos, XPR_FALSE);
    //------------------------------------------------------------

    // Limit 500KB
    ((CEdit *)GetDlgItem(IDC_RENAME_EDIT_OLD_NAME_EDIT))->LimitText(500000);
    ((CEdit *)GetDlgItem(IDC_RENAME_EDIT_NEW_NAME_EDIT))->LimitText(500000);

    CRect sOldRect, sArrowRect, sNewRect, sCancelRect;
    GetDlgItem(IDC_RENAME_EDIT_OLD_NAME_EDIT)->GetWindowRect(&sOldRect);
    GetDlgItem(IDC_RENAME_EDIT_LABEL_ARROW)->GetWindowRect(&sArrowRect);
    GetDlgItem(IDC_RENAME_EDIT_NEW_NAME_EDIT)->GetWindowRect(&sNewRect);
    GetDlgItem(IDCANCEL)->GetWindowRect(&sCancelRect);
    ScreenToClient(&sOldRect);
    ScreenToClient(&sArrowRect);
    ScreenToClient(&sNewRect);
    ScreenToClient(&sCancelRect);

    mWidth1 = sArrowRect.left - sOldRect.right;
    mWidth2 = sNewRect.left - sArrowRect.right;
    mHeight1 = sCancelRect.top - sNewRect.bottom;

    xpr_sint_t i, nCount;
    nCount = (xpr_sint_t)mOldList.size();
    for (i = 0; i < nCount; ++i)
    {
        mOldEditCtrl.ReplaceSel(mOldList[i]);
        if (i != (nCount-1))
            mOldEditCtrl.ReplaceSel(XPR_STRING_LITERAL("\r\n"));
    }

    nCount = (xpr_sint_t)mNewList.size();
    for (i = 0; i < nCount; ++i)
    {
        mNewEditCtrl.ReplaceSel(mNewList[i]);
        if (i != (nCount-1))
            mNewEditCtrl.ReplaceSel(XPR_STRING_LITERAL("\r\n"));
    }

    SetWindowText(gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename_edit.title")));
    SetDlgItemText(IDC_RENAME_EDIT_LABEL_OLD_NAME, gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename_edit.label.old_name")));
    SetDlgItemText(IDC_RENAME_EDIT_LABEL_NEW_NAME, gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename_edit.label.new_name")));
    SetDlgItemText(IDC_RENAME_EDIT_SYNC_SCROLL,    gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename_edit.check.sync_scroll")));
    SetDlgItemText(IDOK,                           gApp.loadString(XPR_STRING_LITERAL("popup.common.button.ok")));
    SetDlgItemText(IDCANCEL,                       gApp.loadString(XPR_STRING_LITERAL("popup.common.button.cancel")));

    mDlgState = DlgStateManager::instance().getDlgState(XPR_STRING_LITERAL("RenameEdit"));
    if (XPR_IS_NOT_NULL(mDlgState))
    {
        mDlgState->setDialog(this, XPR_TRUE);
        mDlgState->setCheckBox(XPR_STRING_LITERAL("Sync Scroll"), IDC_RENAME_EDIT_SYNC_SCROLL);
        mDlgState->load();
    }

    OnSyncScroll();

    return XPR_TRUE;
}

void BatchRenameEditDlg::OnSyncScroll(void) 
{
    mSyncScroll = ((CButton *)GetDlgItem(IDC_RENAME_EDIT_SYNC_SCROLL))->GetCheck();
    if (mSyncScroll == XPR_TRUE)
    {
        CEdit *sOldEditCtrl = ((CEdit *)GetDlgItem(IDC_RENAME_EDIT_OLD_NAME_EDIT));
        CEdit *sNewEditCtrl = ((CEdit *)GetDlgItem(IDC_RENAME_EDIT_NEW_NAME_EDIT));

        xpr_sint_t nOldVert = sOldEditCtrl->GetScrollPos(SB_VERT);
        xpr_sint_t nNewVert = sNewEditCtrl->GetScrollPos(SB_VERT);

        mOldEditScroll = XPR_FALSE;
        mNewEditScroll = XPR_TRUE;

        sOldEditCtrl->LineScroll(nNewVert - nOldVert);

        mPrevOldVert = nNewVert;
        mPrevNewVert = nNewVert;
    }
}

void BatchRenameEditDlg::OnVscrollSrnNewEdit(void) 
{
    if (mSyncScroll == XPR_TRUE && (mOldEditScroll == XPR_FALSE || (mOldEditScroll == XPR_FALSE && mNewEditScroll == XPR_FALSE)))
    {
        mOldEditScroll = XPR_FALSE;
        mNewEditScroll = XPR_TRUE;

        CEdit *sOldEditCtrl = ((CEdit *)GetDlgItem(IDC_RENAME_EDIT_OLD_NAME_EDIT));
        CEdit *sNewEditCtrl = ((CEdit *)GetDlgItem(IDC_RENAME_EDIT_NEW_NAME_EDIT));

        xpr_sint_t sVert = sNewEditCtrl->GetScrollPos(SB_VERT);
        sOldEditCtrl->LineScroll(sVert - mPrevNewVert);

        mPrevNewVert = sVert;
    }

    mOldEditScroll = XPR_FALSE;
    mNewEditScroll = XPR_FALSE;
}

void BatchRenameEditDlg::OnVscrollSrnOldEdit(void) 
{
    if (mSyncScroll == XPR_TRUE && (mNewEditScroll == XPR_FALSE || (mOldEditScroll == XPR_FALSE && mNewEditScroll == XPR_FALSE)))
    {
        mOldEditScroll = XPR_TRUE;
        mNewEditScroll = XPR_FALSE;

        CEdit *sOldEditCtrl = ((CEdit *)GetDlgItem(IDC_RENAME_EDIT_OLD_NAME_EDIT));
        CEdit *sNewEditCtrl = ((CEdit *)GetDlgItem(IDC_RENAME_EDIT_NEW_NAME_EDIT));

        xpr_sint_t sVert = sOldEditCtrl->GetScrollPos(SB_VERT);
        sNewEditCtrl->LineScroll(sVert - mPrevOldVert);

        mPrevOldVert = sVert;
    }

    mOldEditScroll = XPR_FALSE;
    mNewEditScroll = XPR_FALSE;
}

void BatchRenameEditDlg::OnOK(void) 
{
    xpr_sint_t sOldCount = mOldEditCtrl.GetLineCount();
    xpr_sint_t sNewCount = mNewEditCtrl.GetLineCount();

    if (sNewCount < sOldCount)
    {
        const xpr_tchar_t *sMsg = gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename_edit.msg.not_matched_count"));
        MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);

        GetDlgItem(IDC_RENAME_EDIT_NEW_NAME_EDIT)->SetFocus();
        return;
    }

    xpr_bool_t sIncorrected = XPR_FALSE;

    xpr_sint_t i;
    xpr_sint_t sLen;
    xpr_tchar_t sText[XPR_MAX_PATH * 2];

    for (i = 0; i < sOldCount; ++i)
    {
        sLen = mNewEditCtrl.LineLength(mNewEditCtrl.LineIndex(i));

        sText[0] = XPR_STRING_LITERAL('\0');
        mNewEditCtrl.GetLine(i, sText, sLen+1);
        sText[sLen] = XPR_STRING_LITERAL('\0');

        if (VerifyFileName(sText) == XPR_FALSE)
        {
            sIncorrected = XPR_TRUE;
            break;
        }

        _tcstrimleft(sText);
        _tcstrimright(sText);

        if (_tcslen(sText) <= 0)
        {
            sIncorrected = XPR_TRUE;
            break;
        }
    }

    if (sIncorrected == XPR_TRUE)
    {
        const xpr_tchar_t *sMsg = gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename_edit.msg.invalid_filename"));
        MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);

        GetDlgItem(IDC_RENAME_EDIT_NEW_NAME_EDIT)->SetFocus();
        return;
    }

    if (sNewCount != sOldCount)
    {
        xpr_bool_t sSkip = XPR_TRUE;

        for (i = sOldCount; i < sNewCount; ++i)
        {
            sLen = mNewEditCtrl.LineLength(mNewEditCtrl.LineIndex(i));

            sText[0] = XPR_STRING_LITERAL('\0');
            mNewEditCtrl.GetLine(i, sText, sLen+1);
            sText[sLen] = XPR_STRING_LITERAL('\0');

            _tcstrimleft(sText);
            _tcstrimright(sText);

            if (_tcslen(sText) > 0)
            {
                sSkip = XPR_FALSE;
                break;
            }
        }

        if (sSkip == XPR_FALSE)
        {
            const xpr_tchar_t *sMsg = gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename_edit.msg.not_matched_count"));
            MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);

            GetDlgItem(IDC_RENAME_EDIT_NEW_NAME_EDIT)->SetFocus();
            return;
        }
    }

    {
        std::deque<xpr_tchar_t *>::iterator sIterator;

        sIterator = mEditedList.begin();
        for (; sIterator != mEditedList.end(); ++sIterator)
            delete *sIterator;
        mEditedList.clear();
    }

    xpr_tchar_t *sNewName;
    for (i = 0; i < sOldCount; ++i)
    {
        sLen = mNewEditCtrl.LineLength(mNewEditCtrl.LineIndex(i));
        if (sLen <= 0)
            continue;

        sNewName = new xpr_tchar_t[sLen+1];
        mNewEditCtrl.GetLine(i, sNewName, sLen);
        sNewName[sLen] = '\0';

        mEditedList.push_back(sNewName);
    }

    if (mEditedList.size() != mNewList.size())
    {
        const xpr_tchar_t *sMsg = gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename_edit.msg.not_matched_count"));
        MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);

        GetDlgItem(IDC_RENAME_EDIT_NEW_NAME_EDIT)->SetFocus();
        return;
    }

    super::OnOK();
}

void BatchRenameEditDlg::OnDestroy(void) 
{
    super::OnDestroy();

    DESTROY_ICON(mIcon);

    if (XPR_IS_NOT_NULL(mDlgState))
    {
        mDlgState->reset();
        mDlgState->save();
    }
}

void BatchRenameEditDlg::OnSize(xpr_uint_t nType, xpr_sint_t cx, xpr_sint_t cy) 
{
    super::OnSize(nType, cx, cy);

    CWnd *sWnd = GetDlgItem(IDC_RENAME_EDIT_OLD_NAME_EDIT);
    if (sWnd != XPR_NULL && sWnd->m_hWnd != XPR_NULL)
    {
        CRect sOldRect, sArrowRect, sNewRect, sNewLabelRect, sCancelRect;
        GetDlgItem(IDC_RENAME_EDIT_OLD_NAME_EDIT)->GetWindowRect(&sOldRect);
        GetDlgItem(IDC_RENAME_EDIT_LABEL_ARROW)->GetWindowRect(&sArrowRect);
        GetDlgItem(IDC_RENAME_EDIT_NEW_NAME_EDIT)->GetWindowRect(&sNewRect);
        GetDlgItem(IDC_RENAME_EDIT_LABEL_NEW_NAME)->GetWindowRect(&sNewLabelRect);
        GetDlgItem(IDCANCEL)->GetWindowRect(&sCancelRect);
        ScreenToClient(&sOldRect);
        ScreenToClient(&sArrowRect);
        ScreenToClient(&sNewRect);
        ScreenToClient(&sNewLabelRect);
        ScreenToClient(&sCancelRect);

        sOldRect.right  = sArrowRect.left  - mWidth1;
        sNewRect.left   = sArrowRect.right + mWidth2;
        sNewRect.right  = sCancelRect.right;
        sOldRect.bottom = sCancelRect.top - mHeight1;
        sNewRect.bottom = sCancelRect.top - mHeight1;

        GetDlgItem(IDC_RENAME_EDIT_OLD_NAME_EDIT)->MoveWindow(sOldRect);
        GetDlgItem(IDC_RENAME_EDIT_LABEL_ARROW)->MoveWindow(sArrowRect);
        GetDlgItem(IDC_RENAME_EDIT_NEW_NAME_EDIT)->MoveWindow(sNewRect);
        GetDlgItem(IDC_RENAME_EDIT_LABEL_NEW_NAME)->SetWindowPos(XPR_NULL, sNewRect.left, sNewLabelRect.top, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
    }
}
} // namespace cmd
} // namespace fxfile
