//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "CfgAdvDlg.h"

#include "../resource.h"
#include "../Option.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// option id
enum
{
    OPT_ID_SINGLE_INSTANCE        = 1,
    OPT_ID_CONFIRM_EXIT           = 2,
    OPT_ID_RECENT_FILE            = 3,
    OPT_ID_REG_SHELL_CONTEXT_MENU = 4,

    OPT_ID_TRAY_HIDE              = 30,
    OPT_ID_TRAY_SHOW              = 31,
    OPT_ID_TRAY_CLOSE             = 32,
    OPT_ID_TRAY_MINIMIZE          = 33,
    OPT_ID_TRAY_INIT_FOLDER       = 34,
    OPT_ID_TRAY_ONE_CLICK         = 35,
};

CfgAdvDlg::CfgAdvDlg(void)
    : super(IDD_CFG_ADV, XPR_NULL)
{
}

void CfgAdvDlg::DoDataExchange(CDataExchange* pDX)
{
    super::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_CFG_ADV_OPTION, mOptionPanel);
}

BEGIN_MESSAGE_MAP(CfgAdvDlg, super)
    ON_BN_CLICKED(IDC_CFG_ADV_DEFAULT,                OnDefault)
    ON_BN_CLICKED(IDC_CFG_ADV_REMOVE_RECENT_FILELIST, OnRemoveRecentFileList)
    ON_NOTIFY(OPN_SELCHANGE, IDC_CFG_ADV_OPTION, OnOptionsSelChange)
END_MESSAGE_MAP()

