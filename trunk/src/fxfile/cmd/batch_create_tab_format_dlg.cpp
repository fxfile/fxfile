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
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
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
    ON_BN_CLICKED(IDC_CREATE_IS_NUMBER_END,   OnIsNumberEnd)
    ON_BN_CLICKED(IDC_CREATE_IS_NUMBER_COUNT, OnIsNumberCount)
    ON_BN_CLICKED(IDC_CREATE_FORMAT_MENU,     OnFormatMenu)
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

    AddControl(IDC_CREATE_FORMAT,          sizeResize, sizeNone);
    AddControl(IDC_CREATE_FORMAT_MENU,     sizeRepos,  sizeNone, XPR_FALSE);

    AddControl(IDC_CREATE_FORMAT_AUTO_NUM, sizeRepos,  sizeNone, XPR_FALSE);
    AddControl(IDC_CREATE_FORMAT_DATE,     sizeRepos,  sizeNone, XPR_FALSE);
    AddControl(IDC_CREATE_FORMAT_TIME,     sizeRepos,  sizeNone, XPR_FALSE);
    //------------------------------------------------------------

    ((CSpinButtonCtrl*)GetDlgItem(IDC_CREATE_NUMBER_START_SPIN   ))->SetRange32(0, 999999999);
    ((CSpinButtonCtrl*)GetDlgItem(IDC_CREATE_NUMBER_END_SPIN     ))->SetRange32(0, 999999999);
    ((CSpinButtonCtrl*)GetDlgItem(IDC_CREATE_NUMBER_COUNT_SPIN   ))->SetRange32(0, 999999999);
    ((CSpinButtonCtrl*)GetDlgItem(IDC_CREATE_NUMBER_INCREASE_SPIN))->SetRange32(0, 999999999);
    ((CSpinButtonCtrl*)GetDlgItem(IDC_CREATE_NUMBER_LENGTH_SPIN  ))->SetRange32(1, 9);

    SetDlgItemInt(IDC_CREATE_NUMBER_LENGTH,   1, XPR_NULL);
    SetDlgItemInt(IDC_CREATE_NUMBER_INCREASE, 1, XPR_NULL);

    ((CEdit *)GetDlgItem(IDC_CREATE_FORMAT))->LimitText(XPR_MAX_PATH);
    ((CButton *)GetDlgItem(IDC_CREATE_IS_NUMBER_END))->SetCheck(1);

    mDlgState = DlgStateManager::instance().getDlgState(XPR_STRING_LITERAL("BatchCreateFormat"));
    if (XPR_IS_NOT_NULL(mDlgState))
    {
        mDlgState->setDialog(this);
        mDlgState->setCheckBox(XPR_STRING_LITERAL("Zero"),       IDC_CREATE_NUMBER_ZERO);
        mDlgState->setCheckBox(XPR_STRING_LITERAL("EndOrCount"), IDC_CREATE_IS_NUMBER_END);
        mDlgState->setEditCtrl(XPR_STRING_LITERAL("Format"),     IDC_CREATE_FORMAT);
        mDlgState->setEditCtrl(XPR_STRING_LITERAL("Start"),      IDC_CREATE_NUMBER_START);
        mDlgState->setEditCtrl(XPR_STRING_LITERAL("End"),        IDC_CREATE_NUMBER_END);
        mDlgState->setEditCtrl(XPR_STRING_LITERAL("Count"),      IDC_CREATE_NUMBER_COUNT);
        mDlgState->setEditCtrl(XPR_STRING_LITERAL("Increase"),   IDC_CREATE_NUMBER_INCREASE);
        mDlgState->setEditCtrl(XPR_STRING_LITERAL("Length"),     IDC_CREATE_NUMBER_LENGTH);
        mDlgState->load();
    }

    xpr_bool_t sEnd = ((CButton *)GetDlgItem(IDC_CREATE_IS_NUMBER_END))->GetCheck();
    ((CButton *)GetDlgItem((sEnd == XPR_TRUE) ? IDC_CREATE_IS_NUMBER_END : IDC_CREATE_IS_NUMBER_COUNT))->SetCheck(1);
    EnableWindow(sEnd);

    SetDlgItemText(IDC_CREATE_LABEL_FORMAT,          theApp.loadString(XPR_STRING_LITERAL("popup.batch_create.tab.format.label.format")));
    SetDlgItemText(IDC_CREATE_FORMAT_AUTO_NUM,       theApp.loadString(XPR_STRING_LITERAL("popup.batch_create.tab.format.button.number_digit")));
    SetDlgItemText(IDC_CREATE_FORMAT_DATE,           theApp.loadString(XPR_STRING_LITERAL("popup.batch_create.tab.format.button.date")));
    SetDlgItemText(IDC_CREATE_FORMAT_TIME,           theApp.loadString(XPR_STRING_LITERAL("popup.batch_create.tab.format.button.time")));
    SetDlgItemText(IDC_CREATE_LABEL_NUMBER_INCREASE, theApp.loadString(XPR_STRING_LITERAL("popup.batch_create.tab.format.label.increase")));
    SetDlgItemText(IDC_CREATE_LABEL_NUMBER_START,    theApp.loadString(XPR_STRING_LITERAL("popup.batch_create.tab.format.label.start_number")));
    SetDlgItemText(IDC_CREATE_IS_NUMBER_END,         theApp.loadString(XPR_STRING_LITERAL("popup.batch_create.tab.format.label.end_number")));
    SetDlgItemText(IDC_CREATE_IS_NUMBER_COUNT,       theApp.loadString(XPR_STRING_LITERAL("popup.batch_create.tab.format.label.count")));
    SetDlgItemText(IDC_CREATE_LABEL_NUMBER_LENGTH,   theApp.loadString(XPR_STRING_LITERAL("popup.batch_create.tab.format.label.length")));
    SetDlgItemText(IDC_CREATE_NUMBER_ZERO,           theApp.loadString(XPR_STRING_LITERAL("popup.batch_create.tab.format.check.zero_filled")));

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

                    sString = theApp.loadString(sMenuText.GetBuffer());
                    sBCPopupMenu->SetMenuText(i, (xpr_tchar_t *)sString, MF_BYPOSITION);
                }
                else
                {
                    sStringId = sCommandStringTable.loadString(sId);
                    if (sStringId != XPR_NULL)
                    {
                        sString = theApp.loadString(sStringId);

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

void BatchCreateTabFormatDlg::getFormat(xpr_tchar_t *aFormat, xpr_sint_t aFormatLen)
{
    if (aFormat == XPR_NULL || aFormatLen <= 0)
        return;

    GetDlgItemText(IDC_CREATE_FORMAT, aFormat, aFormatLen);
}

xpr_uint_t BatchCreateTabFormatDlg::getStart(void)
{
    return GetDlgItemInt(IDC_CREATE_NUMBER_START, XPR_NULL, XPR_FALSE);
}

xpr_uint_t BatchCreateTabFormatDlg::getEnd(void)
{
    return GetDlgItemInt(IDC_CREATE_NUMBER_END, XPR_NULL, XPR_FALSE);
}

xpr_uint_t BatchCreateTabFormatDlg::getCount(void)
{
    return GetDlgItemInt(IDC_CREATE_NUMBER_COUNT, XPR_NULL, XPR_FALSE);
}

xpr_uint_t BatchCreateTabFormatDlg::getIncrease(void)
{
    return GetDlgItemInt(IDC_CREATE_NUMBER_INCREASE, XPR_NULL, XPR_FALSE);
}

xpr_bool_t BatchCreateTabFormatDlg::getCountType(void)
{
    return ((CButton *)GetDlgItem(IDC_CREATE_IS_NUMBER_COUNT))->GetCheck();
}

void BatchCreateTabFormatDlg::enableWindow(xpr_bool_t aEnd)
{
    GetDlgItem(IDC_CREATE_NUMBER_END)->EnableWindow(aEnd);
    GetDlgItem(IDC_CREATE_NUMBER_END_SPIN)->EnableWindow(aEnd);
    GetDlgItem(IDC_CREATE_NUMBER_COUNT)->EnableWindow(!aEnd);
    GetDlgItem(IDC_CREATE_NUMBER_COUNT_SPIN)->EnableWindow(!aEnd);
}

void BatchCreateTabFormatDlg::OnIsNumberEnd(void) 
{
    EnableWindow(XPR_TRUE);
}

void BatchCreateTabFormatDlg::OnIsNumberCount(void) 
{
    EnableWindow(XPR_FALSE);
}

void BatchCreateTabFormatDlg::OnFormatMenu(void) 
{
    CRect sRect;
    GetDlgItem(IDC_CREATE_FORMAT_MENU)->GetWindowRect(&sRect);

    BCMenu sMenu;
    if (sMenu.LoadMenu(IDR_NEW_ITEMS) == XPR_TRUE)
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

    xpr_tchar_t sText[XPR_MAX_PATH + 1] = {0};
    xpr_tchar_t sFormat[XPR_MAX_PATH + 1] = {0};
    GetDlgItemText(IDC_CREATE_FORMAT, sText, XPR_MAX_PATH);

    if (sId == IDC_CREATE_FORMAT_AUTO_NUM || sId == ID_NEW_FORMAT_MENU_NUMBER || sId == ID_NEW_FORMAT_MENU_NUMBER_ZERO)
    {
        xpr_sint_t sLength = GetDlgItemInt(IDC_CREATE_NUMBER_LENGTH);
        if (!XPR_IS_RANGE(1, sLength, 9))
        {
            const xpr_tchar_t *sMsg = theApp.loadString(XPR_STRING_LITERAL("popup.batch_create.msg.wrong_number_length"));
            MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONWARNING);

            GetDlgItem(IDC_CREATE_NUMBER_LENGTH)->SetFocus();
        }
        else
        {
            xpr_bool_t sZero = (sId == ID_NEW_FORMAT_MENU_NUMBER_ZERO) || ((sId == IDC_CREATE_FORMAT_AUTO_NUM) && GetKeyState(VK_CONTROL) >= 0);

            if (sId == IDC_CREATE_FORMAT_AUTO_NUM)
                sZero = ((CButton *)GetDlgItem(IDC_CREATE_NUMBER_ZERO))->GetCheck();

            _stprintf(sFormat, XPR_STRING_LITERAL("%s/%s%d"), sText, sZero ? XPR_STRING_LITERAL("0") : XPR_STRING_LITERAL(""), sLength);

            SetDlgItemText(IDC_CREATE_FORMAT, sFormat);
        }
    }
    else
    {
        xpr_bool_t sOk = XPR_TRUE;
        switch (sId)
        {
        case IDC_CREATE_FORMAT_DATE:
        case ID_NEW_FORMAT_MENU_DATE:   _stprintf(sFormat, XPR_STRING_LITERAL("%s/D"), sText); break;
        case ID_NEW_FORMAT_MENU_YEAR:   _stprintf(sFormat, XPR_STRING_LITERAL("%s/y"), sText); break;
        case ID_NEW_FORMAT_MENU_MONTH:  _stprintf(sFormat, XPR_STRING_LITERAL("%s/m"), sText); break;
        case ID_NEW_FORMAT_MENU_DAY:    _stprintf(sFormat, XPR_STRING_LITERAL("%s/d"), sText); break;
        case IDC_CREATE_FORMAT_TIME:
        case ID_NEW_FORMAT_MENU_TIME:   _stprintf(sFormat, XPR_STRING_LITERAL("%s/T"), sText); break;
        case ID_NEW_FORMAT_MENU_HOUR:   _stprintf(sFormat, XPR_STRING_LITERAL("%s/h"), sText); break;
        case ID_NEW_FORMAT_MENU_MINUTE: _stprintf(sFormat, XPR_STRING_LITERAL("%s/i"), sText); break;
        case ID_NEW_FORMAT_MENU_SECOND: _stprintf(sFormat, XPR_STRING_LITERAL("%s/s"), sText); break;
        default: sOk = XPR_FALSE; break;
        }

        if (sOk == XPR_TRUE)
            SetDlgItemText(IDC_CREATE_FORMAT, sFormat);
    }

    return super::OnCommand(wParam, lParam);
}
} // namespace cmd
} // namespace fxfile
