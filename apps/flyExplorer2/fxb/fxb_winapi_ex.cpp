//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "fxb_winapi_ex.h"

#include <mapi.h> // for sending mail

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static xpr_char_t THIS_FILE[] = __FILE__;
#endif

namespace fxb
{
xpr_bool_t SetForceForegroundWindow(HWND aHwnd)
{
    xpr_bool_t sResult;

    HWND aForegroundHwnd = ::GetForegroundWindow();

    ::AttachThreadInput(::GetWindowThreadProcessId(aForegroundHwnd, XPR_NULL), ::GetCurrentThreadId(), XPR_TRUE);
    sResult = ::SetForegroundWindow(aHwnd);
    ::AttachThreadInput(::GetWindowThreadProcessId(aForegroundHwnd, XPR_NULL), ::GetCurrentThreadId(), XPR_FALSE);

    return sResult;
}

void GetDesktopRect(const POINT &aPoint, RECT &aDesktopRect, xpr_bool_t aWorkArea)
{
    if (XPR_IS_TRUE(aWorkArea))
    {
        SystemParametersInfo(SPI_GETWORKAREA, 0, &aDesktopRect, 0);
    }
    else
    {
        xpr_sint_t sCxScreen = ::GetSystemMetrics(SM_CXSCREEN);
        xpr_sint_t sCyScreen = ::GetSystemMetrics(SM_CYSCREEN);

        aDesktopRect.left   = 0;
        aDesktopRect.top    = 0;
        aDesktopRect.right  = sCxScreen;
        aDesktopRect.bottom = sCyScreen;
    }

    if (!::PtInRect(&aDesktopRect, aPoint))
    {
        HMONITOR hMonitor = ::MonitorFromPoint(aPoint, MONITOR_DEFAULTTONEAREST);
        if (hMonitor)
        {
            MONITORINFO sMonitorInfo = {0};
            sMonitorInfo.cbSize = sizeof(MONITORINFO);
            if (GetMonitorInfo(hMonitor, &sMonitorInfo) == XPR_TRUE)
            {
                if (XPR_IS_TRUE(aWorkArea)) aDesktopRect = sMonitorInfo.rcWork;
                else                        aDesktopRect = sMonitorInfo.rcMonitor;
            }
        }
    }
}

void GetDesktopRect(const RECT &rc, RECT &aDesktopRect, xpr_bool_t aWorkArea)
{
    if (XPR_IS_TRUE(aWorkArea))
    {
        SystemParametersInfo(SPI_GETWORKAREA, 0, &aDesktopRect, 0);
    }
    else
    {
        xpr_sint_t sCxScreen = ::GetSystemMetrics(SM_CXSCREEN);
        xpr_sint_t sCyScreen = ::GetSystemMetrics(SM_CYSCREEN);

        aDesktopRect.left   = 0;
        aDesktopRect.top    = 0;
        aDesktopRect.right  = sCxScreen;
        aDesktopRect.bottom = sCyScreen;
    }

    HMONITOR hMonitor = ::MonitorFromRect(&rc, MONITOR_DEFAULTTONEAREST);
    if (hMonitor)
    {
        MONITORINFO sMonitorInfo = {0};
        sMonitorInfo.cbSize = sizeof(MONITORINFO);
        if (GetMonitorInfo(hMonitor, &sMonitorInfo) == XPR_TRUE)
        {
            if (XPR_IS_TRUE(aWorkArea)) aDesktopRect = sMonitorInfo.rcWork;
            else                        aDesktopRect = sMonitorInfo.rcMonitor;
        }
    }
}

void GetPrimaryDesktopRect(RECT &aDesktopRect, xpr_bool_t aWorkArea)
{
    if (XPR_IS_TRUE(aWorkArea))
    {
        SystemParametersInfo(SPI_GETWORKAREA, 0, &aDesktopRect, 0);
    }
    else
    {
        xpr_sint_t sCxScreen = ::GetSystemMetrics(SM_CXSCREEN);
        xpr_sint_t sCyScreen = ::GetSystemMetrics(SM_CYSCREEN);

        aDesktopRect.left   = 0;
        aDesktopRect.top    = 0;
        aDesktopRect.right  = sCxScreen;
        aDesktopRect.bottom = sCyScreen;
    }
}

typedef std::deque<RECT> DesktopDeque;

typedef struct DesktopEnumData
{
    DesktopDeque mDesktopDeque;
    xpr_bool_t   mWorkArea;
} DesktopEnumData;

xpr_bool_t CALLBACK MonitorEnumProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData)
{
    DesktopEnumData *sDesktopEnumData = (DesktopEnumData *)dwData;
    if (XPR_IS_NULL(sDesktopEnumData))
        return XPR_FALSE;

    MONITORINFO sMonitorInfo = {0};
    sMonitorInfo.cbSize = sizeof(sMonitorInfo);
    if (::GetMonitorInfo(hMonitor, &sMonitorInfo) == XPR_TRUE)
    {
        if (XPR_IS_TRUE(sDesktopEnumData->mWorkArea))
            sDesktopEnumData->mDesktopDeque.push_back(sMonitorInfo.rcWork);
        else
            sDesktopEnumData->mDesktopDeque.push_back(sMonitorInfo.rcMonitor);
    }

    return XPR_TRUE;
}