xpr_bool_t CfgAdvDlg::OnInitDialog(void) 
{
    super::OnInitDialog();

    // disable apply button event
    addIgnoreApply(IDC_CFG_ADV_OPTION);
    addIgnoreApply(IDC_CFG_ADV_REMOVE_RECENT_FILELIST);

    CImageList sImgList;
    sImgList.Create(13, 13, ILC_COLORDDB | ILC_MASK, 5, 1);

    CBitmap sBitmap;
    sBitmap.LoadBitmap(IDB_OPTIONS_PANEL_IMAGES);
    sImgList.Add(&sBitmap, RGB(255,0,255));

    mOptionPanel.SetImageList(sImgList);

    sImgList.DeleteImageList();
    sBitmap.DeleteObject();

    SetDlgItemText(IDC_CFG_ADV_REMOVE_RECENT_FILELIST, theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.advanced.button.remove_recent_file_list")));
    SetDlgItemText(IDC_CFG_ADV_DEFAULT,                theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.advanced.button.restore_to_default")));

    return XPR_TRUE;
}

void CfgAdvDlg::onInit(Option::Config &aConfig)
{
    mOptionPanel.AddGroup(
        theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.advanced.option.advanced")),
        OptionsPanel::groupBold,
        0,
        4);

    mOptionPanel.AddCheck(
        theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.advanced.option.advanced.single_instance")),
        XPR_IS_TRUE(aConfig.mSingleInstance) ? OptionsPanel::itemChecked : 0,
        1,
        OPT_ID_SINGLE_INSTANCE);

    mOptionPanel.AddCheck(
        theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.advanced.option.advanced.confirm_exit")),
        XPR_IS_TRUE(aConfig.mConfirmExit) ? OptionsPanel::itemChecked : 0,
        1, OPT_ID_CONFIRM_EXIT);

    mOptionPanel.AddCheck(
        theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.advanced.option.advanced.enable_recent_file_list")),
        XPR_IS_TRUE(aConfig.mRecentFile) ? OptionsPanel::itemChecked : 0,
        1,
        OPT_ID_RECENT_FILE);

    mOptionPanel.AddCheck(
        theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.advanced.option.advanced.enable_shell_context_menu")),
        XPR_IS_TRUE(aConfig.mRegShellContextMenu) ? OptionsPanel::itemChecked : 0,
        1,
        OPT_ID_REG_SHELL_CONTEXT_MENU);

    mOptionPanel.AddGroup(
        theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.advanced.option.system_tray")),
        OptionsPanel::groupBold,
        0,
        4);

    mOptionPanel.AddRadio(
        theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.advanced.option.system_tray.none")),
        XPR_IS_FALSE(aConfig.mTray) ? OptionsPanel::itemChecked : 0,
        1,
        OPT_ID_TRAY_HIDE);

    mOptionPanel.AddRadio(
        theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.advanced.option.system_tray.show_tray")),
        XPR_IS_TRUE(aConfig.mTray) ? OptionsPanel::itemChecked : 0,
        1,
        OPT_ID_TRAY_SHOW);

    mOptionPanel.AddCheck(
        theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.advanced.option.system_tray.show_tray_on_exit")),
        XPR_IS_TRUE(aConfig.mTrayOnClose) ? OptionsPanel::itemChecked : 0,
        2,
        OPT_ID_TRAY_CLOSE);

    mOptionPanel.AddCheck(
        theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.advanced.option.system_tray.show_tray_on_minimize")),
        XPR_IS_TRUE(aConfig.mTrayOnMinmize) ? OptionsPanel::itemChecked : 0,
        1,
        OPT_ID_TRAY_MINIMIZE);

    mOptionPanel.AddCheck(
        theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.advanced.option.system_tray.go_init_folder_on_restore")),
        XPR_IS_TRUE(aConfig.mTrayRestoreInitFolder) ? OptionsPanel::itemChecked : 0,
        1,
        OPT_ID_TRAY_INIT_FOLDER);

    mOptionPanel.AddCheck(
        theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.advanced.option.system_tray.restore_hide_on_one_click")),
        XPR_IS_TRUE(aConfig.mTrayOneClick) ? OptionsPanel::itemChecked : 0,
        1,
        OPT_ID_TRAY_ONE_CLICK);
}

void CfgAdvDlg::onApply(Option::Config &aConfig)
{
    aConfig.mSingleInstance        = mOptionPanel.GetCheckState(OPT_ID_SINGLE_INSTANCE);
    aConfig.mConfirmExit           = mOptionPanel.GetCheckState(OPT_ID_CONFIRM_EXIT);
    aConfig.mRecentFile            = mOptionPanel.GetCheckState(OPT_ID_RECENT_FILE);
    aConfig.mRegShellContextMenu   = mOptionPanel.GetCheckState(OPT_ID_REG_SHELL_CONTEXT_MENU);

    aConfig.mTray                  = mOptionPanel.GetCheckState(OPT_ID_TRAY_SHOW);
    aConfig.mTrayOnClose           = mOptionPanel.GetCheckState(OPT_ID_TRAY_CLOSE);
    aConfig.mTrayOnMinmize         = mOptionPanel.GetCheckState(OPT_ID_TRAY_MINIMIZE);
    aConfig.mTrayRestoreInitFolder = mOptionPanel.GetCheckState(OPT_ID_TRAY_INIT_FOLDER);
    aConfig.mTrayOneClick          = mOptionPanel.GetCheckState(OPT_ID_TRAY_ONE_CLICK);
}

void CfgAdvDlg::OnDefault(void) 
{
    mOptionPanel.SetCheckState(OPT_ID_SINGLE_INSTANCE,        XPR_FALSE);
    mOptionPanel.SetCheckState(OPT_ID_CONFIRM_EXIT,           XPR_FALSE);
    mOptionPanel.SetCheckState(OPT_ID_RECENT_FILE,            XPR_TRUE);
    mOptionPanel.SetCheckState(OPT_ID_REG_SHELL_CONTEXT_MENU, XPR_FALSE);

    mOptionPanel.SetCheckState(OPT_ID_TRAY_HIDE,              XPR_TRUE);
    mOptionPanel.SetCheckState(OPT_ID_TRAY_SHOW,              XPR_FALSE);
    mOptionPanel.SetCheckState(OPT_ID_TRAY_CLOSE,             XPR_FALSE);
    mOptionPanel.SetCheckState(OPT_ID_TRAY_MINIMIZE,          XPR_FALSE);
    mOptionPanel.SetCheckState(OPT_ID_TRAY_INIT_FOLDER,       XPR_FALSE);
    mOptionPanel.SetCheckState(OPT_ID_TRAY_ONE_CLICK,         XPR_FALSE);

    mOptionPanel.RedrawWindow();
    mOptionPanel.UpdateWindow();

    setModified();
}

void CfgAdvDlg::OnOptionsSelChange(NMHDR *pNMHDR, LRESULT *pResult)
{
    setModified();
}

void CfgAdvDlg::OnRemoveRecentFileList(void) 
{
    const xpr_tchar_t *sMsg = theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.advanced.msg.confirm_remove_recent_file_list"));
    xpr_sint_t sMsgId = MessageBox(sMsg, XPR_NULL, MB_YESNO | MB_ICONQUESTION);
    if (sMsgId != IDYES)
        return;

    theApp.removeRecentFileList();
}
