//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "text_merge_dlg.h"
#include "text_merge.h"

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
// user defined timer
enum
{
    TM_ID_STATUS = 100,
};

// user defined message
enum
{
    WM_FINALIZE = WM_USER + 100,
};

TextMergeDlg::TextMergeDlg(void)
    : super(IDD_TEXT_MERGE, XPR_NULL)
    , mTextMerge(XPR_NULL)
    , mDlgState(XPR_NULL)
{
    mIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

TextMergeDlg::~TextMergeDlg(void)
{
    XPR_SAFE_DELETE(mTextMerge);

    mPathDeque.clear();

    DESTROY_ICON(mIcon);
}

void TextMergeDlg::DoDataExchange(CDataExchange* pDX)
{
    super::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_TEXT_MERGE_PROGRESS, mProgressCtrl);
    DDX_Control(pDX, IDC_TEXT_MERGE_ENCODING, mEncodingComboBox);
    DDX_Control(pDX, IDC_TEXT_MERGE_LIST,     mListCtrl);
}

BEGIN_MESSAGE_MAP(TextMergeDlg, super)
    ON_BN_CLICKED(IDC_TEXT_MERGE_ITEM_UP,   OnItemUp)
    ON_BN_CLICKED(IDC_TEXT_MERGE_ITEM_DOWN, OnItemDown)
    ON_MESSAGE(WM_FINALIZE, OnFinalize)
    ON_WM_TIMER()
END_MESSAGE_MAP()

xpr_bool_t TextMergeDlg::OnInitDialog(void) 
{
    super::OnInitDialog();

    SetIcon(mIcon, XPR_TRUE);

    // CResizingDialog -------------------------------------------
    //HideSizeIcon();

    //sizeNone:     Don't resize at all  
    //sizeResize:   The control will be stretched in the appropriate direction 
    //sizeRepos:    The control will be moved in the appropriate direction 
    //sizeRelative: The control will be moved proportionally in the appropriate direction 
    AddControl(IDC_TEXT_MERGE_LIST,      sizeResize, sizeResize);
    AddControl(IDC_TEXT_MERGE_PROGRESS,  sizeResize, sizeRepos);
    AddControl(IDC_TEXT_MERGE_STATUS,    sizeResize, sizeRepos);
    AddControl(IDC_TEXT_MERGE_ITEM_UP,   sizeNone,   sizeRepos, XPR_FALSE);
    AddControl(IDC_TEXT_MERGE_ITEM_DOWN, sizeNone,   sizeRepos, XPR_FALSE);
    AddControl(IDOK,                     sizeRepos,  sizeRepos, XPR_FALSE);
    AddControl(IDCANCEL,                 sizeRepos,  sizeRepos, XPR_FALSE);
    //------------------------------------------------------------

    mEncodingComboBox.AddString(XPR_STRING_LITERAL("ANSI"));
    mEncodingComboBox.AddString(XPR_STRING_LITERAL("UNICODE"));

    mEncodingComboBox.SetCurSel(0);
    if (xpr::getOsVer() < xpr::kOsVerWinNT)
        mEncodingComboBox.EnableWindow(XPR_FALSE);

    mListCtrl.SetExtendedStyle(mListCtrl.GetExtendedStyle() | LVS_EX_FULLROWSELECT);
    mListCtrl.InsertColumn(0, gApp.loadString(XPR_STRING_LITERAL("popup.text_merge.list.column.no")),       LVCFMT_RIGHT,  35, -1);
    mListCtrl.InsertColumn(1, gApp.loadString(XPR_STRING_LITERAL("popup.text_merge.list.column.text_doc")), LVCFMT_LEFT,  305, -1);

    xpr_sint_t i;
    PathDeque::iterator sIterator;
    xpr_tchar_t sText[0xff] = {0};

    sIterator = mPathDeque.begin();
    for (i = 0; sIterator != mPathDeque.end(); ++sIterator, ++i)
    {
        _stprintf(sText, XPR_STRING_LITERAL("%d"), i + 1);

        mListCtrl.InsertItem(i, sText);
        mListCtrl.SetItem(i, 1, LVIF_TEXT, sIterator->c_str(), -1, 0, 0, 0);
    }

    xpr_tchar_t sStatusText[0xff] = {0};
    _stprintf(sStatusText, gApp.loadFormatString(XPR_STRING_LITERAL("popup.text_merge.status.count"), XPR_STRING_LITERAL("%d")), mPathDeque.size());
    setStatus(sStatusText);

    SetWindowText(gApp.loadString(XPR_STRING_LITERAL("popup.text_merge.title")));
    SetDlgItemText(IDC_TEXT_MERGE_LABEL_ENCODING, gApp.loadString(XPR_STRING_LITERAL("popup.text_merge.label.encoding")));
    SetDlgItemText(IDC_TEXT_MERGE_LABEL_LIST,     gApp.loadString(XPR_STRING_LITERAL("popup.text_merge.label.list")));
    SetDlgItemText(IDOK,                          gApp.loadString(XPR_STRING_LITERAL("popup.text_merge.button.merge")));
    SetDlgItemText(IDCANCEL,                      gApp.loadString(XPR_STRING_LITERAL("popup.text_merge.button.close")));

    mDlgState = DlgStateManager::instance().getDlgState(XPR_STRING_LITERAL("TextMerge"));
    if (XPR_IS_NOT_NULL(mDlgState))
    {
        mDlgState->setDialog(this, XPR_TRUE);
        mDlgState->setListCtrl(XPR_STRING_LITERAL("List"), mListCtrl.GetDlgCtrlID());
        mDlgState->load();
    }

    return XPR_TRUE;
}

