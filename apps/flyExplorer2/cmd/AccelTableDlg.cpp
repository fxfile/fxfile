//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "AccelTableDlg.h"

#include "Option.h"
#include "resource.h"

#include "../command_string_table.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

struct AccelTableDlg::Command
{
    xpr_tchar_t mText[0xff];
    xpr_uint_t  mId;
    xpr_bool_t  mDesc;
};

struct AccelTableDlg::Category
{
    ~Category(void)
    {
        Command *sCommand;
        CommandList::iterator sIterator;

        sIterator = mCommandList.begin();
        for (; sIterator != mCommandList.end(); ++sIterator)
        {
            sCommand = *sIterator;
            XPR_SAFE_DELETE(sCommand);
        }

        mCommandList.clear();
    }

    xpr_tchar_t  mText[0xff];
    CMenu       *mMenu;
    xpr_sint_t   mSubMenu;
    CommandList  mCommandList;
};

AccelTableDlg::AccelTableDlg(HACCEL aAccelHandle, xpr_uint_t aResourceId)
    : super(IDD_ACCEL, XPR_NULL)
    , mCount(0)
    , mAccelHandle(aAccelHandle)
    , mResourceId(aResourceId)
{
    memset(mAccel, 0, sizeof(ACCEL) * MAX_ACCEL);
}

void AccelTableDlg::DoDataExchange(CDataExchange* pDX)
{
    super::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_ACCEL_HOTKEY,   mHotKeyCtrl);
    DDX_Control(pDX, IDC_ACCEL_COMMANDS, mCommandWnd);
    DDX_Control(pDX, IDC_ACCEL_CUR_KEYS, mCurKeysWnd);
    DDX_Control(pDX, IDC_ACCEL_CATEGORY, mCategoryWnd);
    DDX_Control(pDX, IDC_ACCEL_VIRTUAL,  mVirtualWnd);
}

BEGIN_MESSAGE_MAP(AccelTableDlg, super)
    ON_WM_DESTROY()
    ON_CBN_SELCHANGE(IDC_ACCEL_CATEGORY,         OnSelchangeCategory)
    ON_CBN_SELCHANGE(IDC_ACCEL_VIRTUAL,          OnKeyInput)
    ON_LBN_SELCHANGE(IDC_ACCEL_COMMANDS,         OnSelchangeCommands)
    ON_LBN_SELCHANGE(IDC_ACCEL_CUR_KEYS,         OnSelchangeCurKeys)
    ON_EN_CHANGE    (IDC_ACCEL_HOTKEY,           OnEnChangeHotKey)
    ON_BN_CLICKED   (IDC_ACCEL_BUTTON_ASSIGN,    OnAssign)
    ON_BN_CLICKED   (IDC_ACCEL_BUTTON_REMOVE,    OnRemove)
    ON_BN_CLICKED   (IDC_ACCEL_BUTTON_RESET,     OnReset)
    ON_BN_CLICKED   (IDC_ACCEL_BUTTON_RESET_ALL, OnResetAll)
    ON_BN_CLICKED   (IDC_ACCEL_CTRL,             OnKeyInput)
    ON_BN_CLICKED   (IDC_ACCEL_ALT,              OnKeyInput)
    ON_BN_CLICKED   (IDC_ACCEL_SHIFT,            OnKeyInput)
END_MESSAGE_MAP()

