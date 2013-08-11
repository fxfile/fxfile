//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "cfg_main_dlg.h"

#include "../resource.h"
#include "../option_manager.h"

#include "conf_dir.h"
#include "cfg_general_dlg.h"
#include "cfg_general_startup_dlg.h"
#include "cfg_appearance_dlg.h"
#include "cfg_appearance_layout_dlg.h"
#include "cfg_appearance_font_dlg.h"
#include "cfg_appearance_color_dlg.h"
#include "cfg_appearance_file_list_dlg.h"
#include "cfg_appearance_view_set_dlg.h"
#include "cfg_appearance_filtering_dlg.h"
#include "cfg_appearance_thumbnail_dlg.h"
#include "cfg_appearance_history_dlg.h"
#include "cfg_appearance_size_format_dlg.h"
#include "cfg_appearance_folder_tree_dlg.h"
#include "cfg_appearance_language_dlg.h"
#include "cfg_func_dlg.h"
#include "cfg_func_bookmark_dlg.h"
#include "cfg_func_rename_dlg.h"
#include "cfg_func_refresh_dlg.h"
#include "cfg_func_drag_dlg.h"
#include "cfg_func_program_ass_dlg.h"
#include "cfg_func_file_scrap_dlg.h"
#include "cfg_func_file_op_dlg.h"
#include "cfg_adv_dlg.h"
#include "cfg_adv_conf_dir_dlg.h"
#include "cfg_adv_hotkey_dlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace fxfile
{
namespace cfg
{
xpr_sint_t CfgMainDlg::mLastShowCfg = 0;

CfgMainDlg::CfgMainDlg(xpr_sint_t aInitShowCfg)
    : super(IDD_CFG_MAIN, XPR_NULL)
    , mInitShowCfg(aInitShowCfg)
    , mShowCfg(-1)
    , mChanging(XPR_FALSE)
{
    if (mInitShowCfg == -1)
        mInitShowCfg = mLastShowCfg;
}

void CfgMainDlg::DoDataExchange(CDataExchange* pDX)
{
    super::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_CFG_TREE, mTreeCtrl);
}

BEGIN_MESSAGE_MAP(CfgMainDlg, super)
    ON_WM_CTLCOLOR()
    ON_WM_DESTROY()
    ON_NOTIFY(TVN_SELCHANGED, IDC_CFG_TREE, OnSelchangedTree)
    ON_BN_CLICKED(IDC_CFG_DEFAULT, OnDefault)
    ON_BN_CLICKED(IDC_CFG_APPLY,   OnApply)
END_MESSAGE_MAP()

xpr_bool_t CfgMainDlg::OnInitDialog(void) 
{
    super::OnInitDialog();

    mTreeCtrl.enableVistaEnhanced(XPR_TRUE);

    mDarkGrayBrush.CreateSolidBrush(RGB(127,127,127));

    CBitmap sBitmap;
    sBitmap.LoadBitmap(IDB_CONFIG_TREE);
    mImgList.Create(16, 16, ILC_COLORDDB | ILC_MASK, 18, 18);
    mImgList.Add(&sBitmap, RGB(255,0,255));
    sBitmap.DeleteObject();

    mTreeCtrl.SetImageList(&mImgList, TVSIL_NORMAL);
    mTreeCtrl.SetItemHeight(mTreeCtrl.GetItemHeight() + 2);

    // copy config
    gOpt->copyConfig(mNewConfig);

    // initialize configuration dialog
    xpr_sint_t sIndex;
    sIndex = addCfgItem( 0,     -1, new CfgGeneralDlg,              theApp.loadString(XPR_STRING_LITERAL("popup.cfg.tree.general")));
    /*    */ addCfgItem(31, sIndex, new CfgGeneralStartupDlg,       theApp.loadString(XPR_STRING_LITERAL("popup.cfg.tree.general.startup")));
    sIndex = addCfgItem(18,     -1, new CfgAppearanceDlg,           theApp.loadString(XPR_STRING_LITERAL("popup.cfg.tree.appearance")));
    /*    */ addCfgItem(29, sIndex, new CfgAppearanceLayoutDlg,     theApp.loadString(XPR_STRING_LITERAL("popup.cfg.tree.appearance.layout")));
    /*    */ addCfgItem(30, sIndex, new CfgAppearanceFontDlg,       theApp.loadString(XPR_STRING_LITERAL("popup.cfg.tree.appearance.font")));
    /*    */ addCfgItem(28, sIndex, new CfgAppearanceColorDlg,      theApp.loadString(XPR_STRING_LITERAL("popup.cfg.tree.appearance.color")));
    /*    */ addCfgItem(12, sIndex, new CfgAppearanceFileListDlg,   theApp.loadString(XPR_STRING_LITERAL("popup.cfg.tree.appearance.file_list")));
    /*    */ addCfgItem(20, sIndex, new CfgAppearanceViewSetDlg,    theApp.loadString(XPR_STRING_LITERAL("popup.cfg.tree.appearance.view_set")));
    /*    */ addCfgItem(11, sIndex, new CfgAppearanceFilteringDlg,  theApp.loadString(XPR_STRING_LITERAL("popup.cfg.tree.appearance.filtering")));
    /*    */ addCfgItem( 5, sIndex, new CfgAppearanceThumbnailDlg,  theApp.loadString(XPR_STRING_LITERAL("popup.cfg.tree.appearance.thumbnail")));
    /*    */ addCfgItem(25, sIndex, new CfgAppearanceHistoryDlg,    theApp.loadString(XPR_STRING_LITERAL("popup.cfg.tree.appearance.history")));
    /*    */ addCfgItem(22, sIndex, new CfgAppearanceSizeFormatDlg, theApp.loadString(XPR_STRING_LITERAL("popup.cfg.tree.appearance.size_format")));
    /*    */ addCfgItem( 3, sIndex, new CfgAppearanceFolderTreeDlg, theApp.loadString(XPR_STRING_LITERAL("popup.cfg.tree.appearance.folder_tree")));
    /*    */ addCfgItem(27, sIndex, new CfgAppearanceLanguageDlg,   theApp.loadString(XPR_STRING_LITERAL("popup.cfg.tree.appearance.language")));
    sIndex = addCfgItem(17,     -1, new CfgFuncDlg,                 theApp.loadString(XPR_STRING_LITERAL("popup.cfg.tree.function")));
    /*    */ addCfgItem(13, sIndex, new CfgFuncBookmarkDlg,         theApp.loadString(XPR_STRING_LITERAL("popup.cfg.tree.function.bookmark")));
    /*    */ addCfgItem( 7, sIndex, new CfgFuncRenameDlg,           theApp.loadString(XPR_STRING_LITERAL("popup.cfg.tree.function.rename")));
    /*    */ addCfgItem(14, sIndex, new CfgFuncRefreshDlg,          theApp.loadString(XPR_STRING_LITERAL("popup.cfg.tree.function.refresh")));
    /*    */ addCfgItem( 8, sIndex, new CfgFuncDragDlg,             theApp.loadString(XPR_STRING_LITERAL("popup.cfg.tree.function.drag_drop")));
    /*    */ addCfgItem(21, sIndex, new CfgFuncProgramAssDlg,       theApp.loadString(XPR_STRING_LITERAL("popup.cfg.tree.function.program_ass")));
    /*    */ addCfgItem(16, sIndex, new CfgFuncFileScrapDlg,        theApp.loadString(XPR_STRING_LITERAL("popup.cfg.tree.function.file_scrap")));
    /*    */ addCfgItem(26, sIndex, new CfgFuncFileOpDlg,           theApp.loadString(XPR_STRING_LITERAL("popup.cfg.tree.function.file_operation")));
    sIndex = addCfgItem( 6,     -1, new CfgAdvDlg,                  theApp.loadString(XPR_STRING_LITERAL("popup.cfg.tree.advanced")));
    /*    */ addCfgItem(23, sIndex, new CfgAdvConfDirDlg,           theApp.loadString(XPR_STRING_LITERAL("popup.cfg.tree.advanced.conf_dir")));
    /*    */ addCfgItem(24, sIndex, new CfgAdvHotKeyDlg,            theApp.loadString(XPR_STRING_LITERAL("popup.cfg.tree.advanced.global_hotkey")));

    if (!FXFILE_STL_IS_INDEXABLE(mInitShowCfg, mCfgDeque))
        mInitShowCfg = 0;

    // expand all
    HTREEITEM sTreeItem = mTreeCtrl.GetRootItem();
    while (sTreeItem != XPR_NULL)
    {
        if (mTreeCtrl.ItemHasChildren(sTreeItem) == XPR_TRUE)
            mTreeCtrl.Expand(sTreeItem, TVE_EXPAND);

        sTreeItem = mTreeCtrl.GetNextSiblingItem(sTreeItem);
    }

    mTreeCtrl.SelectItem(mCfgDeque[mInitShowCfg]->mTreeItem);
    mTreeCtrl.EnsureVisible(mCfgDeque[mInitShowCfg]->mTreeItem);

    GetDlgItem(IDC_CFG_APPLY)->EnableWindow(XPR_FALSE);
    mCfgDeque[mInitShowCfg]->mModified = XPR_FALSE;

    SetWindowText(theApp.loadString(XPR_STRING_LITERAL("popup.cfg.title")));
    SetDlgItemText(IDC_CFG_DEFAULT, theApp.loadString(XPR_STRING_LITERAL("popup.cfg.button.default")));
    SetDlgItemText(IDC_CFG_APPLY,   theApp.loadString(XPR_STRING_LITERAL("popup.cfg.button.apply")));
    SetDlgItemText(IDOK,            theApp.loadString(XPR_STRING_LITERAL("popup.common.button.ok")));
    SetDlgItemText(IDCANCEL,        theApp.loadString(XPR_STRING_LITERAL("popup.common.button.cancel")));

    return XPR_TRUE;
}

xpr_sint_t CfgMainDlg::addCfgItem(xpr_sint_t aImage, xpr_sint_t aParent, CfgDlg *aCfgDlg, const xpr_tchar_t *aText)
{
    CfgItem *sCfgItem = new CfgItem;
    if (sCfgItem == XPR_NULL)
        return -1;

    if (!FXFILE_STL_IS_INDEXABLE(aParent, mCfgDeque))
        aParent = -1;

    HTREEITEM sParentTreeItem = TVI_ROOT;
    if (aParent != -1)
        sParentTreeItem = mCfgDeque[aParent]->mTreeItem;

    sCfgItem->mTreeItem = mTreeCtrl.InsertItem(aText, aImage, aImage, sParentTreeItem);
    sCfgItem->mText     = aText;
    sCfgItem->mImage    = aImage;
    sCfgItem->mParent   = aParent;
    sCfgItem->mCfgDlg   = aCfgDlg;
    sCfgItem->mModified = XPR_FALSE;

    mCfgDeque.push_back(sCfgItem);

    return (xpr_sint_t)mCfgDeque.size() - 1;
}

xpr_bool_t CfgMainDlg::showCfg(xpr_sint_t aCfgIndex)
{
    if (!FXFILE_STL_IS_INDEXABLE(aCfgIndex, mCfgDeque))
        return XPR_FALSE;

    CfgItem *sCfgItem = mCfgDeque[aCfgIndex];

    // hide
    if (mShowCfg != -1)
    {
        if (FXFILE_STL_IS_INDEXABLE(mShowCfg, mCfgDeque))
        {
            CfgItem *sOldCfgItem = mCfgDeque[mShowCfg];

            if (sOldCfgItem->mCfgDlg->m_hWnd != XPR_NULL)
                sOldCfgItem->mCfgDlg->ShowWindow(SW_HIDE);
        }
    }

    // show
    if (sCfgItem->mCfgDlg->m_hWnd == XPR_NULL)
    {
        sCfgItem->mCfgDlg->setObserver(dynamic_cast<CfgDlgObserver *>(this));
        sCfgItem->mCfgDlg->Create(aCfgIndex, this);
        sCfgItem->mCfgDlg->onInit(mNewConfig);

        CRect sDescRect;
        GetDlgItem(IDC_CFG_DESC)->GetWindowRect(&sDescRect);
        ScreenToClient(&sDescRect);

        HDC sDC = ::GetDC(XPR_NULL);
        xpr_sint_t y = MulDiv(44, GetDeviceCaps(sDC , LOGPIXELSY), 96);
        ::ReleaseDC(XPR_NULL, sDC);

        sCfgItem->mCfgDlg->SetWindowPos(XPR_NULL, sDescRect.left, y, 0, 0, SWP_NOSIZE);
    }

    sCfgItem->mCfgDlg->ShowWindow(SW_SHOW);

    // description (e.g. Configuration > Function > Bookmark)
    xpr::tstring sDesc(XPR_STRING_LITERAL("  "));
    xpr_sint_t sParent = sCfgItem->mParent;

    sDesc += theApp.loadString(XPR_STRING_LITERAL("popup.cfg.title"));

    xpr_size_t sInsertPos = sDesc.length();
    while (sParent != -1)
    {
        if (!FXFILE_STL_IS_INDEXABLE(sParent, mCfgDeque))
            break;

        sDesc.insert(sInsertPos, mCfgDeque[sParent]->mText);
        sDesc.insert(sInsertPos, XPR_STRING_LITERAL(" > "));

        sParent = mCfgDeque[sParent]->mParent;
    }

    sDesc += XPR_STRING_LITERAL(" > ");
    sDesc += sCfgItem->mText;

    SetDlgItemText(IDC_CFG_DESC, sDesc.c_str());

    UpdateWindow();

    return XPR_TRUE;
}

void CfgMainDlg::OnDestroy(void)
{
    super::OnDestroy();

    CfgItem *sCfgItem;
    CfgDeque::iterator sIterator;

    sIterator = mCfgDeque.begin();
    for (; sIterator != mCfgDeque.end(); ++sIterator)
    {
        sCfgItem = *sIterator;
        if (sCfgItem == XPR_NULL)
            continue;

        if (sCfgItem->mCfgDlg->m_hWnd != XPR_NULL)
            sCfgItem->mCfgDlg->DestroyWindow();

        XPR_SAFE_DELETE(sCfgItem->mCfgDlg);
        XPR_SAFE_DELETE(sCfgItem);
    }

    mCfgDeque.clear();
}

void CfgMainDlg::showCfgByTree(xpr_sint_t aCfgIndex)
{
    if (!FXFILE_STL_IS_INDEXABLE(aCfgIndex, mCfgDeque))
        return;

    mChanging = XPR_TRUE;

    showCfg(aCfgIndex);

    mChanging = XPR_FALSE;
}

void CfgMainDlg::OnOK(void) 
{
    OnApply();

    super::OnOK();
}

void CfgMainDlg::OnCancel(void) 
{
    super::OnCancel();
}

void CfgMainDlg::onSetModified(CfgDlg &aCfgDlg, xpr_bool_t aModified)
{
    if (mChanging == XPR_TRUE)
        return;

    xpr_size_t sCfgIndex = aCfgDlg.getCfgIndex();

    onSetModified(sCfgIndex, aModified);
}

void CfgMainDlg::onSetModified(xpr_size_t aCfgIndex, xpr_bool_t aModified)
{
    if (mChanging == XPR_TRUE)
        return;

    xpr_size_t sCount = mCfgDeque.size();
    if (FXFILE_STL_IS_INDEXABLE(aCfgIndex, mCfgDeque))
    {
        mCfgDeque[aCfgIndex]->mModified = aModified;

        xpr_size_t i;
        CfgItem *sCfgItem;
        CfgDeque::iterator sIterator;
        xpr_bool_t sEnable = XPR_FALSE;

        sIterator = mCfgDeque.begin();
        for (i = 0; sIterator != mCfgDeque.end(); ++sIterator, ++i)
        {
            sCfgItem = *sIterator;
            if (sCfgItem == XPR_NULL)
                continue;

            if (sCfgItem->mModified == XPR_TRUE)
            {
                sEnable = XPR_TRUE;
                break;
            }
        }

        GetDlgItem(IDC_CFG_APPLY)->EnableWindow(sEnable);
    }
}

xpr_bool_t CfgMainDlg::onIsModified(CfgDlg &aCfgDlg)
{
    xpr_size_t sCfgIndex = aCfgDlg.getCfgIndex();

    if (!FXFILE_STL_IS_INDEXABLE(sCfgIndex, mCfgDeque))
        return 0;

    return mCfgDeque[sCfgIndex]->mModified;
}

void CfgMainDlg::OnSelchangedTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
    NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
    *pResult = 0;

    HTREEITEM sTreeItem = pNMTreeView->itemNew.hItem;

    xpr_sint_t i;
    CfgItem *sCfgItem;
    CfgDeque::iterator sIterator;

    sIterator = mCfgDeque.begin();
    for (i = 0; sIterator != mCfgDeque.end(); ++sIterator, ++i)
    {
        sCfgItem = *sIterator;
        if (sCfgItem == XPR_NULL)
            continue;

        if (sTreeItem == sCfgItem->mTreeItem)
        {
            showCfgByTree(i);
            break;
        }
    }

    mShowCfg = i;
    mLastShowCfg = mShowCfg;
}

