//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "filter_set_dlg.h"

#include "../img_list_manager.h"

#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace fxfile
{
namespace cfg
{
#define ICON_OFFSET 6

FilterSetDlg::FilterSetDlg(FilterItem *aFilterItem)
    : super(IDD_FILTER_SET, XPR_NULL)
{
    if (XPR_IS_NOT_NULL(aFilterItem))
        mFilterItem = *aFilterItem;
    else
    {
        mFilterItem.clear();
        mFilterItem.mColor = GetSysColor(COLOR_WINDOWTEXT);
    }
}

FilterSetDlg::~FilterSetDlg(void)
{
    mNameSet.clear();
}

void FilterSetDlg::DoDataExchange(CDataExchange* pDX)
{
    super::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_FILTER_SET_COLOR,     mColorCtrl);
    DDX_Control(pDX, IDC_FILTER_SET_ICON_LIST, mIconListCtrl);
}

BEGIN_MESSAGE_MAP(FilterSetDlg, super)
    ON_WM_DESTROY()
    ON_BN_CLICKED(IDC_FILTER_SET_APPLY_COLOR_CODE, OnApplyColorCode)
END_MESSAGE_MAP()

xpr_bool_t FilterSetDlg::OnInitDialog(void) 
{
    super::OnInitDialog();

    ImgListManager &sImgListManager = SingletonManager::get<ImgListManager>();

    mIconListCtrl.setIconOffset(ICON_OFFSET);
    mIconListCtrl.setImageList(&sImgListManager.mCustomImgList32, &sImgListManager.mCustomImgList16);
    mIconListCtrl.SetCurSel(mFilterItem.mIconIndex - ICON_OFFSET);

    ((CEdit *)GetDlgItem(IDC_FILTER_SET_NAME))->LimitText(MAX_FILTER_NAME);
    ((CEdit *)GetDlgItem(IDC_FILTER_SET_EXTS))->LimitText(MAX_FILTER_EXTS);

    SetDlgItemText(IDC_FILTER_SET_NAME, mFilterItem.mName.c_str());
    SetDlgItemText(IDC_FILTER_SET_EXTS, mFilterItem.mExts.c_str());

    mColorCtrl.SetDefaultText(gApp.loadString(XPR_STRING_LITERAL("popup.common.color_ctrl.automatic")));
    mColorCtrl.SetCustomText(gApp.loadString(XPR_STRING_LITERAL("popup.common.color_ctrl.other_color")));
    mColorCtrl.SetDefaultColor(::GetSysColor(COLOR_WINDOWTEXT));
    mColorCtrl.SetColor(mFilterItem.mColor);

    if (mFilterItem.mExts == XPR_STRING_LITERAL("*") || mFilterItem.mExts == XPR_STRING_LITERAL("*.*"))
        GetDlgItem(IDC_FILTER_SET_EXTS)->EnableWindow(XPR_FALSE);

    SetWindowText(gApp.loadString(XPR_STRING_LITERAL("popup.filter_set.title")));
    SetDlgItemText(IDC_FILTER_SET_LABEL_NAME,       gApp.loadString(XPR_STRING_LITERAL("popup.filter_set.label.name")));
    SetDlgItemText(IDC_FILTER_SET_LABEL_EXTS,       gApp.loadString(XPR_STRING_LITERAL("popup.filter_set.label.extensions")));
    SetDlgItemText(IDC_FILTER_SET_LABEL_COLOR,      gApp.loadString(XPR_STRING_LITERAL("popup.filter_set.label.color")));
    SetDlgItemText(IDC_FILTER_SET_LABEL_COLOR_CODE, gApp.loadString(XPR_STRING_LITERAL("popup.filter_set.label.color_code")));
    SetDlgItemText(IDC_FILTER_SET_APPLY_COLOR_CODE, gApp.loadString(XPR_STRING_LITERAL("popup.filter_set.button.apply_color_code")));
    SetDlgItemText(IDC_FILTER_SET_LABEL_ICON,       gApp.loadString(XPR_STRING_LITERAL("popup.filter_set.label.icon")));
    SetDlgItemText(IDOK,                            gApp.loadString(XPR_STRING_LITERAL("popup.common.button.ok")));
    SetDlgItemText(IDCANCEL,                        gApp.loadString(XPR_STRING_LITERAL("popup.common.button.cancel")));

    return XPR_TRUE;
}

FilterItem *FilterSetDlg::getFilter(void)
{
    return &mFilterItem;
}

void FilterSetDlg::setFilter(FilterItem *aFilterItem)
{
    if (XPR_IS_NOT_NULL(aFilterItem))
        mFilterItem = *aFilterItem;
}

void FilterSetDlg::addName(const xpr_tchar_t *aName)
{
    if (XPR_IS_NOT_NULL(aName))
        mNameSet.insert(aName);
}

void FilterSetDlg::OnApplyColorCode(void)
{
    xpr_tchar_t sText[0xff] = {0};
    GetDlgItemText(IDC_FILTER_SET_COLOR_CODE, sText, 0xfe);

    xpr_bool_t sWebColor = XPR_TRUE;
    if (sText[0] == '#')
        sWebColor = XPR_TRUE;

    if (_tcschr(sText, ',') != XPR_NULL)
        sWebColor = XPR_FALSE;

    xpr_sint_t sR = 0, sG = 0, sB = 0;
    if (XPR_IS_TRUE(sWebColor))
        _stscanf(sText[0] == '#' ? sText+1 : sText, XPR_STRING_LITERAL("%02x%02x%02x"), &sR, &sG, &sB);
    else
        _stscanf(sText, XPR_STRING_LITERAL("%d,%d,%d"), &sR, &sG, &sB);

    COLORREF sColor = RGB((xpr_byte_t)sR, (xpr_byte_t)sG, (xpr_byte_t)sB);
    mColorCtrl.SetColor(sColor);
}

void FilterSetDlg::OnOK(void) 
{
    xpr_tchar_t sName[MAX_FILTER_NAME + 1] = {0};
    xpr_tchar_t sExts[MAX_FILTER_EXTS + 1] = {0};
    GetDlgItemText(IDC_FILTER_SET_NAME, sName, MAX_FILTER_NAME);
    GetDlgItemText(IDC_FILTER_SET_EXTS, sExts, MAX_FILTER_EXTS);

    if (_tcslen(sName) <= 0)
    {
        const xpr_tchar_t *sMsg = gApp.loadString(XPR_STRING_LITERAL("popup.filter_set.msg.input_name"));
        MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);

        GetDlgItem(IDC_FILTER_SET_NAME)->SetFocus();
        return;
    }

    NameSet::iterator sIterator = mNameSet.find(sName);
    if (sIterator != mNameSet.end())
    {
        const xpr_tchar_t *sMsg = gApp.loadString(XPR_STRING_LITERAL("popup.filter_set.msg.duplicated_name"));
        MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);

        GetDlgItem(IDC_FILTER_SET_NAME)->SetFocus();
        return;
    }

    if (_tcslen(sExts) <= 0)
    {
        const xpr_tchar_t *sMsg = gApp.loadString(XPR_STRING_LITERAL("popup.filter_set.msg.input_extension"));
        MessageBox(sMsg, XPR_NULL, MB_OK | MB_ICONSTOP);

        GetDlgItem(IDC_FILTER_SET_EXTS)->SetFocus();
        return;
    }

    xpr_sint_t sIconIndex = mIconListCtrl.GetCurSel();
    if (sIconIndex == CB_ERR)
        sIconIndex = -1;
    else
        sIconIndex += ICON_OFFSET;

    mFilterItem.mName      = sName;
    mFilterItem.mExts      = sExts;
    mFilterItem.mIconIndex = sIconIndex;
    mFilterItem.mColor     = mColorCtrl.GetColor();
    if (mFilterItem.mColor == CLR_DEFAULT)
        mFilterItem.mColor = mColorCtrl.GetDefaultColor();

    super::OnOK();
}

void FilterSetDlg::OnDestroy(void)
{
    super::OnDestroy();
}
} // namespace cfg
} // namespace fxfile