xpr_bool_t AccelTableDlg::OnInitDialog(void) 
{
    super::OnInitDialog();

    mCount = ::CopyAcceleratorTable(mAccelHandle, mAccel, MAX_ACCEL);
    mMenu.LoadMenu(mResourceId);

    // initialize category
    xpr_sint_t i, sCount, sIndex;
    xpr_tchar_t sText[0xff];
    Category *sCategory;
    Command *sCommand;

    sCount = mMenu.GetMenuItemCount();
    for (i = 0; i < sCount; ++i)
    {
        sCategory = new Category;
        sCategory->mMenu = &mMenu;
        sCategory->mSubMenu = i;

        mMenu.GetMenuString(i, sCategory->mText, 0xfe, MF_BYPOSITION);

        _tcscpy(sCategory->mText, theApp.loadString(sCategory->mText));

        xpr_tchar_t *sEnd = _tcsrchr(sCategory->mText, '(');
        if (sEnd != XPR_NULL)
            *sEnd = '\0';

        _tcscat(sCategory->mText, XPR_STRING_LITERAL(" "));
        _tcscat(sCategory->mText, theApp.loadString(XPR_STRING_LITERAL("popup.accel.menu")));

        mCategoryWnd.AddString(sCategory->mText);
        mCategoryWnd.SetItemData(i, (DWORD_PTR)sCategory);
    }

    // add to all menu item
    sCategory = new Category;
    sCategory->mMenu = &mMenu;
    sCategory->mSubMenu = -1;
    _tcscpy(sCategory->mText, theApp.loadString(XPR_STRING_LITERAL("popup.accel.all_menu")));
    sIndex = mCategoryWnd.AddString(sCategory->mText);
    mCategoryWnd.SetItemData(sIndex, (DWORD_PTR)sCategory);

    // add to working folder item
    sCategory = new Category;
    sCategory->mMenu = XPR_NULL;
    sCategory->mSubMenu = -1;
    _tcscpy(sCategory->mText, theApp.loadString(XPR_STRING_LITERAL("popup.accel.working_folder")));

    for (i = 0; i < MAX_WORKING_FOLDER; ++i)
    {
        sCommand = new Command;
        sCommand->mId = ID_GO_WORKING_FOLDER_FIRST + i;
        sCommand->mDesc = XPR_FALSE;
        _stprintf(sCommand->mText, XPR_STRING_LITERAL("%s %d"), theApp.loadString(XPR_STRING_LITERAL("popup.accel.working_folder")), i + 1);
        sCategory->mCommandList.push_back(sCommand);
    }

    sIndex = mCategoryWnd.AddString(sCategory->mText);
    mCategoryWnd.SetItemData(sIndex, (DWORD_PTR)sCategory);

    mCategoryWnd.SetCurSel(0);
    OnSelchangeCategory();

    GetDlgItem(IDC_ACCEL_BUTTON_ASSIGN)->EnableWindow(XPR_FALSE);

    mCommandWnd.SetHorizontalExtent(350);

    SetDlgItemText(IDC_ACCEL_ASSIGNED_TO, theApp.loadString(XPR_STRING_LITERAL("popup.accel.none")));

    struct
    {
        const xpr_tchar_t *mText;
        xpr_ushort_t       mVirtualKey;
        xpr_ushort_t       mExtendedKey;
    } sVirtualKeys[] = {
        { XPR_STRING_LITERAL("F1"),          VK_F1,         0 },
        { XPR_STRING_LITERAL("F2"),          VK_F2,         0 },
        { XPR_STRING_LITERAL("F3"),          VK_F3,         0 },
        { XPR_STRING_LITERAL("F4"),          VK_F4,         0 },
        { XPR_STRING_LITERAL("F5"),          VK_F5,         0 },
        { XPR_STRING_LITERAL("F6"),          VK_F6,         0 },
        { XPR_STRING_LITERAL("F7"),          VK_F7,         0 },
        { XPR_STRING_LITERAL("F8"),          VK_F8,         0 },
        { XPR_STRING_LITERAL("F9"),          VK_F9,         0 },
        { XPR_STRING_LITERAL("F10"),         VK_F10,        0 },
        { XPR_STRING_LITERAL("F11"),         VK_F11,        0 },
        { XPR_STRING_LITERAL("F12"),         VK_F12,        0 },

        { XPR_STRING_LITERAL("1"),           '1',           0 },
        { XPR_STRING_LITERAL("2"),           '2',           0 },
        { XPR_STRING_LITERAL("3"),           '3',           0 },
        { XPR_STRING_LITERAL("4"),           '4',           0 },
        { XPR_STRING_LITERAL("5"),           '5',           0 },
        { XPR_STRING_LITERAL("6"),           '6',           0 },
        { XPR_STRING_LITERAL("7"),           '7',           0 },
        { XPR_STRING_LITERAL("8"),           '8',           0 },
        { XPR_STRING_LITERAL("9"),           '9',           0 },
        { XPR_STRING_LITERAL("0"),           '0',           0 },

        { XPR_STRING_LITERAL("Backspace"),   VK_BACK,       0 },
        { XPR_STRING_LITERAL("Esc"),         VK_ESCAPE,     0 },
        { XPR_STRING_LITERAL("Enter"),       VK_RETURN,     0 },
        { XPR_STRING_LITERAL("Space"),       VK_SPACE,      0 },
        { XPR_STRING_LITERAL("Tab"),         VK_TAB,        0 },
        { XPR_STRING_LITERAL("Pause"),       VK_PAUSE,      0 },
        { XPR_STRING_LITERAL("Scroll Lock"), VK_SCROLL,     0 },

        { XPR_STRING_LITERAL("Insert"),      VK_INSERT,     HOTKEYF_EXT },
        { XPR_STRING_LITERAL("Delete"),      VK_DELETE,     HOTKEYF_EXT },
        { XPR_STRING_LITERAL("Home"),        VK_HOME,       HOTKEYF_EXT },
        { XPR_STRING_LITERAL("Down"),        VK_DOWN,       HOTKEYF_EXT },
        { XPR_STRING_LITERAL("Page Up"),     VK_PRIOR,      HOTKEYF_EXT },
        { XPR_STRING_LITERAL("Page Down"),   VK_NEXT,       HOTKEYF_EXT },

        { XPR_STRING_LITERAL("Left"),        VK_LEFT,       HOTKEYF_EXT },
        { XPR_STRING_LITERAL("Right"),       VK_RIGHT,      HOTKEYF_EXT },
        { XPR_STRING_LITERAL("End"),         VK_END,        HOTKEYF_EXT },
        { XPR_STRING_LITERAL("Up"),          VK_UP,         HOTKEYF_EXT },

        { XPR_STRING_LITERAL(";"),           VK_OEM_1,      0 },
        { XPR_STRING_LITERAL("="),           VK_OEM_PLUS,   0 },
        { XPR_STRING_LITERAL(","),           VK_OEM_COMMA,  0 },
        { XPR_STRING_LITERAL("-"),           VK_OEM_MINUS,  0 },
        { XPR_STRING_LITERAL("."),           VK_OEM_PERIOD, 0 },
        { XPR_STRING_LITERAL("/"),           VK_OEM_2,      0 },
        { XPR_STRING_LITERAL("`"),           VK_OEM_3,      0 },
        { XPR_STRING_LITERAL("["),           VK_OEM_4,      0 },
        { XPR_STRING_LITERAL("\\"),          VK_OEM_5,      0 },
        { XPR_STRING_LITERAL("]"),           VK_OEM_6,      0 },
        { XPR_STRING_LITERAL("'"),           VK_OEM_7,      0 },

        { XPR_STRING_LITERAL("Num Lock"),    VK_NUMLOCK,    0 },
        { XPR_STRING_LITERAL("Num 0"),       VK_NUMPAD0,    0 },
        { XPR_STRING_LITERAL("Num 1"),       VK_NUMPAD1,    0 },
        { XPR_STRING_LITERAL("Num 2"),       VK_NUMPAD2,    0 },
        { XPR_STRING_LITERAL("Num 3"),       VK_NUMPAD3,    0 },
        { XPR_STRING_LITERAL("Num 4"),       VK_NUMPAD4,    0 },
        { XPR_STRING_LITERAL("Num 5"),       VK_NUMPAD5,    0 },
        { XPR_STRING_LITERAL("Num 6"),       VK_NUMPAD6,    0 },
        { XPR_STRING_LITERAL("Num 7"),       VK_NUMPAD7,    0 },
        { XPR_STRING_LITERAL("Num 8"),       VK_NUMPAD8,    0 },
        { XPR_STRING_LITERAL("Num 9"),       VK_NUMPAD9,    0 },
        { XPR_STRING_LITERAL("Num /"),       VK_DIVIDE,     HOTKEYF_EXT },
        { XPR_STRING_LITERAL("Num *"),       VK_MULTIPLY,   0 },
        { XPR_STRING_LITERAL("Num -"),       VK_SUBTRACT,   0 },
        { XPR_STRING_LITERAL("Num +"),       VK_ADD,        0 },
        { XPR_STRING_LITERAL("Num Del"),     VK_DECIMAL,    0 },

        { XPR_NULL,                          0,             0 }
    };

    DWORD sKey;
    xpr_tchar_t sChar;

    // A~Z
    for (i = 0; i < 26; ++i)
    {
        sChar = 'A' + i;

        _stprintf(sText, XPR_STRING_LITERAL("%c"), sChar);
        sIndex = mVirtualWnd.AddString(sText);
        sKey = MAKELONG((xpr_ushort_t)sChar, 0);
        mVirtualWnd.SetItemData(sIndex, (DWORD_PTR)sKey);
    }

    for (i = 0; sVirtualKeys[i].mText != XPR_NULL; ++i)
    {
        sIndex = mVirtualWnd.AddString(sVirtualKeys[i].mText);
        sKey = MAKELONG(sVirtualKeys[i].mVirtualKey, sVirtualKeys[i].mExtendedKey);
        mVirtualWnd.SetItemData(sIndex, (DWORD_PTR)sKey);
    }

    SetWindowText(theApp.loadString(XPR_STRING_LITERAL("popup.accel.title")));
    SetDlgItemText(IDC_ACCEL_LABEL_CATEGORY,         theApp.loadString(XPR_STRING_LITERAL("popup.accel.label.category")));
    SetDlgItemText(IDC_ACCEL_LABEL_COMMANDS,         theApp.loadString(XPR_STRING_LITERAL("popup.accel.label.commands")));
    SetDlgItemText(IDC_ACCEL_LABEL_CUR_KEYS,         theApp.loadString(XPR_STRING_LITERAL("popup.accel.label.current_keys")));
    SetDlgItemText(IDC_ACCEL_LABEL_NEW_SHORTCUT_KEY, theApp.loadString(XPR_STRING_LITERAL("popup.accel.label.new_shortcut_key")));
    SetDlgItemText(IDC_ACCEL_LABEL_ASSIGNED_TO,      theApp.loadString(XPR_STRING_LITERAL("popup.accel.label.assigned_to")));
    SetDlgItemText(IDC_ACCEL_BUTTON_ASSIGN,          theApp.loadString(XPR_STRING_LITERAL("popup.accel.button.assign")));
    SetDlgItemText(IDC_ACCEL_BUTTON_REMOVE,          theApp.loadString(XPR_STRING_LITERAL("popup.accel.button.remove")));
    SetDlgItemText(IDC_ACCEL_BUTTON_RESET,           theApp.loadString(XPR_STRING_LITERAL("popup.accel.button.reset")));
    SetDlgItemText(IDC_ACCEL_BUTTON_RESET_ALL,       theApp.loadString(XPR_STRING_LITERAL("popup.accel.button.reset_all")));
    SetDlgItemText(IDOK,                             theApp.loadString(XPR_STRING_LITERAL("popup.common.button.ok")));
    SetDlgItemText(IDCANCEL,                         theApp.loadString(XPR_STRING_LITERAL("popup.common.button.cancel")));

    return XPR_TRUE;
}

