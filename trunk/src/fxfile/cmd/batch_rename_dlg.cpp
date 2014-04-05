//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "batch_rename_dlg.h"
#include "multi_rename.h"

#include "resource.h"
#include "dlg_state.h"
#include "dlg_state_manager.h"
#include "batch_rename_tab_dlg.h"
#include "batch_rename_tab_format_dlg.h"
#include "batch_rename_tab_replace_dlg.h"
#include "batch_rename_tab_insert_dlg.h"
#include "batch_rename_tab_delete_dlg.h"
#include "batch_rename_tab_case_dlg.h"
#include "batch_rename_edit_dlg.h"
#include "batch_rename_tip_dlg.h"
#include "input_dlg.h"

#include "gui/BCMenu.h"
#include "gui/FileDialogST.h"

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

BatchRenameDlg::BatchRenameDlg(void)
    : super(IDD_RENAME, XPR_NULL)
    , mBatchRename(new BatchRename)
    , mRenaming(XPR_FALSE)
    , mOldShowDlg(-1)
    , mDlgState(XPR_NULL)
{
    mIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

BatchRenameDlg::~BatchRenameDlg(void)
{
    XPR_SAFE_DELETE(mBatchRename);
    DESTROY_ICON(mIcon);
}

void BatchRenameDlg::DoDataExchange(CDataExchange* pDX)
{
    super::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_BATCH_RENAME_TAB,      mTabCtrl);
    DDX_Control(pDX, IDC_BATCH_RENAME_PROGRESS, mProgressCtrl);
    DDX_Control(pDX, IDC_BATCH_RENAME_LISTCTRL, mListCtrl);
}

BEGIN_MESSAGE_MAP(BatchRenameDlg, super)
    ON_WM_SIZE()
    ON_WM_QUERYDRAGICON()
    ON_WM_DESTROY()
    ON_WM_WINDOWPOSCHANGING()
    ON_WM_WINDOWPOSCHANGED()
    ON_WM_TIMER()
    ON_NOTIFY(TCN_SELCHANGE,    IDC_BATCH_RENAME_TAB,      OnSelchangeSrnTab)
    ON_NOTIFY(LVN_ITEMACTIVATE, IDC_BATCH_RENAME_LISTCTRL, OnItemActivate)
    ON_NOTIFY(LVN_GETDISPINFO,  IDC_BATCH_RENAME_LISTCTRL, OnLvnGetdispinfoList)
    ON_NOTIFY_EX_RANGE(TTN_NEEDTEXT, 0, 0xFFFF, OnToolTipNotify)
    ON_COMMAND(IDC_BATCH_RENAME_INIT,            OnInit)
    ON_COMMAND(IDC_BATCH_RENAME_HISTORY_ARCHIVE, OnHistoryArchive)
    ON_COMMAND(IDC_BATCH_RENAME_NO_CHANGE_EXT,   OnNoChangeExt)
    ON_COMMAND(IDC_BATCH_RENAME_ON_RESULT,       OnResult)
    ON_COMMAND(IDC_BATCH_RENAME_EDIT,            OnEdit)
    ON_COMMAND(IDC_BATCH_RENAME_LOAD,            OnLoad)
    ON_COMMAND(IDC_BATCH_RENAME_SAVE,            OnSave)
    ON_COMMAND(IDC_BATCH_RENAME_UNDO,            OnUndo)
    ON_COMMAND(IDC_BATCH_RENAME_REDO,            OnRedo)
    ON_COMMAND(IDC_BATCH_RENAME_ITEM_UP,         OnItemUp)
    ON_COMMAND(IDC_BATCH_RENAME_ITEM_DOWN,       OnItemDown)
    ON_COMMAND(IDC_BATCH_RENAME_ITEM_MOVE,       OnItemMove)
    ON_UPDATE_COMMAND_UI(IDC_BATCH_RENAME_INIT,            OnUpdateInit)
    ON_UPDATE_COMMAND_UI(IDC_BATCH_RENAME_HISTORY_ARCHIVE, OnUpdateBatchFormatArchive)
    ON_UPDATE_COMMAND_UI(IDC_BATCH_RENAME_NO_CHANGE_EXT,   OnUpdateNoChangeExt)
    ON_UPDATE_COMMAND_UI(IDC_BATCH_RENAME_ON_RESULT,       OnUpdateResult)
    ON_UPDATE_COMMAND_UI(IDC_BATCH_RENAME_LOAD,            OnUpdateLoad)
    ON_UPDATE_COMMAND_UI(IDC_BATCH_RENAME_SAVE,            OnUpdateSave)
    ON_UPDATE_COMMAND_UI(IDC_BATCH_RENAME_UNDO,            OnUpdateUndo)
    ON_UPDATE_COMMAND_UI(IDC_BATCH_RENAME_REDO,            OnUpdateRedo)
    ON_UPDATE_COMMAND_UI(IDC_BATCH_RENAME_ITEM_UP,         OnUpdateItemUp)
    ON_UPDATE_COMMAND_UI(IDC_BATCH_RENAME_ITEM_DOWN,       OnUpdateItemDown)
    ON_UPDATE_COMMAND_UI(IDC_BATCH_RENAME_ITEM_MOVE,       OnUpdateItemMove)
    ON_BN_CLICKED(IDC_BATCH_RENAME_ITEM_EDIT,          OnItemEdit)
    ON_MESSAGE(WM_FINALIZE,           OnFinalize)
    ON_MESSAGE(WM_BATCH_RENAME_APPLY, OnApply)
END_MESSAGE_MAP()

