//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "batch_create_dlg.h"

#include "dlg_state.h"
#include "dlg_state_manager.h"
#include "batch_create_tab_format_dlg.h"
#include "batch_create_tab_text_dlg.h"
#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace fxfile
{
namespace cmd
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

BatchCreateDlg::BatchCreateDlg(const xpr_tchar_t *aPath)
    : super(IDD_BATCH_CREATE, XPR_NULL)
    , mBatchCreate(new BatchCreate)
    , mOldShowDlg(-1)
    , mDlgState(XPR_NULL)
{
    if (aPath != XPR_NULL)
        mPath = aPath;

    mIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

BatchCreateDlg::~BatchCreateDlg(void)
{
    XPR_SAFE_DELETE(mBatchCreate);
    DESTROY_ICON(mIcon);
}

void BatchCreateDlg::DoDataExchange(CDataExchange* pDX)
{
    super::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_BATCH_CREATE_TYPE,     mTypeWnd);
    DDX_Control(pDX, IDC_BATCH_CREATE_TAB,      mTabCtrl);
    DDX_Control(pDX, IDC_BATCH_CREATE_PROGRESS, mProgressCtrl);
}

BEGIN_MESSAGE_MAP(BatchCreateDlg, super)
    ON_WM_DESTROY()
    ON_WM_SIZE()
    ON_WM_TIMER()
    ON_MESSAGE(WM_FINALIZE, OnFinalize)
    ON_NOTIFY(TCN_SELCHANGE, IDC_BATCH_CREATE_TAB, OnSelChangeTab)
END_MESSAGE_MAP()

xpr_bool_t BatchCreateDlg::OnInitDialog(void) 
{
    super::OnInitDialog();

    SetIcon(mIcon, XPR_TRUE);

    // CResizingDialog -------------------------------------------
    //HideSizeIcon();

    //sizeNone:     Don't resize at all  
    //sizeResize:   The control will be stretched in the appropriate direction 
    //sizeRepos:    The control will be moved in the appropriate direction 
    //sizeRelative: The control will be moved proportionally in the appropriate direction 

    AddControl(IDC_BATCH_CREATE_PATH,     sizeResize, sizeNone);
    AddControl(IDC_BATCH_CREATE_TAB,      sizeResize, sizeResize, XPR_FALSE);

    AddControl(IDC_BATCH_CREATE_STATUS,   sizeResize, sizeRepos);
    AddControl(IDC_BATCH_CREATE_PROGRESS, sizeResize, sizeRepos);

    AddControl(IDOK,                      sizeRepos,  sizeRepos,  XPR_FALSE);
    AddControl(IDCANCEL,                  sizeRepos,  sizeRepos,  XPR_FALSE);
    //------------------------------------------------------------

    addDialog(new BatchCreateTabFormatDlg, gApp.loadString(XPR_STRING_LITERAL("popup.batch_create.tab.format")));
    addDialog(new BatchCreateTabTextDlg,   gApp.loadString(XPR_STRING_LITERAL("popup.batch_create.tab.text")));

    SetDlgItemText(IDC_BATCH_CREATE_PATH, mPath.c_str());

    xpr_sint_t sIndex;
    sIndex = mTypeWnd.AddString(gApp.loadString(XPR_STRING_LITERAL("popup.batch_create.combo.type_folder")));
    mTypeWnd.SetItemData(sIndex, BatchCreate::CreateTypeFolder);
    sIndex = mTypeWnd.AddString(gApp.loadString(XPR_STRING_LITERAL("popup.batch_create.combo.type_file")));
    mTypeWnd.SetItemData(sIndex, BatchCreate::CreateTypeFile);
    sIndex = mTypeWnd.AddString(gApp.loadString(XPR_STRING_LITERAL("popup.batch_create.combo.type_text_file")));
    mTypeWnd.SetItemData(sIndex, BatchCreate::CreateTypeTextFile);
    setCreateType(BatchCreate::CreateTypeFolder);

    mProgressCtrl.SetRange32(0, 100);
    mProgressCtrl.SetPos(0);

    xpr_sint_t sActiveTab = 1;

    mDlgState = DlgStateManager::instance().getDlgState(XPR_STRING_LITERAL("BatchCreate"));
    if (XPR_IS_NOT_NULL(mDlgState))
    {
        mDlgState->setDialog(this, XPR_TRUE);
        mDlgState->setComboBox(XPR_STRING_LITERAL("Type"), IDC_BATCH_CREATE_TYPE);
        mDlgState->load();

        sActiveTab = mDlgState->getStateI(XPR_STRING_LITERAL("Active Tab"), 1); // one-based index

        sActiveTab = sActiveTab - 1;
        if (sActiveTab < 0 || mTabCtrl.GetItemCount() <= sActiveTab)
            sActiveTab = 0;
    }

    showDialog(sActiveTab);

    SetWindowText(gApp.loadString(XPR_STRING_LITERAL("popup.batch_create.title")));
    SetDlgItemText(IDC_BATCH_CREATE_LABEL_PATH, gApp.loadString(XPR_STRING_LITERAL("popup.batch_create.label.path")));
    SetDlgItemText(IDC_BATCH_CREATE_LABEL_TYPE, gApp.loadString(XPR_STRING_LITERAL("popup.batch_create.label.type")));
    SetDlgItemText(IDOK,                        gApp.loadString(XPR_STRING_LITERAL("popup.common.button.ok")));
    SetDlgItemText(IDCANCEL,                    gApp.loadString(XPR_STRING_LITERAL("popup.common.button.cancel")));

    return XPR_TRUE;
}

void BatchCreateDlg::OnDestroy(void) 
{
    super::OnDestroy();

    if (XPR_IS_NOT_NULL(mDlgState))
    {
        mDlgState->reset();
        mDlgState->setStateI(XPR_STRING_LITERAL("Active Tab"), getCurTab() + 1);
        mDlgState->save();
    }

    XPR_SAFE_DELETE(mBatchCreate);

    DESTROY_ICON(mIcon);

    BatchCreateTabDlg *sDlg;
    TabDeque::iterator sIterator;

    sIterator = mTabDeque.begin();
    for (; sIterator != mTabDeque.end(); ++sIterator)
    {
        sDlg = *sIterator;
        DESTROY_DELETE(sDlg);
    }

    mTabDeque.clear();
}

void BatchCreateDlg::OnSize(xpr_uint_t nType, xpr_sint_t cx, xpr_sint_t cy)
{
    super::OnSize(nType, cx, cy);

    if (mTabCtrl.m_hWnd)
    {
        CRect sTabRect, sItemRect;
        TabDeque::iterator sIterator;
        BatchCreateTabDlg *sDlg;

        sIterator = mTabDeque.begin();
        for (; sIterator != mTabDeque.end(); ++sIterator)
        {
            sDlg = *sIterator;
            if (sDlg == XPR_NULL || sDlg->m_hWnd == XPR_NULL)
                continue;

            mTabCtrl.GetWindowRect(&sTabRect);
            ScreenToClient(&sTabRect);

            mTabCtrl.GetItemRect(0, &sItemRect);

            sTabRect.DeflateRect(3, sItemRect.Height()+5, 3, 3);
            sDlg->MoveWindow(sTabRect);
        }
    }
}

void BatchCreateDlg::addDialog(BatchCreateTabDlg *sDlg, const xpr_tchar_t *aTitle)
{
    xpr_sint_t sIndex = mTabCtrl.GetItemCount();

    mTabCtrl.InsertItem(sIndex, aTitle);
    mTabDeque.push_back(sDlg);
}

xpr_sint_t BatchCreateDlg::getCurTab(void)
{
    return mTabCtrl.GetCurSel();
}

BatchCreateTabDlg *BatchCreateDlg::getDialog(xpr_sint_t aIndex)
{
    if (!FXFILE_STL_IS_INDEXABLE(aIndex, mTabDeque))
        return XPR_NULL;

    return mTabDeque[aIndex];
}

void BatchCreateDlg::OnSelChangeTab(NMHDR* pNMHDR, LRESULT* pResult) 
{
    showDialog(mTabCtrl.GetCurSel());
    *pResult = 0;
}

BatchCreate::CreateType BatchCreateDlg::getCreateType(void)
{
    xpr_sint_t sCurSel = mTypeWnd.GetCurSel();
    if (sCurSel == CB_ERR)
        return BatchCreate::CreateTypeNone;

    return (BatchCreate::CreateType)mTypeWnd.GetItemData(sCurSel);
}

void BatchCreateDlg::setCreateType(BatchCreate::CreateType aType)
{
    xpr_sint_t sCurSel = CB_ERR;

    xpr_sint_t i;
    xpr_sint_t sCount;

    sCount = mTypeWnd.GetCount();
    for (i = 0; i < sCount; ++i)
    {
        if ((BatchCreate::CreateType)mTypeWnd.GetItemData(i) == aType)
        {
            sCurSel = i;
            break;
        }
    }

    if (sCurSel == CB_ERR)
        sCurSel = 0;

    mTypeWnd.SetCurSel(sCurSel);
}

void BatchCreateDlg::setStatus(const xpr_tchar_t *aStatus)
{
    if (aStatus != XPR_NULL)
        SetDlgItemText(IDC_BATCH_CREATE_STATUS, aStatus);
}

void BatchCreateDlg::showDialog(xpr_sint_t aIndex)
{
    if (!FXFILE_STL_IS_INDEXABLE(aIndex, mTabDeque))
        return;

    BatchCreateTabDlg *sOldDlg;
    BatchCreateTabDlg *sNewDlg;

    sNewDlg = mTabDeque[aIndex];
    if (sNewDlg == XPR_NULL)
        return;

    // Hide
    if (FXFILE_STL_IS_INDEXABLE(mOldShowDlg, mTabDeque))
    {
        sOldDlg = mTabDeque[mOldShowDlg];
        if (sOldDlg != XPR_NULL)
        {
            if (sOldDlg->m_hWnd != XPR_NULL)
                sOldDlg->ShowWindow(SW_HIDE);
        }
    }

    // Show
    if (sNewDlg->m_hWnd == XPR_NULL)
        sNewDlg->create(this);

    sNewDlg->OnTabInit();

    CRect sTabRect(0,0,0,0);
    mTabCtrl.GetWindowRect(&sTabRect);
    ScreenToClient(&sTabRect);

    CRect sItemRect(0,0,0,0);
    mTabCtrl.GetItemRect(0, &sItemRect);

    sTabRect.DeflateRect(3, sItemRect.Height()+5, 3, 3);

    sNewDlg->MoveWindow(sTabRect);
    sNewDlg->SetWindowPos(&wndTop, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
    sNewDlg->ShowWindow(SW_SHOW);
    sNewDlg->SetFocus();
    UpdateWindow();

    mTabCtrl.SetCurSel(aIndex);
    mOldShowDlg = aIndex;
}

void BatchCreateDlg::setEnableWindow(xpr_bool_t aEnable)
{
    GetDlgItem(IDOK)->SetWindowText((aEnable == XPR_TRUE) ? gApp.loadString(XPR_STRING_LITERAL("popup.common.button.ok")) : gApp.loadString(XPR_STRING_LITERAL("popup.batch_create.button.stop")));

    CWnd *sChildWnd;
    TabDeque::iterator sIterator;
    BatchCreateTabDlg *sDlg;

    sIterator = mTabDeque.begin();
    for (; sIterator != mTabDeque.end(); ++sIterator)
    {
        sDlg = *sIterator;
        if (sDlg == XPR_NULL || sDlg->m_hWnd == XPR_NULL)
            continue;

        sChildWnd = sDlg->GetWindow(GW_CHILD);
        while (sChildWnd != XPR_NULL)
        {
            sChildWnd->EnableWindow(aEnable);
            sChildWnd = sChildWnd->GetNextWindow();
        }

        sDlg->EnableWindow(aEnable);
    }

    mTabCtrl.EnableWindow(aEnable);

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

void BatchCreateDlg::OnOK(void) 
{
    if (mBatchCreate != XPR_NULL)
    {
        BatchCreate::Status sStatus = mBatchCreate->getStatus();
        if (sStatus == BatchCreate::StatusPreparing  ||
            sStatus == BatchCreate::StatusVerifying ||
            sStatus == BatchCreate::StatusCreating)
        {
            mBatchCreate->Stop();
            return;
        }
    }

    mBatchCreate->clear();

    BatchCreate::CreateType sCreateType = getCreateType();
    BatchCreate::Result sResult;

    if (sCreateType == BatchCreate::CreateTypeNone)
    {
        const xpr_tchar_t *sMsg = gApp.loadString(XPR_STRING_LITERAL("popup.batch_create.msg.select_type"));
        MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);

        mTypeWnd.SetFocus();
        return;
    }

    mBatchCreate->setOwner(m_hWnd, WM_FINALIZE);
    mBatchCreate->setCreateType(sCreateType);

    xpr_sint_t sCurTab = getCurTab();
    switch (sCurTab)
    {
    case 0: // format
        {
            BatchCreateTabFormatDlg *sDlg = (BatchCreateTabFormatDlg *)getDialog(sCurTab);

            xpr_tchar_t sFormat[FXFILE_BATCH_CREATE_FORMAT_MAX_LENGTH + 1] = {0};
            sDlg->getFormat(sFormat, FXFILE_BATCH_CREATE_FORMAT_MAX_LENGTH);

            xpr_sint_t sStart    = sDlg->getStart();
            xpr_sint_t sIncrease = sDlg->getIncrease();
            xpr_size_t sCount    = sDlg->getCount();

            sResult = mBatchCreate->addFormat(mPath.c_str(), sFormat, sCount);

            switch (sResult)
            {
            case BatchCreate::ResultWrongFormat:
                {
                    const xpr_tchar_t *sMsg = gApp.loadString(XPR_STRING_LITERAL("popup.batch_create.msg.wrong_format"));
                    MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);
                    return;
                }

            case BatchCreate::ResultExcessPathLength:
                {
                    xpr_tchar_t sMsg[0xff] = {0};
                    _stprintf(sMsg, gApp.loadFormatString(XPR_STRING_LITERAL("popup.batch_create.msg.excess_path_length"), XPR_STRING_LITERAL("%d")), XPR_MAX_PATH);
                    MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);
                    return;
                }
            }

            break;
        }

    case 1: // list
        {
            BatchCreateTabTextDlg *sDlg = (BatchCreateTabTextDlg *)getDialog(sCurTab);

            xpr_sint_t i;
            xpr::tstring sDir;
            xpr::tstring sPath;
            xpr_tchar_t sFileName[XPR_MAX_PATH + 1] = {0};
            xpr_sint_t sLineCount = sDlg->getLineCount();

            sDir = mPath;
            if (sDir[sDir.length()-1] != XPR_STRING_LITERAL('\\'))
                sDir += XPR_STRING_LITERAL('\\');

            for (i = 0; i < sLineCount; ++i)
            {
                sFileName[0] = XPR_STRING_LITERAL('\0');
                if (!sDlg->getLineText(i, sFileName, XPR_MAX_PATH))
                    continue;

                sPath = sFileName;
                if (VerifyFileName(sPath) == XPR_FALSE)
                {
                    const xpr_tchar_t *sMsg = gApp.loadString(XPR_STRING_LITERAL("popup.batch_create.msg.invalid_filename"));
                    MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);
                    return;
                }

                sPath.trim();
                if (sPath.empty())
                    continue;

                if (sCreateType == BatchCreate::CreateTypeTextFile)
                    _tcscat(sFileName, XPR_STRING_LITERAL(".txt"));

                sPath = sDir + sFileName;

                sResult = mBatchCreate->addPath(sPath);
                switch (sResult)
                {
                case BatchCreate::ResultExcessPathLength:
                    {
                        xpr_tchar_t sMsg[0xff] = {0};
                        _stprintf(sMsg, gApp.loadFormatString(XPR_STRING_LITERAL("popup.batch_create.msg.excess_path_length"), XPR_STRING_LITERAL("%d")), XPR_MAX_PATH);
                        MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);
                        return;
                    }
                }
            }

            break;
        }
    }

    xpr_size_t sCount = mBatchCreate->getCount();
    if (sCount == 0)
    {
        const xpr_tchar_t *sMsg = gApp.loadString(XPR_STRING_LITERAL("popup.batch_create.msg.none"));
        MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);
        return;
    }
    else if (sCount > FXFILE_BATCH_CREATE_FORMAT_COUNT_MAX)
    {
        xpr::tstring sMsg;
        sMsg.format(gApp.loadFormatString(XPR_STRING_LITERAL("popup.batch_create.msg.excess_max_count"), XPR_STRING_LITERAL("%d")), FXFILE_BATCH_CREATE_FORMAT_COUNT_MAX);
        MessageBox(sMsg.c_str(), XPR_NULL, MB_OK | MB_ICONSTOP);
        return;
    }

    mProgressCtrl.SetRange32(0, (xpr_sint_t)sCount);
    mProgressCtrl.SetPos(0);

    if (mBatchCreate->Start())
    {
        setEnableWindow(XPR_FALSE);
        SetTimer(TM_ID_STATUS, 100, XPR_NULL);
    }
    else
    {
        BatchCreate::Status sStatus = mBatchCreate->getStatus();
    }
}