void AccelTableDlg::OnDestroy(void) 
{
    clearCommands();
    clearCategory();

    super::OnDestroy();
}

bool AccelTableDlg::sortAllCommands(Command *aCommand1, Command *aCommand2)
{
    return (_tcsicmp(aCommand1->mText, aCommand2->mText) < 0) ? true : false;
}

void AccelTableDlg::OnSelchangeCategory(void) 
{
    xpr_sint_t sCurSel = mCategoryWnd.GetCurSel();
    Category *sCategory = (Category *)mCategoryWnd.GetItemData(sCurSel);

    xpr_sint_t sIndex;
    Command *sCommand;
    CommandList::iterator sIterator;
    CommandList sCommandList;

    if (sCategory->mMenu != XPR_NULL)
    {
        CMenu *sSubMenu = sCategory->mMenu->GetSubMenu(sCategory->mSubMenu);
        if (sSubMenu != XPR_NULL)
        {
            fillRecursiveCategory(sCommandList, sSubMenu);
        }
        else
        {
            // all menu
            fillRecursiveCategory(sCommandList, sCategory->mMenu, XPR_NULL, XPR_FALSE);
            sCommandList.sort(sortAllCommands);
        }
    }

    if (sCategory->mCommandList.empty() == false)
    {
        Command *sNewCommand;

        sIterator = sCategory->mCommandList.begin();
        for (; sIterator != sCategory->mCommandList.end(); ++sIterator)
        {
            sCommand = *sIterator;

            sNewCommand = new Command;
            *sNewCommand = *sCommand;

            sCommandList.push_back(sNewCommand);
        }
    }

    clearCommands();

    sIterator = sCommandList.begin();
    for (; sIterator != sCommandList.end(); ++sIterator)
    {
        sCommand = *sIterator;

        sIndex = mCommandWnd.AddString(sCommand->mText);
        mCommandWnd.SetItemData(sIndex, (DWORD_PTR)sCommand);
    }

    sCommandList.clear();

    mCommandWnd.SetCurSel(0);
    OnSelchangeCommands();

    GetDlgItem(IDC_ACCEL_BUTTON_REMOVE)->EnableWindow(XPR_FALSE);
}