void TextMergeDlg::addPath(const xpr_tchar_t *aPath)
{
    if (aPath != XPR_NULL)
        mPathDeque.push_back(aPath);
}

xpr_bool_t TextMergeDlg::DestroyWindow(void) 
{
    if (mTextMerge != XPR_NULL)
        mTextMerge->Stop();

    if (XPR_IS_NOT_NULL(mDlgState))
    {
        mDlgState->reset();
        mDlgState->save();
    }

    return super::DestroyWindow();
}

void TextMergeDlg::setStatus(const xpr_tchar_t *aStatusText)
{
    if (aStatusText != XPR_NULL)
        SetDlgItemText(IDC_TEXT_MERGE_STATUS, aStatusText);
}

void TextMergeDlg::OnItemUp(void) 
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

void TextMergeDlg::OnItemDown(void) 
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

void TextMergeDlg::enableWindow(xpr_bool_t aEnable)
{
    SetDlgItemText(IDOK, (aEnable == XPR_TRUE) ?
        gApp.loadString(XPR_STRING_LITERAL("popup.text_merge.button.merge")) : gApp.loadString(XPR_STRING_LITERAL("popup.text_merge.button.stop")));

    GetDlgItem(IDC_TEXT_MERGE_ENCODING )->EnableWindow(aEnable);
    GetDlgItem(IDC_TEXT_MERGE_ITEM_UP  )->EnableWindow(aEnable);
    GetDlgItem(IDC_TEXT_MERGE_ITEM_DOWN)->EnableWindow(aEnable);
    GetDlgItem(IDCANCEL                )->EnableWindow(aEnable);

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

void TextMergeDlg::OnOK(void) 
{
    if (mTextMerge != XPR_NULL)
    {
        if (mTextMerge->getStatus() == TextMerge::StatusMerging)
        {
            mTextMerge->Stop();
            return;
        }
    }

    xpr_tchar_t sFilter[0xff] = {0};
    _stprintf(sFilter, XPR_STRING_LITERAL("%s (*.txt)\0*.txt\0\0"), gApp.loadString(XPR_STRING_LITERAL("popup.common.file_dialog.filter.text_files")));

    CFileDialogST sFileDialog(XPR_FALSE, XPR_STRING_LITERAL("*.txt"), XPR_NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, sFilter, this);
    if (sFileDialog.DoModal() != IDOK)
        return;

    xpr_bool_t aEncoding = mEncodingComboBox.GetCurSel();

    XPR_SAFE_DELETE(mTextMerge);

    mTextMerge = new TextMerge;
    mTextMerge->setOwner(m_hWnd, WM_FINALIZE);

    mTextMerge->setTextFile(sFileDialog.GetPathName());
    mTextMerge->setEncoding(aEncoding);

    PathDeque::iterator sIterator = mPathDeque.begin();
    for (; sIterator != mPathDeque.end(); ++sIterator)
        mTextMerge->addPath(sIterator->c_str());

    if (mTextMerge->Start() == XPR_TRUE)
    {
        enableWindow(XPR_FALSE);
        SetTimer(TM_ID_STATUS, 100, XPR_NULL);

        mProgressCtrl.SetRange32(0, mListCtrl.GetItemCount());
        mProgressCtrl.SetPos(0);
    }
}

void TextMergeDlg::OnTimer(UINT_PTR nIDEvent)
{
    if (nIDEvent == TM_ID_STATUS)
    {
        xpr_sint_t sCount = mListCtrl.GetItemCount();

        xpr_size_t sSucceeded = 0;
        mTextMerge->getStatus(&sSucceeded);

        xpr_tchar_t sStatusText[0xff] = {0};
        _stprintf(sStatusText, gApp.loadFormatString(XPR_STRING_LITERAL("popup.text_merge.status.in_progress"), XPR_STRING_LITERAL("%d,%d,%d")), sSucceeded, sCount, (xpr_sint_t)((xpr_float_t)sSucceeded / (xpr_float_t)sCount * 100.0));
        setStatus(sStatusText);

        mProgressCtrl.SetPos((xpr_sint_t)sSucceeded);
    }

    super::OnTimer(nIDEvent);
}

LRESULT TextMergeDlg::OnFinalize(WPARAM wParam, LPARAM lParam)
{
    mTextMerge->Stop();

    KillTimer(TM_ID_STATUS);
    enableWindow(XPR_TRUE);

    xpr_size_t sSucceeded = 0;
    TextMerge::Status sStatus = mTextMerge->getStatus(&sSucceeded);

    mProgressCtrl.SetPos((xpr_sint_t)sSucceeded);

    xpr_tchar_t sStatusText[0xff] = {0};

    switch (sStatus)
    {
    case TextMerge::StatusFailed:
        {
            const xpr_tchar_t *sMsg = gApp.loadString(XPR_STRING_LITERAL("popup.text_merge.msg.failed"));
            MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);
            break;
        }

    case TextMerge::StatusMergeCompleted:
        {
            xpr_size_t sCount = mPathDeque.size();
            _stprintf(sStatusText, gApp.loadFormatString(XPR_STRING_LITERAL("popup.text_merge.status.completed"), XPR_STRING_LITERAL("%d,%d")), sSucceeded, sCount-sSucceeded);
            setStatus(sStatusText);
            break;
        }

    case TextMerge::StatusStopped:
        {
            _stprintf(sStatusText, gApp.loadFormatString(XPR_STRING_LITERAL("popup.text_merge.status.stoped"), XPR_STRING_LITERAL("%d")), sSucceeded);
            setStatus(sStatusText);
            break;
        }
    }

    return 0;
}
} // namespace cmd
} // namespace fxfile
