//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "CfgContentsDlg.h"

#include "../resource.h"
#include "../Option.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CfgContentsDlg::CfgContentsDlg(void)
    : super(IDD_CFG_CONTENTS, XPR_NULL)
{
}

void CfgContentsDlg::DoDataExchange(CDataExchange* pDX)
{
    super::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_CFG_CONTENTS_BOOKMARK_COLOR, mBookmarkColorCtrl);
}

BEGIN_MESSAGE_MAP(CfgContentsDlg, super)
    ON_BN_CLICKED(IDC_CFG_CONTENTS_NONE,     OnContentsNone)
    ON_BN_CLICKED(IDC_CFG_CONTENTS_STANDARD, OnContentsStandard)
    ON_BN_CLICKED(IDC_CFG_CONTENTS_BASIC,    OnContentsBasic)
    ON_MESSAGE(CPN_SELENDOK, OnSelEndOK)
END_MESSAGE_MAP()

xpr_bool_t CfgContentsDlg::OnInitDialog(void) 
{
    super::OnInitDialog();

    switch (gOpt->mContentsStyle)
    {
    case CONTENTS_NONE:
        ((CButton*)GetDlgItem(IDC_CFG_CONTENTS_NONE))->SetCheck(2);
        setActive(CONTENTS_NONE);
        break;

    case CONTENTS_BASIC:
        ((CButton*)GetDlgItem(IDC_CFG_CONTENTS_BASIC))->SetCheck(2);
        setActive(CONTENTS_BASIC);
        break;

    case CONTENTS_EXPLORER:
    default:
        ((CButton*)GetDlgItem(IDC_CFG_CONTENTS_STANDARD))->SetCheck(2);
        setActive(CONTENTS_EXPLORER);
        break;
    }

    ((CButton *)GetDlgItem(IDC_CFG_CONTENTS_BOOKMARK            ))->SetCheck(gOpt->mContentsBookmark);
    ((CButton *)GetDlgItem(IDC_CFG_CONTENTS_NO_DISP_INFO        ))->SetCheck(gOpt->mContentsNoDispInfo);
    ((CButton *)GetDlgItem(IDC_CFG_CONTENTS_SHOW_ATTR_ARHS_STYLE))->SetCheck(gOpt->mContentsARHSAttribute);

    mBookmarkColorCtrl.SetDefaultText(theApp.loadString(XPR_STRING_LITERAL("popup.common.color_ctrl.automatic")));
    mBookmarkColorCtrl.SetCustomText(theApp.loadString(XPR_STRING_LITERAL("popup.common.color_ctrl.other_color")));
    mBookmarkColorCtrl.SetDefaultColor(RGB(0,0,255));
    mBookmarkColorCtrl.SetColor(gOpt->mContentsBookmarkColor);

    SetDlgItemText(IDC_CFG_CONTENTS_GROUP_OPTION,          theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.contents.group.option")));
    SetDlgItemText(IDC_CFG_CONTENTS_NONE,                  theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.contents.radio.contents.none")));
    SetDlgItemText(IDC_CFG_CONTENTS_STANDARD,              theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.contents.radio.contents.standard")));
    SetDlgItemText(IDC_CFG_CONTENTS_BASIC,                 theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.contents.radio.contents.basic")));
    SetDlgItemText(IDC_CFG_CONTENTS_NO_DISP_INFO,          theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.contents.check.no_display_info")));
    SetDlgItemText(IDC_CFG_CONTENTS_GROUP_BOOKMARK,        theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.contents.group.bookmark")));
    SetDlgItemText(IDC_CFG_CONTENTS_BOOKMARK,              theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.contents.check.enable_bookmark")));
    SetDlgItemText(IDC_CFG_CONTENTS_LABEL_BOOKMARK_COLOR,  theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.contents.label.bookmark_color")));
    SetDlgItemText(IDC_CFG_CONTENTS_SHOW_ATTR_ARHS_STYLE,  theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.contents.check.show_attribute_ARHS_style")));

    return XPR_TRUE;
}

void CfgContentsDlg::OnContentsNone(void)
{
    setActive(CONTENTS_NONE);
    setModified();
}

void CfgContentsDlg::OnContentsStandard(void) 
{
    setActive(CONTENTS_EXPLORER);
    setModified();
}

void CfgContentsDlg::OnContentsBasic(void) 
{
    setActive(CONTENTS_BASIC);
    setModified();
}

void CfgContentsDlg::setActive(xpr_uint_t aContentsType)
{
    xpr_bool_t aActive = (aContentsType == CONTENTS_EXPLORER) ? XPR_TRUE : XPR_FALSE;
    GetDlgItem(IDC_CFG_CONTENTS_BOOKMARK)->EnableWindow(aActive);
    GetDlgItem(IDC_CFG_CONTENTS_BOOKMARK_COLOR)->EnableWindow(aActive);
}

LRESULT CfgContentsDlg::OnSelEndOK(WPARAM wParam, LPARAM lParam)
{
    setModified();
    return XPR_TRUE;
}

void CfgContentsDlg::OnApply(void)
{
    OptionMgr &sOptionMgr = OptionMgr::instance();

    if (((CButton *)GetDlgItem(IDC_CFG_CONTENTS_NONE))->GetCheck())
        gOpt->mContentsStyle = CONTENTS_NONE;
    else if (((CButton *)GetDlgItem(IDC_CFG_CONTENTS_BASIC))->GetCheck())
        gOpt->mContentsStyle = CONTENTS_BASIC;
    else
        gOpt->mContentsStyle = CONTENTS_EXPLORER;

    gOpt->mContentsBookmark      = ((CButton *)GetDlgItem(IDC_CFG_CONTENTS_BOOKMARK))->GetCheck();
    gOpt->mContentsNoDispInfo    = ((CButton *)GetDlgItem(IDC_CFG_CONTENTS_NO_DISP_INFO))->GetCheck();
    gOpt->mContentsBookmarkColor = mBookmarkColorCtrl.GetColor();
    gOpt->mContentsARHSAttribute = ((CButton *)GetDlgItem(IDC_CFG_CONTENTS_SHOW_ATTR_ARHS_STYLE))->GetCheck();

    if (gOpt->mContentsBookmarkColor == CLR_DEFAULT)
        gOpt->mContentsBookmarkColor = mBookmarkColorCtrl.GetDefaultColor();

    sOptionMgr.applyFolderCtrl(1, XPR_FALSE);
    sOptionMgr.applyExplorerView(1, XPR_FALSE);
    sOptionMgr.applyEtc(1);
}

xpr_bool_t CfgContentsDlg::PreTranslateMessage(MSG* pMsg) 
{
    return super::PreTranslateMessage(pMsg);
}

xpr_bool_t CfgContentsDlg::DestroyWindow(void) 
{
    return super::DestroyWindow();
}

xpr_bool_t CfgContentsDlg::OnCommand(WPARAM wParam, LPARAM lParam) 
{
    xpr_uint_t sNotifyMsg = HIWORD(wParam);
    xpr_uint_t sId = LOWORD(wParam);

    if (sNotifyMsg == BN_CLICKED)
    {
        switch (sId)
        {
        case IDC_CFG_CONTENTS_BOOKMARK:
        case IDC_CFG_CONTENTS_NO_DISP_INFO:
        case IDC_CFG_CONTENTS_SHOW_ATTR_ARHS_STYLE:
            setModified();
            break;
        }
    }

    return super::OnCommand(wParam, lParam);
}
