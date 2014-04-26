//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "batch_rename_tab_format_dlg.h"
#include "batch_rename.h"

#include "resource.h"
#include "dlg_state.h"
#include "dlg_state_manager.h"

#include "command_string_table.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace fxfile
{
namespace cmd
{
BatchRenameTabFormatDlg::BatchRenameTabFormatDlg(void)
    : super(IDD_BATCH_RENAME_TAB_FORMAT)
    , mTipDlg(XPR_NULL)
    , mDlgState(XPR_NULL)
{
}

void BatchRenameTabFormatDlg::DoDataExchange(CDataExchange* pDX)
{
    super::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(BatchRenameTabFormatDlg, super)
    ON_WM_INITMENUPOPUP()
    ON_WM_MEASUREITEM()
    ON_WM_MENUCHAR()
    ON_WM_DESTROY()
    ON_BN_CLICKED(IDC_BATCH_RENAME_FORMAT_APPLY, OnApply)
    ON_BN_CLICKED(IDC_BATCH_RENAME_FORMAT_TIP,   OnFormatTip)
    ON_BN_CLICKED(IDC_BATCH_RENAME_FORMAT_MENU,  OnFormatMenu)
END_MESSAGE_MAP()

xpr_bool_t BatchRenameTabFormatDlg::OnInitDialog(void) 
{
    super::OnInitDialog();

    // CResizingDialog -------------------------------------------
    HideSizeIcon();

    //sizeNone:     Don't resize at all  
    //sizeResize:   The control will be stretched in the appropriate direction 
    //sizeRepos:    The control will be moved in the appropriate direction 
    //sizeRelative: The control will be moved proportionally in the appropriate direction 

    AddControl(IDC_BATCH_RENAME_FORMAT,           sizeResize, sizeNone);
    AddControl(IDC_BATCH_RENAME_FORMAT_MENU,      sizeRepos,  sizeNone, XPR_FALSE);

    AddControl(IDC_BATCH_RENAME_FORMAT_NAME,      sizeRepos,  sizeNone, XPR_FALSE);
    AddControl(IDC_BATCH_RENAME_FORMAT_EXT,       sizeRepos,  sizeNone, XPR_FALSE);
    AddControl(IDC_BATCH_RENAME_FORMAT_NUMBERING, sizeRepos,  sizeNone, XPR_FALSE);
    AddControl(IDC_BATCH_RENAME_FORMAT_DATE,      sizeRepos,  sizeNone, XPR_FALSE);
    AddControl(IDC_BATCH_RENAME_FORMAT_TIME,      sizeRepos,  sizeNone, XPR_FALSE);

    AddControl(IDC_BATCH_RENAME_FORMAT_TIP,       sizeRepos,  sizeNone, XPR_FALSE);
    AddControl(IDC_BATCH_RENAME_FORMAT_APPLY,     sizeRepos,  sizeNone, XPR_FALSE);
    //------------------------------------------------------------

    ((CSpinButtonCtrl *)GetDlgItem(IDC_BATCH_RENAME_FORMAT_NUMBER_SPIN  ))->SetRange32(FXFILE_BATCH_RENAME_FORMAT_NUMBER_MIN,   FXFILE_BATCH_RENAME_FORMAT_NUMBER_MAX);
    ((CSpinButtonCtrl *)GetDlgItem(IDC_BATCH_RENAME_FORMAT_INCREASE_SPIN))->SetRange32(FXFILE_BATCH_RENAME_FORMAT_INCREASE_MIN, FXFILE_BATCH_RENAME_FORMAT_INCREASE_MAX);
    ((CSpinButtonCtrl *)GetDlgItem(IDC_BATCH_RENAME_FORMAT_DIGIT_SPIN   ))->SetRange32(FXFILE_BATCH_RENAME_FORMAT_DIGIT_MIN,    FXFILE_BATCH_RENAME_FORMAT_DIGIT_MAX);

    SetDlgItemText(IDC_BATCH_RENAME_FORMAT, FXFILE_BATCH_RENAME_FORMAT_DEF);
    SetDlgItemInt(IDC_BATCH_RENAME_FORMAT_NUMBER,   FXFILE_BATCH_RENAME_FORMAT_NUMBER_DEF,   XPR_FALSE);
    SetDlgItemInt(IDC_BATCH_RENAME_FORMAT_INCREASE, FXFILE_BATCH_RENAME_FORMAT_INCREASE_DEF, XPR_FALSE);
    SetDlgItemInt(IDC_BATCH_RENAME_FORMAT_DIGIT,    FXFILE_BATCH_RENAME_FORMAT_DIGIT_DEF,    XPR_FALSE);

    ((CComboBox *)GetDlgItem(IDC_BATCH_RENAME_FORMAT))->LimitText(FXFILE_BATCH_RENAME_FORMAT_MAX_LENGTH);

    CRect sRect;
    CWnd *sWnd = GetDlgItem(IDC_BATCH_RENAME_FORMAT);
    sWnd->GetClientRect(&sRect);

    const xpr_tchar_t *sTooltipTitle    = gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename.tab.format.tooltip.title"));
    const xpr_tchar_t *sTooltipContents = gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename.tab.format.tooltip.contents"));

    mToolTipCtrl.Create(this, TTS_BALLOON);
    mToolTipCtrl.AddTool(sWnd, sTooltipContents, sRect, 1);
    mToolTipCtrl.SetMaxTipWidth(1000);
    mToolTipCtrl.SendMessage(TTM_SETTITLE, (WPARAM)1, (LPARAM)sTooltipTitle);

    SetDlgItemText(IDC_BATCH_RENAME_LABEL_FORMAT,          gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename.tab.format.label.format")));
    SetDlgItemText(IDC_BATCH_RENAME_FORMAT_LABEL_NUMBER,   gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename.tab.format.label.start")));
    SetDlgItemText(IDC_BATCH_RENAME_FORMAT_LABEL_LENGTH,   gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename.tab.format.label.length")));
    SetDlgItemText(IDC_BATCH_RENAME_FORMAT_LABEL_INCREASE, gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename.tab.format.label.increase")));
    SetDlgItemText(IDC_BATCH_RENAME_FORMAT_NUMBER_ZERO,    gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename.tab.format.check.zero_filled")));
    SetDlgItemText(IDC_BATCH_RENAME_FORMAT_NUMBERING,      gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename.tab.format.button.number_digit")));
    SetDlgItemText(IDC_BATCH_RENAME_FORMAT_NAME,           gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename.tab.format.button.filename")));
    SetDlgItemText(IDC_BATCH_RENAME_FORMAT_EXT,            gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename.tab.format.button.extension")));
    SetDlgItemText(IDC_BATCH_RENAME_FORMAT_DATE,           gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename.tab.format.button.date")));
    SetDlgItemText(IDC_BATCH_RENAME_FORMAT_TIME,           gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename.tab.format.button.time")));
    SetDlgItemText(IDC_BATCH_RENAME_FORMAT_TIP,            gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename.tab.format.button.format_tip")));
    SetDlgItemText(IDC_BATCH_RENAME_FORMAT_APPLY,          gApp.loadString(XPR_STRING_LITERAL("popup.batch_rename.tab.format.button.apply")));

    // Load Dialog State
    mDlgState = DlgStateManager::instance().getDlgState(XPR_STRING_LITERAL("BatchRenameFormat"));
    if (XPR_IS_NOT_NULL(mDlgState))
    {
        mDlgState->setDialog(this);
        mDlgState->setComboBoxList(XPR_STRING_LITERAL("Format"), IDC_BATCH_RENAME_FORMAT);
        mDlgState->setEditCtrl(XPR_STRING_LITERAL("Start"),      IDC_BATCH_RENAME_FORMAT_NUMBER);
        mDlgState->setEditCtrl(XPR_STRING_LITERAL("Increase"),   IDC_BATCH_RENAME_FORMAT_INCREASE);
        mDlgState->setEditCtrl(XPR_STRING_LITERAL("Digit"),      IDC_BATCH_RENAME_FORMAT_DIGIT);
        mDlgState->setCheckBox(XPR_STRING_LITERAL("Zero"),       IDC_BATCH_RENAME_FORMAT_NUMBER_ZERO);
        mDlgState->load();
    }

    return XPR_TRUE;
}

void BatchRenameTabFormatDlg::OnTabInit(void)
{
}

void BatchRenameTabFormatDlg::OnApply(void) 
{
    CComboBox *sComboBox = (CComboBox *)GetDlgItem(IDC_BATCH_RENAME_FORMAT);

    DlgState::insertComboEditString(sComboBox);

    GetParent()->SendMessage(WM_BATCH_RENAME_APPLY, 1);
}

xpr_bool_t BatchRenameTabFormatDlg::PreTranslateMessage(MSG* pMsg) 
{
    if (mToolTipCtrl.m_hWnd)
        mToolTipCtrl.RelayEvent(pMsg);

    if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN && GetKeyState(VK_CONTROL) >= 0)
    {
        HWND sHwnd = pMsg->hwnd;

        xpr_tchar_t sClassName[MAX_CLASS_NAME + 1] = {0};
        ::GetClassName(::GetParent(sHwnd), sClassName, MAX_CLASS_NAME);
        if (_tcsicmp(sClassName, XPR_STRING_LITERAL("ComboBox")) == 0)
            sHwnd = ::GetParent(sHwnd);

        xpr_sint_t sId = ::GetDlgCtrlID(sHwnd);
        switch (sId)
        {
        case IDC_BATCH_RENAME_FORMAT:
        case IDC_BATCH_RENAME_FORMAT_NUMBER:
        case IDC_BATCH_RENAME_FORMAT_NUMBER_SPIN:
        case IDC_BATCH_RENAME_FORMAT_INCREASE:
        case IDC_BATCH_RENAME_FORMAT_INCREASE_SPIN:
        case IDC_BATCH_RENAME_FORMAT_DIGIT:
        case IDC_BATCH_RENAME_FORMAT_DIGIT_SPIN:
        case IDC_BATCH_RENAME_FORMAT_APPLY:
            OnApply();
            break;
        }

        return XPR_TRUE;
    }

    return super::PreTranslateMessage(pMsg);
}

LRESULT BatchRenameTabFormatDlg::OnMenuChar(xpr_uint_t nChar, xpr_uint_t nFlags, CMenu* pMenu) 
{
    LRESULT sResult;

    if (BCMenu::IsMenu(pMenu))
        sResult = BCMenu::FindKeyboardShortcut(nChar, nFlags, pMenu);
    else
        sResult = CResizingDialog::OnMenuChar(nChar, nFlags, pMenu);

    return sResult;
}

void BatchRenameTabFormatDlg::OnInitMenuPopup(CMenu* pPopupMenu, xpr_uint_t nIndex, xpr_bool_t bSysMenu) 
{
    //CResizingDialog::OnInitMenuPopup(pPopupMenu, nIndex, bSysMenu);
    ASSERT(pPopupMenu != XPR_NULL);

    // for multi-language
    BCMenu *pBCPopupMenu = dynamic_cast<BCMenu *>(pPopupMenu);
    if (pBCPopupMenu != XPR_NULL)
    {
        xpr_uint_t sId;
        xpr_sint_t sCount = pBCPopupMenu->GetMenuItemCount();

        const xpr_tchar_t *sStringId;
        const xpr_tchar_t *sString;
        xpr::string sMenuText;
        CommandStringTable &sCommandStringTable = CommandStringTable::instance();

        xpr_sint_t i;
        for (i = 0; i < sCount; ++i)
        {
            sId = pBCPopupMenu->GetMenuItemID(i);

            // apply string table
            if (sId != 0) // if nId is 0, it's separator.
            {
                if (sId == -1)
                {
                    // if nId(xpr_uint_t) is -1, it's sub-menu.
                    pBCPopupMenu->GetMenuText(i, sMenuText, MF_BYPOSITION);

                    sString = gApp.loadString(sMenuText);
                    pBCPopupMenu->SetMenuText(i, (xpr_tchar_t *)sString, MF_BYPOSITION);
                }
                else
                {
                    sStringId = sCommandStringTable.loadString(sId);
                    if (sStringId != XPR_NULL)
                    {
                        sString = gApp.loadString(sStringId);

                        pBCPopupMenu->SetMenuText(sId, (xpr_tchar_t *)sString, MF_BYCOMMAND);
                    }
                }
            }
        }
    }

    // Check the enabled state of various menu items.
    CCmdUI sState;
    sState.m_pMenu = pPopupMenu;
    ASSERT(sState.m_pOther == XPR_NULL);
    ASSERT(sState.m_pParentMenu == XPR_NULL);

    // Determine if menu is popup in top-level menu and set m_pOther to
    // it if so (m_pParentMenu == XPR_NULL indicates that it is secondary popup).
    HMENU sParentMenu;
    if (AfxGetThreadState()->m_hTrackingMenu == pPopupMenu->m_hMenu)
        sState.m_pParentMenu = pPopupMenu;    // Parent == child for tracking popup.
    else if ((sParentMenu = ::GetMenu(m_hWnd)) != XPR_NULL)
    {
        CWnd *sParentWnd = this;
        // Child windows don't have menus--need to go to the top!
        if (sParentWnd != XPR_NULL &&
            (sParentMenu = ::GetMenu(sParentWnd->m_hWnd)) != XPR_NULL)
        {
            xpr_sint_t sIndexMax = ::GetMenuItemCount(sParentMenu);
            for (xpr_sint_t sIndex = 0; sIndex < sIndexMax; ++nIndex)
            {
                if (::GetSubMenu(sParentMenu, nIndex) == pPopupMenu->m_hMenu)
                {
                    // When popup is found, m_pParentMenu is containing menu.
                    sState.m_pParentMenu = CMenu::FromHandle(sParentMenu);
                    break;
                }
            }
        }
    }

    sState.m_nIndexMax = pPopupMenu->GetMenuItemCount();
    for (sState.m_nIndex = 0; sState.m_nIndex < sState.m_nIndexMax; ++sState.m_nIndex)
    {
        sState.m_nID = pPopupMenu->GetMenuItemID(sState.m_nIndex);
        if (sState.m_nID == 0)
            continue; // Menu separator or invalid cmd - ignore it.

        ASSERT(sState.m_pOther == XPR_NULL);
        ASSERT(sState.m_pMenu != XPR_NULL);
        if (sState.m_nID == (xpr_uint_t)-1)
        {
            // Possibly a popup menu, route to first item of that popup.
            sState.m_pSubMenu = pPopupMenu->GetSubMenu(sState.m_nIndex);
            if (sState.m_pSubMenu == XPR_NULL ||
                (sState.m_nID = sState.m_pSubMenu->GetMenuItemID(0)) == 0 ||
                sState.m_nID == (xpr_uint_t)-1)
            {
                continue;       // First item of popup can't be routed to.
            }
            sState.DoUpdate(this, XPR_TRUE);   // Popups are never auto disabled.
        }
        else
        {
            // Normal menu item.
            // Auto enable/disable if frame window has m_bAutoMenuEnable
            // set and command is _not_ a system command.
            sState.m_pSubMenu = XPR_NULL;
            sState.DoUpdate(this, XPR_FALSE);
        }

        // Adjust for menu deletions and additions.
        xpr_uint_t sCount = pPopupMenu->GetMenuItemCount();
        if (sCount < sState.m_nIndexMax)
        {
            sState.m_nIndex -= (sState.m_nIndexMax - sCount);
            while (sState.m_nIndex < sCount &&
                pPopupMenu->GetMenuItemID(sState.m_nIndex) == sState.m_nID)
            {
                sState.m_nIndex++;
            }
        }

        sState.m_nIndexMax = sCount;
    }
}

void BatchRenameTabFormatDlg::OnMeasureItem(xpr_sint_t nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct) 
{
    xpr_bool_t sSetFlag = XPR_FALSE;
    if (lpMeasureItemStruct->CtlType == ODT_MENU)
    {
        if (IsMenu((HMENU)(uintptr_t)lpMeasureItemStruct->itemID))
        {
            CMenu *sMenu = CMenu::FromHandle((HMENU)(uintptr_t)lpMeasureItemStruct->itemID);
            if (BCMenu::IsMenu(sMenu))
            {
                mMenu.MeasureItem(lpMeasureItemStruct);
                sSetFlag = XPR_TRUE;
            }
        }
    }

    if (sSetFlag == XPR_FALSE)
        CResizingDialog::OnMeasureItem(nIDCtl, lpMeasureItemStruct);
}

void BatchRenameTabFormatDlg::OnFormatTip(void) 
{
    if (mTipDlg == XPR_NULL)
    {
        mTipDlg = new BatchRenameTipDlg;
        mTipDlg->Create(GetParent());

        CRect sRect;
        GetParent()->GetWindowRect(&sRect);
        mTipDlg->SetWindowPos(XPR_NULL, sRect.right, sRect.top, 0, 0, SWP_NOSIZE);
    }

    mTipDlg->ShowWindow(SW_SHOW);
    SetFocus();
}

void BatchRenameTabFormatDlg::OnDestroy(void) 
{
    super::OnDestroy();

    if (XPR_IS_NOT_NULL(mDlgState))
    {
        mDlgState->reset();
        mDlgState->save();
    }
}

void BatchRenameTabFormatDlg::OnFormatMenu(void) 
{
    CRect sRect;
    GetDlgItem(IDC_BATCH_RENAME_FORMAT_MENU)->GetWindowRect(&sRect);

    BCMenu sMenu;
    if (sMenu.LoadMenu(IDR_BATCH_RENAME_FORMAT) == XPR_TRUE)
    {
        BCMenu *sPopupMenu = (BCMenu *)sMenu.GetSubMenu(0);
        if (XPR_IS_NOT_NULL(sPopupMenu))
        {
            sPopupMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, sRect.right, sRect.top, this);
        }
    }
}

xpr_bool_t BatchRenameTabFormatDlg::OnCommand(WPARAM wParam, LPARAM lParam) 
{
    xpr_uint_t sNotifyMsg = HIWORD(wParam);
    xpr_uint_t sId = LOWORD(wParam);

    xpr_tchar_t sForamt[FXFILE_BATCH_RENAME_FORMAT_MAX_LENGTH + 1] = {0};
    GetDlgItemText(IDC_BATCH_RENAME_FORMAT, sForamt, FXFILE_BATCH_RENAME_FORMAT_MAX_LENGTH);

    xpr::string sAppendedFormat(sForamt);
    if (sId == IDC_BATCH_RENAME_FORMAT_NUMBERING && sId == ID_BATCH_RENAME_FORMAT_MENU_NUMBERING)
    {
        xpr_sint_t sNumber   = GetDlgItemInt(IDC_BATCH_RENAME_FORMAT_NUMBER);
        xpr_sint_t sIncrease = GetDlgItemInt(IDC_BATCH_RENAME_FORMAT_INCREASE);
        xpr_size_t sDigit    = GetDlgItemInt(IDC_BATCH_RENAME_FORMAT_DIGIT, XPR_FALSE);
        xpr_bool_t sZero     = ((CButton *)GetDlgItem(IDC_BATCH_RENAME_FORMAT_NUMBER_ZERO))->GetCheck();

        if (XPR_IS_TRUE(sZero))
        {
            if (sIncrease == 1)
            {
                xpr::string sStringFormat;
                sStringFormat.format(XPR_STRING_LITERAL("<%%0%dd>"), sDigit);

                sAppendedFormat.append_format(sStringFormat.c_str(), sNumber);
            }
            else
            {
                xpr::string sStringFormat;
                sStringFormat.format(XPR_STRING_LITERAL("<%%0%dd+%%d>"), sDigit);

                sAppendedFormat.append_format(sStringFormat.c_str(), sNumber, sIncrease);
            }
        }
        else
        {
            if (sIncrease == 1)
            {
                sAppendedFormat.append_format(XPR_STRING_LITERAL("<%d>"), sNumber);
            }
            else
            {
                sAppendedFormat.append_format(XPR_STRING_LITERAL("<%d+%d>"), sNumber, sIncrease);
            }
        }

        SetDlgItemText(IDC_BATCH_RENAME_FORMAT, sAppendedFormat.c_str());
    }
    else if (sId == ID_BATCH_RENAME_FORMAT_MENU_ORIGINAL_FILENAME_RANGE)
    {
        sAppendedFormat.append_format(XPR_STRING_LITERAL("<gn?-?"));

        SetDlgItemText(IDC_BATCH_RENAME_FORMAT, sAppendedFormat.c_str());
        xpr_sint_t sLen = (xpr_sint_t)sAppendedFormat.length();

        CComboBox *sComboBox = (CComboBox *)GetDlgItem(IDC_BATCH_RENAME_FORMAT);
        if (sComboBox != XPR_NULL && sComboBox->m_hWnd != XPR_NULL)
        {
            sComboBox->SetFocus();
            sComboBox->SetEditSel(sLen - 4, sLen - 3);
        }
    }
    else
    {
        xpr_bool_t sSuccess = XPR_TRUE;
        switch (sId)
        {
        case ID_BATCH_RENAME_FORMAT_MENU_FULLNAME:              sAppendedFormat.append_format(XPR_STRING_LITERAL("<*>")); break;
        case IDC_BATCH_RENAME_FORMAT_NAME:
        case ID_BATCH_RENAME_FORMAT_MENU_FILENAME:              sAppendedFormat.append_format(XPR_STRING_LITERAL("<n>")); break;
        case IDC_BATCH_RENAME_FORMAT_EXT:
        case ID_BATCH_RENAME_FORMAT_MENU_EXTENSION:             sAppendedFormat.append_format(XPR_STRING_LITERAL("<e>")); break;
        case IDC_BATCH_RENAME_FORMAT_DATE:
        case ID_BATCH_RENAME_FORMAT_MENU_DATE:                  sAppendedFormat.append_format(XPR_STRING_LITERAL("<time:yyyy-MM-dd>"));  break;
        case ID_BATCH_RENAME_FORMAT_MENU_YEAR:                  sAppendedFormat.append_format(XPR_STRING_LITERAL("<time:yyyy>"));        break;
        case ID_BATCH_RENAME_FORMAT_MENU_MONTH:                 sAppendedFormat.append_format(XPR_STRING_LITERAL("<time:MM>"));          break;
        case ID_BATCH_RENAME_FORMAT_MENU_DAY:                   sAppendedFormat.append_format(XPR_STRING_LITERAL("<time:dd>"));          break;
        case IDC_BATCH_RENAME_FORMAT_TIME:
        case ID_BATCH_RENAME_FORMAT_MENU_TIME:                  sAppendedFormat.append_format(XPR_STRING_LITERAL("<time:HH.mm.ss>"));    break;
        case ID_BATCH_RENAME_FORMAT_MENU_HOUR:                  sAppendedFormat.append_format(XPR_STRING_LITERAL("<time:HH>"));          break;
        case ID_BATCH_RENAME_FORMAT_MENU_MINUTE:                sAppendedFormat.append_format(XPR_STRING_LITERAL("<time:mm>"));          break;
        case ID_BATCH_RENAME_FORMAT_MENU_SECOND:                sAppendedFormat.append_format(XPR_STRING_LITERAL("<time:ss>"));          break;
        case ID_BATCH_RENAME_FORMAT_MENU_CREATION_DATE:         sAppendedFormat.append_format(XPR_STRING_LITERAL("<ctime:yyyy-MM-dd>")); break;
        case ID_BATCH_RENAME_FORMAT_MENU_CREATION_YEAR:         sAppendedFormat.append_format(XPR_STRING_LITERAL("<ctime:yyyy>"));       break;
        case ID_BATCH_RENAME_FORMAT_MENU_CREATION_MONTH:        sAppendedFormat.append_format(XPR_STRING_LITERAL("<ctime:MM>"));         break;
        case ID_BATCH_RENAME_FORMAT_MENU_CREATION_DAY:          sAppendedFormat.append_format(XPR_STRING_LITERAL("<ctime:dd>"));         break;
        case ID_BATCH_RENAME_FORMAT_MENU_CREATION_TIME:         sAppendedFormat.append_format(XPR_STRING_LITERAL("<ctime:HH.mm.ss>"));   break;
        case ID_BATCH_RENAME_FORMAT_MENU_CREATION_AM_PM_HOUR:   sAppendedFormat.append_format(XPR_STRING_LITERAL("<ctime:tt>"));         break;
        case ID_BATCH_RENAME_FORMAT_MENU_CREATION_12_HOUR:      sAppendedFormat.append_format(XPR_STRING_LITERAL("<ctime:hh>"));         break;
        case ID_BATCH_RENAME_FORMAT_MENU_CREATION_24_HOUR:      sAppendedFormat.append_format(XPR_STRING_LITERAL("<ctime:HH>"));         break;
        case ID_BATCH_RENAME_FORMAT_MENU_CREATION_MINUTE:       sAppendedFormat.append_format(XPR_STRING_LITERAL("<ctime:mm>"));         break;
        case ID_BATCH_RENAME_FORMAT_MENU_CREATION_SECOND:       sAppendedFormat.append_format(XPR_STRING_LITERAL("<ctime:ss>"));         break;
        case ID_BATCH_RENAME_FORMAT_MENU_LAST_WRITE_DATE:       sAppendedFormat.append_format(XPR_STRING_LITERAL("<mtime:yyyy-MM-dd>")); break;
        case ID_BATCH_RENAME_FORMAT_MENU_LAST_WRITE_YEAR:       sAppendedFormat.append_format(XPR_STRING_LITERAL("<mtime:yyyy>"));       break;
        case ID_BATCH_RENAME_FORMAT_MENU_LAST_WRITE_MONTH:      sAppendedFormat.append_format(XPR_STRING_LITERAL("<mtime:MM>"));         break;
        case ID_BATCH_RENAME_FORMAT_MENU_LAST_WRITE_DAY:        sAppendedFormat.append_format(XPR_STRING_LITERAL("<mtime:dd>"));         break;
        case ID_BATCH_RENAME_FORMAT_MENU_LAST_WRITE_TIME:       sAppendedFormat.append_format(XPR_STRING_LITERAL("<mtime:HH.mm.ss>"));   break;
        case ID_BATCH_RENAME_FORMAT_MENU_LAST_WRITE_AM_PM_HOUR: sAppendedFormat.append_format(XPR_STRING_LITERAL("<mtime:tt>"));         break;
        case ID_BATCH_RENAME_FORMAT_MENU_LAST_WRITE_12_HOUR:    sAppendedFormat.append_format(XPR_STRING_LITERAL("<mtime:hh>"));         break;
        case ID_BATCH_RENAME_FORMAT_MENU_LAST_WRITE_24_HOUR:    sAppendedFormat.append_format(XPR_STRING_LITERAL("<mtime:HH>"));         break;
        case ID_BATCH_RENAME_FORMAT_MENU_LAST_WRITE_MINUTE:     sAppendedFormat.append_format(XPR_STRING_LITERAL("<mtime:mm>"));         break;
        case ID_BATCH_RENAME_FORMAT_MENU_LAST_WRITE_SECOND:     sAppendedFormat.append_format(XPR_STRING_LITERAL("<mtime:ss>"));         break;

        default:
            sSuccess = XPR_FALSE;
            break;
        }

        if (XPR_IS_TRUE(sSuccess))
        {
            SetDlgItemText(IDC_BATCH_RENAME_FORMAT, sAppendedFormat.c_str());
        }
    }

    return CResizingDialog::OnCommand(wParam, lParam);
}
} // namespace cmd
} // namespace fxfile
