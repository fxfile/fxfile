//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "drive_detail_dlg.h"

#include "size_format.h"
#include "option.h"
#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace fxfile
{
namespace cmd
{
DriveDetailDlg::DriveDetailDlg(void)
    : super(IDD_DRIVE_DETAIL, XPR_NULL)
    , mDriveIconHandle(XPR_NULL)
{
}

void DriveDetailDlg::DoDataExchange(CDataExchange* pDX)
{
    super::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_DRIVE_INFO,     mDrivePieCtrl);
    DDX_Control(pDX, IDC_DRIVE_PROGRESS, mProgressCtrl);
}

BEGIN_MESSAGE_MAP(DriveDetailDlg, super)
    ON_WM_DESTROY()
END_MESSAGE_MAP()

xpr_bool_t DriveDetailDlg::OnInitDialog(void) 
{
    super::OnInitDialog();

    SetDlgItemText(IDC_DRIVE_LABEL_LABEL,      gApp.loadString(XPR_STRING_LITERAL("popup.drive_info.label.label")));
    SetDlgItemText(IDC_DRIVE_LABEL_TYPE,       gApp.loadString(XPR_STRING_LITERAL("popup.drive_info.label.type")));
    SetDlgItemText(IDC_DRIVE_LABEL_FILESYSTEM, gApp.loadString(XPR_STRING_LITERAL("popup.drive_info.label.file_system")));
    SetDlgItemText(IDC_DRIVE_LABEL_FREE,       gApp.loadString(XPR_STRING_LITERAL("popup.drive_info.label.free_size")));
    SetDlgItemText(IDC_DRIVE_LABEL_USED,       gApp.loadString(XPR_STRING_LITERAL("popup.drive_info.label.used_size")));
    SetDlgItemText(IDC_DRIVE_LABEL_TOTAL,      gApp.loadString(XPR_STRING_LITERAL("popup.drive_info.label.total_size")));

    mDrivePieCtrl.setString(
        gApp.loadString(XPR_STRING_LITERAL("popup.drive_info.pie.total_size")),
        gApp.loadString(XPR_STRING_LITERAL("popup.drive_info.pie.used_size")),
        gApp.loadString(XPR_STRING_LITERAL("popup.drive_info.pie.free_size")));

    return XPR_TRUE;
}

void DriveDetailDlg::OnDestroy(void) 
{
    super::OnDestroy();

    DESTROY_ICON(mDriveIconHandle);
}

void DriveDetailDlg::setDrive(xpr_tchar_t aDrive)
{
    xpr_tchar_t sDrivePath[XPR_MAX_PATH + 1] = {0};
    _stprintf(sDrivePath, XPR_STRING_LITERAL("%c:\\"), aDrive);

    xpr_tchar_t sText[XPR_MAX_PATH + 1] = {0};
    _stprintf(sText, XPR_STRING_LITERAL("%c %s"), aDrive, gApp.loadString(XPR_STRING_LITERAL("popup.drive_info.drive")));
    GetDlgItem(IDC_DRIVE_DRIVE)->SetWindowText(sText);

    SHFILEINFO sShFileInfo = {0};
    SHGetFileInfo(sDrivePath, 0, &sShFileInfo, sizeof(SHFILEINFO), SHGFI_ICON);
    mDriveIconHandle = sShFileInfo.hIcon;
    ((CStatic *)GetDlgItem(IDC_DRIVE_ICON))->SetIcon(mDriveIconHandle);

    xpr_sint_t sDriveType = GetRootDriveType(aDrive);
    switch (sDriveType)
    {
    case DriveType8InchFloppyDisk:   _tcscpy(sText, gApp.loadString(XPR_STRING_LITERAL("common.drive.type.8_inch_floppy")));    break;
    case DriveType35InchFloppyDisk:  _tcscpy(sText, gApp.loadString(XPR_STRING_LITERAL("common.drive.type.3.5_inch_floppy")));  break;
    case DriveType525InchFloppyDisk: _tcscpy(sText, gApp.loadString(XPR_STRING_LITERAL("common.drive.type.5.25_inch_floppy"))); break;
    case DriveTypeRemovableDisk:     _tcscpy(sText, gApp.loadString(XPR_STRING_LITERAL("common.drive.type.removable")));        break;
    case DriveTypeLocal:             _tcscpy(sText, gApp.loadString(XPR_STRING_LITERAL("common.drive.type.local")));            break;
    case DriveTypeNetwork:           _tcscpy(sText, gApp.loadString(XPR_STRING_LITERAL("common.drive.type.network")));          break;
    case DriveTypeCdRom:             _tcscpy(sText, gApp.loadString(XPR_STRING_LITERAL("common.drive.type.cdrom")));            break;
    case DriveTypeRam:               _tcscpy(sText, gApp.loadString(XPR_STRING_LITERAL("common.drive.type.ram")));              break;

    case DriveTypeUnknown:
    default:                         _tcscpy(sText, gApp.loadString(XPR_STRING_LITERAL("common.drive.type.unknown")));          break;
    }

    GetDlgItem(IDC_DRIVE_TYPE)->SetWindowText(sText);

    xpr_tchar_t sFileSystem[XPR_MAX_PATH + 1] = {0};
    if (GetVolumeInformation(sDrivePath, sText, XPR_MAX_PATH, XPR_NULL, XPR_NULL, XPR_NULL, sFileSystem, XPR_MAX_PATH) == XPR_FALSE)
    {
        _tcscpy(sText, gApp.loadString(XPR_STRING_LITERAL("popup.drive_info.unknown_label")));
        _tcscpy(sFileSystem, gApp.loadString(XPR_STRING_LITERAL("popup.drive_info.unknown_file_system")));
    }
    GetDlgItem(IDC_DRIVE_FILESYSTEM)->SetWindowText(sFileSystem);
    GetDlgItem(IDC_DRIVE_LABEL)->SetWindowText(sText);

    mProgressCtrl.setBoldFont(XPR_TRUE);
    mProgressCtrl.setRange(0, 100);
    mProgressCtrl.setPos(0);

    // [2009/11/15] bug patched
    // Crash Occurrence in Windows NT 4.0 below
    ULARGE_INTEGER sTemp  = {0};
    ULARGE_INTEGER sTotal = {0};
    ULARGE_INTEGER sFree  = {0};
    ULARGE_INTEGER sUsed  = {0};
    if (::GetDiskFreeSpaceEx(sDrivePath, &sTemp, &sTotal, &sFree) == XPR_FALSE)
        return;

    sUsed.QuadPart = sTotal.QuadPart - sFree.QuadPart;

    xpr_tchar_t sFormatedSize[0xff] = {0};
    SizeFormat::getSizeUnitFormat(sFree.QuadPart, SIZE_UNIT_BYTE, sFormatedSize, 0xfe);
    GetDlgItem(IDC_DRIVE_FREE_BYTE)->SetWindowText(sFormatedSize);
    SizeFormat::getSizeUnitFormat(sFree.QuadPart, SIZE_UNIT_AUTO, sFormatedSize, 0xfe);
    GetDlgItem(IDC_DRIVE_FREE_UNIT)->SetWindowText(sFormatedSize);

    SizeFormat::getSizeUnitFormat(sTotal.QuadPart, SIZE_UNIT_BYTE, sFormatedSize, 0xfe);
    GetDlgItem(IDC_DRIVE_TOTAL_BYTE)->SetWindowText(sFormatedSize);
    SizeFormat::getSizeUnitFormat(sTotal.QuadPart, SIZE_UNIT_AUTO, sFormatedSize, 0xfe);
    GetDlgItem(IDC_DRIVE_TOTAL_UNIT)->SetWindowText(sFormatedSize);

    SizeFormat::getSizeUnitFormat(sUsed.QuadPart, SIZE_UNIT_BYTE, sFormatedSize, 0xfe);
    GetDlgItem(IDC_DRIVE_USED_BYTE)->SetWindowText(sFormatedSize);
    SizeFormat::getSizeUnitFormat(sUsed.QuadPart, SIZE_UNIT_AUTO, sFormatedSize, 0xfe);
    GetDlgItem(IDC_DRIVE_USED_UNIT)->SetWindowText(sFormatedSize);

    mProgressCtrl.setPos((xpr_sint_t)(((xpr_double_t)(xpr_sint64_t)sUsed.QuadPart / (xpr_double_t)(xpr_sint64_t)sTotal.QuadPart) * 100.0));
    mDrivePieCtrl.setDrive(sDrivePath);
}
} // namespace cmd
} // namespace fxfile
