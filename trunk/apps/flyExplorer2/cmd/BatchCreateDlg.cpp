//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "BatchCreateDlg.h"

#include "fxb/fxb_batch_create.h"

#include "DlgState.h"
#include "DlgStateMgr.h"
#include "BatchCreateFmtDlg.h"
#include "BatchCreateTextDlg.h"
#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

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
    , mBatchCreate(new fxb::BatchCreate)
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
    DDX_Control(pDX, IDC_CREATE_TYPE,     mTypeWnd);
    DDX_Control(pDX, IDC_CREATE_TAB,      mTabCtrl);
    DDX_Control(pDX, IDC_CREATE_PROGRESS, mProgressCtrl);
}

BEGIN_MESSAGE_MAP(BatchCreateDlg, super)
    ON_WM_DESTROY()
    ON_WM_SIZE()
    ON_WM_TIMER()
    ON_MESSAGE(WM_FINALIZE, OnFinalize)
    ON_NOTIFY(TCN_SELCHANGE, IDC_CREATE_TAB, OnSelChangeTab)
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

    AddControl(IDC_CREATE_PATH,     sizeResize, sizeNone);
    AddControl(IDC_CREATE_TAB,      sizeResize, sizeResize, XPR_FALSE);

    AddControl(IDC_CREATE_STATUS,   sizeResize, sizeRepos);
    AddControl(IDC_CREATE_PROGRESS, sizeResize, sizeRepos);

    AddControl(IDOK,                sizeRepos,  sizeRepos,  XPR_FALSE);
    AddControl(IDCANCEL,            sizeRepos,  sizeRepos,  XPR_FALSE);
    //------------------------------------------------------------

    addDialog(new BatchCreateFmtDlg,  theApp.loadString(XPR_STRING_LITERAL("popup.batch_create.tab.format")));
    addDialog(new BatchCreateTextDlg, theApp.loadString(XPR_STRING_LITERAL("popup.batch_create.tab.text")));

    SetDlgItemText(IDC_CREATE_PATH, mPath.c_str());

    xpr_sint_t sIndex;
    sIndex = mTypeWnd.AddString(theApp.loadString(XPR_STRING_LITERAL("popup.batch_create.combo.type_folder")));
    mTypeWnd.SetItemData(sIndex, fxb::BatchCreate::CreateTypeFolder);
    sIndex = mTypeWnd.AddString(theApp.loadString(XPR_STRING_LITERAL("popup.batch_create.combo.type_file")));
    mTypeWnd.SetItemData(sIndex, fxb::BatchCreate::CreateTypeFile);
    sIndex = mTypeWnd.AddString(theApp.loadString(XPR_STRING_LITERAL("popup.batch_create.combo.type_text_file")));
    mTypeWnd.SetItemData(sIndex, fxb::BatchCreate::CreateTypeTextFile);
    setCreateType(fxb::BatchCreate::CreateTypeFolder);

    mProgressCtrl.SetRange32(0, 100);
    mProgressCtrl.SetPos(0);

    xpr_sint_t sActiveTab = 1;

    mDlgState = DlgStateMgr::instance().getDlgState(XPR_STRING_LITERAL("BatchCreate"));
    if (XPR_IS_NOT_NULL(mDlgState))
    {
        mDlgState->setDialog(this, XPR_TRUE);
        mDlgState->setComboBox(XPR_STRING_LITERAL("Type"), IDC_CREATE_TYPE);
        mDlgState->load();

        sActiveTab = mDlgState->getStateI(XPR_STRING_LITERAL("Active Tab"), 1); // one-based index

        sActiveTab = sActiveTab - 1;
        if (sActiveTab < 0 || mTabCtrl.GetItemCount() <= sActiveTab)
            sActiveTab = 0;
    }

    showDialog(sActiveTab);

    SetWindowText(theApp.loadString(XPR_STRING_LITERAL("popup.batch_create.title")));
    SetDlgItemText(IDC_CREATE_LABEL_PATH, theApp.loadString(XPR_STRING_LITERAL("popup.batch_create.label.path")));
    SetDlgItemText(IDC_CREATE_LABEL_TYPE, theApp.loadString(XPR_STRING_LITERAL("popup.batch_create.label.type")));
    SetDlgItemText(IDOK,                  theApp.loadString(XPR_STRING_LITERAL("popup.common.button.ok")));
    SetDlgItemText(IDCANCEL,              theApp.loadString(XPR_STRING_LITERAL("popup.common.button.cancel")));

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
    if (!XPR_STL_IS_INDEXABLE(aIndex, mTabDeque))
        return XPR_NULL;

    return mTabDeque[aIndex];
}

void BatchCreateDlg::OnSelChangeTab(NMHDR* pNMHDR, LRESULT* pResult) 
{
    showDialog(mTabCtrl.GetCurSel());
    *pResult = 0;
}

