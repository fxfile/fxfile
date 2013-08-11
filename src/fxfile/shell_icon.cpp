//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "shell_icon.h"

#include "sys_img_list.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace fxfile
{
ShellIcon::ShellIcon(void)
    : mHwnd(XPR_NULL), mMsg(0)
    , mEvent(XPR_NULL)
{
}

ShellIcon::~ShellIcon(void)
{
    Stop();

    CLOSE_HANDLE(mEvent);

    clear();
}

void ShellIcon::clear(void)
{
    xpr::MutexGuard sLockGuard(mMutex);

    IconDeque::iterator sIterator;
    AsyncIcon *sAsyncIcon;

    sIterator = mIconDeque.begin();
    for (; sIterator != mIconDeque.end(); ++sIterator)
    {
        sAsyncIcon = *sIterator;
        XPR_SAFE_DELETE(sAsyncIcon);
    }

    mIconDeque.clear();
}

void ShellIcon::setOwner(HWND aHwnd, xpr_uint_t aMsg)
{
    mHwnd = aHwnd;
    mMsg  = aMsg;
}

xpr_bool_t ShellIcon::getAsyncIcon(AsyncIcon *aAsyncIcon)
{
    if (XPR_IS_NULL(aAsyncIcon))
        return XPR_FALSE;

    if ((XPR_IS_NULL(aAsyncIcon->mShellFolder) || XPR_IS_NULL(aAsyncIcon->mPidl)) && (aAsyncIcon->mPath.empty() == XPR_TRUE))
        return XPR_FALSE;

    if (IsRunning() == XPR_FALSE)
    {
        SetPriority(THREAD_PRIORITY_BELOW_NORMAL);
        Start();
    }

    {
        xpr::MutexGuard sLockGuard(mMutex);
        mIconDeque.push_back(aAsyncIcon);

        ::SetEvent(mEvent);
    }

    return XPR_TRUE;
}

xpr_bool_t ShellIcon::OnPreEntry(void)
{
    CLOSE_HANDLE(mEvent);

    mEvent = ::CreateEvent(XPR_NULL, XPR_TRUE, XPR_FALSE, XPR_NULL);

    return XPR_TRUE;
}

unsigned ShellIcon::OnEntryProc(void)
{
    // important!!!
    ::OleInitialize(XPR_NULL);

    xpr_bool_t sResult;
    AsyncIcon *sAsyncIcon;

    DWORD sWait;

    while (IsStop() == XPR_FALSE)
    {
        sWait = ::WaitForSingleObject(mEvent, 10);
        if (sWait != WAIT_OBJECT_0)
            continue;

        {
            xpr::MutexGuard sLockGuard(mMutex);

            if (mIconDeque.empty() == true)
            {
                ::ResetEvent(mEvent);
                continue;
            }

            sAsyncIcon = mIconDeque.front();
            mIconDeque.pop_front();

            if (XPR_IS_NULL(sAsyncIcon))
                continue;
        }

        switch (sAsyncIcon->mType)
        {
        case TypeIcon:
            {
                // 1. Extract Icon from File
                // 2. Shell Icon from Path
                // 3. Shell Icon from Shell Folder & PIDL

                xpr_bool_t sLargeIcon   = XPR_TEST_BITS(sAsyncIcon->mFlags, FlagLargeIcon);
                xpr_bool_t sFastNetIcon = XPR_TEST_BITS(sAsyncIcon->mFlags, FlagFastNetIcon);

                sAsyncIcon->mResult.mIcon = ShellIcon::getIcon(
                    sAsyncIcon->mIconPath,
                    sAsyncIcon->mIconIndex,
                    sAsyncIcon->mPath,
                    sFastNetIcon,
                    sLargeIcon);

                if (XPR_IS_NULL(sAsyncIcon->mResult.mIcon))
                {
                    if (XPR_IS_NOT_NULL(sAsyncIcon->mShellFolder) && XPR_IS_NOT_NULL(sAsyncIcon->mPidl))
                    {
                        sAsyncIcon->mResult.mIcon = GetItemIcon(sAsyncIcon->mShellFolder, sAsyncIcon->mPidl, sLargeIcon);
                    }
                }
            }
            break;

        case TypeIconIndex:
            {
                if (sAsyncIcon->mPath.empty() == XPR_FALSE)
                    sAsyncIcon->mResult.mIconIndex = GetItemIconIndex(sAsyncIcon->mPath.c_str());
                else
                    sAsyncIcon->mResult.mIconIndex = GetItemIconIndex(sAsyncIcon->mShellFolder, sAsyncIcon->mPidl);
            }
            break;

        case TypeOverlayIndex:
            {
                if (sAsyncIcon->mPath.empty() == XPR_FALSE)
                    sAsyncIcon->mResult.mIconIndex = GetItemIconOverlayIndex(sAsyncIcon->mPath.c_str());
                else
                    sAsyncIcon->mResult.mIconIndex = GetItemIconOverlayIndex(sAsyncIcon->mShellFolder, sAsyncIcon->mPidl);
            }
            break;
        }

        sResult = ::PostMessage(mHwnd, mMsg, (WPARAM)sAsyncIcon, (LPARAM)XPR_NULL);
        if (XPR_IS_FALSE(sResult))
        {
            XPR_SAFE_DELETE(sAsyncIcon);
        }
    }

    return 0;
}

HICON ShellIcon::getIcon(const xpr::tstring &aIconPath, xpr_sint_t aIconIndex, const xpr::tstring &aPath, xpr_bool_t aFastNetIcon, xpr_bool_t aLarge)
{
    HICON sIcon = XPR_NULL;

    if (aIconPath.empty() == XPR_FALSE)
    {
        sIcon = extractIcon(aIconPath, aIconIndex, aLarge);
        if (XPR_IS_NULL(sIcon))
        {
            if (aIconPath[0] == XPR_STRING_LITERAL('%'))
            {
                xpr::tstring sRealIconPath;
                GetEnvRealPath(aIconPath, sRealIconPath);

                sIcon = extractIcon(sRealIconPath, aIconIndex, aLarge);
            }
        }
    }

    if (XPR_IS_NULL(sIcon))
    {
        if (aPath.empty() == XPR_FALSE)
        {
            if (XPR_IS_TRUE(aFastNetIcon))
            {
                // network icon index of shell32.dll
                // ---------------------------------
                // network computer : 15
                // network folder   : 85

                if (IsNetItem(aPath.c_str()) == XPR_TRUE)
                {
                    if (aPath.rfind(XPR_STRING_LITERAL('\\')) < 2)
                    {
                        sIcon = extractIcon(XPR_STRING_LITERAL("%SystemRoot%\\system32\\SHELL32.dll"), 15, aLarge);
                    }
                    else
                    {
                        sIcon = extractIcon(XPR_STRING_LITERAL("%SystemRoot%\\system32\\SHELL32.dll"), 85, aLarge);
                    }
                }
            }

            if (XPR_IS_NULL(sIcon))
            {
                LPITEMIDLIST sFullPidl = Path2Pidl(aPath);
                if (XPR_IS_NOT_NULL(sFullPidl))
                {
                    sIcon = GetItemIcon(sFullPidl, XPR_FALSE, aLarge);
                    COM_FREE(sFullPidl);
                }
            }
        }
    }

    return sIcon;
}

HICON ShellIcon::extractIcon(const xpr::tstring &aIconPath, xpr_sint_t aIconIndex, xpr_bool_t aLarge)
{
    HICON sIcon = XPR_NULL;

    ::ExtractIconEx(
        aIconPath.c_str(),
        aIconIndex,
        XPR_IS_TRUE(aLarge) ? &sIcon : XPR_NULL,
        XPR_IS_TRUE(aLarge) ? XPR_NULL : &sIcon,
        1);

    return sIcon;
}
} // namespace fxfile
