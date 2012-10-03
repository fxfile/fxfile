//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "CfgPath.h"

#include "fxb/fxb_ini_file.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static const xpr_tchar_t kCfgPathEntry[] = XPR_STRING_LITERAL("CfgPath");

CfgPath::CfgPath(void)
{
}

CfgPath::~CfgPath(void)
{
    clear();

    mBakOldCfgPathMap.clear();
    mBakNewCfgPathMap.clear();
}

void CfgPath::clear(void)
{
    mCfgPathMap.clear();
}

xpr_bool_t CfgPath::getDir(xpr_sint_t aType, xpr_tchar_t *aDir, xpr_size_t aMaxLen)
{
    if (XPR_IS_NULL(aDir) || aMaxLen <= 0)
        return XPR_FALSE;

    std::tstring sDir;
    if (getDir(aType, sDir) == XPR_FALSE)
        return XPR_FALSE;

    _tcscpy(aDir, sDir.c_str());

    return XPR_TRUE;
}

xpr_bool_t CfgPath::getDir(xpr_sint_t aType, std::tstring &aDir)
{
    std::tstring sCfgPath;
    if (getCfgPath(aType, sCfgPath) == XPR_FALSE)
        return XPR_FALSE;

    fxb::GetEnvRealPath(sCfgPath, aDir);

    xpr_size_t sLen = aDir.length();
    if (aDir[sLen - 1] == XPR_STRING_LITERAL('\\'))
        aDir.erase(sLen - 1);

    return aDir.empty() ? XPR_FALSE : XPR_TRUE;
}

xpr_bool_t CfgPath::getPath(xpr_sint_t aType, xpr_tchar_t *aPath, xpr_size_t aMaxLen, const xpr_tchar_t *aRefName)
{
    if (XPR_IS_NULL(aPath) || aMaxLen <= 0)
        return XPR_FALSE;

    std::tstring sPath;
    if (getDir(aType, sPath) == XPR_FALSE)
        return XPR_FALSE;

    const xpr_tchar_t *sFileName = CfgPath::getFileName(aType);
    if (XPR_IS_NOT_NULL(sFileName))
    {
        sPath += XPR_STRING_LITERAL('\\');
        sPath += sFileName;
    }

    if (XPR_IS_NOT_NULL(aRefName))
    {
        xpr_size_t sFind = sPath.find(XPR_STRING_LITERAL('*'));
        if (sFind != std::tstring::npos)
            sPath.replace(sFind, 1, aRefName);
    }

    _tcscpy(aPath, sPath.c_str());

    return XPR_TRUE;
}

xpr_bool_t CfgPath::getLoadPath(xpr_sint_t aType, xpr_tchar_t *aPath, xpr_size_t aMaxLen, const xpr_tchar_t *aRefName)
{
    return getPath(aType, aPath, aMaxLen, aRefName);
}

xpr_bool_t CfgPath::getSavePath(xpr_sint_t aType, xpr_tchar_t *aPath, xpr_size_t aMaxLen, const xpr_tchar_t *aRefName)
{
    if (getPath(aType, aPath, aMaxLen, aRefName) == XPR_FALSE)
        return XPR_FALSE;

    fxb::CreateDirectoryLevel(aPath, 0, XPR_FALSE);

    return XPR_TRUE;
}

xpr_bool_t CfgPath::getLoadDir(xpr_sint_t aType, xpr_tchar_t *aDir, xpr_size_t aMaxLen)
{
    return getDir(aType, aDir, aMaxLen);
}

xpr_bool_t CfgPath::getSaveDir(xpr_sint_t aType, xpr_tchar_t *aDir, xpr_size_t aMaxLen)
{
    if (getDir(aType, aDir, aMaxLen) == XPR_FALSE)
        return XPR_FALSE;

    fxb::CreateDirectoryLevel(aDir);

    return XPR_TRUE;
}