RECT *GetDesktopListRect(xpr_size_t *aCount, xpr_bool_t aWorkArea)
{
    if (XPR_IS_NULL(aCount))
        return XPR_NULL;

    DesktopEnumData sDesktopEnumData;
    sDesktopEnumData.mWorkArea = aWorkArea;

    HDC sDC = ::GetDC(XPR_NULL);

    ::EnumDisplayMonitors(sDC, XPR_NULL, MonitorEnumProc, (LPARAM)&sDesktopEnumData);

    ::ReleaseDC(XPR_NULL, sDC);

    *aCount = sDesktopEnumData.mDesktopDeque.size();
    if (XPR_IS_NOT_NULL(*aCount))
        return XPR_NULL;

    RECT *sRect = new RECT[*aCount];
    if (XPR_IS_NULL(sRect))
    {
        sDesktopEnumData.mDesktopDeque.clear();
        return XPR_NULL;
    }

    xpr_sint_t i;
    DesktopDeque::iterator sIterator;

    sIterator = sDesktopEnumData.mDesktopDeque.begin();
    for (i = 0; sIterator != sDesktopEnumData.mDesktopDeque.end(); ++sIterator, ++i)
        sRect[i] = *sIterator;

    sDesktopEnumData.mDesktopDeque.clear();

    return sRect;
}