void AccelTableDlg::fillRecursiveCategory(CommandList &aCommandList, CMenu *aMenu, xpr_tchar_t *aSubMenu, xpr_bool_t aSubText)
{
    static xpr_tchar_t sFullText[0xff];
    xpr_tchar_t sText[0xff];
    xpr_sint_t i, sCount;
    xpr_uint_t sId;
    const xpr_tchar_t *sStringId;

    CommandStringTable &sCommandStringTable = CommandStringTable::instance();

    MENUITEMINFO sMenuItemInfo = {0};
    sMenuItemInfo.cbSize  = sizeof(MENUITEMINFO);
    sMenuItemInfo.fMask = MIIM_ID | MIIM_SUBMENU;

    sCount = aMenu->GetMenuItemCount();
    for (i = 0; i < sCount; ++i)
    {
        sId = aMenu->GetMenuItemID(i);

        if (sId == ID_BOOKMARK_DYNAMIC_MENU          ||
            sId == ID_DRIVE_DYNAMIC_MENU             ||
            sId == ID_GO_UP_DYNAMIC_MENU             ||
            sId == ID_GO_BACKWARD_DYNAMIC_MENU       ||
            sId == ID_GO_FORWARD_DYNAMIC_MENU        ||
            sId == ID_GO_HISTORY_DYNAMIC_MENU        ||
            sId == ID_GO_WORKING_FOLDER_DYNAMIC_MENU ||
            sId == ID_FILE_NEW_DYNAMIC_MENU          ||
            sId == ID_FILE_RECENT_DYNAMIC_MENU)
        {
            continue;
        }

        aMenu->GetMenuItemInfo(i, &sMenuItemInfo, XPR_TRUE);

        sText[0] = 0;
        if (XPR_IS_NOT_NULL(sMenuItemInfo.hSubMenu))
        {
            if (aMenu->GetMenuString(i, sText, 0xfe, MF_BYPOSITION) > 0)
            {
                _tcscpy(sText, theApp.loadString(sText));
            }
        }
        else
        {
            sStringId = sCommandStringTable.loadString(sId);
            if (sStringId != XPR_NULL)
            {
                _tcscpy(sText, theApp.loadString(sStringId));
            }
        }

        if (sText[0] == 0)
            continue;

        xpr_tchar_t *sEnd = _tcsrchr(sText, '(');
        if (XPR_IS_NOT_NULL(sEnd))
            *sEnd = '\0';

        aMenu->GetMenuItemInfo(i, &sMenuItemInfo, XPR_TRUE);
        if (XPR_IS_NOT_NULL(sMenuItemInfo.hSubMenu))
        {
            CMenu sSubMenu;
            sSubMenu.Attach(sMenuItemInfo.hSubMenu);
            fillRecursiveCategory(aCommandList, &sSubMenu, sText, aSubText);
            sSubMenu.Detach();
        }
        else
        {
            if (aSubText == XPR_TRUE && aSubMenu != XPR_NULL)
                _stprintf(sFullText, XPR_STRING_LITERAL("%s > %s"), aSubMenu, sText);
            else
                _tcscpy(sFullText, sText);

            Command *sCommand = new Command;
            sCommand->mId = sMenuItemInfo.wID;
            sCommand->mDesc = XPR_TRUE;
            _tcscpy(sCommand->mText, sFullText);

            aCommandList.push_back(sCommand);
        }
    }
}