xpr_bool_t CfgPath::getLoadDir(xpr_sint_t aType, std::tstring &aDir)
{
    return getDir(aType, aDir);
}

xpr_bool_t CfgPath::getSaveDir(xpr_sint_t aType, std::tstring &aDir)
{
    if (getDir(aType, aDir) == XPR_FALSE)
        return XPR_FALSE;

    if (fxb::IsExistFile(aDir) == XPR_TRUE)
        return XPR_TRUE;

    fxb::CreateDirectoryLevel(aDir.c_str());

    return XPR_TRUE;
}

xpr_bool_t CfgPath::getCfgPath(xpr_sint_t aType, std::tstring &aCfgPath)
{
    xpr_bool_t sTypeAll = isTypeAll();

    CfgPathMap::iterator sIterator = mCfgPathMap.find(XPR_IS_TRUE(sTypeAll) ? TypeAll : aType);
    if (sIterator == mCfgPathMap.end())
        return XPR_FALSE;

    aCfgPath = sIterator->second;

    if (XPR_IS_TRUE(sTypeAll))
    {
        const xpr_tchar_t *sDefSubPath = CfgPath::getDefSubPath(aType);
        if (XPR_IS_NOT_NULL(sDefSubPath))
        {
            if (aCfgPath[aCfgPath.length()-1] != XPR_STRING_LITERAL('\\'))
                aCfgPath += XPR_STRING_LITERAL('\\');

            aCfgPath += sDefSubPath;
        }
    }

    return aCfgPath.empty() ? XPR_FALSE : XPR_TRUE;
}

xpr_bool_t CfgPath::setCfgPath(xpr_sint_t aType, const xpr_tchar_t *aPath)
{
    if (XPR_IS_NULL(aPath))
        return XPR_FALSE;

    mCfgPathMap[aType] = aPath;

    return XPR_TRUE;
}

xpr_bool_t CfgPath::setBackup(xpr_bool_t aOldBackup)
{
    CfgPathMap *sCfgPathMap = &mBakOldCfgPathMap;
    if (XPR_IS_FALSE(aOldBackup))
        sCfgPathMap = &mBakNewCfgPathMap;

    sCfgPathMap->clear();

    xpr_sint_t sType;
    xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};

    for (sType = TypeBegin; sType < TypeEnd; ++sType)
    {
        sPath[0] = XPR_STRING_LITERAL('\0');
        getPath(sType, sPath, XPR_MAX_PATH);

        (*sCfgPathMap)[sType] = sPath;
    }

    return XPR_TRUE;
}

xpr_bool_t CfgPath::isChangedCfgPath(void)
{
    if (mBakOldCfgPathMap.size() != mBakNewCfgPathMap.size())
        return XPR_TRUE;

    xpr_bool_t sChanged = XPR_FALSE;

    CfgPathMap::iterator sIterator;
    CfgPathMap::iterator sFindIterator;

    sIterator = mBakOldCfgPathMap.begin();
    for (; sIterator != mBakOldCfgPathMap.end(); ++sIterator)
    {
        sFindIterator = mBakNewCfgPathMap.find(sIterator->first);
        if (sFindIterator == mBakNewCfgPathMap.end())
        {
            sChanged = XPR_TRUE;
            continue;
        }

        if (_tcsicmp(sIterator->second.c_str(), sFindIterator->second.c_str()) != 0)
        {
            sChanged = XPR_TRUE;
            continue;
        }

        mBakNewCfgPathMap.erase(sFindIterator);
    }

    sIterator = mBakNewCfgPathMap.begin();
    while (sIterator != mBakNewCfgPathMap.end())
    {
        sFindIterator = mBakOldCfgPathMap.find(sIterator->first);
        if (sFindIterator == mBakOldCfgPathMap.end())
        {
            sChanged = XPR_TRUE;
            sIterator++;
            continue;
        }

        if (_tcsicmp(sIterator->second.c_str(), sFindIterator->second.c_str()) != 0)
        {
            sChanged = XPR_TRUE;
            sIterator++;
            continue;
        }

        sIterator = mBakOldCfgPathMap.erase(sIterator);
    }

    return sChanged;
}