LRESULT BatchCreateDlg::OnFinalize(WPARAM wParam, LPARAM lParam)
{
    mBatchCreate->Stop();

    KillTimer(TM_ID_STATUS);
    setEnableWindow(XPR_TRUE);

    xpr_size_t sCount;
    sCount = mBatchCreate->getCount();

    xpr_size_t sPrepared = 0;
    xpr_size_t sValidated = 0;
    xpr_size_t sCreated = 0;
    BatchCreate::Status sStatus;
    sStatus = mBatchCreate->getStatus(&sPrepared, &sValidated, &sCreated);

    switch (sStatus)
    {
    case BatchCreate::StatusCreateCompleted:
        {
            const xpr_tchar_t *sStatusText = gApp.loadString(XPR_STRING_LITERAL("popup.batch_create.status.completed"));
            setStatus(sStatusText);

            super::OnOK();
            break;
        }

    case BatchCreate::StatusStopped:
        {
            xpr_tchar_t sStatusText[0xff] = {0};
            _stprintf(sStatusText, gApp.loadFormatString(XPR_STRING_LITERAL("popup.batch_create.status.stoped"), XPR_STRING_LITERAL("%d,%d")), sCreated, sCount);
            setStatus(sStatusText);

            setEnableWindow(XPR_TRUE);
            break;
        }

    case BatchCreate::StatusInvalid:
        {
            xpr_sint_t sInvalidIndex = mBatchCreate->getInvalidItem();

            switch (mBatchCreate->getItemResult(sInvalidIndex))
            {
            case BatchCreate::ResultInvalidName:
                {
                    const xpr_tchar_t *sMsg = gApp.loadString(XPR_STRING_LITERAL("popup.batch_create.msg.invalid_filename"));
                    MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);

                    const xpr_tchar_t *sStatusText = gApp.loadString(XPR_STRING_LITERAL("popup.batch_create.status.re-fill"));
                    setStatus(sStatusText);
                    break;
                }

            case BatchCreate::ResultExcessPathLength:
            case BatchCreate::ResultEqualedName:
                {
                    const xpr_tchar_t *sMsg = gApp.loadString(XPR_STRING_LITERAL("popup.batch_create.msg.re-fill"));
                    MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);

                    const xpr_tchar_t *sStatusText = gApp.loadString(XPR_STRING_LITERAL("popup.batch_create.status.re-fill"));
                    setStatus(sStatusText);
                    break;
                }
            }

            break;
        }
    }

    return 0;
}