void AccelTableDlg::OnSelchangeCommands(void) 
{
    xpr_sint_t sCurSel = mCommandWnd.GetCurSel();
    Command *sCommand = (Command *)mCommandWnd.GetItemData(sCurSel);

    mCurKeysWnd.ResetContent();

    xpr_sint_t i, sIndex;
    xpr_tchar_t sKey[0xff] = {0};
    xpr_tchar_t sComKey[0xff] = {0};
    CString sVirKeyName;

    for (i = 0; i < mCount; ++i)
    {
        sComKey[0] = '\0';

        if (mAccel[i].cmd == sCommand->mId)
        {
            if (mAccel[i].fVirt & FCONTROL) _tcscat(sComKey, XPR_STRING_LITERAL("Ctrl + "));
            if (mAccel[i].fVirt & FSHIFT)   _tcscat(sComKey, XPR_STRING_LITERAL("Shift + "));
            if (mAccel[i].fVirt & FALT)     _tcscat(sComKey, XPR_STRING_LITERAL("Alt + "));

            if (mAccel[i].fVirt & FVIRTKEY)
            {
                sVirKeyName = fxb::GetKeyName(mAccel[i].key);
                _stprintf(sKey, XPR_STRING_LITERAL("%s%s"), sComKey, sVirKeyName);
            }
            else
            {
                _stprintf(sKey, XPR_STRING_LITERAL("%s%c"), sComKey, mAccel[i].key);
            }

            sIndex = mCurKeysWnd.AddString(sKey);
            mCurKeysWnd.SetItemData(sIndex, MAKELONG(mAccel[i].fVirt, mAccel[i].key));
        }
    }

    GetDlgItem(IDC_ACCEL_BUTTON_REMOVE)->EnableWindow(XPR_FALSE);
}

