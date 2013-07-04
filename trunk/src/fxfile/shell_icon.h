//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_SHELL_ICON_H__
#define __FXFILE_SHELL_ICON_H__ 1
#pragma once

#include "xpr_thread_sync.h"
#include "thread.h"

namespace fxfile
{
class ShellIcon : public Thread
{
public:
    enum
    {
        FlagNone        = 0x00000000,
        FlagLargeIcon   = 0x00000001,
        FlagFastNetIcon = 0x00000002,
    };

    enum Type
    {
        TypeNone = 0,
        TypeIcon,
        TypeIconIndex,
        TypeOverlayIndex,
    };

    class AsyncIcon
    {
    public:
        Type          mType;
        xpr_uint_t    mFlags;
        xpr_uint_t    mCode;
        xpr_uintptr_t mItem;
        xpr_uint_t    mSignature;
        LPSHELLFOLDER mShellFolder;
        LPITEMIDLIST  mPidl;
        xpr::tstring  mPath;
        xpr::tstring  mIconPath;
        xpr_sint_t    mIconIndex;

        struct
        {
            xpr_sint_t mIconIndex;
            HICON      mIcon;
        } mResult;

    public:
        AsyncIcon(void)
        {
            mType              = TypeNone;
            mFlags             = FlagNone;
            mCode              = 0;
            mItem              = -1;
            mSignature         = 0;
            mShellFolder       = XPR_NULL;
            mPidl              = XPR_NULL;
            mResult.mIconIndex = -1;
            mResult.mIcon      = XPR_NULL;
        }

        ~AsyncIcon(void)
        {
            COM_RELEASE(mShellFolder);
            COM_FREE(mPidl);
            DESTROY_ICON(mResult.mIcon);
        }
    };

public:
    ShellIcon(void);
    virtual ~ShellIcon(void);

public:
    void setOwner(HWND aHwnd, xpr_uint_t aMsg);

    xpr_bool_t getAsyncIcon(AsyncIcon *aAsyncIcon);
    void clear(void);

public:
    static HICON getIcon(const xpr::tstring &aIconPath, xpr_sint_t aIconIndex, const xpr::tstring &aPath, xpr_bool_t aFastNetIcon, xpr_bool_t aLarge = XPR_FALSE);
    static HICON extractIcon(const xpr::tstring &aIconPath, xpr_sint_t aIconIndex, xpr_bool_t aLarge = XPR_FALSE);

protected:
    virtual xpr_bool_t OnPreEntry(void);
    virtual unsigned OnEntryProc(void);

protected:
    typedef std::deque<AsyncIcon *> IconDeque;
    IconDeque mIconDeque;

    HANDLE     mEvent;
    xpr::Mutex mMutex;

    HWND mHwnd;
    xpr_uint_t mMsg;
};
} // namespace fxfile

#endif // __FXFILE_SHELL_ICON_H__
