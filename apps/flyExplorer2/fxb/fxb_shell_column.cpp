//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "fxb_shell_column.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace fxb
{
ShellColumn::ShellColumn(void)
    : mEvent(XPR_NULL)
    , mFullPidl(XPR_NULL)
{
}

ShellColumn::~ShellColumn(void)
{
    {
        ColumnInfo *sColumnInfo;
        ColumnInfoMap::iterator sIterator;

        sIterator = mColumnInfoMap.begin();
        for (; sIterator != mColumnInfoMap.end(); ++sIterator)
        {
            sColumnInfo = sIterator->second;
            XPR_SAFE_DELETE(sColumnInfo);
        }

        mColumnInfoMap.clear();
    }

    Stop();

    CLOSE_HANDLE(mEvent);

    {
        AsyncInfo *sAsyncInfo;
        AsyncDeque::iterator sIterator;

        sIterator = mAsyncDeque.begin();
        for (; sIterator != mAsyncDeque.end(); ++sIterator)
        {
            sAsyncInfo = *sIterator;
            XPR_SAFE_DELETE(sAsyncInfo);
        }

        mAsyncDeque.clear();
    }
}

void ShellColumn::setBaseItem(LPITEMIDLIST aFullPidl)
{
    COM_FREE(mFullPidl);

    mFullPidl = aFullPidl;
}

void ShellColumn::loadColumn(void)
{
    if (isLoadedColumn() == XPR_TRUE)
        return;

    LPSHELLFOLDER sShellFolder;
    if (FAILED(::SHGetDesktopFolder(&sShellFolder)))
        return;

    if (XPR_IS_NOT_NULL(mFullPidl))
    {
        HRESULT sHResult;
        LPSHELLFOLDER sShellFolderForBind = XPR_NULL;
        sHResult = sShellFolder->BindToObject(mFullPidl, XPR_NULL, IID_IShellFolder, (LPVOID *)&sShellFolderForBind);
        if (SUCCEEDED(sHResult))
        {
            COM_RELEASE(sShellFolder);
            sShellFolder = sShellFolderForBind;
        }
    }

    LPSHELLFOLDER2 sShellFolder2 = XPR_NULL;
    if (SUCCEEDED(sShellFolder->QueryInterface(IID_IShellFolder2, (LPVOID *)&sShellFolder2)))
    {
        xpr_sint_t sColumnIndex;
        ColumnId sColumnId;
        SHCOLUMNID sShColumnId;
        SHELLDETAILS sShellDetails;
        xpr_tchar_t sName[XPR_MAX_PATH + 1] = {0};
        ColumnInfo *sColumnInfo;

        for (sColumnIndex = 0; ; ++sColumnIndex)
        {
            if (FAILED(sShellFolder2->GetDetailsOf(XPR_NULL, sColumnIndex, &sShellDetails)))
                break;

            if (FAILED(::StrRetToBuf(&sShellDetails.str, XPR_NULL, sName, XPR_MAX_PATH)))
                continue;

            if (sName[0] == XPR_STRING_LITERAL('\0'))
                continue;

            if (FAILED(sShellFolder2->MapColumnToSCID(sColumnIndex, &sShColumnId)))
                break;

            sColumnId.mFormatId   = sShColumnId.fmtid;
            sColumnId.mPropertyId = sShColumnId.pid;

            sColumnInfo = new ColumnInfo;
            sColumnInfo->mColumn     = sColumnIndex;
            sColumnInfo->mFormatId   = sColumnId.mFormatId;
            sColumnInfo->mPropertyId = sColumnId.mPropertyId;
            sColumnInfo->mWidth      = sShellDetails.cxChar;
            sColumnInfo->mAlign      = sShellDetails.fmt;
            sColumnInfo->mName       = sName;

            mColumnInfoMap[sColumnId] = sColumnInfo;
        }

        COM_RELEASE(sShellFolder2);
    }

    COM_RELEASE(sShellFolder);
}

xpr_bool_t ShellColumn::isLoadedColumn(void)
{
    return mColumnInfoMap.empty() ? XPR_FALSE : XPR_TRUE;
}

xpr_sint_t ShellColumn::getDetailColumn(const ColumnId &aColumnId)
{
    if (isLoadedColumn() == XPR_FALSE)
        loadColumn();

    ColumnInfo *sColumnInfo;
    ColumnInfoMap::iterator sIterator;

    sIterator = mColumnInfoMap.find(aColumnId);
    if (sIterator == mColumnInfoMap.end())
        return -1;

    sColumnInfo = sIterator->second;
    if (XPR_IS_NULL(sColumnInfo))
        return -1;

    return sColumnInfo->mColumn;
}

ColumnInfo *ShellColumn::getColumnInfo(const ColumnId &aColumnId)
{
    if (isLoadedColumn() == XPR_FALSE)
        loadColumn();

    ColumnInfoMap::iterator sIterator;

    sIterator = mColumnInfoMap.find(aColumnId);
    if (sIterator == mColumnInfoMap.end())
        return XPR_NULL;

    return sIterator->second;
}

xpr_sint_t ShellColumn::getAvgCharWidth(CWnd *aWnd)
{
    if (XPR_IS_NULL(aWnd))
        return 6;

    CClientDC dc(aWnd);

    CDC sMemDC;
    sMemDC.CreateCompatibleDC(&dc);

    CFont *sFont = aWnd->GetFont();
    CFont *sOldFont = sMemDC.SelectObject(sFont);

    std::tstring sText = XPR_STRING_LITERAL("ABCDEFGHIJKLMNOPQRSTUVWXYZ");

    CRect sRect(0,0,1000,100);
    sMemDC.DrawText(sText.c_str(), (xpr_sint_t)sText.length(), &sRect, DT_SINGLELINE | DT_TOP | DT_LEFT | DT_CALCRECT);

    xpr_sint_t sAvgCharWidth = sRect.Width() / (xpr_sint_t)sText.length();

    sMemDC.SelectObject(sOldFont);

    return sAvgCharWidth;
}

xpr_bool_t ShellColumn::isAsyncColumn(LPSHELLFOLDER2 aShellFolder2, const ColumnId &aColumnId)
{
    if (XPR_IS_NULL(aShellFolder2))
        return XPR_FALSE;

    xpr_bool_t sResult = XPR_FALSE;

    xpr_sint_t sColumnIndex = getDetailColumn(aColumnId);

    HRESULT sHResult;
    SHCOLSTATEF sShColStateF = 0;

    sHResult = aShellFolder2->GetDefaultColumnState(sColumnIndex, &sShColStateF);
    if (SUCCEEDED(sHResult))
    {
        if (XPR_TEST_BITS(sShColStateF, SHCOLSTATE_SLOW))
            return XPR_TRUE;
    }

    return XPR_FALSE;
}

xpr_bool_t ShellColumn::getColumnText(LPSHELLFOLDER2 aShellFolder2, LPITEMIDLIST aPidl, xpr_sint_t aColumnIndex, xpr_tchar_t *aText, xpr_sint_t aMaxLen)
{
    if (XPR_IS_NULL(aShellFolder2) || XPR_IS_NULL(aPidl) || XPR_IS_NULL(aText))
        return XPR_FALSE;

    xpr_bool_t sResult = XPR_FALSE;

    SHELLDETAILS sShellDetails = {0};
    HRESULT sHResult = aShellFolder2->GetDetailsOf(aPidl, aColumnIndex, &sShellDetails);
    if (SUCCEEDED(sHResult))
    {
        sHResult = ::StrRetToBuf(&sShellDetails.str, XPR_NULL, aText, aMaxLen);
        if (SUCCEEDED(sHResult))
            sResult = XPR_TRUE;
    }

    return sResult;
}

xpr_bool_t ShellColumn::getColumnText(LPSHELLFOLDER2 aShellFolder2, LPITEMIDLIST aPidl, const ColumnId &aColumnId, xpr_tchar_t *aText, xpr_sint_t aMaxLen)
{
    if (XPR_IS_NULL(aShellFolder2) || XPR_IS_NULL(aPidl) || XPR_IS_NULL(aText))
        return XPR_FALSE;

    xpr_sint_t sColumnIndex;
    sColumnIndex = getDetailColumn(aColumnId);

    return getColumnText(aShellFolder2, aPidl, sColumnIndex, aText, aMaxLen);
}

xpr_bool_t ShellColumn::getAsyncColumnText(AsyncInfo *aAsyncInfo)
{
    if (XPR_IS_NULL(aAsyncInfo))
        return XPR_FALSE;

    if (XPR_IS_NULL(aAsyncInfo->mHwnd) || aAsyncInfo->mMsg <= 0 || XPR_IS_NULL(aAsyncInfo->mShellFolder2) || XPR_IS_NULL(aAsyncInfo->mPidl))
        return XPR_FALSE;

    if (IsRunning() == XPR_FALSE)
    {
        SetPriority(THREAD_PRIORITY_BELOW_NORMAL);
        Start();
    }

    {
        CsLocker sLocker(mCs);
        mAsyncDeque.push_back(aAsyncInfo);

        ::SetEvent(mEvent);
    }

    return XPR_TRUE;
}

xpr_bool_t ShellColumn::OnPreEntry()
{
    CLOSE_HANDLE(mEvent);

    mEvent = ::CreateEvent(XPR_NULL, XPR_TRUE, XPR_FALSE, XPR_NULL);

    return XPR_TRUE;
}

unsigned ShellColumn::OnEntryProc()
{
    // important!!!
    ::OleInitialize(XPR_NULL);

    xpr_bool_t sResult;
    AsyncInfo *sAsyncInfo;

    DWORD sWait;
    HANDLE sEvents[2] = { mStopThread, mEvent };

    while (true)
    {
        sWait = ::WaitForMultipleObjects(2, sEvents, XPR_FALSE, INFINITE);
        if (sWait == WAIT_OBJECT_0)
            break;

        if (sWait != WAIT_OBJECT_0+1)
            continue;

        {
            CsLocker sLocker(mCs);

            if (mAsyncDeque.empty())
            {
                ::ResetEvent(mEvent);
                continue;
            }

            sAsyncInfo = mAsyncDeque.front();
            mAsyncDeque.pop_front();

            if (XPR_IS_NULL(sAsyncInfo))
                continue;
        }

        sAsyncInfo->mText[0] = XPR_STRING_LITERAL('\0');

        sResult = ShellColumn::getColumnText(
            sAsyncInfo->mShellFolder2,
            sAsyncInfo->mPidl,
            sAsyncInfo->mColumnIndex,
            sAsyncInfo->mText,
            sAsyncInfo->mMaxLen);

        if (XPR_IS_TRUE(sResult))
        {
            if (!XPR_TEST_BITS(sAsyncInfo->mFlags, FlagsEmptyNotify))
            {
                if (sAsyncInfo->mText[0] == XPR_STRING_LITERAL('\0'))
                    sResult = XPR_FALSE;
            }

            if (!XPR_TEST_BITS(sAsyncInfo->mFlags, FlagsEqualNotify))
            {
                if (XPR_IS_TRUE(sResult))
                {
                    if (sAsyncInfo->mOldText)
                    {
                        if (_tcscmp(sAsyncInfo->mOldText, sAsyncInfo->mText) == 0)
                            sResult = XPR_FALSE;
                    }
                }
            }

            if (XPR_IS_TRUE(sResult))
            {
                sResult = ::PostMessage(
                    sAsyncInfo->mHwnd,
                    sAsyncInfo->mMsg,
                    (WPARAM)sAsyncInfo,
                    (LPARAM)XPR_NULL);
            }
        }

        if (XPR_IS_FALSE(sResult))
        {
            XPR_SAFE_DELETE(sAsyncInfo);
        }
    }

    return 0;
}

void ShellColumn::clearAsyncColumn(xpr_uint_t uCode)
{
    CsLocker sLocker(mCs);

    AsyncInfo *sAsyncInfo;
    AsyncDeque::iterator sIterator;

    sIterator = mAsyncDeque.begin();
    while (sIterator != mAsyncDeque.end())
    {
        sAsyncInfo = *sIterator;
        if (XPR_IS_NOT_NULL(sAsyncInfo))
        {
            if (uCode == 0 || sAsyncInfo->mCode == uCode)
            {
                XPR_SAFE_DELETE(sAsyncInfo);

                sIterator = mAsyncDeque.erase(sIterator);
                continue;
            }
        }

        sIterator++;
    }
}
} // namespace fxb