void CfgPath::moveToNewCfgPath(void)
{
    CfgPathMap::iterator sIterator;
    CfgPathMap::iterator sFindIterator;
    xpr_tchar_t sOldPath[XPR_MAX_PATH + 1] = {0};
    xpr_tchar_t sNewPath[XPR_MAX_PATH + 1] = {0};

    sIterator = mBakOldCfgPathMap.begin();
    for (; sIterator != mBakOldCfgPathMap.end(); ++sIterator)
    {
        sFindIterator = mBakNewCfgPathMap.find(sIterator->first);
        if (sFindIterator == mBakNewCfgPathMap.end())
            continue;

        _tcscpy(sOldPath, sIterator->second.c_str());
        _tcscpy(sNewPath, sFindIterator->second.c_str());

        sOldPath[_tcslen(sOldPath) + 1] = XPR_STRING_LITERAL('\0');
        sNewPath[_tcslen(sNewPath) + 1] = XPR_STRING_LITERAL('\0');

        const xpr_tchar_t *sFind = _tcsrchr(sOldPath, XPR_STRING_LITERAL('*'));

        if (fxb::IsFileSystemFolder(sOldPath) == XPR_FALSE || XPR_IS_NOT_NULL(sFind))
        {
            xpr_tchar_t *sSplit = (xpr_tchar_t *)_tcsrchr(sNewPath, XPR_STRING_LITERAL('\\'));
            if (XPR_IS_NOT_NULL(sSplit))
            {
                *sSplit = XPR_STRING_LITERAL('\0');

                if (fxb::IsExistFile(sNewPath) == XPR_FALSE)
                    fxb::CreateDirectoryLevel(sNewPath);

                if (XPR_IS_NULL(sFind))
                {
                    *sSplit = XPR_STRING_LITERAL('\\');
                    sNewPath[_tcslen(sNewPath) + 1] = XPR_STRING_LITERAL('\0');
                }
            }
        }

        fxb::SHSilentMoveFile(sOldPath, sNewPath);
    }

    mBakOldCfgPathMap.clear();
    mBakNewCfgPathMap.clear();
}

xpr_bool_t CfgPath::isTypeAll(void)
{
    if (mCfgPathMap.size() != 1)
        return XPR_FALSE;

    return (mCfgPathMap.begin()->first == TypeAll);
}

const xpr_tchar_t *CfgPath::getDispName(xpr_sint_t aType)
{
    switch (aType)
    {
    case TypeMain:      return theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.advanced.cfg_path.type.main"));
    case TypeConfig:    return theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.advanced.cfg_path.type.config"));
    case TypeBookmark:  return theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.advanced.cfg_path.type.bookmark"));
    case TypeFileScrap: return theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.advanced.cfg_path.type.file_scrap"));
    case TypeFileAss:   return theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.advanced.cfg_path.type.file_ass"));
    case TypeFilter:    return theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.advanced.cfg_path.type.filter"));
    case TypeSearchDir: return theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.advanced.cfg_path.type.search_dir"));
    case TypeSizeFmt:   return theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.advanced.cfg_path.type.size_format"));
    case TypeViewSet:   return theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.advanced.cfg_path.type.view_set"));
    case TypeDlgState:  return theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.advanced.cfg_path.type.dialog"));
    case TypeAccel:     return theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.advanced.cfg_path.type.accel"));
    case TypeCoolBar:   return theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.advanced.cfg_path.type.coolbar"));
    case TypeToolBar:   return theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.advanced.cfg_path.type.toolbar"));
    case TypeBarState:  return theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.advanced.cfg_path.type.bar_state"));
    case TypeThumbnail: return theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.advanced.cfg_path.type.thumbnail"));
    case TypeRecent:    return theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.advanced.cfg_path.type.recent"));
    case TypeHistory:   return theApp.loadString(XPR_STRING_LITERAL("popup.cfg.body.advanced.cfg_path.type.history"));
    }

    return XPR_NULL;
}