xpr_bool_t SendMail(xpr_char_t **aFiles, xpr_sint_t aCount)
{
    HINSTANCE sDll = ::LoadLibrary(XPR_STRING_LITERAL("MAPI32.DLL"));
    if (sDll == XPR_NULL)
    {
        AfxMessageBox(AFX_IDP_FAILED_MAPI_LOAD);
        return XPR_FALSE;
    }
    ASSERT(sDll != XPR_NULL);

    typedef xpr_ulong_t (PASCAL *SendMailFunc)(xpr_ulong_t, xpr_ulong_t, MapiMessage*, FLAGS, xpr_ulong_t);

    SendMailFunc sSendMailFunc = (SendMailFunc)GetProcAddress(sDll, "MAPISendMail");
    if (sSendMailFunc == XPR_NULL)
    {
        AfxMessageBox(AFX_IDP_INVALID_MAPI_DLL);

        ::FreeLibrary(sDll);
        return XPR_FALSE;
    }

    // prepare the file description (for the attachment)
    MapiFileDesc *sMapiFileDesc = new MapiFileDesc[aCount];
    memset(sMapiFileDesc, 0, sizeof(MapiFileDesc) * aCount);

    xpr_sint_t i;
    for (i = 0; i < aCount; ++i)
    {
        sMapiFileDesc[i].nPosition    = (xpr_ulong_t)-1; // 0xffffffff
        sMapiFileDesc[i].lpszPathName = aFiles[i];
        sMapiFileDesc[i].lpszFileName = aFiles[i];
    }

    // prepare the message (empty with 1 attachment)
    MapiMessage sMapiMessage = {0};
    sMapiMessage.nFileCount  = aCount;
    sMapiMessage.lpFiles     = sMapiFileDesc;

    // prepare for modal dialog box
    AfxGetApp()->EnableModeless(XPR_FALSE);
    HWND sTopHwnd;
    CWnd *sParentWnd = CWnd::GetSafeOwner(XPR_NULL, &sTopHwnd);

    // some extra precautions are required to use MAPISendMail as it
    // tends to enable the parent window in between dialogs (after
    // the login dialog, but before the send note dialog).
    sParentWnd->SetCapture();
    ::SetFocus(XPR_NULL);
    sParentWnd->m_nFlags |= WF_STAYDISABLED;

    xpr_sint_t sError = sSendMailFunc(0, (xpr_ulong_t)XPR_NULL, &sMapiMessage, MAPI_LOGON_UI|MAPI_DIALOG, 0);

    if (sError != SUCCESS_SUCCESS && sError != MAPI_USER_ABORT && sError != MAPI_E_LOGIN_FAILURE)
    {
        AfxMessageBox(AFX_IDP_FAILED_MAPI_SEND);
    }

    // after returning from the MAPISendMail call, the window must
    // be re-enabled and focus returned to the frame to undo the workaround
    // done before the MAPI call.
    ::ReleaseCapture();
    sParentWnd->m_nFlags &= ~WF_STAYDISABLED;

    sParentWnd->EnableWindow(XPR_TRUE);
    ::SetActiveWindow(XPR_NULL);
    sParentWnd->SetActiveWindow();
    sParentWnd->SetFocus();
    if (sTopHwnd != XPR_NULL)
        ::EnableWindow(sTopHwnd, XPR_TRUE);
    AfxGetApp()->EnableModeless(XPR_TRUE);

    XPR_SAFE_DELETE_ARRAY(sMapiFileDesc);

    ::FreeLibrary(sDll);
    sDll = XPR_NULL;

    return XPR_TRUE;
}

void _tcstrimleft(xpr_tchar_t *aStr)
{
    if (XPR_IS_NULL(aStr))
        return;

    const xpr_tchar_t *sStr = aStr;

    while (_istspace(*sStr))
        sStr = _tcsinc(sStr);

    if (sStr != aStr)
    {
        // fix up data and length
        xpr_sint_t sDataLength = (xpr_sint_t)_tcslen(aStr) - ((xpr_sint_t)(sStr - aStr));
        memmove(aStr, sStr, (sDataLength+1)*sizeof(xpr_tchar_t));
    }
}

void _tcstrimright(xpr_tchar_t *aStr)
{
    if (XPR_IS_NULL(aStr))
        return;

    xpr_tchar_t *sStr = aStr;
    xpr_tchar_t *sLast = XPR_NULL;

    while (*sStr != '\0')
    {
        if (_istspace(*sStr))
        {
            if (sLast == XPR_NULL)
                sLast = sStr;
        }
        else
            sLast = XPR_NULL;
        sStr = _tcsinc(sStr);
    }

    if (sLast != XPR_NULL)
    {
        // truncate at trailing space start
        *sLast = '\0';
    }
}

xpr_bool_t GetFileLastWriteTime(const xpr_tchar_t *aPath, LPFILETIME aModifiedFileTime)
{
    if (XPR_IS_NULL(aPath) || XPR_IS_NULL(aModifiedFileTime))
        return XPR_FALSE;

    HANDLE sFindFile;
    WIN32_FIND_DATA sWin32FindData = {0};

    sFindFile = ::FindFirstFile(aPath, &sWin32FindData);
    if (sFindFile == INVALID_HANDLE_VALUE)
        return XPR_FALSE;

    *aModifiedFileTime = sWin32FindData.ftLastWriteTime;

    ::FindClose(sFindFile);

    return XPR_TRUE;
}