HBRUSH CfgMainDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, xpr_uint_t nCtlColor)
{
    HBRUSH sBrush = super::OnCtlColor(pDC, pWnd, nCtlColor);

    if (pWnd != XPR_NULL)
    {
        switch (pWnd->GetDlgCtrlID())
        {
        case IDC_CFG_DESC:
            {
                pDC->SetTextColor(RGB(255,255,255));
                pDC->SetBkMode(TRANSPARENT);
                return mDarkGrayBrush;
            }
        }
    }

    return sBrush;
}

void CfgMainDlg::OnDefault(void)
{
    const xpr_tchar_t *sMsg = theApp.loadString(XPR_STRING_LITERAL("popup.cfg.msg.confirm_default"));
    xpr_sint_t sMsgId = MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONWARNING | MB_YESNO);
    if (sMsgId != IDYES)
        return;

    ConfDir &sConfDir = ConfDir::instance();
    OptionManager &sOptionManager = OptionManager::instance();

    // save configuration path to save configuration files
    sConfDir.save();

    // initialize default option and save one
    sOptionManager.initDefault();

    // save default option
    sOptionManager.saveConfigOption();

    // notify default option
    gOpt->notifyConfig();

    xpr_sint_t i;
    CfgItem *sCfgItem;
    CfgDeque::iterator sIterator;

    sIterator = mCfgDeque.begin();
    for (i = 0; sIterator != mCfgDeque.end(); ++sIterator, ++i)
    {
        sCfgItem = *sIterator;
        if (sCfgItem == XPR_NULL)
            continue;

        if (sCfgItem->mCfgDlg != XPR_NULL && sCfgItem->mCfgDlg->m_hWnd != XPR_NULL)
            sCfgItem->mCfgDlg->DestroyWindow();

        onSetModified(i, XPR_TRUE);
    }

    mTreeCtrl.SelectItem(mCfgDeque[0]->mTreeItem);
    showCfg(0);

    GetDlgItem(IDC_CFG_APPLY)->EnableWindow(XPR_FALSE);

    sMsg = theApp.loadString(XPR_STRING_LITERAL("popup.cfg.msg.default_ok"));
    MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONINFORMATION);
}

void CfgMainDlg::OnApply(void)
{
    CWaitCursor sWaitCursor;

    CfgItem *sCfgItem;
    CfgDeque::iterator sIterator;

    sIterator = mCfgDeque.begin();
    for (; sIterator != mCfgDeque.end(); ++sIterator)
    {
        sCfgItem = *sIterator;
        if (sCfgItem == XPR_NULL)
            continue;

        if (sCfgItem->mModified == XPR_TRUE && sCfgItem->mCfgDlg != XPR_NULL && sCfgItem->mCfgDlg->m_hWnd != XPR_NULL)
        {
            sCfgItem->mCfgDlg->onApply(mNewConfig);

            sCfgItem->mModified = XPR_FALSE;
        }
    }

    ConfDir &sConfDir = ConfDir::instance();
    OptionManager &sOptionManager = OptionManager::instance();

    // save configuration path to save configuration files
    sConfDir.save();

    // set new option
    gOpt->setConfig(mNewConfig);

    // save new option
    sOptionManager.saveConfigOption();

    // notify new option
    gOpt->notifyConfig();

    GetDlgItem(IDC_CFG_APPLY)->EnableWindow(XPR_FALSE);
}
} // namespace cfg
} // namespace fxfile
