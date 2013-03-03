//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "fxb_env_path.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace fxb
{
EnvPath::EnvPath(void)
{
    addSpec(XPR_STRING_LITERAL("flyExplorer"),              CSIDL_FLYEXPLORER);
    addSpec(XPR_STRING_LITERAL("fxFile"),                   CSIDL_FXFILE);
    addSpec(XPR_STRING_LITERAL("fxDrive"),                  CSIDL_FXDRIVE);
    addSpec(XPR_STRING_LITERAL("WinDrive"),                 CSIDL_WINDRIVE);
    addSpec(XPR_STRING_LITERAL("Desktop"),                  CSIDL_DESKTOP);
    addSpec(XPR_STRING_LITERAL("Internet"),                 CSIDL_INTERNET);
    addSpec(XPR_STRING_LITERAL("Programs"),                 CSIDL_PROGRAMS);
    addSpec(XPR_STRING_LITERAL("Controls"),                 CSIDL_CONTROLS);
    addSpec(XPR_STRING_LITERAL("Printers"),                 CSIDL_PRINTERS);
    addSpec(XPR_STRING_LITERAL("Personal"),                 CSIDL_PERSONAL);
    addSpec(XPR_STRING_LITERAL("Favorites"),                CSIDL_FAVORITES);
    addSpec(XPR_STRING_LITERAL("Startup"),                  CSIDL_STARTUP);
    addSpec(XPR_STRING_LITERAL("Recent"),                   CSIDL_RECENT);
    addSpec(XPR_STRING_LITERAL("SendTo"),                   CSIDL_SENDTO);
    addSpec(XPR_STRING_LITERAL("BitBucket"),                CSIDL_BITBUCKET);
    addSpec(XPR_STRING_LITERAL("RecycledBin"),              CSIDL_BITBUCKET);
    addSpec(XPR_STRING_LITERAL("StartMenu"),                CSIDL_STARTMENU);
    addSpec(XPR_STRING_LITERAL("Libraries"),                CSIDL_LIBRARIES);
    addSpec(XPR_STRING_LITERAL("MyDocuments"),              CSIDL_MYDOCUMENTS);
    addSpec(XPR_STRING_LITERAL("MyMusic"),                  CSIDL_MYMUSIC);
    addSpec(XPR_STRING_LITERAL("MyVideo"),                  CSIDL_MYVIDEO);
    addSpec(XPR_STRING_LITERAL("DesktopDirectory"),         CSIDL_DESKTOPDIRECTORY);
    addSpec(XPR_STRING_LITERAL("MyComputer"),               CSIDL_DRIVES);
    addSpec(XPR_STRING_LITERAL("Drives"),                   CSIDL_DRIVES);
    addSpec(XPR_STRING_LITERAL("Network"),                  CSIDL_NETWORK);
    addSpec(XPR_STRING_LITERAL("MyNetwork"),                CSIDL_NETWORK);
    addSpec(XPR_STRING_LITERAL("NetHood"),                  CSIDL_NETHOOD);
    addSpec(XPR_STRING_LITERAL("Fonts"),                    CSIDL_FONTS);
    addSpec(XPR_STRING_LITERAL("Templates"),                CSIDL_TEMPLATES);
    addSpec(XPR_STRING_LITERAL("CommonStartMenu"),          CSIDL_COMMON_STARTMENU);
    addSpec(XPR_STRING_LITERAL("CommonPrograms"),           CSIDL_COMMON_PROGRAMS);
    addSpec(XPR_STRING_LITERAL("CommonStartup"),            CSIDL_COMMON_STARTUP);
    addSpec(XPR_STRING_LITERAL("CommonDesktopDirectory"),   CSIDL_COMMON_DESKTOPDIRECTORY);
    addSpec(XPR_STRING_LITERAL("AppData"),                  CSIDL_APPDATA);
    addSpec(XPR_STRING_LITERAL("PrintHood"),                CSIDL_PRINTHOOD);
    addSpec(XPR_STRING_LITERAL("LocalAppData"),             CSIDL_LOCAL_APPDATA);
    //addSpec(XPR_STRING_LITERAL("AltStartup"),               CSIDL_ALTSTARTUP);
    //addSpec(XPR_STRING_LITERAL("CommonAltStartup"),         CSIDL_COMMON_ALTSTARTUP);
    addSpec(XPR_STRING_LITERAL("CommonFavorites"),          CSIDL_COMMON_FAVORITES);
    addSpec(XPR_STRING_LITERAL("InternetCache"),            CSIDL_INTERNET_CACHE);
    addSpec(XPR_STRING_LITERAL("Cookies"),                  CSIDL_COOKIES);
    addSpec(XPR_STRING_LITERAL("History"),                  CSIDL_HISTORY);
    addSpec(XPR_STRING_LITERAL("CommonAppData"),            CSIDL_COMMON_APPDATA);
    addSpec(XPR_STRING_LITERAL("Windows"),                  CSIDL_WINDOWS);
    addSpec(XPR_STRING_LITERAL("System"),                   CSIDL_SYSTEM);
    addSpec(XPR_STRING_LITERAL("ProgramFiles"),             CSIDL_PROGRAM_FILES);
    addSpec(XPR_STRING_LITERAL("MyPictures"),               CSIDL_MYPICTURES);
    addSpec(XPR_STRING_LITERAL("Profile"),                  CSIDL_PROFILE);
    addSpec(XPR_STRING_LITERAL("SystemX86"),                CSIDL_SYSTEMX86);
    addSpec(XPR_STRING_LITERAL("ProgramFilesX86"),          CSIDL_PROGRAM_FILESX86);
    addSpec(XPR_STRING_LITERAL("ProgramFilesCommon"),       CSIDL_PROGRAM_FILES_COMMON);
    addSpec(XPR_STRING_LITERAL("ProgramFilesCommonX86"),    CSIDL_PROGRAM_FILES_COMMONX86);
    addSpec(XPR_STRING_LITERAL("CommonTemplates"),          CSIDL_COMMON_TEMPLATES);
    addSpec(XPR_STRING_LITERAL("CommonDocuments"),          CSIDL_COMMON_DOCUMENTS);
    addSpec(XPR_STRING_LITERAL("CommonAdministrators"),     CSIDL_COMMON_ADMINTOOLS);
    addSpec(XPR_STRING_LITERAL("Administrators"),           CSIDL_ADMINTOOLS);
    addSpec(XPR_STRING_LITERAL("Connections"),              CSIDL_CONNECTIONS);
    addSpec(XPR_STRING_LITERAL("CommonMusic"),              CSIDL_COMMON_MUSIC);
    addSpec(XPR_STRING_LITERAL("CommonPictures"),           CSIDL_COMMON_PICTURES);
    addSpec(XPR_STRING_LITERAL("CommonVideo"),              CSIDL_COMMON_VIDEO);
    addSpec(XPR_STRING_LITERAL("Resources"),                CSIDL_RESOURCES);
    //addSpec(XPR_STRING_LITERAL("ResourcesLocalized"),       CSIDL_RESOURCES_LOCALIZED);
    //addSpec(XPR_STRING_LITERAL("CommonOEMLinks"),           CSIDL_COMMON_OEM_LINKS);
    addSpec(XPR_STRING_LITERAL("CDBurnArea"),               CSIDL_CDBURN_AREA);
    //addSpec(XPR_STRING_LITERAL("ComputersNearMe"),          CSIDL_COMPUTERSNEARME);
}

EnvPath::~EnvPath(void)
{
    mSpecMap.clear();
}

void EnvPath::addSpec(const xpr_tchar_t *aSpec, xpr_uint_t aCSIDL)
{
    std::tstring sSpec;
    sSpec  = XPR_STRING_LITERAL("%");
    sSpec += aSpec;
    sSpec += XPR_STRING_LITERAL("%");

    mSpecMap[sSpec] = aCSIDL;
}

LPITEMIDLIST EnvPath::getPidl(const std::tstring &aSpec, xpr_uint_t *aCSIDL)
{
    SpecMap::iterator sIterator = mSpecMap.find(aSpec);
    if (sIterator == mSpecMap.end())
        return XPR_NULL;

    LPITEMIDLIST sFullPidl = XPR_NULL;

    switch (sIterator->second)
    {
    case CSIDL_FLYEXPLORER:
    case CSIDL_FXFILE:
        {
            xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
            GetModuleDir(sPath, XPR_MAX_PATH);

            sFullPidl = SHGetPidlFromPath(sPath);
            break;
        }

    case CSIDL_FXDRIVE:
        {
            xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
            GetModuleDir(sPath, XPR_MAX_PATH);
            sPath[3] = XPR_STRING_LITERAL('\0');

            sFullPidl = SHGetPidlFromPath(sPath);
            break;
        }

    case CSIDL_WINDRIVE:
        {
            xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
            ::SHGetSpecialFolderPath(XPR_NULL, sPath, CSIDL_WINDOWS, XPR_FALSE);
            sPath[3] = XPR_STRING_LITERAL('\0');

            sFullPidl = SHGetPidlFromPath(sPath);
            break;
        }

    case CSIDL_LIBRARIES:
        {
            LPSHELLFOLDER sShellFolder = XPR_NULL;
            HRESULT sHResult = ::SHGetDesktopFolder(&sShellFolder);
            if (SUCCEEDED(sHResult) && XPR_IS_NOT_NULL(sShellFolder))
            {
                sHResult = sShellFolder->ParseDisplayName(
                    XPR_NULL,
                    XPR_NULL,
                    XPR_WIDE_STRING_LITERAL("::{031E4825-7B94-4DC3-B131-E946B44C8DD5}"),
                    XPR_NULL,
                    &sFullPidl,
                    XPR_NULL);
            }

            if (FAILED(sHResult))
                COM_FREE(sFullPidl);

            COM_RELEASE(sShellFolder);
            break;
        }

    case CSIDL_MYDOCUMENTS:
        {
            LPSHELLFOLDER sShellFolder = XPR_NULL;
            HRESULT sHResult = ::SHGetDesktopFolder(&sShellFolder);
            if (SUCCEEDED(sHResult) && XPR_IS_NOT_NULL(sShellFolder))
            {
                sHResult = sShellFolder->ParseDisplayName(
                    XPR_NULL,
                    XPR_NULL,
                    XPR_WIDE_STRING_LITERAL("::{450d8fba-ad25-11d0-98a8-0800361b1103}"),
                    XPR_NULL,
                    &sFullPidl,
                    XPR_NULL);
            }

            if (FAILED(sHResult))
                COM_FREE(sFullPidl);

            COM_RELEASE(sShellFolder);
            break;
        }

    default:
        {
            HRESULT sHResult = ::SHGetSpecialFolderLocation(XPR_NULL, sIterator->second, &sFullPidl);
            if (FAILED(sHResult))
                COM_FREE(sFullPidl);
            break;
        }
    }

    if (XPR_IS_NOT_NULL(sFullPidl))
    {
        if (XPR_IS_NOT_NULL(aCSIDL))
            *aCSIDL = sIterator->second;
    }

    return sFullPidl;
}

xpr_bool_t EnvPath::getPidl(const std::tstring &aSpec, LPITEMIDLIST *aFullPidl, xpr_uint_t *aCSIDL)
{
    if (XPR_IS_NULL(aFullPidl))
        return XPR_FALSE;

    *aFullPidl = getPidl(aSpec, aCSIDL);

    return (*aFullPidl != XPR_NULL) ? XPR_TRUE : XPR_FALSE;
}

xpr_bool_t EnvPath::getPath(const std::tstring &aSpec, std::tstring &aPath, xpr_uint_t *aCSIDL)
{
    aPath.clear();

    SpecMap::iterator sIterator = mSpecMap.find(aSpec);
    if (sIterator == mSpecMap.end())
        return XPR_FALSE;

    switch (sIterator->second)
    {
    case CSIDL_FLYEXPLORER:
    case CSIDL_FXFILE:
        {
            GetModuleDir(aPath);
            break;
        }

    case CSIDL_FXDRIVE:
        {
            GetModuleDir(aPath);
            aPath.erase(3);
            break;
        }

    case CSIDL_WINDRIVE:
        {
            xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
            ::SHGetSpecialFolderPath(XPR_NULL, sPath, CSIDL_WINDOWS, XPR_FALSE);
            sPath[3] = XPR_STRING_LITERAL('\0');

            aPath = sPath;
            break;
        }

    case CSIDL_MYDOCUMENTS:
        {
            xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
            ::SHGetSpecialFolderPath(XPR_NULL, sPath, CSIDL_PERSONAL, XPR_FALSE);

            aPath = sPath;
            break;
        }

    default:
        {
            xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
            ::SHGetSpecialFolderPath(XPR_NULL, sPath, sIterator->second, XPR_FALSE);

            aPath = sPath;
            break;
        }
    }

    xpr_bool_t sResult = (aPath.empty() == true) ? XPR_FALSE : XPR_TRUE;
    if (XPR_IS_TRUE(sResult))
    {
        if (XPR_IS_NOT_NULL(aCSIDL))
            *aCSIDL = sIterator->second;
    }

    return sResult;
}
} // namespace fxb