LRESULT SendNotifyCodeMessage(HWND aHwnd, LPNMHDR aNmHdr)
{
    if (XPR_IS_NULL(aHwnd) || XPR_IS_NULL(aNmHdr))
        return 0;

    return ::SendMessage(aHwnd, WM_NOTIFY, (WPARAM)aNmHdr->idFrom, (LPARAM)aNmHdr);
}

LRESULT SendNotifyCodeMessage(CWnd *aWnd, LPNMHDR aNmHdr)
{
    if (XPR_IS_NULL(aWnd) || XPR_IS_NULL(aWnd->m_hWnd) || XPR_IS_NULL(aNmHdr))
        return 0;

    return SendNotifyCodeMessage(aWnd->m_hWnd, aNmHdr);
}

void GetModulePath(xpr_tchar_t *aPath, xpr_size_t aMaxLen)
{
    if (XPR_IS_NULL(aPath) || aMaxLen <= 0)
        return;

    HMODULE sModule = ::GetModuleHandle(XPR_NULL);
    ::GetModuleFileName(sModule, aPath, (DWORD)aMaxLen);
}

void GetModulePath(std::tstring &aPath)
{
    xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
    GetModulePath(sPath, XPR_MAX_PATH);

    aPath = sPath;
}

void GetModuleDir(xpr_tchar_t *aDir, xpr_size_t aMaxLen)
{
    if (XPR_IS_NULL(aDir) || aMaxLen <= 0)
        return;

    GetModulePath(aDir, aMaxLen);

    xpr_tchar_t *sSplit = _tcsrchr(aDir, '\\');
    if (XPR_IS_NOT_NULL(sSplit))
        *sSplit = '\0';
}

void GetModuleDir(std::tstring &strDir)
{
    xpr_tchar_t sDir[XPR_MAX_PATH + 1] = {0};
    GetModuleDir(sDir, XPR_MAX_PATH);

    strDir = sDir;
}

xpr_uint_t MsgBox(const xpr_tchar_t *aFormat, ...)
{
    va_list sArgs;
    va_start(sArgs, aFormat);

    xpr_sint_t sMsgLength = _vsctprintf(aFormat, sArgs);
    xpr_tchar_t *sMsg = new xpr_tchar_t[sMsgLength + 1];

    _vstprintf(sMsg, aFormat, sArgs);

    va_end(sArgs);

    xpr_uint_t sResult = AfxMessageBox(sMsg);

    delete[] sMsg;
    sMsg = XPR_NULL;

    return sResult;
}

HRESULT StreamToFile(LPSTREAM aStream, xpr_tchar_t *aFileName, const xpr_sint_t aBlockSize)
{
    if (XPR_IS_NULL(aStream) || XPR_IS_NULL(aFileName) || aBlockSize <= 0)
        return E_FAIL;

    xpr_byte_t *sBuffer = new xpr_byte_t[aBlockSize];
    if (XPR_IS_NULL(sBuffer))
        return E_FAIL;

    xpr_ulong_t sRead = 0;
    xpr_size_t sWrite = 0;

    HRESULT sHResult = S_OK;
    FILE *sFile = _tfopen(aFileName, XPR_STRING_LITERAL("wb"));
    if (sFile != XPR_NULL)
    {
        do
        {
            sHResult = aStream->Read(sBuffer, aBlockSize, &sRead);
            if (sRead > 0)
                sWrite = fwrite(sBuffer, (xpr_size_t)sRead, 1, sFile);
        }
        while (S_OK == sHResult && sRead == (xpr_ulong_t)aBlockSize);

        fclose(sFile);
    }
    else
    {
        DWORD sLastError = GetLastError();
        if (sLastError == 0)
            sLastError = _doserrno;

        sHResult = HRESULT_FROM_WIN32(sLastError);
    }

    XPR_SAFE_DELETE_ARRAY(sBuffer);

    return sHResult;
}

