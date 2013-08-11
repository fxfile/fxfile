//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "drive_info_dlg.h"

#include "sys_img_list.h"
#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace fxfile
{
namespace cmd
{
DriveInfoDlg::DriveInfoDlg(xpr_tchar_t aDrive)
    : super(IDD_DRIVE_INFO, XPR_NULL)
    , mInitDrive(aDrive)
    , mDetailDlgs(XPR_NULL)
    , mTreeItems(XPR_NULL)
    , mCount(0)
{
}

void DriveInfoDlg::DoDataExchange(CDataExchange* pDX)
{
    super::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_DRIVE_INFO_TREE, mTreeCtrl);
}

BEGIN_MESSAGE_MAP(DriveInfoDlg, super)
    ON_WM_DESTROY()
    ON_NOTIFY(TVN_SELCHANGED, IDC_DRIVE_INFO_TREE, OnSelchangedTree)
END_MESSAGE_MAP()

xpr_bool_t DriveInfoDlg::OnInitDialog(void) 
{
    super::OnInitDialog();

    DWORD sLogicalDrives = GetLogicalDrives();

    xpr_sint_t i;
    xpr_sint_t sCount;

    sCount = 0;
    for (i = 0; i < 26; ++i)
    {
        if (sLogicalDrives & (1 << i))
            ++sCount;
    }

    setDriveCount(sCount);

    SetWindowText(theApp.loadString(XPR_STRING_LITERAL("popup.drive_info.title")));
    SetDlgItemText(IDOK, theApp.loadString(XPR_STRING_LITERAL("popup.common.button.ok")));

    return XPR_TRUE;
}

void DriveInfoDlg::setDriveCount(xpr_sint_t aCount)
{
    mDetailDlgs = new DriveDetailDlg[aCount];
    mTreeItems = new HTREEITEM[aCount];

    mTreeCtrl.SetImageList(&SysImgListMgr::instance().mSysImgList16, TVSIL_NORMAL);

    xpr_tchar_t sDrives[XPR_MAX_PATH + 1] = {0};
    GetDriveStrings(sDrives);

    xpr_tchar_t sText[XPR_MAX_PATH + 1] = {0};
    xpr_sint_t sPos = 0, sInitDrive = -1;
    if (mInitDrive == 0)
        mInitDrive = 'C';

    xpr_sint_t i;
    xpr_sint_t sRealCount = 0;
    for (i = 0; i < aCount; ++i)
    {
        if (sDrives[sPos] == XPR_STRING_LITERAL('\0'))
            break;

        mDrives[i] = *(sDrives+sPos);
        if (mDrives[i] == mInitDrive)
            sInitDrive = i;
        sPos += (xpr_sint_t)_tcslen(sDrives+sPos)+1;

        SHFILEINFO sShFileInfo = {0};
        _stprintf(sText, XPR_STRING_LITERAL("%c:\\"), mDrives[i]);
        SHGetFileInfo(sText, 0, &sShFileInfo, sizeof(SHFILEINFO), SHGFI_SYSICONINDEX);

        _stprintf(sText, XPR_STRING_LITERAL("%c %s"), mDrives[i], theApp.loadString(XPR_STRING_LITERAL("popup.drive_info.drive")));
        mTreeItems[i] = mTreeCtrl.InsertItem(sText, sShFileInfo.iIcon, sShFileInfo.iIcon);

        ++sRealCount;
    }

    mCount = sRealCount;

    if (sInitDrive == -1)
        sInitDrive = 0;

    mTreeCtrl.SelectItem(mTreeItems[sInitDrive]);
}

void DriveInfoDlg::showDialog(xpr_sint_t aIndex)
{
    xpr_sint_t i;
    for (i = 0; i < mCount; ++i)
    {
        if (i == aIndex)
        {
            if (mDetailDlgs[i].m_hWnd == XPR_NULL)
            {
                CRect sTreeRect;
                mTreeCtrl.GetWindowRect(sTreeRect);
                ScreenToClient(&sTreeRect);

                CRect sRect(0,0,0,0);
                mDetailDlgs[i].Create(IDD_DRIVE_DETAIL, this);
                mDetailDlgs[i].ShowWindow(SW_SHOW);
                mDetailDlgs[i].GetWindowRect(&sRect);
                ScreenToClient(sRect);
                mDetailDlgs[i].MoveWindow(sTreeRect.right + 10, sTreeRect.top, sRect.Width(), sRect.Height());
            }

            mDetailDlgs[i].ShowWindow(SW_SHOW);
        }
        else
        {
            if (mDetailDlgs[i].m_hWnd != XPR_NULL)
                mDetailDlgs[i].ShowWindow(SW_HIDE);
        }
    }

    UpdateWindow();
}

void DriveInfoDlg::OnDestroy(void) 
{
    super::OnDestroy();

    if (mDetailDlgs != XPR_NULL)
    {
        xpr_sint_t i;
        for (i = 0; i < mCount; ++i)
        {
            DESTROY_WINDOW(mDetailDlgs[i]);
        }

        XPR_SAFE_DELETE_ARRAY(mDetailDlgs);
    }

    XPR_SAFE_DELETE_ARRAY(mTreeItems);
}

void DriveInfoDlg::OnSelchangedTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
    NM_TREEVIEW *pNMTreeView = (NM_TREEVIEW *)pNMHDR;
    HTREEITEM sTreeItem = pNMTreeView->itemNew.hItem;

    xpr_sint_t i;
    for (i = 0; i < mCount; ++i)
    {
        if (mTreeItems[i] == sTreeItem)
        {
            showDialog(i);
            mDetailDlgs[i].setDrive(mDrives[i]);
            break;
        }
    }

    *pResult = 0;
}
} // namespace cmd
} // namespace fxfile