void AccelTableDlg::OnSelchangeCurKeys(void) 
{
    xpr_sint_t sCurSel = mCurKeysWnd.GetCurSel();
    if (sCurSel >= 0)
        GetDlgItem(IDC_ACCEL_BUTTON_REMOVE)->EnableWindow(XPR_TRUE);
}

void AccelTableDlg::OnEnChangeHotKey(void)
{
    xpr_ushort_t sVirtualKeyCode = 0;
    xpr_ushort_t sModifier = 0;
    mHotKeyCtrl.GetHotKey(sVirtualKeyCode, sModifier);

    xpr_bool_t sNone = XPR_TRUE;
    if (sVirtualKeyCode != 0)
    {
        xpr_byte_t sVirt = 0;
        xpr_ushort_t sKey = 0;
        if (sModifier & HOTKEYF_CONTROL) sVirt |= FCONTROL;
        if (sModifier & HOTKEYF_SHIFT  ) sVirt |= FSHIFT;
        if (sModifier & HOTKEYF_ALT    ) sVirt |= FALT;
        sVirt |= FVIRTKEY;
        sVirt |= FNOINVERT;
        sKey = sVirtualKeyCode;

        xpr_sint_t i;
        xpr_uint_t sId = 0;

        for (i = 0; i < mCount; ++i)
        {
            if (mAccel[i].fVirt == sVirt && mAccel[i].key == sKey)
            {
                sId = mAccel[i].cmd;
                break;
            }
        }

        if (sId != 0)
        {
            xpr_tchar_t sText[0xff] = {0};
            if (getRecursiveTextToId(&mMenu, sId, sText) == XPR_TRUE)
            {
                SetDlgItemText(IDC_ACCEL_ASSIGNED_TO, sText);
                sNone = XPR_FALSE;
            }
        }
    }

    if (sNone == XPR_TRUE)
        SetDlgItemText(IDC_ACCEL_ASSIGNED_TO, theApp.loadString(XPR_STRING_LITERAL("popup.accel.none")));

    ((CButton *)GetDlgItem(IDC_ACCEL_CTRL ))->SetCheck(XPR_TEST_BITS(sModifier, HOTKEYF_CONTROL));
    ((CButton *)GetDlgItem(IDC_ACCEL_SHIFT))->SetCheck(XPR_TEST_BITS(sModifier, HOTKEYF_SHIFT));
    ((CButton *)GetDlgItem(IDC_ACCEL_ALT  ))->SetCheck(XPR_TEST_BITS(sModifier, HOTKEYF_ALT));

    xpr_sint_t i;
    xpr_sint_t sCount;
    xpr_ushort_t sVirtualKey;
    DWORD sKey;

    sCount = mVirtualWnd.GetCount();
    for (i = 0; i < sCount; ++i)
    {
        sKey = (DWORD)mVirtualWnd.GetItemData(i);
        sVirtualKey = LOWORD(sKey);
        if (sVirtualKey == sVirtualKeyCode)
        {
            mVirtualWnd.SetCurSel(i);
            break;
        }
    }

    if (i == sCount)
        mVirtualWnd.SetCurSel(-1);

    GetDlgItem(IDC_ACCEL_BUTTON_ASSIGN)->EnableWindow(sVirtualKeyCode != 0);
}

xpr_bool_t AccelTableDlg::getRecursiveTextToId(CMenu *aMenu, xpr_uint_t aId, xpr_tchar_t *aText)
{
    static xpr_tchar_t sSubText[0xff];
    xpr_tchar_t sText[0xff];
    xpr_sint_t i, sCount;
    xpr_uint_t sId;
    const xpr_tchar_t *sStringId;

    CommandStringTable &sCommandStringTable = CommandStringTable::instance();

    MENUITEMINFO sMenuItemInfo = {0};
    sMenuItemInfo.cbSize = sizeof(MENUITEMINFO);
    sMenuItemInfo.fMask  = MIIM_ID | MIIM_SUBMENU;

    sCount = aMenu->GetMenuItemCount();
    for (i = 0; i < sCount; ++i)
    {
        sId = aMenu->GetMenuItemID(i);

        aMenu->GetMenuItemInfo(i, &sMenuItemInfo, XPR_TRUE);

        sText[0] = 0;
        if (XPR_IS_NOT_NULL(sMenuItemInfo.hSubMenu))
        {
            if (aMenu->GetMenuString(i, sText, 0xfe, MF_BYPOSITION) > 0)
            {
                _tcscpy(sText, theApp.loadString(sText));
            }
        }
        else
        {
            sStringId = sCommandStringTable.loadString(sId);
            if (sStringId != XPR_NULL)
            {
                _tcscpy(sText, theApp.loadString(sStringId));
            }
        }

        if (sText[0] == 0)
            continue;

        xpr_tchar_t *sEnd = _tcsrchr(sText, '(');
        if (XPR_IS_NOT_NULL(sEnd))
            *sEnd = '\0';

        if (XPR_IS_NOT_NULL(sMenuItemInfo.hSubMenu))
        {
            CMenu sSubMenu;
            sSubMenu.Attach(sMenuItemInfo.hSubMenu);

            xpr_bool_t sResult = getRecursiveTextToId(&sSubMenu, aId, aText);

            sSubMenu.Detach();

            if (sResult == XPR_TRUE)
            {
                _tcscpy(sSubText, aText);
                _stprintf(aText, XPR_STRING_LITERAL("%s > %s"), sText, sSubText);
                return XPR_TRUE;
            }
            else
            {
                xpr_tchar_t *sSplit = _tcsrchr(aText, '>');
                if (XPR_IS_NOT_NULL(sSplit))
                {
                    *sSplit = '\0';
                }
            }
        }
        else
        {
            if (sMenuItemInfo.wID == aId)
            {
                _tcscpy(aText, sText);
                return XPR_TRUE;
            }
        }
    }

    return XPR_FALSE;
}

