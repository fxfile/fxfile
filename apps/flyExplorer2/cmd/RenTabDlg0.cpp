//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "RenTabDlg0.h"

#include "fxb/fxb_batch_rename.h"

#include "resource.h"
#include "DlgState.h"
#include "DlgStateMgr.h"

#include "command_string_table.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

RenTabDlg0::RenTabDlg0(void)
    : super(IDD_RENAME_1)
    , mTipDlg(XPR_NULL)
    , mDlgState(XPR_NULL)
{
}

void RenTabDlg0::DoDataExchange(CDataExchange* pDX)
{
    super::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(RenTabDlg0, super)
    ON_WM_INITMENUPOPUP()
    ON_WM_MEASUREITEM()
    ON_WM_MENUCHAR()
    ON_WM_DESTROY()
    ON_BN_CLICKED(IDC_BATCH_RENAME_FORMAT_APPLY, OnApply)
    ON_BN_CLICKED(IDC_BATCH_RENAME_FORMAT_DESC,  OnFormatDesc)
    ON_BN_CLICKED(IDC_BATCH_RENAME_FORMAT_MENU,  OnFormatMenu)
END_MESSAGE_MAP()

xpr_bool_t RenTabDlg0::OnInitDialog(void) 
{
    super::OnInitDialog();

    // CResizingDialog -------------------------------------------
    HideSizeIcon();

    //sizeNone:     Don't resize at all  
    //sizeResize:   The control will be stretched in the appropriate direction 
    //sizeRepos:    The control will be moved in the appropriate direction 
    //sizeRelative: The control will be moved proportionally in the appropriate direction 

    AddControl(IDC_BATCH_RENAME_FORMAT,          sizeResize, sizeNone);
    AddControl(IDC_BATCH_RENAME_FORMAT_MENU,     sizeRepos,  sizeNone, XPR_FALSE);

    AddControl(IDC_BATCH_RENAME_FORMAT_NAME,     sizeRepos,  sizeNone, XPR_FALSE);
    AddControl(IDC_BATCH_RENAME_FORMAT_EXT,      sizeRepos,  sizeNone, XPR_FALSE);
    AddControl(IDC_BATCH_RENAME_FORMAT_AUTO_NUM, sizeRepos,  sizeNone, XPR_FALSE);
    AddControl(IDC_BATCH_RENAME_FORMAT_DATE,     sizeRepos,  sizeNone, XPR_FALSE);
    AddControl(IDC_BATCH_RENAME_FORMAT_TIME,     sizeRepos,  sizeNone, XPR_FALSE);

    AddControl(IDC_BATCH_RENAME_FORMAT_DESC,     sizeRepos,  sizeNone, XPR_FALSE);
    AddControl(IDC_BATCH_RENAME_FORMAT_APPLY,    sizeRepos,  sizeNone, XPR_FALSE);
    //------------------------------------------------------------

    CSpinButtonCtrl* pSpinCtrl;
    pSpinCtrl = (CSpinButtonCtrl *)GetDlgItem(IDC_BATCH_RENAME_FORMAT_NUMBER_SPIN);
    pSpinCtrl->SetRange32(0, 400000000);
    pSpinCtrl = (CSpinButtonCtrl *)GetDlgItem(IDC_BATCH_RENAME_FORMAT_INCREASE_SPIN);
    pSpinCtrl->SetRange32(1, 400000000);
    pSpinCtrl = (CSpinButtonCtrl *)GetDlgItem(IDC_BATCH_RENAME_FORMAT_LENGTH_SPIN);
    pSpinCtrl->SetRange32(1, 9);
    ((CComboBox *)GetDlgItem(IDC_BATCH_RENAME_FORMAT))->LimitText(XPR_MAX_PATH);

    SetDlgItemText(IDC_BATCH_RENAME_FORMAT, XPR_STRING_LITERAL("/n/e"));
    SetDlgItemInt(IDC_BATCH_RENAME_FORMAT_INCREASE, 1, XPR_FALSE);
    SetDlgItemInt(IDC_BATCH_RENAME_FORMAT_LENGTH,   1, XPR_FALSE);

    CRect sRect;
    CWnd *sWnd = GetDlgItem(IDC_BATCH_RENAME_FORMAT);
    sWnd->GetClientRect(&sRect);

    const xpr_tchar_t *sTooltipTitle    = theApp.loadString(XPR_STRING_LITERAL("popup.batch_rename.tab.format.tooltip.title"));
    const xpr_tchar_t *sTooltipContents = theApp.loadString(XPR_STRING_LITERAL("popup.batch_rename.tab.format.tooltip.contents"));

    mToolTipCtrl.Create(this, TTS_BALLOON);
    mToolTipCtrl.AddTool(sWnd, sTooltipContents, sRect, 1);
    mToolTipCtrl.SetMaxTipWidth(1000);
    mToolTipCtrl.SendMessage(TTM_SETTITLE, (WPARAM)1, (LPARAM)sTooltipTitle);

    SetDlgItemText(IDC_BATCH_RENAME_LABEL_FORMAT,          theApp.loadString(XPR_STRING_LITERAL("popup.batch_rename.tab.format.label.format")));
    SetDlgItemText(IDC_BATCH_RENAME_FORMAT_LABEL_NUMBER,   theApp.loadString(XPR_STRING_LITERAL("popup.batch_rename.tab.format.label.start")));
    SetDlgItemText(IDC_BATCH_RENAME_FORMAT_LABEL_LENGTH,   theApp.loadString(XPR_STRING_LITERAL("popup.batch_rename.tab.format.label.length")));
    SetDlgItemText(IDC_BATCH_RENAME_FORMAT_LABEL_INCREASE, theApp.loadString(XPR_STRING_LITERAL("popup.batch_rename.tab.format.label.increase")));
    SetDlgItemText(IDC_BATCH_RENAME_FORMAT_NUMBER_ZERO,    theApp.loadString(XPR_STRING_LITERAL("popup.batch_rename.tab.format.check.zero_filled")));
    SetDlgItemText(IDC_BATCH_RENAME_FORMAT_AUTO_NUM,       theApp.loadString(XPR_STRING_LITERAL("popup.batch_rename.tab.format.button.number_digit")));
    SetDlgItemText(IDC_BATCH_RENAME_FORMAT_NAME,           theApp.loadString(XPR_STRING_LITERAL("popup.batch_rename.tab.format.button.filename")));
    SetDlgItemText(IDC_BATCH_RENAME_FORMAT_EXT,            theApp.loadString(XPR_STRING_LITERAL("popup.batch_rename.tab.format.button.extension")));
    SetDlgItemText(IDC_BATCH_RENAME_FORMAT_DATE,           theApp.loadString(XPR_STRING_LITERAL("popup.batch_rename.tab.format.button.date")));
    SetDlgItemText(IDC_BATCH_RENAME_FORMAT_TIME,           theApp.loadString(XPR_STRING_LITERAL("popup.batch_rename.tab.format.button.time")));
    SetDlgItemText(IDC_BATCH_RENAME_FORMAT_DESC,           theApp.loadString(XPR_STRING_LITERAL("popup.batch_rename.tab.format.button.format_tip")));
    SetDlgItemText(IDC_BATCH_RENAME_FORMAT_APPLY,          theApp.loadString(XPR_STRING_LITERAL("popup.batch_rename.tab.format.button.apply")));

    // Load Dialog State
    mDlgState = DlgStateMgr::instance().getDlgState(XPR_STRING_LITERAL("Rename1"));
    if (XPR_IS_NOT_NULL(mDlgState))
    {
        mDlgState->setDialog(this);
        mDlgState->setComboBoxList(XPR_STRING_LITERAL("Format"), IDC_BATCH_RENAME_FORMAT);
        mDlgState->setEditCtrl(XPR_STRING_LITERAL("Start"),      IDC_BATCH_RENAME_FORMAT_NUMBER);
        mDlgState->setEditCtrl(XPR_STRING_LITERAL("Increase"),   IDC_BATCH_RENAME_FORMAT_INCREASE);
        mDlgState->setEditCtrl(XPR_STRING_LITERAL("Length"),     IDC_BATCH_RENAME_FORMAT_LENGTH);
        mDlgState->setCheckBox(XPR_STRING_LITERAL("Zero"),       IDC_BATCH_RENAME_FORMAT_NUMBER_ZERO);
        mDlgState->load();
    }

    return XPR_TRUE;
}

void RenTabDlg0::OnTabInit(void)
{
}

void RenTabDlg0::OnApply(void) 
{
    CComboBox *sComboBox = (CComboBox *)GetDlgItem(IDC_BATCH_RENAME_FORMAT);

    DlgState::insertComboEditString(sComboBox);

    GetParent()->SendMessage(WM_BATCH_RENAME_APPLY, 1);
}

xpr_bool_t RenTabDlg0::PreTranslateMessage(MSG* pMsg) 
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
        case IDC_BATCH_RENAME_FORMAT_LENGTH:
        case IDC_BATCH_RENAME_FORMAT_LENGTH_SPIN:
        case IDC_BATCH_RENAME_FORMAT_APPLY:
            OnApply();
            break;
        }

        return XPR_TRUE;
    }

    return super::PreTranslateMessage(pMsg);
}

