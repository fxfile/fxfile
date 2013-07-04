//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "cfg_adv_dlg.h"

#include "../recent_file_list.h"
#include "../resource.h"
#include "../option.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace fxfile
{
namespace cfg
{
CfgAdvDlg::CfgAdvDlg(void)
    : super(IDD_CFG_ADV, XPR_NULL)
{
}

void CfgAdvDlg::DoDataExchange(CDataExchange* pDX)
{
    super::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CfgAdvDlg, super)
    ON_BN_CLICKED(IDC_CFG_ADV_TRAY_NONE,              OnTray)
    ON_BN_CLICKED(IDC_CFG_ADV_TRAY_SHOW,              OnTray)
    ON_BN_CLICKED(IDC_CFG_ADV_DEFAULT,                OnDefault)
    ON_BN_CLICKED(IDC_CFG_ADV_REMOVE_RECENT_FILELIST, OnRemoveRecentFileList)
    ON_BN_CLICKED(IDC_CFG_ADV_UPDATE_ENABLE,          OnUpdateChecker)
END_MESSAGE_MAP()

xpr_bool_t CfgAdvDlg::OnInitDialog(void) 
{
    super::OnInitDialog();

    // disable apply button event
    addIgnoreApply(IDC_CFG_ADV_OPTION);
    addIgnoreApply(IDC_CFG_ADV_REMOVE_RECENT_FILELIST);

    SetDlgItemText(IDC_CFG_ADV_SINGLE_INSTANCE,                      theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.advanced.check.single_instance")));
    SetDlgItemText(IDC_CFG_ADV_CONFIRM_TO_EXIT,                      theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.advanced.check.confirm_to_exit")));
    SetDlgItemText(IDC_CFG_ADV_RECENT_FILE_LIST,                     theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.advanced.check.use_recent_file_list")));
    SetDlgItemText(IDC_CFG_ADV_SHELL_CONTEXT_MENU,                   theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.advanced.check.register_to_shell_context_menu")));
    SetDlgItemText(IDC_CFG_ADV_GROUP_TRAY,                           theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.advanced.group.system_tray")));
    SetDlgItemText(IDC_CFG_ADV_TRAY_NONE,                            theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.advanced.radio.system_tray_none")));
    SetDlgItemText(IDC_CFG_ADV_TRAY_SHOW,                            theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.advanced.radio.system_tray_show")));
    SetDlgItemText(IDC_CFG_ADV_TRAY_HIDE_ON_EXIT,                    theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.advanced.check.system_tray_hide_on_exit")));
    SetDlgItemText(IDC_CFG_ADV_TRAY_SHOW_WHEN_MINIMIZING,            theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.advanced.check.system_tray_show_tray_when_minimizing")));
    SetDlgItemText(IDC_CFG_ADV_TRAY_OPEN_INIT_FOLDER_WHEN_RESTORING, theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.advanced.check.system_tray_open_init_folder_when_restoring")));
    SetDlgItemText(IDC_CFG_ADV_TRAY_RESTORE_ON_ONE_CLICK,            theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.advanced.check.system_tray_restore_on_one_click")));
    SetDlgItemText(IDC_CFG_ADV_UPDATE_ENABLE,                        theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.advanced.check.update_enable")));
    SetDlgItemText(IDC_CFG_ADV_UPDATE_CHECK_MINOR_VER_UP,            theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.advanced.check.update_check_minor_version_up")));
    SetDlgItemText(IDC_CFG_ADV_REMOVE_RECENT_FILELIST,               theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.advanced.button.remove_recent_file_list")));
    SetDlgItemText(IDC_CFG_ADV_DEFAULT,                              theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.advanced.button.restore_to_default")));

    return XPR_TRUE;
}

void CfgAdvDlg::onInit(Option::Config &aConfig)
{
    ((CButton *)GetDlgItem(IDC_CFG_ADV_SINGLE_INSTANCE                     ))->SetCheck(aConfig.mSingleInstance);
    ((CButton *)GetDlgItem(IDC_CFG_ADV_CONFIRM_TO_EXIT                     ))->SetCheck(aConfig.mConfirmExit);
    ((CButton *)GetDlgItem(IDC_CFG_ADV_RECENT_FILE_LIST                    ))->SetCheck(aConfig.mRecentFile);
    ((CButton *)GetDlgItem(IDC_CFG_ADV_SHELL_CONTEXT_MENU                  ))->SetCheck(aConfig.mRegShellContextMenu);
    ((CButton *)GetDlgItem(IDC_CFG_ADV_TRAY_NONE                           ))->SetCheck(XPR_IS_FALSE(aConfig.mTray));
    ((CButton *)GetDlgItem(IDC_CFG_ADV_TRAY_SHOW                           ))->SetCheck(XPR_IS_TRUE(aConfig.mTray));
    ((CButton *)GetDlgItem(IDC_CFG_ADV_TRAY_HIDE_ON_EXIT                   ))->SetCheck(aConfig.mTrayOnClose);
    ((CButton *)GetDlgItem(IDC_CFG_ADV_TRAY_SHOW_WHEN_MINIMIZING           ))->SetCheck(aConfig.mTrayOnMinmize);
    ((CButton *)GetDlgItem(IDC_CFG_ADV_TRAY_OPEN_INIT_FOLDER_WHEN_RESTORING))->SetCheck(aConfig.mTrayRestoreInitFolder);
    ((CButton *)GetDlgItem(IDC_CFG_ADV_TRAY_RESTORE_ON_ONE_CLICK           ))->SetCheck(aConfig.mTrayOneClick);
    ((CButton *)GetDlgItem(IDC_CFG_ADV_UPDATE_ENABLE                       ))->SetCheck(aConfig.mUpdateCheckEnable);
    ((CButton *)GetDlgItem(IDC_CFG_ADV_UPDATE_CHECK_MINOR_VER_UP           ))->SetCheck(aConfig.mUpdateCheckMinorVer);

    OnTray();
    OnUpdateChecker();
}

void CfgAdvDlg::onApply(Option::Config &aConfig)
{
    aConfig.mSingleInstance        = ((CButton *)GetDlgItem(IDC_CFG_ADV_SINGLE_INSTANCE                     ))->GetCheck();
    aConfig.mConfirmExit           = ((CButton *)GetDlgItem(IDC_CFG_ADV_CONFIRM_TO_EXIT                     ))->GetCheck();
    aConfig.mRecentFile            = ((CButton *)GetDlgItem(IDC_CFG_ADV_RECENT_FILE_LIST                    ))->GetCheck();
    aConfig.mRegShellContextMenu   = ((CButton *)GetDlgItem(IDC_CFG_ADV_SHELL_CONTEXT_MENU                  ))->GetCheck();
    aConfig.mTray                  = ((CButton *)GetDlgItem(IDC_CFG_ADV_TRAY_SHOW                           ))->GetCheck() ? XPR_TRUE : XPR_FALSE;
    aConfig.mTrayOnClose           = ((CButton *)GetDlgItem(IDC_CFG_ADV_TRAY_HIDE_ON_EXIT                   ))->GetCheck();
    aConfig.mTrayOnMinmize         = ((CButton *)GetDlgItem(IDC_CFG_ADV_TRAY_SHOW_WHEN_MINIMIZING           ))->GetCheck();
    aConfig.mTrayRestoreInitFolder = ((CButton *)GetDlgItem(IDC_CFG_ADV_TRAY_OPEN_INIT_FOLDER_WHEN_RESTORING))->GetCheck();
    aConfig.mTrayOneClick          = ((CButton *)GetDlgItem(IDC_CFG_ADV_TRAY_RESTORE_ON_ONE_CLICK           ))->GetCheck();
    aConfig.mUpdateCheckEnable     = ((CButton *)GetDlgItem(IDC_CFG_ADV_UPDATE_ENABLE                       ))->GetCheck();
    aConfig.mUpdateCheckMinorVer   = ((CButton *)GetDlgItem(IDC_CFG_ADV_UPDATE_CHECK_MINOR_VER_UP           ))->GetCheck();
}

void CfgAdvDlg::OnTray(void)
{
    xpr_bool_t sShowTray = ((CButton *)GetDlgItem(IDC_CFG_ADV_TRAY_SHOW))->GetCheck();

    GetDlgItem(IDC_CFG_ADV_TRAY_HIDE_ON_EXIT)->EnableWindow(sShowTray);
}

void CfgAdvDlg::OnDefault(void) 
{
    ((CButton *)GetDlgItem(IDC_CFG_ADV_SINGLE_INSTANCE                     ))->SetCheck(XPR_FALSE);
    ((CButton *)GetDlgItem(IDC_CFG_ADV_CONFIRM_TO_EXIT                     ))->SetCheck(XPR_FALSE);
    ((CButton *)GetDlgItem(IDC_CFG_ADV_RECENT_FILE_LIST                    ))->SetCheck(XPR_TRUE);
    ((CButton *)GetDlgItem(IDC_CFG_ADV_SHELL_CONTEXT_MENU                  ))->SetCheck(XPR_FALSE);
    ((CButton *)GetDlgItem(IDC_CFG_ADV_TRAY_NONE                           ))->SetCheck(XPR_TRUE);
    ((CButton *)GetDlgItem(IDC_CFG_ADV_TRAY_SHOW                           ))->SetCheck(XPR_FALSE);
    ((CButton *)GetDlgItem(IDC_CFG_ADV_TRAY_HIDE_ON_EXIT                   ))->SetCheck(XPR_FALSE);
    ((CButton *)GetDlgItem(IDC_CFG_ADV_TRAY_SHOW_WHEN_MINIMIZING           ))->SetCheck(XPR_FALSE);
    ((CButton *)GetDlgItem(IDC_CFG_ADV_TRAY_OPEN_INIT_FOLDER_WHEN_RESTORING))->SetCheck(XPR_FALSE);
    ((CButton *)GetDlgItem(IDC_CFG_ADV_TRAY_RESTORE_ON_ONE_CLICK           ))->SetCheck(XPR_FALSE);
    ((CButton *)GetDlgItem(IDC_CFG_ADV_UPDATE_ENABLE                       ))->SetCheck(XPR_TRUE);
    ((CButton *)GetDlgItem(IDC_CFG_ADV_UPDATE_CHECK_MINOR_VER_UP           ))->SetCheck(XPR_FALSE);

    setModified();
}

void CfgAdvDlg::OnRemoveRecentFileList(void) 
{
    const xpr_tchar_t *sMsg = theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.advanced.msg.confirm_remove_recent_file_list"));
    xpr_sint_t sMsgId = MessageBox(sMsg, XPR_NULL, MB_YESNO | MB_ICONQUESTION);
    if (sMsgId != IDYES)
        return;

    RecentFileList &sRecentFileList = RecentFileList::instance();
    sRecentFileList.clear();
}

void CfgAdvDlg::OnUpdateChecker(void)
{
    xpr_bool_t sUpdateCheckerEnable = ((CButton *)GetDlgItem(IDC_CFG_ADV_UPDATE_ENABLE))->GetCheck();

    GetDlgItem(IDC_CFG_ADV_UPDATE_CHECK_MINOR_VER_UP)->EnableWindow(sUpdateCheckerEnable);
}
} // namespace cfg
} // namespace fxfile
