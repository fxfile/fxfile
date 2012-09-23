//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FX_ACCEL_TABLE_DLG_H__
#define __FX_ACCEL_TABLE_DLG_H__
#pragma once

#define MAX_ACCEL 200

class AccelTableDlg : public CDialog
{
    typedef CDialog super;

public:
    AccelTableDlg(HACCEL aAccelHandle, xpr_uint_t aResourceId);

public:
    ACCEL mAccel[MAX_ACCEL];
    xpr_sint_t mCount;

protected:
    struct Category;
    struct Command;
    typedef std::list<Command *> CommandList;

    void fillRecursiveCategory(CommandList &aCommandList, CMenu *aMenu, xpr_tchar_t *aSubMenu = XPR_NULL, xpr_bool_t aSubText = XPR_TRUE);
    xpr_bool_t getRecursiveTextToId(CMenu *aMenu, xpr_uint_t aID, xpr_tchar_t *aText);
    static bool sortAllCommands(Command *aCommand1, Command *aCommand2);

    void clearCategory(void);
    void clearCommands(void);

protected:
    HACCEL     mAccelHandle;
    CMenu      mMenu;
    xpr_uint_t mResourceId;

protected:
    CComboBox   mCategoryWnd;
    CListBox    mCommandWnd;
    CListBox    mCurKeysWnd;
    CHotKeyCtrl mHotKeyCtrl;
    CComboBox   mVirtualWnd;

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

protected:
    DECLARE_MESSAGE_MAP()
    virtual xpr_bool_t OnInitDialog(void);
    virtual void OnOK(void);
    afx_msg void OnDestroy(void);
    afx_msg void OnSelchangeCategory(void);
    afx_msg void OnSelchangeCommands(void);
    afx_msg void OnSelchangeCurKeys(void);
    afx_msg void OnEnChangeHotKey(void);
    afx_msg void OnAssign(void);
    afx_msg void OnRemove(void);
    afx_msg void OnReset(void);
    afx_msg void OnResetAll(void);
    afx_msg void OnKeyInput(void);
};

#endif // __FX_ACCEL_TABLE_DLG_H__