xpr_bool_t BatchRenameDlg::OnInitDialog(void)
{
    //-----------------------------------------------------------------------------
    // ToolBar Creation

    mToolBar.setImageListId(IDB_TB_RENAME, CSize(16, 16));
    mToolBar.enableToolTips(XPR_TRUE);

    if (mToolBar.createToolBar(this) == XPR_FALSE)
    {
        XPR_TRACE(XPR_STRING_LITERAL("Failed to create toolbar\n"));
        return -1;
    }

    TBBUTTON sTbButtons[] = {
        {  0, IDC_BATCH_RENAME_INIT,                 TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0, 0 },
        {  0, 0,                                     TBSTATE_ENABLED, TBSTYLE_SEP,    0, 0, 0 },
        {  1, IDC_BATCH_RENAME_UNDO,                 TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0, 0 },
        {  2, IDC_BATCH_RENAME_REDO,                 TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0, 0 },
        {  0, 0,                                     TBSTATE_ENABLED, TBSTYLE_SEP,    0, 0, 0 },
        {  3, IDC_BATCH_RENAME_ITEM_UP,              TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0, 0 },
        {  4, IDC_BATCH_RENAME_ITEM_DOWN,            TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0, 0 },
        {  5, IDC_BATCH_RENAME_ITEM_MOVE,            TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0, 0 },
        {  0, 0,                                     TBSTATE_ENABLED, TBSTYLE_SEP,    0, 0, 0 },
        {  6, IDC_BATCH_RENAME_ON_RESULT,            TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0, 0 },
        {  7, IDC_BATCH_RENAME_NO_CHANGE_EXT,        TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0, 0 },
        {  8, IDC_BATCH_RENAME_HISTORY_ARCHIVE,      TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0, 0 },
        {  0, 0,                                     TBSTATE_ENABLED, TBSTYLE_SEP,    0, 0, 0 },
        {  9, IDC_BATCH_RENAME_LOAD,                 TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0, 0 },
        { 10, IDC_BATCH_RENAME_SAVE,                 TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0, 0 },
    };

    mToolBar.setToolBarButtons(sTbButtons, sizeof(sTbButtons) / sizeof(sTbButtons[0]));

    mToolBar.reposition(this);

    //-----------------------------------------------------------------------------

    super::OnInitDialog();

    SetIcon(mIcon, XPR_TRUE);

    // CResizingDialog ------------------------------------------------------------
    //HideSizeIcon();

    //sizeNone:     Don't resize at all  
    //sizeResize:   The control will be stretched in the appropriate direction 
    //sizeRepos:    The control will be moved in the appropriate direction 
    //sizeRelative: The control will be moved proportionally in the appropriate direction 
    AddControl(mToolBar.GetDlgCtrlID(),             sizeResize, sizeNone);

    AddControl(IDC_BATCH_RENAME_PATH,               sizeResize, sizeNone);
    AddControl(IDC_BATCH_RENAME_TAB,                sizeResize, sizeNone,  XPR_FALSE);

    AddControl(IDC_BATCH_RENAME_LABEL_BACKUP,       sizeRepos,  sizeNone);
    AddControl(IDC_BATCH_RENAME_BACKUP,             sizeRepos,  sizeNone);

    AddControl(IDC_BATCH_RENAME_STATUS,             sizeResize, sizeRepos);
    AddControl(IDC_BATCH_RENAME_PROGRESS,           sizeRepos,  sizeRepos);

    AddControl(IDC_BATCH_RENAME_ITEM_EDIT,          sizeNone,   sizeRepos, XPR_FALSE);
    AddControl(IDC_BATCH_RENAME_EDIT,               sizeNone,   sizeRepos, XPR_FALSE);
    AddControl(IDC_BATCH_RENAME_LISTCTRL,           sizeResize, sizeResize);

    AddControl(IDOK,                                sizeRepos,  sizeRepos, XPR_FALSE);
    AddControl(IDCANCEL,                            sizeRepos,  sizeRepos, XPR_FALSE);

    //-----------------------------------------------------------------------------

    EnableToolTips();

    addTabDialog(new BatchRenameTabFormatDlg,  gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename.tab.format")));
    addTabDialog(new BatchRenameTabReplaceDlg, gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename.tab.replace")));
    addTabDialog(new BatchRenameTabInsertDlg,  gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename.tab.insert")));
    addTabDialog(new BatchRenameTabDeleteDlg,  gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename.tab.delete")));
    addTabDialog(new BatchRenameTabCaseDlg,    gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename.tab.case")));

    ((CEdit *)GetDlgItem(IDC_BATCH_RENAME_BACKUP))->LimitText(XPR_MAX_PATH);

    mListCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT);
    mListCtrl.InsertColumn(0, gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename.list.column.no")),       LVCFMT_LEFT,  30, -1);
    mListCtrl.InsertColumn(1, gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename.list.column.old_name")), LVCFMT_LEFT, 233, -1);
    mListCtrl.InsertColumn(2, gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename.list.column.new_name")), LVCFMT_LEFT, 233, -1);

    {
        xpr::tstring sDir;

        xpr_size_t i;
        xpr_size_t sCount;
        BatchRename::Item *sBatchRenameItem;
        LVITEM sLvItem = {0};

        sCount = mBatchRename->getCount();
        for (i = 0; i < sCount; ++i)
        {
            sBatchRenameItem = mBatchRename->getItem(i);
            if (sBatchRenameItem == XPR_NULL)
                continue;

            sLvItem.mask       = LVIF_TEXT | LVIF_PARAM;
            sLvItem.iItem      = (xpr_sint_t)i;
            sLvItem.iSubItem   = 0;
            sLvItem.pszText    = LPSTR_TEXTCALLBACK;
            sLvItem.cchTextMax = XPR_MAX_PATH;
            sLvItem.lParam     = (LPARAM)sBatchRenameItem;
            mListCtrl.InsertItem(&sLvItem);

            if (sDir.empty())
                sDir = sBatchRenameItem->mDir;
            else
            {
                if (sDir != gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename.path.multiple")))
                {
                    if (sDir != sBatchRenameItem->mDir)
                        sDir = gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename.path.multiple"));
                }
            }
        }

        GetDlgItem(IDC_BATCH_RENAME_PATH)->SetWindowText(sDir.c_str());
    }

    GetDlgItem(IDC_BATCH_RENAME_BACKUP)->SetWindowText(gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename.backup_prefix")));

    xpr_size_t sCount = mBatchRename->getCount();

    mProgressCtrl.SetRange32(0, (xpr_sint_t)sCount);
    mProgressCtrl.SetPos(0);

    xpr_tchar_t sStatusText[0xff] = {0};
    _stprintf(sStatusText, gApp.loadFormatString(XPR_STRING_LITERAL("popup.batch_rename.status.count"), XPR_STRING_LITERAL("%d")), sCount);
    setStatus(sStatusText);

    SetWindowText(gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename.title")));
    SetDlgItemText(IDC_BATCH_RENAME_LABEL_PATH,         gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename.label.path")));
    SetDlgItemText(IDC_BATCH_RENAME_LABEL_BACKUP,       gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename.label.backup_prefix")));
    SetDlgItemText(IDC_BATCH_RENAME_LABEL_LIST,         gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename.label.list")));
    SetDlgItemText(IDC_BATCH_RENAME_ITEM_EDIT,          gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename.button.edit_item")));
    SetDlgItemText(IDC_BATCH_RENAME_EDIT,               gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename.button.directly_edit")));
    SetDlgItemText(IDOK,                                gApp.loadString(XPR_STRING_LITERAL("popup.common.button.ok")));
    SetDlgItemText(IDCANCEL,                            gApp.loadString(XPR_STRING_LITERAL("popup.common.button.cancel")));

    xpr_bool_t sNoChangeExt    = XPR_TRUE;
    xpr_bool_t sResultApply    = XPR_TRUE;
    xpr_bool_t sHistoryArchive = XPR_TRUE;
    xpr_sint_t sActiveTab      = 1;

    mDlgState = DlgStateManager::instance().getDlgState(XPR_STRING_LITERAL("BatchRename"));
    if (XPR_IS_NOT_NULL(mDlgState))
    {
        mDlgState->setDialog(this, XPR_TRUE);
        mDlgState->setListCtrl(XPR_STRING_LITERAL("List"),   mListCtrl.GetDlgCtrlID());
        mDlgState->setEditCtrl(XPR_STRING_LITERAL("Backup"), IDC_BATCH_RENAME_BACKUP);
        mDlgState->load();

        sNoChangeExt    = mDlgState->getStateB(XPR_STRING_LITERAL("No Change Extension"), XPR_TRUE);
        sResultApply    = mDlgState->getStateB(XPR_STRING_LITERAL("Result Apply"),        XPR_TRUE);
        sHistoryArchive = mDlgState->getStateB(XPR_STRING_LITERAL("History Archive"),     XPR_TRUE);
        sActiveTab      = mDlgState->getStateI(XPR_STRING_LITERAL("Active Tab"),          1); // one-based index
    }

    xpr_uint_t sFlags = 0;
    if (sNoChangeExt    == XPR_TRUE) sFlags |= BatchRename::FlagNoChangeExt;
    if (sResultApply    == XPR_TRUE) sFlags |= BatchRename::FlagResultRename;
    if (sHistoryArchive == XPR_TRUE) sFlags |= BatchRename::FlagHistoryArchive;

    mBatchRename->setFlags(sFlags);

    BatchRename::setString(
        gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename.error.excess_max_length")),
        gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename.error.max_filename_legnth")),
        gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename.error.invalid_filename")));

    sActiveTab = sActiveTab - 1;
    if (sActiveTab < 0 || mTabCtrl.GetItemCount() <= sActiveTab)
        sActiveTab = 0;

    showTabDialog(sActiveTab);

    return XPR_TRUE;
}

void BatchRenameDlg::OnDestroy(void)
{
    super::OnDestroy();

    xpr_uint_t sFlags;
    sFlags = mBatchRename->getFlags();

    if (XPR_IS_NOT_NULL(mDlgState))
    {
        mDlgState->reset();
        mDlgState->setStateB(XPR_STRING_LITERAL("No Change Extension"),   XPR_TEST_BITS(sFlags, BatchRename::FlagNoChangeExt));
        mDlgState->setStateB(XPR_STRING_LITERAL("Rename by Result"),      XPR_TEST_BITS(sFlags, BatchRename::FlagResultRename));
        mDlgState->setStateB(XPR_STRING_LITERAL("Batch Format Archive"),  XPR_TEST_BITS(sFlags, BatchRename::FlagHistoryArchive));
        mDlgState->setStateI(XPR_STRING_LITERAL("Active Tab"),            mTabCtrl.GetCurSel() + 1);
        mDlgState->save();
    }

    XPR_SAFE_DELETE(mBatchRename);

    DESTROY_ICON(mIcon);

    mToolBar.DestroyWindow();

    BatchRenameTabDlg *sDlg;
    TabDeque::iterator sIterator;

    sIterator = mTabDeque.begin();
    for (; sIterator != mTabDeque.end(); ++sIterator)
    {
        sDlg = *sIterator;
        DESTROY_DELETE(sDlg);
    }

    mTabDeque.clear();
}

void BatchRenameDlg::addTabDialog(BatchRenameTabDlg *aDlg, const xpr_tchar_t *aTitle)
{
    xpr_sint_t sIndex = mTabCtrl.GetItemCount();

    mTabCtrl.InsertItem(sIndex, aTitle);
    mTabDeque.push_back(aDlg);
}

BatchRenameTabDlg *BatchRenameDlg::getTabDialog(xpr_sint_t aIndex)
{
    if (!FXFILE_STL_IS_INDEXABLE(aIndex, mTabDeque))
        return XPR_NULL;

    return mTabDeque[aIndex];
}

void BatchRenameDlg::OnSelchangeSrnTab(NMHDR* pNMHDR, LRESULT* pResult)
{
    showTabDialog(mTabCtrl.GetCurSel());
    *pResult = 0;
}

void BatchRenameDlg::showTabDialog(xpr_sint_t aIndex)
{
    if (!FXFILE_STL_IS_INDEXABLE(aIndex, mTabDeque))
        return;

    BatchRenameTabDlg *sOldDlg;
    BatchRenameTabDlg *sNewDlg;

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
        sNewDlg->Create(this);

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

void BatchRenameDlg::setStatus(const xpr_tchar_t *aStatusText)
{
    if (aStatusText != XPR_NULL)
        SetDlgItemText(IDC_BATCH_RENAME_STATUS, aStatusText);
}

void BatchRenameDlg::addPath(const xpr_tchar_t *aPath, xpr_bool_t aFolder)
{
    if (aPath == XPR_NULL)
        return;

    xpr_sint_t sLen = (xpr_sint_t)_tcslen(aPath);
    if (sLen < 0 || sLen >= XPR_MAX_PATH)
        return;

    xpr::tstring sPath = aPath;

    xpr_size_t sFind = sPath.rfind(XPR_STRING_LITERAL('\\'));
    if (sFind == xpr::tstring::npos)
        return;

    BatchRename::Item *sBatchRenameItem = new BatchRename::Item;
    sBatchRenameItem->mDir    = sPath.substr(0, sFind);
    sBatchRenameItem->mOld    = sPath.substr(sFind+1);
    sBatchRenameItem->mNew    = sBatchRenameItem->mOld;
    sBatchRenameItem->mFolder = aFolder;

    addPath(sBatchRenameItem);
}

void BatchRenameDlg::addPath(BatchRename::Item *aBatchRenameItem)
{
    if (aBatchRenameItem != XPR_NULL)
        mBatchRename->addItem(aBatchRenameItem);
}

xpr_size_t BatchRenameDlg::getFileCount(void)
{
    return mBatchRename->getCount();
}

void BatchRenameDlg::OnLvnGetdispinfoList(NMHDR *pNMHDR, LRESULT *pResult)
{
    NMLVDISPINFO *sDispInfo = reinterpret_cast<NMLVDISPINFO *>(pNMHDR);
    *pResult = 0;

    BatchRename::Item *sBatchRenameItem = (BatchRename::Item *)sDispInfo->item.lParam;
    if (sBatchRenameItem == XPR_NULL)
        return;

    if (XPR_TEST_BITS(sDispInfo->item.mask, LVIF_TEXT))
    {
        switch (sDispInfo->item.iSubItem)
        {
        case 0:
            _stprintf(sDispInfo->item.pszText, _T("%d"), sDispInfo->item.iItem + 1);
            break;

        case 1:
            _tcscpy(sDispInfo->item.pszText, sBatchRenameItem->mOld.c_str());
            break;

        case 2:
            _tcscpy(sDispInfo->item.pszText, sBatchRenameItem->mNew.c_str());
            break;
        }
    }
}

void BatchRenameDlg::OnSize(xpr_uint_t nType, xpr_sint_t cx, xpr_sint_t cy)
{
    super::OnSize(nType, cx, cy);

    if (mTabDeque.size() >= 1 && mTabDeque[0])
    {
        if (((BatchRenameTabFormatDlg *)mTabDeque[0])->mTipDlg && ((BatchRenameTabFormatDlg *)mTabDeque[0])->mTipDlg->m_hWnd)
        {
            CRect sRect, sClientRect;
            GetWindowRect(&sRect);
            ((BatchRenameTabFormatDlg *)mTabDeque[0])->mTipDlg->GetWindowRect(&sClientRect);

            ((BatchRenameTabFormatDlg *)mTabDeque[0])->mTipDlg->MoveWindow(sRect.right, sRect.top, sClientRect.Width(), sClientRect.Height());
        }
    }

    if (mTabCtrl.m_hWnd != XPR_NULL)
    {
        CRect sTabRect, sItemRect;
        TabDeque::iterator sIterator;
        BatchRenameTabDlg *sDlg;

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

void BatchRenameDlg::OnWindowPosChanging(WINDOWPOS FAR* lpwp)
{
    super::OnWindowPosChanging(lpwp);
}

void BatchRenameDlg::OnWindowPosChanged(WINDOWPOS FAR* lpwp)
{
    super::OnWindowPosChanged(lpwp);

    if ((mTabDeque.empty() == false) && ((BatchRenameTabFormatDlg *)mTabDeque[0])->mTipDlg != XPR_NULL && (lpwp->flags & SWP_NOMOVE) != SWP_NOMOVE)
    {
        CRect sRect, sClientRect;
        GetWindowRect(&sRect);
        ((BatchRenameTabFormatDlg *)mTabDeque[0])->mTipDlg->GetWindowRect(&sClientRect);
        ((BatchRenameTabFormatDlg *)mTabDeque[0])->mTipDlg->MoveWindow(sRect.right, sRect.top, sClientRect.Width(), sClientRect.Height());
    }
}

HCURSOR BatchRenameDlg::OnQueryDragIcon(void)
{
    return (HCURSOR)mIcon;
}

void BatchRenameDlg::OnInit(void)
{
    mBatchRename->reset();
    mBatchRename->clearHistory();

    mListCtrl.Invalidate(XPR_FALSE);
    mListCtrl.UpdateWindow();
}

void BatchRenameDlg::OnUpdateInit(CCmdUI *pCmdUI)
{
    pCmdUI->Enable(!mRenaming);
}

void BatchRenameDlg::OnLoad(void)
{
    xpr::tstring sFilter;
    sFilter.format(XPR_STRING_LITERAL("%s (*.xml)\0*.xml\0\0"), gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename.file_dialog.filter.preset_files")));

    CFileDialogST sFileDialog(XPR_TRUE,
                              XPR_STRING_LITERAL("xml"),    // default extension
                              XPR_STRING_LITERAL("*.xml"),  // filename
                              OFN_HIDEREADONLY,
                              sFilter.c_str(),
                              this);

    if (sFileDialog.DoModal() != IDOK)
    {
        return;
    }

    xpr::tstring sFilePath(sFileDialog.GetPathName());
    if (mBatchRename->load(sFilePath) == XPR_FALSE)
    {
        const xpr_tchar_t *sMsg = gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename.msg.load_failed"));
        MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);
        return;
    }

    mListCtrl.Invalidate(XPR_FALSE);
    mListCtrl.UpdateWindow();
}

void BatchRenameDlg::OnSave(void)
{
    xpr::tstring sFilter;
    sFilter.format(XPR_STRING_LITERAL("%s (*.xml)\0*.xml\0\0"), gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename.file_dialog.filter.preset_files")));

    CFileDialogST sFileDialog(XPR_FALSE,
                              XPR_STRING_LITERAL("xml"),    // default extension
                              XPR_STRING_LITERAL("*.xml"),  // filename
                              OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
                              sFilter.c_str(),
                              this);

    if (sFileDialog.DoModal() != IDOK)
    {
        return;
    }

    xpr::tstring sFilePath(sFileDialog.GetPathName());
    if (mBatchRename->save(sFilePath) == XPR_FALSE)
    {
        const xpr_tchar_t *sMsg = gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename.msg.save_failed"));
        MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);
        return;
    }
}

void BatchRenameDlg::OnUndo(void)
{
    mBatchRename->goBackward();

    mListCtrl.Invalidate(XPR_FALSE);
    mListCtrl.UpdateWindow();
}

void BatchRenameDlg::OnRedo(void)
{
    mBatchRename->goForward();

    mListCtrl.Invalidate(XPR_FALSE);
    mListCtrl.UpdateWindow();
}

void BatchRenameDlg::OnUpdateLoad(CCmdUI *pCmdUI)
{
    pCmdUI->Enable(XPR_TRUE);
}

void BatchRenameDlg::OnUpdateSave(CCmdUI *pCmdUI)
{
    pCmdUI->Enable(XPR_TRUE);
}

void BatchRenameDlg::OnUpdateUndo(CCmdUI *pCmdUI)
{
    pCmdUI->Enable((mRenaming == XPR_FALSE) && (mBatchRename->getBackwardCount() != 0));
}

void BatchRenameDlg::OnUpdateRedo(CCmdUI *pCmdUI)
{
    pCmdUI->Enable((mRenaming == XPR_FALSE) && (mBatchRename->getForwardCount() != 0));
}

void BatchRenameDlg::enableWindow(xpr_bool_t aEnable)
{
    GetDlgItem(IDOK)->SetWindowText(
        (aEnable == XPR_TRUE) ?
        gApp.loadString(XPR_STRING_LITERAL("popup.common.button.ok")) : gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename.button.stop")));

    CWnd *sChildWnd;
    TabDeque::iterator sIterator;
    BatchRenameTabDlg *sDlg;

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

    GetDlgItem(IDC_BATCH_RENAME_BACKUP)->EnableWindow(aEnable);
    GetDlgItem(IDC_BATCH_RENAME_LISTCTRL)->EnableWindow(aEnable);
    GetDlgItem(IDC_BATCH_RENAME_ITEM_EDIT)->EnableWindow(aEnable);
    GetDlgItem(IDC_BATCH_RENAME_EDIT)->EnableWindow(aEnable);

    GetDlgItem(IDCANCEL)->EnableWindow(aEnable);

    // System Button
    CMenu *sMenu = GetSystemMenu(XPR_FALSE);
    if (sMenu != XPR_NULL)
    {
        xpr_uint_t sFlags = MF_BYCOMMAND;
        if (aEnable == XPR_FALSE) sFlags |= MF_GRAYED;
        sMenu->EnableMenuItem(SC_CLOSE, sFlags);
    }

    mRenaming = !aEnable;
}

void BatchRenameDlg::OnOK(void)
{
    if (mBatchRename != XPR_NULL)
    {
        MultiRename::Status uStatus = mBatchRename->getStatus();
        if (uStatus == MultiRename::StatusPreparing  ||
            uStatus == MultiRename::StatusValidating ||
            uStatus == MultiRename::StatusRenaming)
        {
            mBatchRename->stop();
            return;
        }
    }

    xpr_tchar_t sBackup[XPR_MAX_PATH + 1];
    GetDlgItemText(IDC_BATCH_RENAME_BACKUP, sBackup, XPR_MAX_PATH);
    if (_tcslen(sBackup) < 1)
    {
        const xpr_tchar_t *sMsg = gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename.msg.wrong_format"));
        MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);

        GetDlgItem(IDC_BATCH_RENAME_BACKUP)->SetFocus();
        return;
    }

    mBatchRename->setOwner(m_hWnd, WM_FINALIZE);
    mBatchRename->setBackupName(sBackup);

    if (mBatchRename->start() == XPR_TRUE)
    {
        enableWindow(XPR_FALSE);
        SetTimer(TM_ID_STATUS, 100, XPR_NULL);
    }
}

LRESULT BatchRenameDlg::OnFinalize(WPARAM wParam, LPARAM lParam)
{
    mBatchRename->stop();

    KillTimer(TM_ID_STATUS);
    enableWindow(XPR_TRUE);

    xpr_size_t sCount;
    sCount = mBatchRename->getCount();

    xpr_size_t sPreparedCount = 0;
    xpr_size_t sValidatedCount = 0;
    xpr_size_t sRenamedCount = 0;
    MultiRename::Status sStatus = mBatchRename->getStatus(&sPreparedCount, &sValidatedCount, &sRenamedCount);

    switch (sStatus)
    {
    case MultiRename::StatusRenameCompleted:
        {
            const xpr_tchar_t *sStatusText = gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename.status.completed"));
            setStatus(sStatusText);

            super::OnOK();
            break;
        }

    case MultiRename::StatusStopped:
        {
            mListCtrl.Invalidate(XPR_FALSE);
            mListCtrl.UpdateWindow();

            xpr_tchar_t sStatusText[0xff] = {0};
            _stprintf(sStatusText, gApp.loadFormatString(XPR_STRING_LITERAL("popup.batch_rename.status.stoped"), XPR_STRING_LITERAL("%d,%d")), sRenamedCount, sCount);
            setStatus(sStatusText);

            enableWindow(XPR_TRUE);
            break;
        }

    case MultiRename::StatusInvalid:
        {
            xpr_sint_t sInvalidIndex = mBatchRename->getInvalidItem();

            switch (mBatchRename->getItemResult(sInvalidIndex))
            {
            case MultiRename::ResultInvalidName:
                {
                    mListCtrl.Invalidate(XPR_FALSE);
                    mListCtrl.UpdateWindow();

                    const xpr_tchar_t *sMsg = gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename.msg.invalid_filename"));
                    MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);

                    mListCtrl.SetFocus();
                    mListCtrl.EnsureVisible(sInvalidIndex, XPR_TRUE);
                    mListCtrl.SetSelectionMark(sInvalidIndex);
                    mListCtrl.SetItemState(sInvalidIndex, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);

                    const xpr_tchar_t *sStatusText = gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename.status.re-input"));
                    setStatus(sStatusText);
                    break;
                }

            case MultiRename::ResultExcessPathLength:
            case MultiRename::ResultEmptiedName:
            case MultiRename::ResultEqualedName:
                {
                    mListCtrl.Invalidate(XPR_FALSE);
                    mListCtrl.UpdateWindow();

                    const xpr_tchar_t *sMsg = gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename.msg.wrong_input"));
                    MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);

                    mListCtrl.SetFocus();
                    mListCtrl.EnsureVisible(sInvalidIndex, XPR_TRUE);
                    mListCtrl.SetSelectionMark(sInvalidIndex);
                    mListCtrl.SetItemState(sInvalidIndex, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);

                    const xpr_tchar_t *sStatusText = gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename.status.re-input"));
                    setStatus(sStatusText);
                    break;
                }
            }

            break;
        }
    }

    return 0;
}

void BatchRenameDlg::OnTimer(UINT_PTR nIDEvent)
{
    if (nIDEvent == TM_ID_STATUS)
    {
        xpr_size_t sPreparedCount = 0;
        xpr_size_t sValidatedCount = 0;
        xpr_size_t sRenamedCount = 0;
        MultiRename::Status sStatus = mBatchRename->getStatus(&sPreparedCount, &sValidatedCount, &sRenamedCount);

        xpr_tchar_t sStatusText[0xff] = {0};
        xpr_size_t sCount = mBatchRename->getCount();

        switch (sStatus)
        {
        case MultiRename::StatusPreparing:
            {
                _stprintf(sStatusText, gApp.loadFormatString(XPR_STRING_LITERAL("popup.batch_rename.status.preparing"), XPR_STRING_LITERAL("%d,%d,%d")), sPreparedCount, sCount, (xpr_sint_t)((xpr_float_t)sPreparedCount / (xpr_float_t)sCount * 100));
                setStatus(sStatusText);
                break;
            }

        case MultiRename::StatusValidating:
            {
                _stprintf(sStatusText, gApp.loadFormatString(XPR_STRING_LITERAL("popup.batch_rename.status.validating"), XPR_STRING_LITERAL("%d,%d,%d")), sValidatedCount, sCount, (xpr_sint_t)((xpr_float_t)sValidatedCount / (xpr_float_t)sCount * 100));
                setStatus(sStatusText);

                mProgressCtrl.SetPos((xpr_sint_t)sValidatedCount);
                break;
            }

        case MultiRename::StatusRenaming:
            {
                _stprintf(sStatusText, gApp.loadFormatString(XPR_STRING_LITERAL("popup.batch_rename.status.renaming"), XPR_STRING_LITERAL("%d,%d,%d")), sRenamedCount, sCount, (xpr_sint_t)((xpr_float_t)sRenamedCount / (xpr_float_t)sCount * 100));
                setStatus(sStatusText);

                mProgressCtrl.SetPos((xpr_sint_t)sRenamedCount);
                break;
            }
        }
    }

    super::OnTimer(nIDEvent);
}

xpr_bool_t BatchRenameDlg::translateHotKey(MSG *pMsg)
{
    if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
    {
        xpr_sint_t sId = ::GetDlgCtrlID(pMsg->hwnd);

        // Ctrl+Enter or Enter in the IDOK
        if (GetKeyState(VK_CONTROL) < 0 || sId == IDOK)
        {
            OnOK();
            return XPR_TRUE;
        }

        return XPR_TRUE;
    }
    else if (pMsg->message == WM_KEYDOWN || pMsg->message == WM_SYSKEYDOWN)
    {
        // Alt + -> : backward
        // Alt + <- : forward
        xpr_bool_t sFuncKey = XPR_FALSE;
        switch (pMsg->wParam)
        {
        case VK_LEFT:
            if (GetKeyState(VK_MENU) < 0)
            {
                OnUndo();
                sFuncKey = XPR_TRUE;
            }
            break;

        case VK_RIGHT:
            if (GetKeyState(VK_MENU) < 0)
            {
                OnRedo();
                sFuncKey = XPR_TRUE;
            }
            break;

        case VK_UP:
            if (pMsg->hwnd == mListCtrl.m_hWnd && GetKeyState(VK_CONTROL) < 0)
            {
                OnItemUp();
                sFuncKey = XPR_TRUE;
            }
            break;

        case VK_DOWN:
            if (pMsg->hwnd == mListCtrl.m_hWnd && GetKeyState(VK_CONTROL) < 0)
            {
                OnItemDown();
                sFuncKey = XPR_TRUE;
            }
            break;

        case 'g':
        case 'G':
            if (pMsg->hwnd == mListCtrl.m_hWnd && GetKeyState(VK_CONTROL) < 0)
            {
                OnItemMove();
                sFuncKey = XPR_TRUE;
            }
            break;

        case 'u':
        case 'U':
            if (GetKeyState(VK_CONTROL) < 0)
            {
                OnInit();
                sFuncKey = XPR_TRUE;
            }
            break;

        case 'r':
        case 'R':
            if (GetKeyState(VK_CONTROL) < 0)
            {
                OnResult();
                sFuncKey = XPR_TRUE;
            }
            break;

        case 'e':
        case 'E':
            if (GetKeyState(VK_CONTROL) < 0)
            {
                OnNoChangeExt();
                sFuncKey = XPR_TRUE;
            }
            break;

        case 'w':
        case 'W':
            if (GetKeyState(VK_CONTROL) < 0)
            {
                OnHistoryArchive();
                sFuncKey = XPR_TRUE;
            }
            break;

        case 'o':
        case 'O':
            if (GetKeyState(VK_CONTROL) < 0)
            {
                OnLoad();
                sFuncKey = XPR_TRUE;
            }
            break;

        case 's':
        case 'S':
            if (GetKeyState(VK_CONTROL) < 0)
            {
                OnSave();
                sFuncKey = XPR_TRUE;
            }
            break;

            // Alt + 1,2,3,4,5
        case '1':
            if (GetKeyState(VK_MENU) < 0)
            {
                showTabDialog(0);
                sFuncKey = XPR_TRUE;
            }
            break;

        case '2':
            if (GetKeyState(VK_MENU) < 0)
            {
                showTabDialog(1);
                sFuncKey = XPR_TRUE;
            }
            break;

        case '3':
            if (GetKeyState(VK_MENU) < 0)
            {
                showTabDialog(2);
                sFuncKey = XPR_TRUE;
            }
            break;

        case '4':
            if (GetKeyState(VK_MENU) < 0)
            {
                showTabDialog(3);
                sFuncKey = XPR_TRUE;
            }
            break;

        case '5':
            if (GetKeyState(VK_MENU) < 0)
            {
                showTabDialog(4);
                sFuncKey = XPR_TRUE;
            }
            break;
        }

        if (sFuncKey == XPR_TRUE)
            return XPR_TRUE;
    }

    return XPR_FALSE;
}

xpr_bool_t BatchRenameDlg::PreTranslateMessage(MSG* pMsg)
{
    if (translateHotKey(pMsg))
        return XPR_TRUE;

    if (pMsg->hwnd)
    {
        if (::GetDlgCtrlID(pMsg->hwnd) == IDC_BATCH_RENAME_LISTCTRL)
        {
            if (pMsg->message == WM_KEYDOWN)
            {
                if (pMsg->wParam == VK_F2)
                {
                    if (GetKeyState(VK_CONTROL) >= 0 && GetKeyState(VK_SHIFT) >= 0 && GetKeyState(VK_MENU) >= 0)
                    {
                        OnItemEdit();
                        return XPR_TRUE;
                    }
                }
            }
        }
    }

    return super::PreTranslateMessage(pMsg);
}

void BatchRenameDlg::OnCancel(void)
{
    if (mRenaming == XPR_TRUE)
        return;

    super::OnCancel();
}

void BatchRenameDlg::OnItemActivate(NMHDR* pNMHDR, LRESULT* pResult)
{
    LPNMITEMACTIVATE sNmItemActivate = (LPNMITEMACTIVATE)pNMHDR;
    *pResult = 0;

    xpr_sint_t sIndex = sNmItemActivate->iItem;
    xpr_sint_t sSubIndex = sNmItemActivate->iSubItem;

    editItem(sIndex);
}

void BatchRenameDlg::OnItemEdit(void)
{
    editItem();
}

void BatchRenameDlg::editItem(xpr_sint_t aIndex)
{
    if (aIndex == -1)
        aIndex = mListCtrl.GetSelectionMark();

    if (aIndex < 0 || mListCtrl.GetSelectedCount() <= 0)
        return;

    BatchRename::Item *sBatchRenameItem = mBatchRename->getItem(aIndex);
    if (sBatchRenameItem == XPR_NULL)
        return;

    xpr_bool_t sNoChangeExt = mBatchRename->isFlag(BatchRename::FlagNoChangeExt);

    xpr::tstring sExt;
    xpr::tstring sName;

    sName = sBatchRenameItem->mOld;
    if (sNoChangeExt == XPR_TRUE)
    {
        const xpr_tchar_t *sFindedExt = (const xpr_tchar_t *)GetFileExt(sName);
        if (sFindedExt != XPR_NULL)
            sName.erase(sName.length() - _tcslen(sFindedExt));
    }

    xpr_tchar_t sDesc[0xff] = {0};
    _stprintf(sDesc, gApp.loadFormatString(XPR_STRING_LITERAL("popup.batch_rename_input.label.desc"), XPR_STRING_LITERAL("%s")), sName.c_str());

    sName = sBatchRenameItem->mNew;
    if (sNoChangeExt == XPR_TRUE)
    {
        const xpr_tchar_t *sFindedExt = (const xpr_tchar_t *)GetFileExt(sName);
        if (sFindedExt != XPR_NULL)
        {
            sExt = sFindedExt;
            sName.erase(sName.length() - _tcslen(sFindedExt));
        }
    }

    InputDlg sDlg;
    sDlg.setProfile(XPR_STRING_LITERAL("Rename-DirectInput"));
    sDlg.setTitle(gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename_input.title")));
    sDlg.setDesc(sDesc);
    sDlg.setText(sName.c_str());
    sDlg.setFileRename();

    if (sDlg.DoModal() == IDOK)
    {
        sName  = sDlg.getText();
        sName += sExt;

        sBatchRenameItem->mNew = sName;

        mListCtrl.Invalidate(XPR_FALSE);
        mListCtrl.UpdateWindow();
    }
}

void BatchRenameDlg::OnHistoryArchive(void)
{
    xpr_bool_t sHistoryArchive = mBatchRename->isFlag(BatchRename::FlagHistoryArchive);

    xpr_uint_t sFlags = mBatchRename->getFlags();
    XPR_SET_OR_CLR_BITS(sFlags, BatchRename::FlagHistoryArchive, sHistoryArchive);

    mBatchRename->setFlags(sFlags);
}

void BatchRenameDlg::OnNoChangeExt(void)
{
    xpr_bool_t sNoChangeExt = mBatchRename->isFlag(BatchRename::FlagNoChangeExt);

    xpr_uint_t sFlags = mBatchRename->getFlags();
    XPR_SET_OR_CLR_BITS(sFlags, BatchRename::FlagNoChangeExt, sNoChangeExt);

    mBatchRename->setFlags(sFlags);
}

void BatchRenameDlg::OnResult(void)
{
    xpr_bool_t sResultRename = mBatchRename->isFlag(BatchRename::FlagResultRename);

    xpr_uint_t sFlags = mBatchRename->getFlags();
    XPR_SET_OR_CLR_BITS(sFlags, BatchRename::FlagResultRename, sResultRename);

    mBatchRename->setFlags(sFlags);
}

void BatchRenameDlg::OnUpdateBatchFormatArchive(CCmdUI *pCmdUI)
{
    pCmdUI->Enable(!mRenaming);
    pCmdUI->SetCheck(mBatchRename->isFlag(BatchRename::FlagHistoryArchive));
}

void BatchRenameDlg::OnUpdateNoChangeExt(CCmdUI *pCmdUI)
{
    pCmdUI->Enable(!mRenaming);
    pCmdUI->SetCheck(mBatchRename->isFlag(BatchRename::FlagNoChangeExt));
}

void BatchRenameDlg::OnUpdateResult(CCmdUI *pCmdUI)
{
    pCmdUI->Enable(!mRenaming);
    pCmdUI->SetCheck(mBatchRename->isFlag(BatchRename::FlagResultRename));
}

void BatchRenameDlg::OnEdit(void)
{
    BatchRenameEditDlg sDlg;

    xpr_size_t i;
    xpr_size_t sCount;
    BatchRename::Item *sBatchRenameItem;

    sCount = mBatchRename->getCount();
    for (i = 0; i < sCount; ++i)
    {
        sBatchRenameItem = mBatchRename->getItem(i);
        if (sBatchRenameItem == XPR_NULL)
            continue;

        sDlg.add(sBatchRenameItem->mOld.c_str(), sBatchRenameItem->mNew.c_str());
    }

    if (sDlg.DoModal() == IDOK)
    {
        xpr_size_t sCount2 = sDlg.getNewCount();
        if (sCount2 >= sCount)
        {
            for (i = 0; i < sCount; ++i)
                mBatchRename->setItemName(i, sDlg.getNewName((xpr_sint_t)i));

            mListCtrl.Invalidate(XPR_FALSE);
            mListCtrl.UpdateWindow();
        }
    }
}

void BatchRenameDlg::OnItemUp(void)
{
    xpr_sint_t sIndex = mListCtrl.GetSelectionMark();
    moveItem(sIndex, sIndex - 1);
}

void BatchRenameDlg::OnItemDown(void)
{
    xpr_sint_t sIndex = mListCtrl.GetSelectionMark();
    moveItem(sIndex, sIndex + 1);
}

void BatchRenameDlg::OnItemMove(void)
{
    xpr_sint_t sIndex = mListCtrl.GetSelectionMark();
    if (sIndex < 0)
        return;

    xpr_tchar_t sText[0xff] = {0};
    _stprintf(sText, XPR_STRING_LITERAL("%d"), sIndex + 1);

    InputDlg sDlg;
    sDlg.setTitle(gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename_move.title")));
    sDlg.setDesc(gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename_move.label.desc")));
    sDlg.setText(sText);
    if (sDlg.DoModal() == IDOK)
    {
        _tcscpy(sText, sDlg.getText());

        xpr_sint_t sNewIndex = 0;
        _stscanf(sText, XPR_STRING_LITERAL("%d"), &sNewIndex);
        sNewIndex--;

        moveItem(sIndex, sNewIndex);
    }
}

void BatchRenameDlg::moveItem(xpr_sint_t aIndex, xpr_sint_t aNewIndex)
{
    if (aIndex == aNewIndex)
        return;

    xpr_sint_t sCount = mListCtrl.GetItemCount();
    if (!XPR_IS_RANGE(0, aIndex, sCount-1) || !XPR_IS_RANGE(0, aNewIndex, sCount-1))
        return;

    BatchRename::Item *sBatchRenameItem1 = (BatchRename::Item *)mListCtrl.GetItemData(aIndex);
    BatchRename::Item *sBatchRenameItem2 = (BatchRename::Item *)mListCtrl.GetItemData(aNewIndex);

    if (!mBatchRename->moveItem(aIndex, aNewIndex))
        return;

    mListCtrl.SetItemData(aIndex,    (DWORD_PTR)sBatchRenameItem2);
    mListCtrl.SetItemData(aNewIndex, (DWORD_PTR)sBatchRenameItem1);

    mListCtrl.Invalidate(XPR_FALSE);
    mListCtrl.UpdateWindow();

    mListCtrl.SetRedraw(XPR_FALSE);

    mListCtrl.SetFocus();
    mListCtrl.EnsureVisible(aNewIndex, XPR_TRUE);
    mListCtrl.SetSelectionMark(aNewIndex);
    mListCtrl.SetItemState(aNewIndex, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);

    mListCtrl.SetRedraw();
}

void BatchRenameDlg::OnUpdateItemUp(CCmdUI *pCmdUI)
{
    xpr_bool_t sEnable = XPR_FALSE;
    CWnd *sWnd = GetFocus();
    if ((sWnd != XPR_NULL) && (sWnd->m_hWnd == mListCtrl.m_hWnd))
    {
        sEnable = mListCtrl.GetSelectionMark() >= 0;
    }

    pCmdUI->Enable(mRenaming == XPR_FALSE && sEnable);
}

void BatchRenameDlg::OnUpdateItemDown(CCmdUI *pCmdUI)
{
    xpr_bool_t sEnable = XPR_FALSE;
    CWnd *sWnd = GetFocus();
    if ((sWnd != XPR_NULL) && (sWnd->m_hWnd == mListCtrl.m_hWnd))
    {
        sEnable = mListCtrl.GetSelectionMark() >= 0;
    }

    pCmdUI->Enable(mRenaming == XPR_FALSE && sEnable);
}

void BatchRenameDlg::OnUpdateItemMove(CCmdUI *pCmdUI)
{
    xpr_bool_t sEnable = XPR_FALSE;
    CWnd *sWnd = GetFocus();
    if ((sWnd != XPR_NULL) && (sWnd->m_hWnd == mListCtrl.m_hWnd))
    {
        sEnable = mListCtrl.GetSelectionMark() >= 0;
    }

    pCmdUI->Enable(mRenaming == XPR_FALSE && sEnable);
}

LRESULT BatchRenameDlg::OnApply(WPARAM wParam, LPARAM lParam)
{
    xpr_size_t sIndex = (xpr_size_t)wParam;

    xpr_size_t sCount = mTabDeque.size();
    if (1 <= sIndex && sIndex <= sCount)
    {
        switch (sIndex)
        {
        case 1: OnFormatApply();  break;
        case 2: OnReplaceApply(); break;
        case 3: OnInsertApply();  break;
        case 4: OnDeleteApply();  break;
        case 5: OnCaseApply();    break;
        }
    }

    return 0;
}

void BatchRenameDlg::OnFormatApply(void)
{
    BatchRenameTabInsertDlg *sDlg = (BatchRenameTabInsertDlg *)getTabDialog(0);

    XPR_ASSERT(sDlg != XPR_NULL);

    xpr_tchar_t sFormat[FXFILE_BATCH_RENAME_FORMAT_MAX_LENGTH + 1] = {0};
    sDlg->GetDlgItemText(IDC_BATCH_RENAME_FORMAT, sFormat, FXFILE_BATCH_RENAME_FORMAT_MAX_LENGTH);

    xpr_sint_t sNumber   = sDlg->GetDlgItemInt(IDC_BATCH_RENAME_FORMAT_NUMBER);
    xpr_sint_t sIncrease = sDlg->GetDlgItemInt(IDC_BATCH_RENAME_FORMAT_INCREASE);

    if (sIncrease == 0)
    {
        sDlg->SetDlgItemInt(IDC_BATCH_RENAME_FORMAT_INCREASE, 1, XPR_FALSE);
        sIncrease = 1;
    }

    if (_tcslen(sFormat) == 0)
    {
        // no change
        return;
    }

    BatchRename::Result sResult = mBatchRename->renameFormat(sFormat, sNumber, sIncrease);
    if (sResult == BatchRename::ResultWrongFormat)
    {
        const xpr_tchar_t *sMsg = gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename.msg.wrong_format"));
        MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);

        sDlg->GetDlgItem(IDC_BATCH_RENAME_FORMAT)->SetFocus();
        return;
    }

    mListCtrl.Invalidate(XPR_FALSE);
    mListCtrl.UpdateWindow();
}

void BatchRenameDlg::OnReplaceApply(void)
{
    BatchRenameTabReplaceDlg *sDlg = (BatchRenameTabReplaceDlg *)getTabDialog(1);

    XPR_ASSERT(sDlg != XPR_NULL);

    xpr_tchar_t sOld[FXFILE_BATCH_RENAME_REPLACE_OLD_MAX_LENGTH + 1] = {0};
    xpr_tchar_t sNew[FXFILE_BATCH_RENAME_REPLACE_NEW_MAX_LENGTH + 1] = {0};
    sDlg->GetDlgItemText(IDC_BATCH_RENAME_REPLACE_OLD, sOld, FXFILE_BATCH_RENAME_REPLACE_OLD_MAX_LENGTH);
    sDlg->GetDlgItemText(IDC_BATCH_RENAME_REPLACE_NEW, sNew, FXFILE_BATCH_RENAME_REPLACE_NEW_MAX_LENGTH);

    xpr_sint_t sRepeat  = sDlg->GetDlgItemInt(IDC_BATCH_RENAME_REPLACE_REPEAT);
    xpr_bool_t sNotCase = ((CButton *)sDlg->GetDlgItem(IDC_BATCH_RENAME_REPLACE_NOTCASE))->GetCheck();

    if (VerifyFileName(sOld) == XPR_FALSE || _tcslen(sOld) == 0)
    {
        const xpr_tchar_t *sMsg = gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename.msg.invalid_filename"));
        MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);

        sDlg->GetDlgItem(IDC_BATCH_RENAME_REPLACE_OLD)->SetFocus();
        return;
    }

    if (VerifyFileName(sNew) == XPR_FALSE)
    {
        const xpr_tchar_t *sMsg = gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename.msg.invalid_filename"));
        MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);

        sDlg->GetDlgItem(IDC_BATCH_RENAME_REPLACE_NEW)->SetFocus();
        return;
    }

    mBatchRename->renameReplace(sOld, sNew, sRepeat, sNotCase ? XPR_FALSE : XPR_TRUE);

    mListCtrl.Invalidate(XPR_FALSE);
    mListCtrl.UpdateWindow();
}

void BatchRenameDlg::OnInsertApply(void)
{
    BatchRenameTabInsertDlg *sDlg = (BatchRenameTabInsertDlg *)getTabDialog(2);

    XPR_ASSERT(sDlg != XPR_NULL);

    xpr_tchar_t sInsert[FXFILE_BATCH_RENAME_INSERT_MAX_LENGTH + 1] = {0};
    sDlg->GetDlgItemText(IDC_BATCH_RENAME_INSERT_STRING, sInsert, FXFILE_BATCH_RENAME_INSERT_MAX_LENGTH);

    xpr_sint_t sType = ((CComboBox *)sDlg->GetDlgItem(IDC_BATCH_RENAME_INSERT_TYPE))->GetCurSel();
    xpr_sint_t sPos  = sDlg->GetDlgItemInt(IDC_BATCH_RENAME_INSERT_POS);

    if (VerifyFileName(sInsert) == XPR_FALSE || _tcslen(sInsert) == 0)
    {
        const xpr_tchar_t *sMsg = gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename.msg.invalid_filename"));
        MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);

        sDlg->GetDlgItem(IDC_BATCH_RENAME_INSERT_STRING)->SetFocus();
        return;
    }

    mBatchRename->renameInsert((Format::InsertPosType)sType, sPos, sInsert);

    mListCtrl.Invalidate(XPR_FALSE);
    mListCtrl.UpdateWindow();
}

void BatchRenameDlg::OnDeleteApply(void)
{
    BatchRenameTabDeleteDlg *sDlg = (BatchRenameTabDeleteDlg *)getTabDialog(3);

    XPR_ASSERT(sDlg != XPR_NULL);

    xpr_sint_t sType   = ((CComboBox *)sDlg->GetDlgItem(IDC_BATCH_RENAME_DELETE_TYPE))->GetCurSel();
    xpr_sint_t sPos    = sDlg->GetDlgItemInt(IDC_BATCH_RENAME_DELETE_POS);
    xpr_sint_t sLength = sDlg->GetDlgItemInt(IDC_BATCH_RENAME_DELETE_LENGTH);

    mBatchRename->renameDelete((Format::DeletePosType)sType, sPos, sLength);

    mListCtrl.Invalidate(XPR_FALSE);
    mListCtrl.UpdateWindow();
}

void BatchRenameDlg::OnCaseApply(void)
{
    BatchRenameTabCaseDlg *sDlg = (BatchRenameTabCaseDlg *)getTabDialog(4);

    XPR_ASSERT(sDlg != XPR_NULL);

    xpr_sint_t sType = ((CComboBox *)sDlg->GetDlgItem(IDC_BATCH_RENAME_CASE_TYPE))->GetCurSel();
    xpr_sint_t sCase = ((CComboBox *)sDlg->GetDlgItem(IDC_BATCH_RENAME_CASE))->GetCurSel();

    xpr_tchar_t sSkipSpecChar[FXFILE_BATCH_RENAME_CASE_SKIP_CHARS_MAX_LENGTH + 1];
    sDlg->GetDlgItemText(IDC_BATCH_RENAME_SKIP_SPEC_CHAR, sSkipSpecChar, FXFILE_BATCH_RENAME_CASE_SKIP_CHARS_MAX_LENGTH);

    mBatchRename->renameCase((Format::CaseTargetType)sType, (Format::CaseType)sCase, sSkipSpecChar);

    mListCtrl.Invalidate(XPR_FALSE);
    mListCtrl.UpdateWindow();
}

xpr_bool_t BatchRenameDlg::OnToolTipNotify(xpr_uint_t id, NMHDR *pNMHDR, LRESULT *pResult)
{
    LPTOOLTIPTEXT sTooltipText = (LPTOOLTIPTEXT)pNMHDR;
    *pResult = 0;

    if (sTooltipText->szText == XPR_NULL)
        return XPR_TRUE;

    xpr_uint_t sId = (xpr_uint_t)pNMHDR->idFrom;
    if ((pNMHDR->code == TTN_NEEDTEXT) && (sTooltipText->uFlags & TTF_IDISHWND))
        sId = ::GetDlgCtrlID((HWND)(uintptr_t)sId);

    switch (sId)
    {
    case IDC_BATCH_RENAME_INIT:            _tcscpy(sTooltipText->szText, gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename.toolbar.initialize")));       break;
    case IDC_BATCH_RENAME_UNDO:            _tcscpy(sTooltipText->szText, gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename.toolbar.undo")));             break;
    case IDC_BATCH_RENAME_REDO:            _tcscpy(sTooltipText->szText, gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename.toolbar.redo")));             break;
    case IDC_BATCH_RENAME_ITEM_UP:         _tcscpy(sTooltipText->szText, gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename.toolbar.move_up")));          break;
    case IDC_BATCH_RENAME_ITEM_DOWN:       _tcscpy(sTooltipText->szText, gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename.toolbar.move_down")));        break;
    case IDC_BATCH_RENAME_ITEM_MOVE:       _tcscpy(sTooltipText->szText, gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename.toolbar.move")));             break;
    case IDC_BATCH_RENAME_ON_RESULT:       _tcscpy(sTooltipText->szText, gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename.toolbar.rename_by_result"))); break;
    case IDC_BATCH_RENAME_NO_CHANGE_EXT:   _tcscpy(sTooltipText->szText, gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename.toolbar.no_change_ext")));    break;
    case IDC_BATCH_RENAME_HISTORY_ARCHIVE: _tcscpy(sTooltipText->szText, gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename.toolbar.history_archive")));  break;
    case IDC_BATCH_RENAME_LOAD:            _tcscpy(sTooltipText->szText, gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename.toolbar.load")));             break;
    case IDC_BATCH_RENAME_SAVE:            _tcscpy(sTooltipText->szText, gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename.toolbar.save")));             break;

    default: sTooltipText->szText[0] = '\0'; break;
    }

    return XPR_TRUE;
}
} // namespace cmd
} // namespace fxfile
