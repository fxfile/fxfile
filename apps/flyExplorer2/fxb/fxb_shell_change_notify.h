//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXB_SHELL_CHANGE_NOTIFY_H__
#define __FXB_SHELL_CHANGE_NOTIFY_H__
#pragma once

#include "xpr_pattern.h"

namespace fxb
{
#pragma pack(push, 2)
typedef struct ShcnDrives
{
    xpr_ushort_t mStructSize;
    DWORD        mDriveBits1;
    DWORD        mDriveBits2;
} ShcnDrives;
#pragma pack(pop)

typedef struct Shcn
{
    void Free(void)
    {
        if (mEventId != SHCNE_UPDATEIMAGE && mEventId != SHCNE_FREESPACE)
        {
            COM_FREE(mPidl1);
            COM_FREE(mPidl2);
        }
    }

    HWND          mHwnd;
    xpr_uint_t    mMsg;

    xpr_uintptr_t mItem1;
    xpr_uintptr_t mItem2;
    xpr_slong_t   mEventId;

    union
    {
        struct
        {
            LPITEMIDLIST mPidl1;
            LPITEMIDLIST mPidl2;
        };

        // SHCNE_FREESPACE
        ShcnDrives mDrives;
    };
} Shcn;

typedef struct NotifyItem
{
    xpr_uint_t   mId;
    HWND         mHwnd;
    xpr_uint_t   mMsg;
    LPITEMIDLIST mFullPidl;
    xpr_slong_t  mEventMask;
    xpr_uint_t   mNotifyMsg;
    HANDLE       mNotify;
} NotifyItem;

typedef struct ShNotifyInfo
{
    xpr_slong_t  mEventId;
    xpr_tchar_t  mOldName[XPR_MAX_PATH + 1];
    xpr_tchar_t  mNewName[XPR_MAX_PATH + 1];
    HTREEITEM    mParentTreeItem;
    Shcn        *mShcn;
} ShNotifyInfo;

typedef struct SHNotifyStruct
{
    uintptr_t mItem1;
    uintptr_t mItem2;
} SHNotifyStruct;

class ShellChangeNotify : public CWnd, public xpr::Singleton<ShellChangeNotify>
{
    friend class xpr::Singleton<ShellChangeNotify>;

protected: ShellChangeNotify(void);
public: virtual ~ShellChangeNotify(void);

public:
    static void setNoRefresh(xpr_bool_t aNoRefresh);

public:
    void create(void);
    void destroy(void);

    void start(void);
    void stop(DWORD aStopMillseconds = INFINITE);

    xpr_uint_t registerWatch(HWND  aHwnd, xpr_uint_t aMsg, LPITEMIDLIST aFullPidl, xpr_slong_t aEventMask, xpr_bool_t aSubItems);
    xpr_uint_t registerWatch(CWnd *aWnd, xpr_uint_t aMsg, LPITEMIDLIST aFullPidl, xpr_slong_t aEventMask, xpr_bool_t aSubItems);
    xpr_bool_t modifyWatch(xpr_uint_t aId, LPITEMIDLIST aFullPidl, xpr_bool_t aSubItems);
    void unregisterWatch(xpr_uint_t aId);

protected:
    static unsigned __stdcall NotifyProc(void *aParam);
    DWORD OnNotify(void);

    void insertQueue(HWND aHwnd, xpr_uint_t aMsg, WPARAM wParam, LPARAM lParam);

protected:
    static xpr_uint_t mIdMgr;

    static xpr_bool_t mNoRefresh;

    HANDLE   mThread;
    HANDLE   mEvent;
    HANDLE   mStopEvent;
    unsigned mThreadId;
    Cs       mCs;

    typedef std::deque<Shcn *> ShcnDeque;
    ShcnDeque mShcnDeque;

    typedef std::set<xpr_uint_t> MsgSet;
    MsgSet mMsgSet;

    typedef std::map<xpr_uint_t, NotifyItem *> NotifyMap;
    NotifyMap mNotifyMap;

    typedef std::map<xpr_uint_t, NotifyItem *> NotifyMsgMap;
    NotifyMsgMap mNotifyMsgMap;

protected:
    virtual LRESULT WindowProc(xpr_uint_t aMsg, WPARAM wParam, LPARAM lParam);

protected:
    DECLARE_MESSAGE_MAP()
};
} // namespace fxb

#endif // __FXB_SHELL_CHANGE_NOTIFY_H__
