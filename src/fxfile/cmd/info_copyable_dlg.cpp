//
// Copyright (c) 2014 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "info_copyable_dlg.h"

#include "resource.h"
#include "clipboard.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace fxfile
{
namespace cmd
{
InfoCopyableDlg::InfoCopyableDlg(void)
    : super(IDD_INFO_COPYABLE, XPR_NULL)
{
    mIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

InfoCopyableDlg::~InfoCopyableDlg(void)
{
    DESTROY_ICON(mIcon);
}

void InfoCopyableDlg::DoDataExchange(CDataExchange* pDX)
{
    super::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(InfoCopyableDlg, super)
    ON_WM_DESTROY()
    ON_COMMAND(IDC_INFO_COPY, OnCopy)
END_MESSAGE_MAP()

void InfoCopyableDlg::setTitle(const xpr_tchar_t *aTitle)
{
    XPR_ASSERT(aTitle != XPR_NULL);

    mTitle = aTitle;
}

void InfoCopyableDlg::setInfo(const xpr_tchar_t *aInfo)
{
    XPR_ASSERT(aInfo != XPR_NULL);

    mInfo = aInfo;
}

void InfoCopyableDlg::setInfo(const xpr::string &aInfo)
{
    const xpr_tchar_t *sInfo = aInfo.c_str();

    setInfo(sInfo);
}

void InfoCopyableDlg::setInfoFont(const xpr_tchar_t *aFaceName)
{
    XPR_ASSERT(aFaceName != XPR_NULL);

    mInfoFontFaceName = aFaceName;
}

xpr_bool_t InfoCopyableDlg::OnInitDialog(void) 
{
    super::OnInitDialog();

    SetIcon(mIcon, XPR_TRUE);

    // CResizingDialog -------------------------------------------
    //HideSizeIcon();

    AllowSizing(sizeResize, sizeResize);

    //sizeNone:     Don't resize at all  
    //sizeResize:   The control will be stretched in the appropriate direction 
    //sizeRepos:    The control will be moved in the appropriate direction 
    //sizeRelative: The control will be moved proportionally in the appropriate direction 
    AddControl(IDC_INFO_EDIT,        sizeResize, sizeResize);
    AddControl(IDC_INFO_COPY,        sizeNone,   sizeRepos, XPR_FALSE);
    AddControl(IDOK,                 sizeRepos,  sizeRepos, XPR_FALSE);
    //------------------------------------------------------------

    SetWindowText(mTitle.c_str());
    SetDlgItemText(IDC_INFO_EDIT, mInfo.c_str());

    if (XPR_IS_FALSE(mInfoFontFaceName.empty()))
    {
        LOGFONT sLogFont = {0};
        GetDlgItem(IDC_INFO_EDIT)->GetFont()->GetLogFont(&sLogFont);

        sLogFont.lfCharSet = 0;
        _tcscpy(sLogFont.lfFaceName, mInfoFontFaceName.c_str());

        mFont.CreateFontIndirect(&sLogFont);

        GetDlgItem(IDC_INFO_EDIT)->SetFont(&mFont, XPR_TRUE);
    }

    SetDlgItemText(IDOK,          gApp.loadString(XPR_STRING_LITERAL("popup.common.button.close")));
    SetDlgItemText(IDC_INFO_COPY, gApp.loadString(XPR_STRING_LITERAL("popup.info_copyable.button.copy")));

    return XPR_TRUE;
}

void InfoCopyableDlg::OnDestroy(void) 
{
    super::OnDestroy();

    DESTROY_ICON(mIcon);
}

void InfoCopyableDlg::OnCopy(void)
{
    SetTextClipboard(NULL, mInfo.c_str());
}
} // namespace cmd
} // namespace fxfile
