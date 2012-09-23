//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "fxb_drive_shcn.h"

#include "fxb_shell_change_notify.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace fxb
{
//
// user defined message
//
enum
{
    WM_SHELL_CHANGE_NOTIFY = WM_USER + 1500,
};

DriveShcn::DriveShcn(void)
    : mShcnId(0)
{
}

DriveShcn::~DriveShcn(void)
{
}

BEGIN_MESSAGE_MAP(DriveShcn, CWnd)
    ON_MESSAGE(WM_SHELL_CHANGE_NOTIFY, OnShellChangeNotify)
END_MESSAGE_MAP()

void DriveShcn::create(void)
{
    CreateEx(0, AfxRegisterWndClass(CS_GLOBALCLASS), XPR_STRING_LITERAL(""), 0,0,0,0,0,0,0);
}

void DriveShcn::destroy(void)
{
    NotifyInfo *sNotifyInfo;
    DriveShcnMap::iterator sIterator;

    sIterator = mDriveShcnMap.begin();
    for (; sIterator != mDriveShcnMap.end(); ++sIterator)
    {
        sNotifyInfo = sIterator->second;
        XPR_SAFE_DELETE(sNotifyInfo);
    }

    mDriveShcnMap.clear();

    DestroyWindow();
}

void DriveShcn::start(void)
{
    HRESULT sHResult;
    LPITEMIDLIST sFullPidl = XPR_NULL;

    sHResult = ::SHGetSpecialFolderLocation(XPR_NULL, CSIDL_DRIVES, &sFullPidl);

    if (SUCCEEDED(sHResult) && XPR_IS_NOT_NULL(sFullPidl))
    {
        xpr_slong_t sEventMask = 0;
        sEventMask |= SHCNE_MEDIAINSERTED;
        sEventMask |= SHCNE_MEDIAREMOVED;
        sEventMask |= SHCNE_DRIVEADD;
        sEventMask |= SHCNE_DRIVEREMOVED;
        sEventMask |= SHCNE_FREESPACE;
        sEventMask |= SHCNE_RENAMEFOLDER;

        mShcnId = ShellChangeNotify::instance().registerWatch(this, WM_SHELL_CHANGE_NOTIFY, sFullPidl, sEventMask, XPR_FALSE);
    }

    COM_FREE(sFullPidl);
}

void DriveShcn::stop(void)
{
    ShellChangeNotify::instance().unregisterWatch(mShcnId);
}

void DriveShcn::registerWatch(CWnd *aWnd, xpr_uint_t aMsg, xpr_slong_t aEventMask)
{
    if (XPR_IS_NULL(aWnd) || XPR_IS_NULL(aWnd->m_hWnd))
        return;

    registerWatch(aWnd->m_hWnd, aMsg, aEventMask);
}

void DriveShcn::unregisterWatch(CWnd *aWnd)
{
    if (XPR_IS_NULL(aWnd) || XPR_IS_NULL(aWnd->m_hWnd))
        return;

    unregisterWatch(aWnd->m_hWnd);
}

xpr_bool_t DriveShcn::isRegisteredWatch(CWnd *aWnd)
{
    if (XPR_IS_NULL(aWnd) || XPR_IS_NULL(aWnd->m_hWnd))
        return XPR_FALSE;

    return isRegisteredWatch(aWnd->m_hWnd);
}

xpr_bool_t DriveShcn::isRegisteredWatch(HWND aHwnd)
{
    return (mDriveShcnMap.find(aHwnd) != mDriveShcnMap.end());
}

void DriveShcn::registerWatch(HWND aHwnd, xpr_uint_t aMsg, xpr_slong_t aEventMask)
{
    unregisterWatch(aHwnd);

    NotifyInfo *sNotifyInfo = new NotifyInfo;
    sNotifyInfo->mMsg       = aMsg;
    sNotifyInfo->mEventMask = aEventMask;

    mDriveShcnMap[aHwnd] = sNotifyInfo;
}

void DriveShcn::unregisterWatch(HWND aHwnd)
{
    DriveShcnMap::iterator sIterator = mDriveShcnMap.find(aHwnd);
    if (sIterator == mDriveShcnMap.end())
        return;

    NotifyInfo *sNotifyInfo = (NotifyInfo *)sIterator->second;
    XPR_SAFE_DELETE(sNotifyInfo);

    mDriveShcnMap.erase(sIterator);
}

LRESULT DriveShcn::OnShellChangeNotify(WPARAM wParam, LPARAM lParam)
{
    Shcn *sShcn = (Shcn *)wParam;
    DWORD sEventId = (DWORD)lParam;

    if (XPR_IS_NULL(sShcn))
        return 0;

    HWND sHwnd;
    NotifyInfo *sNotifyInfo;
    DriveShcnMap::iterator sIterator;

    sIterator = mDriveShcnMap.begin();
    for (; sIterator != mDriveShcnMap.end(); ++sIterator)
    {
        sHwnd = sIterator->first;
        sNotifyInfo = sIterator->second;

        if (XPR_IS_NULL(sNotifyInfo))
            continue;

        if (!XPR_TEST_BITS(sNotifyInfo->mEventMask, sEventId))
            continue;

        ::SendMessage(sHwnd, sNotifyInfo->mMsg, (WPARAM)sShcn, (LPARAM)sEventId);
    }

    XPR_SAFE_DELETE(sShcn);

    return 0;
}
} // namespace fxb