void BatchCreateDlg::OnTimer(UINT_PTR nIDEvent)
{
    if (nIDEvent == TM_ID_STATUS)
    {
        xpr_size_t sPreparedCount = 0;
        xpr_size_t sValidatedCount = 0;
        xpr_size_t sRenamedCount = 0;
        BatchCreate::Status sStatus;
        sStatus = mBatchCreate->getStatus(&sPreparedCount, &sValidatedCount, &sRenamedCount);

        xpr_tchar_t sStatusText[0xff] = {0};
        xpr_size_t sCount = mBatchCreate->getCount();

        switch (sStatus)
        {
        case BatchCreate::StatusPreparing:
            {
                _stprintf(sStatusText, gApp.loadFormatString(XPR_STRING_LITERAL("popup.batch_create.status.preparing"), XPR_STRING_LITERAL("%d,%d,%d")), sPreparedCount, sCount, (xpr_sint_t)((xpr_float_t)sPreparedCount / (xpr_float_t)sCount * 100));
                setStatus(sStatusText);
                break;
            }

        case BatchCreate::StatusVerifying:
            {
                _stprintf(sStatusText, gApp.loadFormatString(XPR_STRING_LITERAL("popup.batch_create.status.verifying"), XPR_STRING_LITERAL("%d,%d,%d")), sValidatedCount, sCount, (xpr_sint_t)((xpr_float_t)sValidatedCount / (xpr_float_t)sCount * 100));
                setStatus(sStatusText);

                mProgressCtrl.SetPos((xpr_sint_t)sValidatedCount);
                break;
            }

        case BatchCreate::StatusCreating:
            {
                _stprintf(sStatusText, gApp.loadFormatString(XPR_STRING_LITERAL("popup.batch_create.status.creating"), XPR_STRING_LITERAL("%d,%d,%d")), sRenamedCount, sCount, (xpr_sint_t)((xpr_float_t)sRenamedCount / (xpr_float_t)sCount * 100));
                setStatus(sStatusText);

                mProgressCtrl.SetPos((xpr_sint_t)sRenamedCount);
                break;
            }
        }
    }

    super::OnTimer(nIDEvent);
}

