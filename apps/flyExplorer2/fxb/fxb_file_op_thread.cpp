//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "fxb_file_op_thread.h"

#include "../Option.h"

#include "fxb_file_op_undo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace fxb
{
//
// timer id
//
enum
{
    TM_ID_THREAD_END = 1,
};

//
// user defined message
//
enum
{
    WM_POST_END = WM_USER+100,
};

xpr_sint_t FileOpThread::mRefCount = 0;

FileOpThread::FileOpThread(void)
    : mRefIndex(mRefCount++)
    , mShFileOpStruct(XPR_NULL)
    , mSelectItem(XPR_FALSE)
    , mNotifyHwnd(XPR_NULL), mMsg(0)
    , mUndo(XPR_TRUE)
    , mThread(XPR_NULL), mThreadId(0), mStopEvent(XPR_NULL)
{
    CreateEx(0, AfxRegisterWndClass(CS_GLOBALCLASS), XPR_STRING_LITERAL(""), 0,0,0,0,0,AfxGetMainWnd()->m_hWnd,0);
}

FileOpThread::~FileOpThread(void)
{
    mRefCount--;
}

BEGIN_MESSAGE_MAP(FileOpThread, CWnd)
    ON_WM_TIMER()
    ON_WM_CLOSE()
    ON_MESSAGE(WM_POST_END, OnPostEnd)
END_MESSAGE_MAP()

void FileOpThread::setUndo(xpr_bool_t aUndo)
{
    mUndo = aUndo;
}

void FileOpThread::start(xpr_bool_t   aCopy,
                         xpr_tchar_t *aSource,
                         xpr_sint_t   aSourceCount,
                         xpr_tchar_t *aTarget,
                         xpr_bool_t   aDuplicate,
                         xpr_bool_t   aSelectedItem,
                         HWND         aNotifyHwnd,
                         xpr_uint_t   aMsg)
{
    xpr_uint_t sFunc = 0;
    WORD sFlags = 0;
    // Drop Effect
    // 1-Copy, 2-Move, 3-Link, 5-Copy+Link
    if (XPR_IS_FALSE(aCopy))
        sFunc = FO_MOVE;
    else
    {
        sFunc = FO_COPY;
        CString sCompare;
        AfxExtractSubString(sCompare, aSource, 0, '\0');
        sCompare = sCompare.Left(sCompare.ReverseFind('\\'));
        if (_tcscmp(sCompare, aTarget) == 0)
            sFlags |= FOF_RENAMEONCOLLISION;
    }

    // windows vista bug
    //if (aSourceCount > 1)
    //    sFlags |= FOF_MULTIDESTFILES;

    if (XPR_IS_TRUE(aDuplicate))
        sFlags |= FOF_RENAMEONCOLLISION;

    SHFILEOPSTRUCT *sShFileOpStruct = new SHFILEOPSTRUCT;
    memset(sShFileOpStruct, 0, sizeof(SHFILEOPSTRUCT));
    sShFileOpStruct->hwnd   = AfxGetMainWnd()->GetSafeHwnd();
    sShFileOpStruct->wFunc  = sFunc;
    sShFileOpStruct->fFlags = sFlags;
    sShFileOpStruct->pFrom  = aSource;
    sShFileOpStruct->pTo    = aTarget;

    start(sShFileOpStruct, aMsg >= WM_USER, aNotifyHwnd, aMsg);
}

void FileOpThread::start(SHFILEOPSTRUCT *aShFileOpStruct,
                         xpr_bool_t      aSelectItem,
                         HWND            aNotifyHwnd,
                         xpr_uint_t      aMsg)
{
    if (XPR_IS_NOT_NULL(mThread))
        return;

    mShFileOpStruct = aShFileOpStruct;
    mSelectItem     = aSelectItem;
    mNotifyHwnd     = aNotifyHwnd;
    mMsg            = aMsg;

    if ((XPR_IS_TRUE(mUndo)) || (XPR_IS_TRUE(mSelectItem) && XPR_IS_NOT_NULL(mNotifyHwnd) && mMsg > 0))
        mShFileOpStruct->fFlags |= FOF_WANTMAPPINGHANDLE;

    mStopEvent = ::CreateEvent(XPR_NULL, XPR_TRUE, XPR_FALSE, XPR_NULL);
    mThread = (HANDLE)::_beginthreadex(XPR_NULL, 0, FileOpProc, this, 0, &mThreadId);
}

unsigned __stdcall FileOpThread::FileOpProc(LPVOID lpParam)
{
    FileOpThread *sFileOpThread = (FileOpThread *)lpParam;

    if (XPR_IS_NOT_NULL(sFileOpThread))
        sFileOpThread->OnFileOp();

    ::_endthreadex(0);
    return 0;
}

//
// undo
//   - copy, move, rename: post-processing
//   - delete: pre-processing
//
void FileOpThread::OnFileOp(void)
{
    if (XPR_IS_TRUE(mUndo) && mShFileOpStruct->wFunc == FO_DELETE && XPR_TEST_BITS(mShFileOpStruct->fFlags, FOF_ALLOWUNDO))
    {
        FileOpUndo aFileOpUndo;
        aFileOpUndo.addOperation(mShFileOpStruct);
    }

    ::SHFileOperation(mShFileOpStruct);

    xpr_tchar_t *sSource = (xpr_tchar_t *)mShFileOpStruct->pFrom;
    xpr_tchar_t *sTarget = (xpr_tchar_t *)mShFileOpStruct->pTo;

    xpr_tchar_t *sUpdatePath = XPR_NULL;

    if (XPR_IS_NOT_NULL(sTarget) && mShFileOpStruct->wFunc != FO_DELETE)
    {
        sUpdatePath = sTarget;
    }
    else
    {
        sUpdatePath = new xpr_tchar_t[XPR_MAX_PATH + 1];
        _tcscpy(sUpdatePath, sSource);

        xpr_tchar_t *sSplit = _tcschr(sUpdatePath, '\\');
        if (XPR_IS_NOT_NULL(sSplit))
            sSplit[0] = '\0';
    }

    xpr_size_t sLen = _tcslen(sUpdatePath);
    if (sLen == 2)
    {
        sUpdatePath[sLen] = '\\';
        sUpdatePath[sLen + 2] = '\0'; // last double null
    }

    ::SHChangeNotify(SHCNE_UPDATEDIR, SHCNF_PATH | SHCNF_FLUSH, (LPCVOID)sUpdatePath, XPR_NULL);

    if (sUpdatePath != sTarget)
        XPR_SAFE_DELETE_ARRAY(sUpdatePath);

    ::SetEvent(mStopEvent);

    PostMessage(WM_POST_END);
}

LRESULT FileOpThread::OnPostEnd(WPARAM, LPARAM)
{
    SetTimer(TM_ID_THREAD_END, 10, XPR_NULL);

    // [bug patched] 2007/08/15, 2007/10/27
    // If file operation is copy or move, then this program hide behind foreground. I prevented it.
    // And, if file operation completed on background, taskbar flush like FlushWindow API function.
    xpr_bool_t sSetForegournd = XPR_FALSE;

    HWND sHwnd;
    HWND sParentHwnd;

    sHwnd = ::GetForegroundWindow();
    while (true)
    {
        sParentHwnd = ::GetParent(sHwnd);
        if (XPR_IS_NULL(sParentHwnd))
            break;

        sHwnd = sParentHwnd;
    }

    sSetForegournd = (sHwnd == mShFileOpStruct->hwnd) || (sHwnd == m_hWnd);

    if (XPR_IS_TRUE(sSetForegournd))
    {
        SetForceForegroundWindow(mShFileOpStruct->hwnd);
    }
    else
    {
        if (XPR_IS_TRUE(gOpt->mFileOpCompleteFlash))
        {
            FLASHWINFO sFlashWInfo = {0};
            sFlashWInfo.cbSize  = sizeof(sFlashWInfo);
            sFlashWInfo.dwFlags = FLASHW_TRAY;
            sFlashWInfo.hwnd    = mShFileOpStruct->hwnd;
            sFlashWInfo.uCount  = 3;
            ::FlashWindowEx(&sFlashWInfo);
        }
    }

    return 0;
}

void FileOpThread::OnTimer(xpr_uint_t nIDEvent) 
{
    if (nIDEvent == TM_ID_THREAD_END)
    {
        DWORD sResult;
        DWORD sExitCode = -1;
        xpr_bool_t sSucceeded;

        sResult    = ::WaitForSingleObject(mStopEvent, 0);
        sSucceeded = ::GetExitCodeThread(mThread, &sExitCode);
        if (sResult == WAIT_OBJECT_0 && sExitCode == 0 && XPR_IS_TRUE(sSucceeded))
        {
            KillTimer(TM_ID_THREAD_END);

            if (XPR_IS_NOT_NULL(mNotifyHwnd) && mMsg > 0 && XPR_IS_TRUE(mSelectItem))
            {
                PasteSelItems sPasteSelItems = {0};
                sPasteSelItems.mSource           = mShFileOpStruct->pFrom;
                sPasteSelItems.mTarget           = mShFileOpStruct->pTo;
                sPasteSelItems.mHandleToMappings = (HandleToMappings *)mShFileOpStruct->hNameMappings;

                ::SendMessage(mNotifyHwnd, mMsg, (WPARAM)&sPasteSelItems, 0);
            }

            if (XPR_IS_TRUE(mUndo) && mShFileOpStruct->wFunc != FO_DELETE)
            {
                FileOpUndo aFileOpUndo;
                aFileOpUndo.addOperation(mShFileOpStruct);
            }

            if (XPR_IS_NOT_NULL(mShFileOpStruct->hNameMappings))
            {
                ::SHFreeNameMappings(mShFileOpStruct->hNameMappings);
                mShFileOpStruct->hNameMappings = XPR_NULL;
            }

            CWnd::OnTimer(nIDEvent);

            DestroyWindow();
            return;
        }
    }

    CWnd::OnTimer(nIDEvent);
}

void FileOpThread::OnClose(void) 
{
    DestroyWindow();
}

xpr_bool_t FileOpThread::DestroyWindow(void)
{
    xpr_tchar_t *sSource = (xpr_tchar_t *)mShFileOpStruct->pFrom;
    xpr_tchar_t *sTarget = (xpr_tchar_t *)mShFileOpStruct->pTo;
    XPR_SAFE_DELETE_ARRAY(sSource);
    XPR_SAFE_DELETE_ARRAY(sTarget);
    XPR_SAFE_DELETE(mShFileOpStruct);

    return CWnd::DestroyWindow();
}

void FileOpThread::PostNcDestroy(void) 
{
    delete this;
}
} // namespace fxb