fxb::BatchCreate::CreateType BatchCreateDlg::getCreateType(void)
{
    xpr_sint_t sCurSel = mTypeWnd.GetCurSel();
    if (sCurSel == CB_ERR)
        return fxb::BatchCreate::CreateTypeNone;

    return (fxb::BatchCreate::CreateType)mTypeWnd.GetItemData(sCurSel);
}

void BatchCreateDlg::setCreateType(fxb::BatchCreate::CreateType aType)
{
    xpr_sint_t sCurSel = CB_ERR;

    xpr_sint_t i;
    xpr_sint_t sCount;

    sCount = mTypeWnd.GetCount();
    for (i = 0; i < sCount; ++i)
    {
        if ((fxb::BatchCreate::CreateType)mTypeWnd.GetItemData(i) == aType)
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
        SetDlgItemText(IDC_CREATE_STATUS, aStatus);
}

void BatchCreateDlg::showDialog(xpr_sint_t aIndex)
{
    if (!XPR_STL_IS_INDEXABLE(aIndex, mTabDeque))
        return;

    BatchCreateTabDlg *sOldDlg;
    BatchCreateTabDlg *sNewDlg;

    sNewDlg = mTabDeque[aIndex];
    if (sNewDlg == XPR_NULL)
        return;

    // Hide
    if (XPR_STL_IS_INDEXABLE(mOldShowDlg, mTabDeque))
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
    GetDlgItem(IDOK)->SetWindowText((aEnable == XPR_TRUE) ? theApp.loadString(XPR_STRING_LITERAL("popup.common.button.ok")) : theApp.loadString(XPR_STRING_LITERAL("popup.batch_create.button.stop")));

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
        fxb::BatchCreate::Status sStatus = mBatchCreate->getStatus();
        if (sStatus == fxb::BatchCreate::StatusPreparing  ||
            sStatus == fxb::BatchCreate::StatusVerifying ||
            sStatus == fxb::BatchCreate::StatusCreating)
        {
            mBatchCreate->Stop();
            return;
        }
    }

    mBatchCreate->clear();

    fxb::BatchCreate::CreateType sCreateType = getCreateType();
    fxb::BatchCreate::Result sResult;

    if (sCreateType == fxb::BatchCreate::CreateTypeNone)
    {
        const xpr_tchar_t *sMsg = theApp.loadString(XPR_STRING_LITERAL("popup.batch_create.msg.select_type"));
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
            BatchCreateFmtDlg *sDlg = (BatchCreateFmtDlg *)getDialog(sCurTab);

            xpr_tchar_t sFormat[0xff] = {0};
            sDlg->getFormat(sFormat, 0xfe);

            xpr_sint_t sStart     = sDlg->getStart();
            xpr_sint_t sIncrease  = sDlg->getIncrease();
            xpr_sint_t sEnd       = sDlg->getEnd();
            xpr_sint_t sCount     = sDlg->getCount();
            xpr_bool_t sCountType = sDlg->getCountType();

            if (sStart > sEnd)
            {
                const xpr_tchar_t *sMsg = theApp.loadString(XPR_STRING_LITERAL("popup.batch_create.msg.wrong_number_range"));
                MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);
                return;
            }

            sResult = mBatchCreate->addFormat(
                mPath.c_str(),
                sFormat,
                sStart,
                sIncrease,
                (sCountType == XPR_TRUE) ? sCount : sEnd,
                sCountType);

            switch (sResult)
            {
            case fxb::BatchCreate::ResultExcessMaxCount:
                {
                    xpr_tchar_t sMsg[0xff] = {0};
                    _stprintf(sMsg, theApp.loadFormatString(XPR_STRING_LITERAL("popup.batch_create.msg.excess_max_count"), XPR_STRING_LITERAL("%d")), mBatchCreate->getMaxCount());
                    MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);
                    return;
                }

            case fxb::BatchCreate::ResultInvalidFormat:
                {
                    const xpr_tchar_t *sMsg = theApp.loadString(XPR_STRING_LITERAL("popup.batch_create.msg.invalid_format"));
                    MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);
                    return;
                }

            case fxb::BatchCreate::ResultExcessPathLength:
                {
                    xpr_tchar_t sMsg[0xff] = {0};
                    _stprintf(sMsg, theApp.loadFormatString(XPR_STRING_LITERAL("popup.batch_create.msg.excess_path_length"), XPR_STRING_LITERAL("%d")), XPR_MAX_PATH);
                    MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);
                    return;
                }
            }

            break;
        }

    case 1: // list
        {
            BatchCreateTextDlg *sDlg = (BatchCreateTextDlg *)getDialog(sCurTab);

            xpr_sint_t i;
            std::tstring sDir;
            std::tstring sPath;
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
                if (fxb::VerifyFileName(sPath) == XPR_FALSE)
                {
                    const xpr_tchar_t *sMsg = theApp.loadString(XPR_STRING_LITERAL("popup.batch_create.msg.wrong_filename"));
                    MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);
                    return;
                }

                fxb::trim_left_right(sPath);
                if (sPath.empty())
                    continue;

                if (sCreateType == fxb::BatchCreate::CreateTypeTextFile)
                    _tcscat(sFileName, XPR_STRING_LITERAL(".txt"));

                sPath = sDir + sFileName;

                sResult = mBatchCreate->addPath(sPath);
                switch (sResult)
                {
                case fxb::BatchCreate::ResultExcessMaxCount:
                    {
                        xpr_tchar_t sMsg[0xff] = {0};
                        _stprintf(sMsg, theApp.loadFormatString(XPR_STRING_LITERAL("popup.batch_create.msg.excess_max_count"), XPR_STRING_LITERAL("%d")), mBatchCreate->getMaxCount());
                        MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);
                        return;
                    }

                case fxb::BatchCreate::ResultExcessPathLength:
                    {
                        xpr_tchar_t sMsg[0xff] = {0};
                        _stprintf(sMsg, theApp.loadFormatString(XPR_STRING_LITERAL("popup.batch_create.msg.excess_path_length"), XPR_STRING_LITERAL("%d")), XPR_MAX_PATH);
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
        const xpr_tchar_t *sMsg = theApp.loadString(XPR_STRING_LITERAL("popup.batch_create.msg.none"));
        MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);
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
        fxb::BatchCreate::Status sStatus = mBatchCreate->getStatus();
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
    fxb::BatchCreate::Status sStatus;
    sStatus = mBatchCreate->getStatus(&sPrepared, &sValidated, &sCreated);

    switch (sStatus)
    {
    case fxb::BatchCreate::StatusCreateCompleted:
        {
            const xpr_tchar_t *sStatusText = theApp.loadString(XPR_STRING_LITERAL("popup.batch_create.status.completed"));
            setStatus(sStatusText);

            super::OnOK();
            break;
        }

    case fxb::BatchCreate::StatusStopped:
        {
            xpr_tchar_t sStatusText[0xff] = {0};
            _stprintf(sStatusText, theApp.loadFormatString(XPR_STRING_LITERAL("popup.batch_create.status.stoped"), XPR_STRING_LITERAL("%d,%d")), sCreated, sCount);
            setStatus(sStatusText);

            setEnableWindow(XPR_TRUE);
            break;
        }

    case fxb::BatchCreate::StatusInvalid:
        {
            xpr_sint_t sInvalidIndex = mBatchCreate->getInvalidItem();

            switch (mBatchCreate->getItemResult(sInvalidIndex))
            {
            case fxb::BatchCreate::ResultInvalidName:
                {
                    const xpr_tchar_t *sMsg = theApp.loadString(XPR_STRING_LITERAL("popup.batch_create.msg.wrong_filename"));
                    MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);

                    const xpr_tchar_t *sStatusText = theApp.loadString(XPR_STRING_LITERAL("popup.batch_create.status.re-fill"));
                    setStatus(sStatusText);
                    break;
                }

            case fxb::BatchCreate::ResultExcessPathLength:
            case fxb::BatchCreate::ResultEmptiedName:
            case fxb::BatchCreate::ResultEqualedName:
                {
                    const xpr_tchar_t *sMsg = theApp.loadString(XPR_STRING_LITERAL("popup.batch_create.msg.re-fill"));
                    MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);

                    const xpr_tchar_t *sStatusText = theApp.loadString(XPR_STRING_LITERAL("popup.batch_create.status.re-fill"));
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
        fxb::BatchCreate::Status sStatus;
        sStatus = mBatchCreate->getStatus(&sPreparedCount, &sValidatedCount, &sRenamedCount);

        xpr_tchar_t sStatusText[0xff] = {0};
        xpr_size_t sCount = mBatchCreate->getCount();

        switch (sStatus)
        {
        case fxb::BatchCreate::StatusPreparing:
            {
                _stprintf(sStatusText, theApp.loadFormatString(XPR_STRING_LITERAL("popup.batch_create.status.preparing"), XPR_STRING_LITERAL("%d,%d,%d")), sPreparedCount, sCount, (xpr_sint_t)((xpr_float_t)sPreparedCount / (xpr_float_t)sCount * 100));
                setStatus(sStatusText);
                break;
            }

        case fxb::BatchCreate::StatusVerifying:
            {
                _stprintf(sStatusText, theApp.loadFormatString(XPR_STRING_LITERAL("popup.batch_create.status.verifying"), XPR_STRING_LITERAL("%d,%d,%d")), sValidatedCount, sCount, (xpr_sint_t)((xpr_float_t)sValidatedCount / (xpr_float_t)sCount * 100));
                setStatus(sStatusText);

                mProgressCtrl.SetPos((xpr_sint_t)sValidatedCount);
                break;
            }

        case fxb::BatchCreate::StatusCreating:
            {
                _stprintf(sStatusText, theApp.loadFormatString(XPR_STRING_LITERAL("popup.batch_create.status.creating"), XPR_STRING_LITERAL("%d,%d,%d")), sRenamedCount, sCount, (xpr_sint_t)((xpr_float_t)sRenamedCount / (xpr_float_t)sCount * 100));
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