const xpr_tchar_t *CfgPath::getFileName(xpr_sint_t aType)
{
    switch (aType)
    {
    case TypeMain:      return XPR_STRING_LITERAL("main.ini");
    case TypeConfig:    return XPR_STRING_LITERAL("config.ini");
    case TypeBookmark:  return XPR_STRING_LITERAL("bookmark.ini");
    case TypeFileScrap: return XPR_STRING_LITERAL("filescrap.ini");
    case TypeFileAss:   return XPR_STRING_LITERAL("fileass.ini");
    case TypeFilter:    return XPR_STRING_LITERAL("filter.ini");
    case TypeSearchDir: return XPR_STRING_LITERAL("searchdir.ini");
    case TypeSizeFmt:   return XPR_STRING_LITERAL("sizefmt.ini");
    case TypeViewSet:   return XPR_STRING_LITERAL("*.ini");
    case TypeDlgState:  return XPR_STRING_LITERAL("dlgstate.ini");
    case TypeAccel:     return XPR_STRING_LITERAL("accel.dat");
    case TypeCoolBar:   return XPR_STRING_LITERAL("coolbar.dat");
    case TypeToolBar:   return XPR_STRING_LITERAL("toolbar.dat");
    case TypeBarState:  return XPR_STRING_LITERAL("barstate.ini");
    case TypeThumbnail: return XPR_STRING_LITERAL("thumbnail.*");
    case TypeRecent:    return XPR_STRING_LITERAL("recent.ini");
    case TypeHistory:   return XPR_STRING_LITERAL("history*.ini");
    }

    return XPR_NULL;
}

const xpr_tchar_t *CfgPath::getDefPath(xpr_sint_t aType)
{
    return XPR_STRING_LITERAL("%AppData%\\flyExplorer");
}

const xpr_tchar_t *CfgPath::getDefSubPath(xpr_sint_t aType)
{
    switch (aType)
    {
    case TypeViewSet:  return XPR_STRING_LITERAL("Settings\\ViewSet");
    }

    return XPR_NULL;
}

const xpr_tchar_t *CfgPath::getEntry(xpr_sint_t aType)
{
    switch (aType)
    {
    case TypeAll:       return XPR_STRING_LITERAL("all");
    case TypeMain:      return XPR_STRING_LITERAL("main");
    case TypeConfig:    return XPR_STRING_LITERAL("config");
    case TypeBookmark:  return XPR_STRING_LITERAL("bookmark");
    case TypeFileScrap: return XPR_STRING_LITERAL("filescrap");
    case TypeFileAss:   return XPR_STRING_LITERAL("fileass");
    case TypeFilter:    return XPR_STRING_LITERAL("filter");
    case TypeSearchDir: return XPR_STRING_LITERAL("searchdir");
    case TypeSizeFmt:   return XPR_STRING_LITERAL("sizefmt");
    case TypeViewSet:   return XPR_STRING_LITERAL("viewset");
    case TypeDlgState:  return XPR_STRING_LITERAL("dlgstate");
    case TypeAccel:     return XPR_STRING_LITERAL("accel");
    case TypeCoolBar:   return XPR_STRING_LITERAL("coolbar");
    case TypeToolBar:   return XPR_STRING_LITERAL("toolbar");
    case TypeBarState:  return XPR_STRING_LITERAL("barstate");
    case TypeThumbnail: return XPR_STRING_LITERAL("thumbnail");
    case TypeRecent:    return XPR_STRING_LITERAL("recent");
    case TypeHistory:   return XPR_STRING_LITERAL("hitory");
    }

    return XPR_NULL;
}

