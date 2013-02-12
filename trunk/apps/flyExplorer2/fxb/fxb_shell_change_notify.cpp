//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "fxb_shell_change_notify.h"

#include "../Option.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace fxb
{
typedef struct NotifyRegister
{
    LPCITEMIDLIST mFullPidl;
    xpr_bool_t    mSubItems;
} NotifyRegister;

typedef WINSHELLAPI HANDLE (WINAPI *SHChangeNotifyRegisterFunc)(HWND                  aHwnd,
                                                                DWORD                 aFlags, 
                                                                xpr_slong_t           aEventMask,
                                                                xpr_uint_t            aMsg,
                                                                DWORD                 aItemCount,
                                                                const NotifyRegister *aItems);

typedef WINSHELLAPI xpr_bool_t (WINAPI *SHChangeNotifyDeregisterFunc)(HANDLE aNotify);

// second parameter
//
//SHCNF_ACCEPT_INTERRUPTS       0x0001
//SHCNF_ACCEPT_NON_INTERRUPTS   0x0002
//SHCNF_NO_PROXY                0x8000

//SHCNRF_InterruptLevel         0x0001
//SHCNRF_ShellLevel             0x0002
//SHCNRF_RecursiveInterrupt     0x1000 /* Must be combined with SHCNRF_InterruptLevel */
//SHCNRF_NewDelivery            0x8000 /* Messages use shared memory */

static HANDLE createShellChangeNotify(HWND aHwnd, LPCITEMIDLIST aFullPidl, xpr_slong_t aEventMask, xpr_uint_t aMsg, xpr_bool_t aSubItems)
{
    HANDLE sShcnHandle;
    HINSTANCE sDll;
    SHChangeNotifyRegisterFunc sSHChangeNotifyRegisterFunc;

    sDll = ::LoadLibrary(XPR_STRING_LITERAL("shell32.dll"));
    if (XPR_IS_NOT_NULL(sDll))
    {
        sSHChangeNotifyRegisterFunc = (SHChangeNotifyRegisterFunc)GetProcAddress(sDll, (const xpr_char_t *)2);
        if (XPR_IS_NOT_NULL(sSHChangeNotifyRegisterFunc))
        {
            NotifyRegister sNotifyRegister = {0};
            sNotifyRegister.mFullPidl = aFullPidl;
            sNotifyRegister.mSubItems = aSubItems;

            sShcnHandle = sSHChangeNotifyRegisterFunc(aHwnd, 0x000000FF, aEventMask | SHCNE_INTERRUPT, aMsg, 1, &sNotifyRegister);
        }

        ::FreeLibrary(sDll);
    }

    return sShcnHandle;
}

static xpr_bool_t destroyShellChangeNotify(HANDLE aShcnHandle)
{
    xpr_bool_t sResult = XPR_FALSE;
    HINSTANCE sDll;
    SHChangeNotifyDeregisterFunc sSHChangeNotifyDeregisterFunc;

    sDll = ::LoadLibrary(XPR_STRING_LITERAL("shell32.dll"));
    if (XPR_IS_NOT_NULL(sDll))
    {
        sSHChangeNotifyDeregisterFunc = (SHChangeNotifyDeregisterFunc)GetProcAddress(sDll, (const xpr_char_t *)4);
        if (XPR_IS_NOT_NULL(sSHChangeNotifyDeregisterFunc))
            sResult = sSHChangeNotifyDeregisterFunc(aShcnHandle);

        ::FreeLibrary(sDll);
    }

    return sResult;
}

enum
{
    WM_SHCHANGENOTIFY     = WM_USER + 1000,
    WM_SHCHANGENOTIFY_END = 0x7FFF,
};

xpr_uint_t ShellChangeNotify::mIdMgr     = 100;
xpr_bool_t ShellChangeNotify::mNoRefresh = XPR_FALSE;

ShellChangeNotify::ShellChangeNotify(void)
    : mThread(XPR_NULL)
    , mEvent(XPR_NULL)
    , mStopEvent(XPR_NULL)
{
}

ShellChangeNotify::~ShellChangeNotify(void)
{
}

BEGIN_MESSAGE_MAP(ShellChangeNotify, CWnd)
END_MESSAGE_MAP()

void ShellChangeNotify::create(void)
{
    CreateEx(0, AfxRegisterWndClass(CS_GLOBALCLASS), XPR_STRING_LITERAL(""), 0,0,0,0,0,0,0);
}

void ShellChangeNotify::destroy(void)
{
    stop();

    NotifyItem *sNotifyItem;
    NotifyMap::iterator sIterator;

    sIterator = mNotifyMap.begin();
    for (; sIterator != mNotifyMap.end(); ++sIterator)
    {
        sNotifyItem = sIterator->second;
        if (XPR_IS_NULL(sNotifyItem))
            continue;

        destroyShellChangeNotify(sNotifyItem->mNotify);
        COM_FREE(sNotifyItem->mFullPidl);

        XPR_SAFE_DELETE(sNotifyItem);
    }

    mNotifyMap.clear();
    mNotifyMsgMap.clear();
    mMsgSet.clear();

    DestroyWindow();
}

void ShellChangeNotify::setNoRefresh(xpr_bool_t aNoRefresh)
{
    mNoRefresh = aNoRefresh;
}

void ShellChangeNotify::start(void)
{
    if (XPR_IS_NOT_NULL(mThread))
        return;

    mEvent     = ::CreateEvent(XPR_NULL, XPR_TRUE, XPR_FALSE, XPR_NULL);
    mStopEvent = ::CreateEvent(XPR_NULL, XPR_TRUE, XPR_FALSE, XPR_NULL);
    mThread    = (HANDLE)::_beginthreadex(XPR_NULL, 0, NotifyProc, this, 0, &mThreadId);
}

void ShellChangeNotify::stop(DWORD aStopMillseconds)
{
    if (XPR_IS_NULL(mThread))
        return;

    ::SetEvent(mStopEvent);
    if (::WaitForSingleObject(mThread, aStopMillseconds) == WAIT_TIMEOUT)
        ::TerminateThread(mThread, 0);

    CLOSE_HANDLE(mThread);
    CLOSE_HANDLE(mEvent);
    CLOSE_HANDLE(mStopEvent);
}

xpr_uint_t ShellChangeNotify::registerWatch(HWND aHwnd, xpr_uint_t aMsg, LPITEMIDLIST aFullPidl, xpr_slong_t aEventMask, xpr_bool_t aSubItems)
{
    xpr_uint_t sNotifyMsg = WM_SHCHANGENOTIFY;

    xpr_uint_t i;
    for (i = WM_SHCHANGENOTIFY; i <= WM_SHCHANGENOTIFY_END; ++i)
    {
        if (mMsgSet.find(i) == mMsgSet.end())
        {
            sNotifyMsg = i;
            break;
        }
    }

    mMsgSet.insert(sNotifyMsg);

    NotifyItem *sNotifyItem = new NotifyItem;
    sNotifyItem->mId        = mIdMgr++;
    sNotifyItem->mHwnd      = aHwnd;
    sNotifyItem->mMsg       = aMsg;
    sNotifyItem->mFullPidl  = XPR_IS_NOT_NULL(aFullPidl) ? CopyItemIDList(aFullPidl) : XPR_NULL;
    sNotifyItem->mNotifyMsg = sNotifyMsg;
    sNotifyItem->mEventMask = aEventMask;
    sNotifyItem->mNotify    = createShellChangeNotify(m_hWnd, aFullPidl, aEventMask, sNotifyItem->mNotifyMsg, aSubItems);

    mNotifyMap[sNotifyItem->mId] = sNotifyItem;
    mNotifyMsgMap[sNotifyItem->mNotifyMsg] = sNotifyItem;

    return sNotifyItem->mId;
}

xpr_uint_t ShellChangeNotify::registerWatch(CWnd *aWnd, xpr_uint_t aMsg, LPITEMIDLIST aFullPidl, xpr_slong_t aEventMask, xpr_bool_t aSubItems)
{
    if (XPR_IS_NULL(aWnd))
        return 0;

    return registerWatch(aWnd->m_hWnd, aMsg, aFullPidl, aEventMask, aSubItems);
}

xpr_bool_t ShellChangeNotify::modifyWatch(xpr_uint_t aId, LPITEMIDLIST aFullPidl, xpr_bool_t aSubItems)
{
    xpr_bool_t sResult = XPR_FALSE;

    NotifyItem *sNotifyItem;
    NotifyMap::iterator sIterator;

    sIterator = mNotifyMap.find(aId);
    if (sIterator == mNotifyMap.end())
        return XPR_FALSE;

    sNotifyItem = sIterator->second;
    if (XPR_IS_NULL(sNotifyItem))
        return XPR_FALSE;

    if (sNotifyItem->mId != aId)
        return XPR_FALSE;

    destroyShellChangeNotify(sNotifyItem->mNotify);
    sNotifyItem->mNotify = XPR_NULL;
    COM_FREE(sNotifyItem->mFullPidl);

    sNotifyItem->mNotify   = createShellChangeNotify(m_hWnd, aFullPidl, sNotifyItem->mEventMask, sNotifyItem->mNotifyMsg, aSubItems);
    sNotifyItem->mFullPidl = XPR_IS_NOT_NULL(aFullPidl) ? CopyItemIDList(aFullPidl) : XPR_NULL;

    sResult = (sNotifyItem->mNotify != XPR_NULL);

    return sResult;
}

void ShellChangeNotify::unregisterWatch(xpr_uint_t aId)
{
    NotifyItem *sNotifyItem;
    NotifyMap::iterator sIterator;

    sIterator = mNotifyMap.find(aId);
    if (sIterator == mNotifyMap.end())
        return;

    sNotifyItem = sIterator->second;
    if (XPR_IS_NOT_NULL(sNotifyItem))
    {
        destroyShellChangeNotify(sNotifyItem->mNotify);
        COM_FREE(sNotifyItem->mFullPidl);

        MsgSet::iterator itMsg = mMsgSet.find(sNotifyItem->mNotifyMsg);
        if (itMsg != mMsgSet.end())
            mMsgSet.erase(itMsg);

        NotifyMsgMap::iterator sNotifyMsgIterator = mNotifyMsgMap.find(sNotifyItem->mNotifyMsg);
        if (sNotifyMsgIterator != mNotifyMsgMap.end())
            mNotifyMsgMap.erase(sNotifyMsgIterator);

        XPR_SAFE_DELETE(sNotifyItem);
    }

    mNotifyMap.erase(sIterator);
}

unsigned __stdcall ShellChangeNotify::NotifyProc(void *aParam)
{
    DWORD sExitCode = 0;

    ShellChangeNotify *sShellChangeNotify = (ShellChangeNotify *)aParam;
    if (XPR_IS_NOT_NULL(sShellChangeNotify))
        sExitCode = sShellChangeNotify->OnNotify();

    ::_endthreadex(sExitCode);
    return 0;
}

DWORD ShellChangeNotify::OnNotify(void)
{
    Shcn *sShcn;

    DWORD sWait;
    HANDLE sEvents[2] = { mStopEvent, mEvent };

    while (true)
    {
        sWait = ::WaitForMultipleObjects(2, sEvents, XPR_FALSE, INFINITE);
        if (sWait == WAIT_OBJECT_0)
            break;

        if (sWait != WAIT_OBJECT_0+1)
            continue;

        {
            CsLocker sLocker(mCs);

            if (mShcnDeque.empty() == true)
            {
                ::ResetEvent(mEvent);
                continue;
            }

            sShcn = mShcnDeque.front();
            mShcnDeque.pop_front();
        }

        if (XPR_IS_NOT_NULL(sShcn))
        {
            if (::PostMessage(sShcn->mHwnd, sShcn->mMsg, (WPARAM)sShcn, (LPARAM)sShcn->mEventId) == XPR_FALSE)
            {
                COM_FREE(sShcn->mPidl1);
                COM_FREE(sShcn->mPidl2);
                XPR_SAFE_DELETE(sShcn);
            }
        }
    }

    return 0;
}

void ShellChangeNotify::insertQueue(HWND aHwnd, xpr_uint_t aMsg, WPARAM wParam, LPARAM lParam)
{
    ShcnDrives *sShcnDrives = *reinterpret_cast<ShcnDrives **>(wParam);
    SHNotifyStruct *sSHNotifyStruct = (SHNotifyStruct *)wParam;
    LPITEMIDLIST sPidl1 = (LPITEMIDLIST)sSHNotifyStruct->mItem1;
    LPITEMIDLIST sPidl2 = (LPITEMIDLIST)sSHNotifyStruct->mItem2;
    xpr_slong_t sEventId = (xpr_slong_t)lParam;

    // FolderCtrl - filtering
    CWnd *sWnd = CWnd::FromHandle(aHwnd);
    if (sWnd->IsKindOf(RUNTIME_CLASS(CTreeCtrl)))
    {
        if (sEventId == SHCNE_CREATE || 
            sEventId == SHCNE_RENAMEITEM ||
            sEventId == SHCNE_DELETE)
            return;
    }

    Shcn *sShcn = new Shcn;
    if (XPR_IS_NULL(sShcn))
        return;

    sShcn->mHwnd    = aHwnd;
    sShcn->mMsg     = aMsg;
    sShcn->mEventId = sEventId;

    switch (sEventId)
    {
    case SHCNE_UPDATEIMAGE:
        {
            sShcn->mItem1 = sSHNotifyStruct->mItem1;
            sShcn->mItem2 = sSHNotifyStruct->mItem2;
            break;
        }

    case SHCNE_FREESPACE:
        {
            sShcn->mDrives = *sShcnDrives;
            break;
        }

    default:
        {
            sShcn->mItem1 = sSHNotifyStruct->mItem1;
            sShcn->mItem2 = sSHNotifyStruct->mItem2;

            sShcn->mPidl1  = CopyItemIDList(sPidl1);
            sShcn->mPidl2  = CopyItemIDList(sPidl2);

            break;
        }
    }

    {
        CsLocker sLocker(mCs);

        mShcnDeque.push_back(sShcn);
        ::SetEvent(mEvent);
    }
}

LRESULT ShellChangeNotify::WindowProc(xpr_uint_t aMsg, WPARAM wParam, LPARAM lParam) 
{
    if (XPR_IS_RANGE(WM_SHCHANGENOTIFY, aMsg, WM_SHCHANGENOTIFY_END))
    {
        if (XPR_IS_FALSE(mNoRefresh))
        {
            NotifyItem *sNotifyItem;
            NotifyMsgMap::iterator sNotifyMsgIterator;

            sNotifyMsgIterator = mNotifyMsgMap.find(aMsg);
            if (sNotifyMsgIterator != mNotifyMsgMap.end())
            {
                sNotifyItem = sNotifyMsgIterator->second;
                if (XPR_IS_NOT_NULL(sNotifyItem) && sNotifyItem->mNotifyMsg == aMsg)
                {
                    insertQueue(sNotifyItem->mHwnd, sNotifyItem->mMsg, wParam, lParam);
                    return XPR_TRUE;
                }
            }
        }
    }

    return CWnd::WindowProc(aMsg, wParam, lParam);
}
} // namespace fxb
