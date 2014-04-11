//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "batch_create_tab_format_dlg.h"
#include "batch_create.h"

#include "gui/BCMenu.h"

#include "resource.h"
#include "dlg_state.h"
#include "dlg_state_manager.h"
#include "batch_create_dlg.h"

#include "command_string_table.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace fxfile
{
namespace cmd
{
BatchCreateTabFormatDlg::BatchCreateTabFormatDlg(void)
    : super(IDD_BATCH_CREATE_TAB_FORMAT)
    , mDlgState(XPR_NULL)
{
}

void BatchCreateTabFormatDlg::DoDataExchange(CDataExchange* pDX)
{
    super::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(BatchCreateTabFormatDlg, super)
    ON_WM_DESTROY()
    ON_WM_INITMENUPOPUP()
    ON_BN_CLICKED(IDC_BATCH_CREATE_FORMAT_MENU, OnFormatMenu)
END_MESSAGE_MAP()

xpr_bool_t BatchCreateTabFormatDlg::OnInitDialog(void) 
{
    super::OnInitDialog();

    // CResizingDialog -------------------------------------------
    HideSizeIcon();

    //sizeNone:     Don't resize at all  
    //sizeResize:   The control will be stretched in the appropriate direction 
    //sizeRepos:    The control will be moved in the appropriate direction 
    //sizeRelative: The control will be moved proportionally in the appropriate direction 

    AddControl(IDC_BATCH_CREATE_FORMAT,           sizeResize, sizeNone);
    AddControl(IDC_BATCH_CREATE_FORMAT_MENU,      sizeRepos,  sizeNone, XPR_FALSE);

    AddControl(IDC_BATCH_CREATE_FORMAT_NUMBERING, sizeRepos,  sizeNone, XPR_FALSE);
    AddControl(IDC_BATCH_CREATE_FORMAT_DATE,      sizeRepos,  sizeNone, XPR_FALSE);
    AddControl(IDC_BATCH_CREATE_FORMAT_TIME,      sizeRepos,  sizeNone, XPR_FALSE);
    //------------------------------------------------------------

    ((CSpinButtonCtrl*)GetDlgItem(IDC_BATCH_CREATE_FORMAT_NUMBER_START_SPIN   ))->SetRange32(FXFILE_BATCH_CREATE_FORMAT_NUMBER_MIN,   FXFILE_BATCH_CREATE_FORMAT_NUMBER_MAX);
    ((CSpinButtonCtrl*)GetDlgItem(IDC_BATCH_CREATE_FORMAT_NUMBER_COUNT_SPIN   ))->SetRange32(FXFILE_BATCH_CREATE_FORMAT_COUNT_MIN,    FXFILE_BATCH_CREATE_FORMAT_COUNT_MAX);
    ((CSpinButtonCtrl*)GetDlgItem(IDC_BATCH_CREATE_FORMAT_NUMBER_INCREASE_SPIN))->SetRange32(FXFILE_BATCH_CREATE_FORMAT_INCREASE_MIN, FXFILE_BATCH_CREATE_FORMAT_INCREASE_MAX);
    ((CSpinButtonCtrl*)GetDlgItem(IDC_BATCH_CREATE_FORMAT_NUMBER_DIGIT_SPIN   ))->SetRange32(FXFILE_BATCH_CREATE_FORMAT_DIGIT_MIN,    FXFILE_BATCH_CREATE_FORMAT_DIGIT_MAX);

    SetDlgItemInt(IDC_BATCH_CREATE_FORMAT_NUMBER_START,    FXFILE_BATCH_CREATE_FORMAT_NUMBER_DEF,   XPR_NULL);
    SetDlgItemInt(IDC_BATCH_CREATE_FORMAT_NUMBER_COUNT,    FXFILE_BATCH_CREATE_FORMAT_COUNT_DEF,    XPR_NULL);
    SetDlgItemInt(IDC_BATCH_CREATE_FORMAT_NUMBER_DIGIT,    FXFILE_BATCH_CREATE_FORMAT_DIGIT_DEF, XPR_NULL);
    SetDlgItemInt(IDC_BATCH_CREATE_FORMAT_NUMBER_INCREASE, FXFILE_BATCH_CREATE_FORMAT_INCREASE_DEF,    XPR_NULL);

    ((CEdit *)GetDlgItem(IDC_BATCH_CREATE_FORMAT))->LimitText(FXFILE_BATCH_CREATE_FORMAT_MAX_LENGTH);

    mDlgState = DlgStateManager::instance().getDlgState(XPR_STRING_LITERAL("BatchCreateFormat"));
    if (XPR_IS_NOT_NULL(mDlgState))
    {
        mDlgState->setDialog(this);
        mDlgState->setCheckBox(XPR_STRING_LITERAL("Zero"),     IDC_BATCH_CREATE_FORMAT_NUMBER_ZERO);
        mDlgState->setEditCtrl(XPR_STRING_LITERAL("Format"),   IDC_BATCH_CREATE_FORMAT);
        mDlgState->setEditCtrl(XPR_STRING_LITERAL("Start"),    IDC_BATCH_CREATE_FORMAT_NUMBER_START);
        mDlgState->setEditCtrl(XPR_STRING_LITERAL("Count"),    IDC_BATCH_CREATE_FORMAT_NUMBER_COUNT);
        mDlgState->setEditCtrl(XPR_STRING_LITERAL("Increase"), IDC_BATCH_CREATE_FORMAT_NUMBER_INCREASE);
        mDlgState->setEditCtrl(XPR_STRING_LITERAL("Digit"),    IDC_BATCH_CREATE_FORMAT_NUMBER_DIGIT);
        mDlgState->load();
    }

    SetDlgItemText(IDC_BATCH_CREATE_LABEL_FORMAT,          gApp.loadString(XPR_STRING_LITERAL("popup.batch_create.tab.format.label.format")));
    SetDlgItemText(IDC_BATCH_CREATE_FORMAT_NUMBERING,      gApp.loadString(XPR_STRING_LITERAL("popup.batch_create.tab.format.button.numbering")));
    SetDlgItemText(IDC_BATCH_CREATE_FORMAT_DATE,           gApp.loadString(XPR_STRING_LITERAL("popup.batch_create.tab.format.button.date")));
    SetDlgItemText(IDC_BATCH_CREATE_FORMAT_TIME,           gApp.loadString(XPR_STRING_LITERAL("popup.batch_create.tab.format.button.time")));
    SetDlgItemText(IDC_BATCH_CREATE_LABEL_NUMBER_INCREASE, gApp.loadString(XPR_STRING_LITERAL("popup.batch_create.tab.format.label.increase")));
    SetDlgItemText(IDC_BATCH_CREATE_LABEL_NUMBER_START,    gApp.loadString(XPR_STRING_LITERAL("popup.batch_create.tab.format.label.start_number")));
    SetDlgItemText(IDC_BATCH_CREATE_LABEL_NUMBER_COUNT,    gApp.loadString(XPR_STRING_LITERAL("popup.batch_create.tab.format.label.count")));
    SetDlgItemText(IDC_BATCH_CREATE_LABEL_NUMBER_LENGTH,   gApp.loadString(XPR_STRING_LITERAL("popup.batch_create.tab.format.label.length")));
    SetDlgItemText(IDC_BATCH_CREATE_FORMAT_NUMBER_ZERO,    gApp.loadString(XPR_STRING_LITERAL("popup.batch_create.tab.format.check.zero_filled")));

    return XPR_TRUE;
}

void BatchCreateTabFormatDlg::OnDestroy(void) 
{
    super::OnDestroy();

    if (XPR_IS_NOT_NULL(mDlgState))
    {
        mDlgState->reset();
        mDlgState->save();
    }
}

void BatchCreateTabFormatDlg::OnInitMenuPopup(CMenu *aPopupMenu, xpr_uint_t aIndex, xpr_bool_t aSysMenu)
{
    //CResizingDialog::OnInitMenuPopup(aPopupMenu, aIndex, aSysMenu);
    ASSERT(aPopupMenu != XPR_NULL);

    // for multi-language
    BCMenu *sBCPopupMenu = dynamic_cast<BCMenu *>(aPopupMenu);
    if (sBCPopupMenu != XPR_NULL)
    {
        xpr_uint_t sId;
        xpr_sint_t sCount = aPopupMenu->GetMenuItemCount();

        const xpr_tchar_t *sStringId;
        const xpr_tchar_t *sString;
        CString sMenuText;
        CommandStringTable &sCommandStringTable = CommandStringTable::instance();

        xpr_sint_t i;
        for (i = 0; i < sCount; ++i)
        {
            sId = sBCPopupMenu->GetMenuItemID(i);

            // apply string table
            if (sId != 0) // if nId is 0, it's separator.
            {
                if (sId == -1)
                {
                    // if nId(xpr_uint_t) is -1, it's sub-menu.
                    sBCPopupMenu->GetMenuText(i, sMenuText, MF_BYPOSITION);

                    sString = gApp.loadString(sMenuText.GetBuffer());
                    sBCPopupMenu->SetMenuText(i, (xpr_tchar_t *)sString, MF_BYPOSITION);
                }
                else
                {
                    sStringId = sCommandStringTable.loadString(sId);
                    if (sStringId != XPR_NULL)
                    {
                        sString = gApp.loadString(sStringId);

                        sBCPopupMenu->SetMenuText(sId, (xpr_tchar_t *)sString, MF_BYCOMMAND);
                    }
                }
            }
        }
    }

    // Check the enabled state of various menu items.
    CCmdUI sState;
    sState.m_pMenu = aPopupMenu;
    ASSERT(sState.m_pOther == XPR_NULL);
    ASSERT(sState.m_pParentMenu == XPR_NULL);

    // Determine if menu is popup in top-level menu and set m_pOther to
    // it if so (m_pParentMenu == XPR_NULL indicates that it is secondary popup).
    HMENU sParentMenu;
    if (AfxGetThreadState()->m_hTrackingMenu == aPopupMenu->m_hMenu)
        sState.m_pParentMenu = aPopupMenu;    // Parent == child for tracking popup.
    else if ((sParentMenu = ::GetMenu(m_hWnd)) != XPR_NULL)
    {
        CWnd *aParentWnd = this;
        // Child windows don't have menus--need to go to the top!
        if (aParentWnd != XPR_NULL && (sParentMenu = ::GetMenu(aParentWnd->m_hWnd)) != XPR_NULL)
        {
            xpr_sint_t nIndexMax = ::GetMenuItemCount(sParentMenu);
            for (xpr_sint_t sIndex = 0; sIndex < nIndexMax; ++sIndex)
            {
                if (::GetSubMenu(sParentMenu, sIndex) == aPopupMenu->m_hMenu)
                {
                    // When popup is found, m_pParentMenu is containing menu.
                    sState.m_pParentMenu = CMenu::FromHandle(sParentMenu);
                    break;
                }
            }
        }
    }

    sState.m_nIndexMax = aPopupMenu->GetMenuItemCount();
    for (sState.m_nIndex = 0; sState.m_nIndex < sState.m_nIndexMax; ++sState.m_nIndex)
    {
        sState.m_nID = aPopupMenu->GetMenuItemID(sState.m_nIndex);
        if (sState.m_nID == 0)
            continue; // Menu separator or invalid cmd - ignore it.

        ASSERT(sState.m_pOther == XPR_NULL);
        ASSERT(sState.m_pMenu != XPR_NULL);
        if (sState.m_nID == (xpr_uint_t)-1)
        {
            // Possibly a popup menu, route to first item of that popup.
            sState.m_pSubMenu = aPopupMenu->GetSubMenu(sState.m_nIndex);
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
        xpr_uint_t sCount = aPopupMenu->GetMenuItemCount();
        if (sCount < sState.m_nIndexMax)
        {
            sState.m_nIndex -= (sState.m_nIndexMax - sCount);
            while (sState.m_nIndex < sCount && aPopupMenu->GetMenuItemID(sState.m_nIndex) == sState.m_nID)
            {
                sState.m_nIndex++;
            }
        }

        sState.m_nIndexMax = sCount;
    }
}

void BatchCreateTabFormatDlg::OnTabInit(void)
{
}

void BatchCreateTabFormatDlg::getFormat(xpr_tchar_t *aFormat, xpr_sint_t aFormatLen) const
{
    if (aFormat == XPR_NULL || aFormatLen <= 0)
        return;

    GetDlgItemText(IDC_BATCH_CREATE_FORMAT, aFormat, aFormatLen);
}

xpr_sint_t BatchCreateTabFormatDlg::getStart(void) const
{
    return GetDlgItemInt(IDC_BATCH_CREATE_FORMAT_NUMBER_START);
}

xpr_size_t BatchCreateTabFormatDlg::getCount(void) const
{
    return GetDlgItemInt(IDC_BATCH_CREATE_FORMAT_NUMBER_COUNT, XPR_NULL, XPR_FALSE);
}

xpr_sint_t BatchCreateTabFormatDlg::getIncrease(void) const
{
    return GetDlgItemInt(IDC_BATCH_CREATE_FORMAT_NUMBER_INCREASE);
}

void BatchCreateTabFormatDlg::OnFormatMenu(void) 
{
    CRect sRect;
    GetDlgItem(IDC_BATCH_CREATE_FORMAT_MENU)->GetWindowRect(&sRect);

    BCMenu sMenu;
    if (sMenu.LoadMenu(IDR_BATCH_CREATE_FORMAT) == XPR_TRUE)
    {
        BCMenu *sPopupMenu = (BCMenu *)sMenu.GetSubMenu(0);
        if (XPR_IS_NOT_NULL(sPopupMenu))
        {
            sPopupMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, sRect.right, sRect.top, this);
        }
    }
}

xpr_bool_t BatchCreateTabFormatDlg::OnCommand(WPARAM wParam, LPARAM lParam) 
{
    xpr_uint_t sNotifyMsg = HIWORD(wParam);
    xpr_uint_t sId = LOWORD(wParam);

    xpr_tchar_t sFormat[FXFILE_BATCH_CREATE_FORMAT_MAX_LENGTH + 1] = {0};
    GetDlgItemText(IDC_BATCH_CREATE_FORMAT, sFormat, FXFILE_BATCH_CREATE_FORMAT_MAX_LENGTH);

    xpr::string sAppendedFormat(sFormat);
    if (sId == IDC_BATCH_CREATE_FORMAT_NUMBERING || sId == ID_BATCH_CREATE_FORMAT_MENU_NUMBERING)
    {
        xpr_sint_t sNumber   = GetDlgItemInt(IDC_BATCH_CREATE_FORMAT_NUMBER_START);
        xpr_sint_t sIncrease = GetDlgItemInt(IDC_BATCH_CREATE_FORMAT_NUMBER_INCREASE);
        xpr_size_t sDigit    = GetDlgItemInt(IDC_BATCH_CREATE_FORMAT_NUMBER_DIGIT, XPR_FALSE);
        xpr_bool_t sZero     = ((sId == IDC_BATCH_CREATE_FORMAT_NUMBERING) && GetKeyState(VK_CONTROL) >= 0);

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

        SetDlgItemText(IDC_BATCH_CREATE_FORMAT, sAppendedFormat.c_str());
    }
    else
    {
        xpr_bool_t sSuccess = XPR_TRUE;
        switch (sId)
        {
        case IDC_BATCH_CREATE_FORMAT_DATE:
        case ID_BATCH_CREATE_FORMAT_MENU_DATE:   sAppendedFormat.append_format(XPR_STRING_LITERAL("<time:yyyy-MM-dd>")); break;
        case ID_BATCH_CREATE_FORMAT_MENU_YEAR:   sAppendedFormat.append_format(XPR_STRING_LITERAL("<time:yyyy>"));       break;
        case ID_BATCH_CREATE_FORMAT_MENU_MONTH:  sAppendedFormat.append_format(XPR_STRING_LITERAL("<time:MM>"));         break;
        case ID_BATCH_CREATE_FORMAT_MENU_DAY:    sAppendedFormat.append_format(XPR_STRING_LITERAL("<time:dd>"));         break;
        case IDC_BATCH_CREATE_FORMAT_TIME:
        case ID_BATCH_CREATE_FORMAT_MENU_TIME:   sAppendedFormat.append_format(XPR_STRING_LITERAL("<time:HH.mm.ss>"));   break;
        case ID_BATCH_CREATE_FORMAT_MENU_HOUR:   sAppendedFormat.append_format(XPR_STRING_LITERAL("<time:HH>"));         break;
        case ID_BATCH_CREATE_FORMAT_MENU_MINUTE: sAppendedFormat.append_format(XPR_STRING_LITERAL("<time:mm>"));         break;
        case ID_BATCH_CREATE_FORMAT_MENU_SECOND: sAppendedFormat.append_format(XPR_STRING_LITERAL("<time:ss>"));         break;

        default:
            sSuccess = XPR_FALSE;
            break;
        }

        if (XPR_IS_TRUE(sSuccess))
        {
            SetDlgItemText(IDC_BATCH_CREATE_FORMAT, sAppendedFormat.c_str());
        }
    }

    return super::OnCommand(wParam, lParam);
}
} // namespace cmd
} // namespace fxfile