xpr_bool_t CfgPath::load(void)
{
    clear();

    xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
    fxb::GetEnvPath(XPR_STRING_LITERAL("%AppData%"), sPath);
    _tcscat(sPath, XPR_STRING_LITERAL("\\flyExplorer\\cfgpath.ini"));

    fxb::IniFile sIniFile(sPath);
    if (sIniFile.readFileW() == XPR_FALSE)
    {
        const xpr_tchar_t *sDefPath = CfgPath::getDefPath(TypeAll);
        if (XPR_IS_NOT_NULL(sDefPath))
            setCfgPath(TypeAll, sDefPath);

        return XPR_TRUE;
    }

    xpr_bool_t sResult = XPR_FALSE;

    const xpr_tchar_t *sEntryAll = getEntry(TypeAll);

    xpr_size_t sKey = fxb::IniFile::InvalidIndex;
    if (XPR_IS_NOT_NULL(sEntryAll))
        sKey = sIniFile.findEntry(kCfgPathEntry, sEntryAll);

    if (sKey != fxb::IniFile::InvalidIndex)
    {
        const xpr_tchar_t *sValue = sIniFile.getValueS(kCfgPathEntry, sEntryAll);
        if (XPR_IS_NOT_NULL(sValue))
        {
            setCfgPath(TypeAll, sValue);
            sResult = XPR_TRUE;
        }
    }

    if (XPR_IS_FALSE(sResult))
    {
        xpr_sint_t sType;
        const xpr_tchar_t *sEntry;
        const xpr_tchar_t *sValue;
        const xpr_tchar_t *sDefPath;
        const xpr_tchar_t *sDefSubPath;
        std::tstring sPath;

        for (sType = TypeBegin; sType < TypeEnd; ++sType)
        {
            sEntry = getEntry(sType);
            if (XPR_IS_NULL(sEntry))
                continue;

            sDefPath = CfgPath::getDefPath(sType);
            sDefSubPath = CfgPath::getDefSubPath(sType);

            sValue = sIniFile.getValueS(kCfgPathEntry, sEntry);
            if (XPR_IS_NOT_NULL(sValue))
                sPath = sValue;
            else
            {
                sPath = sDefPath;
                if (XPR_IS_NOT_NULL(sDefSubPath))
                {
                    sPath += XPR_STRING_LITERAL('\\');
                    sPath += sDefSubPath;
                }
            }

            setCfgPath(sType, sPath.c_str());
        }
    }

    return XPR_TRUE;
}

xpr_bool_t CfgPath::save(void)
{
    xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
    fxb::GetEnvPath(XPR_STRING_LITERAL("%AppData%"), sPath);
    _tcscat(sPath, XPR_STRING_LITERAL("\\flyExplorer\\cfgpath.ini"));

    fxb::IniFile sIniFile(sPath);
    sIniFile.setComment(XPR_STRING_LITERAL("flyExplorer Configuration Path File"));

    xpr_sint_t sType;
    const xpr_tchar_t *sEntry;
    CfgPathMap::iterator sIterator;

    if (isTypeAll() == XPR_TRUE)
    {
        sType = TypeAll;

        sEntry = getEntry(sType);
        if (XPR_IS_NOT_NULL(sEntry))
        {
            sIterator = mCfgPathMap.find(sType);
            if (sIterator != mCfgPathMap.end())
            {
                sIniFile.setValueS(kCfgPathEntry, sEntry, sIterator->second.c_str());
            }
        }
    }
    else
    {
        for (sType = TypeBegin; sType < TypeEnd; ++sType)
        {
            sEntry = getEntry(sType);
            if (XPR_IS_NULL(sEntry))
                continue;

            sIterator = mCfgPathMap.find(sType);
            if (sIterator == mCfgPathMap.end())
                continue;

            sIniFile.setValueS(kCfgPathEntry, sEntry, sIterator->second.c_str());
        }
    }

    sIniFile.writeFileW();

    return XPR_TRUE;
}
