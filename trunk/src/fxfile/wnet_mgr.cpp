//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "wnet_mgr.h"

#include <afxsock.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace fxfile
{
WnetMgr::WnetMgr(void)
{
    mSockInit = AfxSocketInit();

    if (XPR_IS_FALSE(mSockInit))
    {
        AfxGetMainWnd()->MessageBox(XPR_STRING_LITERAL("socket creation failed!!!"), XPR_NULL, MB_OK | MB_ICONWARNING);
    }
}

WnetMgr::~WnetMgr(void)
{
    if (XPR_IS_TRUE(mSockInit))
    {
        AfxSocketTerm();
    }
}

static xpr_bool_t WINAPI EnumerateFunc(LPNETRESOURCE lpnr2, LPNETRESOURCE aNetResource, in_addr *pin_addr2)
{
    DWORD sWNetResult, sWNetEnumResult;
    HANDLE sWNetEnum;
    DWORD sBufferSize = 16384;
    DWORD i, sEntries = -1;
    LPNETRESOURCE sLocalNetResource;
    xpr_tchar_t sFullName[XPR_MAX_URL_LENGTH + 1] = {0};
    xpr_char_t sAnsiFullName[XPR_MAX_URL_LENGTH + 1] = {0};
    xpr_char_t sHostName[XPR_MAX_URL_LENGTH + 1] = {0};
    xpr_size_t sInputBytes;
    xpr_size_t sOutputBytes;

    sWNetResult = WNetOpenEnum(RESOURCE_GLOBALNET, RESOURCETYPE_ANY, 0, lpnr2, &sWNetEnum);
    if (sWNetResult != NO_ERROR)
        return XPR_FALSE;

    sLocalNetResource = (LPNETRESOURCE)GlobalAlloc(GPTR, sBufferSize);
    if (sLocalNetResource == XPR_NULL) 
        return XPR_FALSE;

    xpr_bool_t sResult = XPR_FALSE;

    do
    {  
        ZeroMemory(sLocalNetResource, sBufferSize);

        sWNetEnumResult = WNetEnumResource(sWNetEnum, &sEntries, sLocalNetResource, &sBufferSize);
        if (sWNetEnumResult == NO_ERROR)
        {
            for (i = 0; i < sEntries; ++i)
            {
                // sLocalNetResource[i]
                _tcscpy(sFullName, sLocalNetResource[i].lpRemoteName);
                if (_tcscmp(sFullName, XPR_STRING_LITERAL("\\\\")) == 0)
                {
                    xpr_sint_t sLen = (xpr_sint_t)_tcslen(sFullName) - 2;
                    memmove(sFullName, sFullName+2, sLen*sizeof(xpr_tchar_t));
                    sFullName[sLen] = '\0';
                }

                sInputBytes = _tcslen(sFullName) * sizeof(xpr_tchar_t);
                sOutputBytes = XPR_MAX_URL_LENGTH * sizeof(xpr_tchar_t);
                XPR_TCS_TO_MBS(sFullName, &sInputBytes, sAnsiFullName, &sOutputBytes);
                sAnsiFullName[sOutputBytes / sizeof(xpr_char_t)] = 0;

                gethostname(sHostName, (xpr_sint_t)strlen(sHostName));

                HOSTENT *sHostEnt = gethostbyname(sAnsiFullName);
                if (XPR_IS_NOT_NULL(sHostEnt))
                {
                    struct in_addr *sInAddr = (struct in_addr *)sHostEnt->h_addr_list[0];
                    if (sInAddr->S_un.S_un_b.s_b1 == pin_addr2->S_un.S_un_b.s_b1 &&
                        sInAddr->S_un.S_un_b.s_b2 == pin_addr2->S_un.S_un_b.s_b2 &&
                        sInAddr->S_un.S_un_b.s_b3 == pin_addr2->S_un.S_un_b.s_b3 &&
                        sInAddr->S_un.S_un_b.s_b4 == pin_addr2->S_un.S_un_b.s_b4)
                    {
                        _tcscpy(aNetResource->lpRemoteName, sLocalNetResource[i].lpRemoteName);
                        sResult = XPR_TRUE;
                        goto ENUM_END;
                    }
                }

                if (RESOURCEUSAGE_CONTAINER == (sLocalNetResource[i].dwUsage & RESOURCEUSAGE_CONTAINER))
                {
                    if (EnumerateFunc(&sLocalNetResource[i], aNetResource, pin_addr2))
                        goto ENUM_END;
                }
            }
        }
        else if (sWNetEnumResult != ERROR_NO_MORE_ITEMS)
        {
            break;
        }
    } while (sWNetEnumResult != ERROR_NO_MORE_ITEMS);

ENUM_END:
    GlobalFree((HGLOBAL)sLocalNetResource);

    sWNetResult = WNetCloseEnum(sWNetEnum);
    if (sWNetResult != NO_ERROR)
        return XPR_FALSE;

    return sResult;
}

xpr_bool_t WnetMgr::checkServer(const xpr_tchar_t *aServer)
{
    if (XPR_IS_NULL(aServer))
        return XPR_FALSE;

    xpr_tchar_t sServer[XPR_MAX_URL_LENGTH + 1] = {0};
    _tcscpy(sServer, aServer);

    NETRESOURCE sNetResource = {0};
    sNetResource.dwScope       = RESOURCE_GLOBALNET;
    sNetResource.dwType        = RESOURCETYPE_ANY;
    //sNetResource.dwDisplayType = RESOURCEDISPLAYTYPE_SERVER;
    //sNetResource.dwUsage       = RESOURCEUSAGE_CONTAINER;
    sNetResource.lpRemoteName  = sServer;

    DWORD sBufferLen = 16384; // 16kb
    xpr_byte_t *sBuffer = new xpr_byte_t[sBufferLen];
    if (sBuffer == XPR_NULL) 
        return XPR_FALSE;

    DWORD sWNetResult = NO_ERROR;

    try
    {
        xpr_tchar_t *sSystem = XPR_NULL;
        while ((sWNetResult = WNetGetResourceInformation(&sNetResource, sBuffer, &sBufferLen, &sSystem)) == ERROR_MORE_DATA)
        {
            XPR_SAFE_DELETE_ARRAY(sBuffer);
            sBuffer = new xpr_byte_t[sBufferLen];
        }
    }
    catch (...)
    {
    }

    XPR_SAFE_DELETE_ARRAY(sBuffer);

    return (sWNetResult == NO_ERROR);
}

xpr_bool_t WnetMgr::getNetDesc(const xpr_tchar_t *aServer, xpr_tchar_t *aDesc)
{
    if (XPR_IS_NULL(aServer) || XPR_IS_NULL(aDesc))
        return XPR_FALSE;

    xpr_bool_t sResult = XPR_FALSE;

    if (aServer[0] == XPR_STRING_LITERAL('\\') && aServer[1] == XPR_STRING_LITERAL('\\'))
    {
        NETRESOURCE sNetResource = {0};
        sNetResource.dwScope       = RESOURCE_GLOBALNET;
        sNetResource.dwType        = RESOURCETYPE_ANY;
        sNetResource.dwDisplayType = RESOURCEDISPLAYTYPE_SERVER;
        sNetResource.dwUsage       = RESOURCEUSAGE_CONTAINER;
        sNetResource.lpRemoteName  = (xpr_tchar_t *)aServer;

        DWORD sBufferLen = sizeof(NETRESOURCE);
        xpr_byte_t *sBuffer = new xpr_byte_t[sBufferLen];

        DWORD sWNetResult = NO_ERROR;

        try
        {
            xpr_tchar_t *sSystem = XPR_NULL;
            while ((sWNetResult = WNetGetResourceInformation(&sNetResource, sBuffer, &sBufferLen, &sSystem)) == ERROR_MORE_DATA)
            {
                XPR_SAFE_DELETE_ARRAY(sBuffer);
                sBuffer = new xpr_byte_t[sBufferLen];
            }
        }
        catch (...)
        {
        }

        if (sWNetResult == NO_ERROR)
        {
            if (sNetResource.lpComment)
            {
                _tcscpy(aDesc, sNetResource.lpComment);
                sResult = XPR_TRUE;
            }
        }

        XPR_SAFE_DELETE_ARRAY(sBuffer);
    }

    return sResult;
}

DWORD WnetMgr::connectNetDrive(HWND aHwnd)
{
    xpr::tstring sFullPath;
    getNetFullPath(sFullPath);

    return ::WNetConnectionDialog(aHwnd, RESOURCETYPE_DISK);
}

DWORD WnetMgr::disconnectNetDrive(HWND aHwnd)
{
    return ::WNetDisconnectDialog(aHwnd, RESOURCETYPE_DISK);
}

xpr_bool_t WnetMgr::isConnectedServer(const xpr::tstring &aNetPath)
{
    if (aNetPath.length() <= 2)
        return XPR_FALSE;

    DWORD sWNetResult, sWNetEnumResult;
    HANDLE sWNetEnum;
    DWORD sBufferSize = 16384; // 16kb
    DWORD sEntries = -1;
    LPNETRESOURCE sLocalNetResource;
    DWORD i;

    sWNetResult = ::WNetOpenEnum(RESOURCE_CONNECTED, RESOURCETYPE_ANY, RESOURCEUSAGE_CONNECTABLE, XPR_NULL, &sWNetEnum);
    if (sWNetResult != NO_ERROR)
        return XPR_FALSE;

    sLocalNetResource = (LPNETRESOURCE)::GlobalAlloc(GPTR, sBufferSize);
    if (sLocalNetResource == XPR_NULL)
    {
        ::WNetCloseEnum(sWNetEnum);
        return XPR_FALSE;
    }

    xpr_bool_t sResult = XPR_FALSE;

    xpr::tstring sServer = aNetPath;
    xpr_size_t nFind = sServer.find(XPR_STRING_LITERAL('\\'), 2);
    if (nFind != xpr::tstring::npos)
        sServer.erase(nFind);

    do
    {  
        memset(sLocalNetResource, 0, sBufferSize);

        sWNetEnumResult = ::WNetEnumResource(sWNetEnum, &sEntries, sLocalNetResource, &sBufferSize);
        if (sWNetEnumResult == NO_ERROR)
        {
            for (i = 0; i < sEntries; ++i)
            {
                if (_tcsnicmp(sServer.c_str(), sLocalNetResource[i].lpRemoteName, sServer.length()) == 0)
                {
                    sResult = XPR_TRUE;
                    break;
                }
            }
        }
        else if (sWNetEnumResult != ERROR_NO_MORE_ITEMS)
        {
            break;
        }

    } while (sWNetEnumResult != ERROR_NO_MORE_ITEMS);

    ::GlobalFree((HGLOBAL)sLocalNetResource);

    ::WNetCloseEnum(sWNetEnum);

    return sResult;
}

void WnetMgr::getNetFullPath(xpr::tstring &aFullPath)
{
    aFullPath.clear();

    xpr::tstring sPath(XPR_STRING_LITERAL("\\\\127.0.0.1"));
    LPITEMIDLIST sFullPidl = fxfile::base::Pidl::create(sPath.c_str());
    if (XPR_IS_NOT_NULL(sFullPidl))
        return;

    GetDispFullPath(sFullPidl, aFullPath);

    xpr_size_t sFind = aFullPath.rfind(XPR_STRING_LITERAL('\\'));
    if (sFind != xpr::tstring::npos)
        aFullPath.erase(sFind);

    COM_FREE(sFullPidl);
}
} // namespace fxfile