void AccelTableDlg::OnAssign(void) 
{
    xpr_ushort_t sVirtualKeyCode = 0;
    xpr_ushort_t sModifier = 0;
    mHotKeyCtrl.GetHotKey(sVirtualKeyCode, sModifier);
    if (sModifier == HOTKEYF_CONTROL && '0' <= sVirtualKeyCode && sVirtualKeyCode <= '9')
    {
        const xpr_tchar_t *sMsg = theApp.loadString(XPR_STRING_LITERAL("popup.accel.msg.only_for_bookmark"));
        MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);
        return;
    }

    xpr_sint_t sCurSel = mCommandWnd.GetCurSel();
    if (sCurSel >= 0)
    {
        Command *sCommand = (Command *)mCommandWnd.GetItemData(sCurSel);

        // Command ID
        mAccel[mCount].cmd = (xpr_ushort_t)sCommand->mId;

        // Flag Virtual Key
        if (sModifier & HOTKEYF_CONTROL) mAccel[mCount].fVirt |= FCONTROL;
        if (sModifier & HOTKEYF_SHIFT)   mAccel[mCount].fVirt |= FSHIFT;
        if (sModifier & HOTKEYF_ALT)     mAccel[mCount].fVirt |= FALT;
        mAccel[mCount].fVirt |= FVIRTKEY;
        mAccel[mCount].fVirt |= FNOINVERT;

        // Virtual Key
        mAccel[mCount].key = sVirtualKeyCode;

        mCount++;
    }

    OnSelchangeCommands();
}

void AccelTableDlg::OnRemove(void) 
{
    xpr_sint_t nComCurSel = mCommandWnd.GetCurSel();
    xpr_sint_t nKeyCurSel = mCurKeysWnd.GetCurSel();

    Command *sCommand = (Command *)mCommandWnd.GetItemData(nComCurSel);
    DWORD sAccelKey = (DWORD)mCurKeysWnd.GetItemData(nKeyCurSel);

    xpr_sint_t i;
    xpr_byte_t sVirt = (xpr_byte_t)LOWORD(sAccelKey);
    xpr_ushort_t sKey = (xpr_ushort_t)HIWORD(sAccelKey);

    for (i = 0; i < mCount; ++i)
    {
        if (mAccel[i].cmd == sCommand->mId)
        {
            if (mAccel[i].fVirt == sVirt && mAccel[i].key == sKey)
            {
                mCount--;

                if ((mCount-i) > 0)
                    memmove(mAccel+i, mAccel+i+1, sizeof(ACCEL) * (mCount-i));

                mAccel[mCount].cmd   = 0;
                mAccel[mCount].fVirt = 0;
                mAccel[mCount].key   = 0;

                break;
            }
        }
    }

    OnSelchangeCommands();
}