xpr_bool_t BatchCreateDlg::PreTranslateMessage(MSG* pMsg)
{
    if (translateHotKey(pMsg))
        return XPR_TRUE;

    return super::PreTranslateMessage(pMsg);
}

xpr_bool_t BatchCreateDlg::translateHotKey(MSG *pMsg)
{
    if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
    {
        xpr_sint_t nID = ::GetDlgCtrlID(pMsg->hwnd);

        // Ctrl+Enter or Enter in the IDOK
        if (GetKeyState(VK_CONTROL) < 0 || nID == IDOK)
        {
            OnOK();
            return XPR_TRUE;
        }

        return XPR_FALSE;
    }
    else if (pMsg->message == WM_KEYDOWN || pMsg->message == WM_SYSKEYDOWN)
    {
        // Alt + -> : backward
        // Alt + <- : forward
        xpr_bool_t sFuncKey = XPR_FALSE;
        switch (pMsg->wParam)
        {
        case '1': // Alt + 1,2
            if (GetKeyState(VK_MENU) < 0)
            {
                showDialog(0);
                sFuncKey = XPR_TRUE;
            }
            break;

        case '2':
            if (GetKeyState(VK_MENU) < 0)
            {
                showDialog(1);
                sFuncKey = XPR_TRUE;
            }
            break;
        }

        if (sFuncKey == XPR_TRUE)
            return XPR_TRUE;
    }

    return XPR_FALSE;
}
} // namespace cmd
} // namespace fxfile