LRESULT RenTabDlg0::OnMenuChar(xpr_uint_t nChar, xpr_uint_t nFlags, CMenu* pMenu) 
{
    LRESULT sResult;

    if (BCMenu::IsMenu(pMenu))
        sResult = BCMenu::FindKeyboardShortcut(nChar, nFlags, pMenu);
    else
        sResult = CResizingDialog::OnMenuChar(nChar, nFlags, pMenu);

    return sResult;
}

void RenTabDlg0::OnInitMenuPopup(CMenu* pPopupMenu, xpr_uint_t nIndex, xpr_bool_t bSysMenu) 
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
        CString sMenuText;
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

                    sString = theApp.loadString(sMenuText.GetBuffer());
                    pBCPopupMenu->SetMenuText(i, (xpr_tchar_t *)sString, MF_BYPOSITION);
                }
                else
                {
                    sStringId = sCommandStringTable.loadString(sId);
                    if (sStringId != XPR_NULL)
                    {
                        sString = theApp.loadString(sStringId);

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

void RenTabDlg0::OnMeasureItem(xpr_sint_t nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct) 
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

void RenTabDlg0::OnFormatDesc(void) 
{
    if (mTipDlg == XPR_NULL)
    {
        mTipDlg = new RenameTipDlg;
        mTipDlg->Create(GetParent());

        CRect sRect;
        GetParent()->GetWindowRect(&sRect);
        mTipDlg->SetWindowPos(XPR_NULL, sRect.right, sRect.top, 0, 0, SWP_NOSIZE);
    }

    mTipDlg->ShowWindow(SW_SHOW);
    SetFocus();
}

void RenTabDlg0::OnDestroy(void) 
{
    super::OnDestroy();

    if (XPR_IS_NOT_NULL(mDlgState))
    {
        mDlgState->reset();
        mDlgState->save();
    }
}

void RenTabDlg0::OnFormatMenu(void) 
{
    CRect sRect;
    GetDlgItem(IDC_BATCH_RENAME_FORMAT_MENU)->GetWindowRect(&sRect);

    BCMenu sMenu;
    if (sMenu.LoadMenu(IDR_RENAME_FORMAT) == XPR_TRUE)
    {
        BCMenu *sPopupMenu = (BCMenu *)sMenu.GetSubMenu(0);
        if (XPR_IS_NOT_NULL(sPopupMenu))
        {
            sPopupMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, sRect.right, sRect.top, this);
        }
    }
}

xpr_bool_t RenTabDlg0::OnCommand(WPARAM wParam, LPARAM lParam) 
{
    xpr_uint_t sNotifyMsg = HIWORD(wParam);
    xpr_uint_t sId = LOWORD(wParam);

    xpr_tchar_t sForamt[MAX_FORMAT + 1] = {0};
    xpr_tchar_t sAppendedFormat[MAX_FORMAT + 1] = {0};
    GetDlgItemText(IDC_BATCH_RENAME_FORMAT, sForamt, MAX_FORMAT);

    if (sId == ID_SRN_FORMAT_MENU_NUMBER || sId == ID_SRN_FORMAT_MENU_NUMBER_ZERO || sId == IDC_BATCH_RENAME_FORMAT_AUTO_NUM)
    {
        xpr_sint_t sLength = GetDlgItemInt(IDC_BATCH_RENAME_FORMAT_LENGTH);
        if (!XPR_IS_RANGE(1, sLength, 9))
        {
            const xpr_tchar_t *sMsg = theApp.loadString(XPR_STRING_LITERAL("popup.batch_rename.tab.format.msg.wrong_number_digit"));
            MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONWARNING);

            GetDlgItem(IDC_BATCH_RENAME_FORMAT_LENGTH)->SetFocus();
        }
        else
        {
            xpr_bool_t sZero = (sId == ID_SRN_FORMAT_MENU_NUMBER_ZERO) || ((sId == IDC_BATCH_RENAME_FORMAT_AUTO_NUM) && GetKeyState(VK_CONTROL) >= 0);

            if (sId == IDC_BATCH_RENAME_FORMAT_AUTO_NUM)
                sZero = ((CButton *)GetDlgItem(IDC_BATCH_RENAME_FORMAT_NUMBER_ZERO))->GetCheck();

            _stprintf(sAppendedFormat, XPR_STRING_LITERAL("%s/%s%d"), sForamt, (sZero == XPR_TRUE) ? XPR_STRING_LITERAL("0") : XPR_STRING_LITERAL(""), sLength);

            SetDlgItemText(IDC_BATCH_RENAME_FORMAT, sAppendedFormat);
        }
    }
    else if (sId == ID_SRN_FORMAT_MENU_ORIGINAL_FILENAME_RANGE)
    {
        _stprintf(sAppendedFormat, XPR_STRING_LITERAL("%s/N(?-?)"), sForamt);

        SetDlgItemText(IDC_BATCH_RENAME_FORMAT, sAppendedFormat);
        xpr_sint_t sLen = (xpr_sint_t)_tcslen(sAppendedFormat);

        CComboBox *sComboBox = (CComboBox *)GetDlgItem(IDC_BATCH_RENAME_FORMAT);
        if (sComboBox != XPR_NULL && sComboBox->m_hWnd != XPR_NULL)
        {
            sComboBox->SetFocus();
            sComboBox->SetEditSel(sLen - 4, sLen - 3);
        }
    }
    else
    {
        xpr_bool_t sOk = XPR_TRUE;
        switch (sId)
        {
        case ID_SRN_FORMAT_MENU_FULLNAME:               _stprintf(sAppendedFormat, XPR_STRING_LITERAL("%s/*"), sForamt); break;
        case IDC_BATCH_RENAME_FORMAT_NAME:
        case ID_SRN_FORMAT_MENU_FILENAME:               _stprintf(sAppendedFormat, XPR_STRING_LITERAL("%s/n"), sForamt); break;
        case IDC_BATCH_RENAME_FORMAT_EXT:
        case ID_SRN_FORMAT_MENU_EXTENSION:              _stprintf(sAppendedFormat, XPR_STRING_LITERAL("%s/e"), sForamt); break;
        case IDC_BATCH_RENAME_FORMAT_DATE:
        case ID_SRN_FORMAT_MENU_DATE:                   _stprintf(sAppendedFormat, XPR_STRING_LITERAL("%s/D"), sForamt); break;
        case ID_SRN_FORMAT_MENU_YEAR:                   _stprintf(sAppendedFormat, XPR_STRING_LITERAL("%s/y"), sForamt); break;
        case ID_SRN_FORMAT_MENU_MONTH:                  _stprintf(sAppendedFormat, XPR_STRING_LITERAL("%s/m"), sForamt); break;
        case ID_SRN_FORMAT_MENU_DAY:                    _stprintf(sAppendedFormat, XPR_STRING_LITERAL("%s/d"), sForamt); break;
        case IDC_BATCH_RENAME_FORMAT_TIME:
        case ID_SRN_FORMAT_MENU_TIME:                   _stprintf(sAppendedFormat, XPR_STRING_LITERAL("%s/T"), sForamt); break;
        case ID_SRN_FORMAT_MENU_HOUR:                   _stprintf(sAppendedFormat, XPR_STRING_LITERAL("%s/h"), sForamt); break;
        case ID_SRN_FORMAT_MENU_MINUTE:                 _stprintf(sAppendedFormat, XPR_STRING_LITERAL("%s/i"), sForamt); break;
        case ID_SRN_FORMAT_MENU_SECOND:                 _stprintf(sAppendedFormat, XPR_STRING_LITERAL("%s/s"), sForamt); break;
        case ID_SRN_FORMAT_MENU_CREATION_DATE:          _stprintf(sAppendedFormat, XPR_STRING_LITERAL("%s/A"), sForamt); break;
        case ID_SRN_FORMAT_MENU_CREATION_YEAR:          _stprintf(sAppendedFormat, XPR_STRING_LITERAL("%s/a"), sForamt); break;
        case ID_SRN_FORMAT_MENU_CREATION_MONTH:         _stprintf(sAppendedFormat, XPR_STRING_LITERAL("%s/b"), sForamt); break;
        case ID_SRN_FORMAT_MENU_CREATION_DAY:           _stprintf(sAppendedFormat, XPR_STRING_LITERAL("%s/c"), sForamt); break;
        case ID_SRN_FORMAT_MENU_CREATION_TIME:          _stprintf(sAppendedFormat, XPR_STRING_LITERAL("%s/F"), sForamt); break;
        case ID_SRN_FORMAT_MENU_CREATION_AM_PM_HOUR:    _stprintf(sAppendedFormat, XPR_STRING_LITERAL("%s/f"), sForamt); break;
        case ID_SRN_FORMAT_MENU_CREATION_12_HOUR:       _stprintf(sAppendedFormat, XPR_STRING_LITERAL("%s/g"), sForamt); break;
        case ID_SRN_FORMAT_MENU_CREATION_24_HOUR:       _stprintf(sAppendedFormat, XPR_STRING_LITERAL("%s/k"), sForamt); break;
        case ID_SRN_FORMAT_MENU_CREATION_MINUTE:        _stprintf(sAppendedFormat, XPR_STRING_LITERAL("%s/l"), sForamt); break;
        case ID_SRN_FORMAT_MENU_CREATION_SECOND:        _stprintf(sAppendedFormat, XPR_STRING_LITERAL("%s/r"), sForamt); break;
        case ID_SRN_FORMAT_MENU_LAST_WRITE_DATE:        _stprintf(sAppendedFormat, XPR_STRING_LITERAL("%s/O"), sForamt); break;
        case ID_SRN_FORMAT_MENU_LAST_WRITE_YEAR:        _stprintf(sAppendedFormat, XPR_STRING_LITERAL("%s/o"), sForamt); break;
        case ID_SRN_FORMAT_MENU_LAST_WRITE_MONTH:       _stprintf(sAppendedFormat, XPR_STRING_LITERAL("%s/p"), sForamt); break;
        case ID_SRN_FORMAT_MENU_LAST_WRITE_DAY:         _stprintf(sAppendedFormat, XPR_STRING_LITERAL("%s/q"), sForamt); break;
        case ID_SRN_FORMAT_MENU_LAST_WRITE_TIME:        _stprintf(sAppendedFormat, XPR_STRING_LITERAL("%s/J"), sForamt); break;
        case ID_SRN_FORMAT_MENU_LAST_WRITE_AM_PM_HOUR:  _stprintf(sAppendedFormat, XPR_STRING_LITERAL("%s/j"), sForamt); break;
        case ID_SRN_FORMAT_MENU_LAST_WRITE_12_HOUR:     _stprintf(sAppendedFormat, XPR_STRING_LITERAL("%s/u"), sForamt); break;
        case ID_SRN_FORMAT_MENU_LAST_WRITE_24_HOUR:     _stprintf(sAppendedFormat, XPR_STRING_LITERAL("%s/w"), sForamt); break;
        case ID_SRN_FORMAT_MENU_LAST_WRITE_MINUTE:      _stprintf(sAppendedFormat, XPR_STRING_LITERAL("%s/v"), sForamt); break;
        case ID_SRN_FORMAT_MENU_LAST_WRITE_SECOND:      _stprintf(sAppendedFormat, XPR_STRING_LITERAL("%s/z"), sForamt); break;

        default:
            sOk = XPR_FALSE;
            break;
        }

        if (sOk == XPR_TRUE)
            SetDlgItemText(IDC_BATCH_RENAME_FORMAT, sAppendedFormat);
    }

    return CResizingDialog::OnCommand(wParam, lParam);
}