void AccelTableDlg::OnReset(void) 
{
    const xpr_tchar_t *sMsg = theApp.loadString(XPR_STRING_LITERAL("popup.accel.msg.question_reset"));
    xpr_sint_t sMsgId = MessageBox(sMsg, XPR_NULL, MB_YESNO | MB_ICONWARNING);
    if (sMsgId == IDNO)
        return;

    xpr_sint_t i;
    xpr_sint_t sComCurSel = mCommandWnd.GetCurSel();
    Command *sCommand = (Command *)mCommandWnd.GetItemData(sComCurSel);

    i = 0;
    while (i < mCount)
    {
        if (mAccel[i].cmd == sCommand->mId)
        {
            mCount--;

            if ((mCount-i) > 0)
                memmove(mAccel+i, mAccel+i+1, sizeof(ACCEL)*(mCount-i));

            mAccel[mCount].cmd   = 0;
            mAccel[mCount].fVirt = 0;
            mAccel[mCount].key   = 0;

            continue;
        }

        i++;
    }

    xpr_sint_t sCount = 0;
    ACCEL sAccel[MAX_ACCEL] = {0};

    memset(sAccel, 0, sizeof(ACCEL) * MAX_ACCEL);

    HACCEL sAccelHandle = ::LoadAccelerators(theApp.m_hInstance, MAKEINTRESOURCE(mResourceId));
    sCount = ::CopyAcceleratorTable(sAccelHandle, sAccel, MAX_ACCEL);
    ::DestroyAcceleratorTable(sAccelHandle);

    for (i = 0; i < sCount; ++i)
    {
        if (sAccel[i].cmd == sCommand->mId)
        {
            mAccel[mCount].cmd   = sAccel[i].cmd;
            mAccel[mCount].fVirt = sAccel[i].fVirt;
            mAccel[mCount].key   = sAccel[i].key;
            mCount++;
        }
    }

    OnSelchangeCommands();
}

void AccelTableDlg::OnResetAll(void) 
{
    const xpr_tchar_t *sMsg = theApp.loadString(XPR_STRING_LITERAL("popup.accel.msg.question_reset_all"));
    xpr_sint_t sMsgId = MessageBox(sMsg, XPR_NULL, MB_YESNO | MB_ICONWARNING);
    if (sMsgId == IDNO)
        return;

    memset(mAccel, 0, sizeof(ACCEL)*MAX_ACCEL);

    HACCEL sAccelHandle = ::LoadAccelerators(theApp.m_hInstance, MAKEINTRESOURCE(mResourceId));
    mCount = ::CopyAcceleratorTable(sAccelHandle, mAccel, MAX_ACCEL);
    ::DestroyAcceleratorTable(sAccelHandle);

    mHotKeyCtrl.SetHotKey(0, 0);
    OnEnChangeHotKey();

    OnSelchangeCommands();
}

void AccelTableDlg::OnOK(void) 
{
    super::OnOK();
}

void AccelTableDlg::OnKeyInput(void)
{
    xpr_bool_t sCtrl  = ((CButton *)GetDlgItem(IDC_ACCEL_CTRL))->GetCheck();
    xpr_bool_t sAlt   = ((CButton *)GetDlgItem(IDC_ACCEL_ALT))->GetCheck();
    xpr_bool_t sShift = ((CButton *)GetDlgItem(IDC_ACCEL_SHIFT))->GetCheck();

    DWORD sKey = 0;
    xpr_sint_t sIndex = mVirtualWnd.GetCurSel();
    if (sIndex != CB_ERR)
        sKey = (DWORD)mVirtualWnd.GetItemData(sIndex);

    xpr_ushort_t sVirtualKey = LOWORD(sKey);
    xpr_ushort_t sExtendedKey = HIWORD(sKey);

    xpr_ushort_t sVirtualKeyCode = sVirtualKey;
    xpr_ushort_t sModifier = 0;

    if (sCtrl  == XPR_TRUE) sModifier |= HOTKEYF_CONTROL;
    if (sAlt   == XPR_TRUE) sModifier |= HOTKEYF_ALT;
    if (sShift == XPR_TRUE) sModifier |= HOTKEYF_SHIFT;
    sModifier |= sExtendedKey;

    mHotKeyCtrl.SetHotKey(sVirtualKeyCode, sModifier);

    GetDlgItem(IDC_ACCEL_BUTTON_ASSIGN)->EnableWindow(sVirtualKeyCode != 0);

    OnEnChangeHotKey();
}

void AccelTableDlg::clearCategory(void)
{
    xpr_sint_t i, sCount;
    Category *sCategory;

    sCount = mCategoryWnd.GetCount();
    for (i = 0; i < sCount; ++i)
    {
        sCategory = (Category *)mCategoryWnd.GetItemData(i);
        XPR_SAFE_DELETE(sCategory);
    }

    mCategoryWnd.ResetContent();
}

void AccelTableDlg::clearCommands(void)
{
    xpr_sint_t i, sCount;
    Command *sCommand;

    sCount = mCommandWnd.GetCount();
    for (i = 0; i < sCount; ++i)
    {
        sCommand = (Command *)mCommandWnd.GetItemData(i);
        XPR_SAFE_DELETE(sCommand);
    }

    mCommandWnd.ResetContent();
}