HRESULT StorageToFile(LPSTORAGE aStorage, xpr_tchar_t *aFileName)
{
    if (XPR_IS_NULL(aStorage) || XPR_IS_NULL(aFileName))
        return E_FAIL;

    HRESULT sHResult;

    LPLOCKBYTES sLockBytes = XPR_NULL;
    LPSTORAGE sDestStorage = XPR_NULL;

    sHResult = ::CreateILockBytesOnHGlobal(XPR_NULL, XPR_TRUE, &sLockBytes);
    if (SUCCEEDED(sHResult) && sLockBytes)
    {
        sHResult = ::StgCreateDocfileOnILockBytes(sLockBytes, STGM_SHARE_EXCLUSIVE | STGM_CREATE | STGM_READWRITE, 0, &sDestStorage);
        if (SUCCEEDED(sHResult))
        {
            sHResult = aStorage->CopyTo(0, XPR_NULL, XPR_NULL, sDestStorage);
            if (SUCCEEDED(sHResult) && sDestStorage)
            {
                sLockBytes->Flush();
                sDestStorage->Commit(STGC_DEFAULT);

                STATSTG sStatStg;
                sLockBytes->Stat(&sStatStg,STATFLAG_NONAME);
                ASSERT(sStatStg.cbSize.HighPart == 0);
                DWORD sSize = sStatStg.cbSize.LowPart;

                HGLOBAL sStorage;
                sHResult = ::GetHGlobalFromILockBytes(sLockBytes, &sStorage);
                if (SUCCEEDED(sHResult))
                {
                    xpr_char_t *sBuffer = (xpr_char_t*)::GlobalLock(sStorage);
                    if (XPR_IS_NOT_NULL(sBuffer))
                    {
                        FILE *sFile = _tfopen(aFileName, XPR_STRING_LITERAL("wb"));
                        if (sFile != XPR_NULL)
                        {
                            fwrite(sBuffer, (xpr_size_t)sSize, 1, sFile);
                            fclose(sFile);
                        }

                        sHResult = S_OK;

                        ::GlobalUnlock(sStorage);
                    }
                    else
                    {
                        sHResult = E_FAIL;
                    }
                }
            }
        }
    }

    COM_RELEASE(sDestStorage);
    COM_RELEASE(sLockBytes);

    return sHResult;
}

xpr_bool_t IsUACRequiredWindows(void)
{
    xpr_bool_t sIsUACRequired = XPR_FALSE;

    OSVERSIONINFO sOsVersionInfo;
    sOsVersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

    if (::GetVersionEx(&sOsVersionInfo) == XPR_TRUE)
    {
        if (sOsVersionInfo.dwPlatformId == VER_PLATFORM_WIN32_NT &&
            sOsVersionInfo.dwMajorVersion >= 6 &&
            sOsVersionInfo.dwMinorVersion >= 0)
        {
            // Windows VISTA or Higher
            sIsUACRequired = XPR_TRUE;
        }
    }

    return sIsUACRequired;
}

HRESULT IsUACElevated(xpr_bool_t *aElevated)
{
    HRESULT sHResult = E_FAIL;
    HANDLE sToken = XPR_NULL;

    if (UserEnv::instance().mWinVer < UserEnv::WinVista)
        return sHResult;

    if (::OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &sToken) == XPR_FALSE)
        return sHResult;

    TOKEN_ELEVATION sTokenElevation = { 0 };
    DWORD sReturnLength = 0;
    const xpr_sint_t sTokenElevationInfo = 20;

    if (::GetTokenInformation(
        sToken,
        (TOKEN_INFORMATION_CLASS)sTokenElevationInfo,
        &sTokenElevation,
        sizeof(sTokenElevation),
        &sReturnLength) ==  XPR_TRUE)
    {
        sHResult = sTokenElevation.TokenIsElevated ? S_OK : S_FALSE;

        if (XPR_IS_NOT_NULL(aElevated))
            *aElevated = (sTokenElevation.TokenIsElevated != 0);
    }

    ::CloseHandle(sToken);

    return sHResult;
}
} // namespace fxb
