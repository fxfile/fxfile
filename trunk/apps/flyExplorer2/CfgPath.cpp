//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "CfgPath.h"

#include "fxb/fxb_ini_file_ex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define CFG_PATH_FILENAME                    XPR_STRING_LITERAL("path.conf")

static const xpr_tchar_t kPathConfPath  [] = XPR_STRING_LITERAL("%AppData%\\flyExplorer2\\")CFG_PATH_FILENAME;
static const xpr_tchar_t kDefRootDir    [] = XPR_STRING_LITERAL("%AppData%\\flyExplorer2");
static const xpr_tchar_t kConfDir       [] = XPR_STRING_LITERAL("conf");

static const xpr_tchar_t kCfgPathSection[] = XPR_STRING_LITERAL("path");
static const xpr_tchar_t kPathKey[]        = XPR_STRING_LITERAL("path");

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
}

const xpr_tchar_t *CfgPath::getRootDir(void) const
{
    return mRootDir.c_str();
}

void CfgPath::setRootDir(const xpr_tchar_t *aRootDir)
{
    if (XPR_IS_NOT_NULL(aRootDir))
        mRootDir = aRootDir;
}

xpr_bool_t CfgPath::getDir(xpr_sint_t aType, xpr_tchar_t *aDir, xpr_size_t aMaxLen) const
{
    if (XPR_IS_NULL(aDir) || aMaxLen <= 0)
        return XPR_FALSE;

    const xpr_tchar_t *sRootDir;

    sRootDir = getRootDir();
    XPR_ASSERT(sRootDir != XPR_NULL);

    std::tstring sDir;
    fxb::GetEnvRealPath(sRootDir, sDir);

    if (sDir.empty() == true)
        return XPR_FALSE;

    if (*sDir.rbegin() != XPR_STRING_LITERAL('\\'))
       sDir += XPR_STRING_LITERAL('\\');

    sDir += kConfDir;

    if (sDir.length() > aMaxLen)
        return XPR_FALSE;

    _tcscpy(aDir, sDir.c_str());

    return XPR_TRUE;
}

xpr_bool_t CfgPath::getPath(xpr_sint_t aType, xpr_tchar_t *aPath, xpr_size_t aMaxLen, const xpr_tchar_t *aRefName) const
{
    xpr_tchar_t sDir[XPR_MAX_PATH + 1] = {0};
    getDir(aType, sDir, XPR_MAX_PATH + 1);

    const xpr_tchar_t *sFileName = CfgPath::getFileName(aType);
    if (XPR_IS_NULL(sFileName))
        return XPR_FALSE;

    std::tstring sPath(sDir);
    sPath += XPR_STRING_LITERAL('\\');
    sPath += sFileName;

    if (XPR_IS_NOT_NULL(aRefName))
    {
        xpr_size_t sFind = sPath.find(XPR_STRING_LITERAL('*'));
        if (sFind != std::tstring::npos)
            sPath.replace(sFind, 1, aRefName);
    }

    if (sPath.length() > aMaxLen)
        return XPR_FALSE;

    _tcscpy(aPath, sPath.c_str());

    return XPR_TRUE;
}

xpr_bool_t CfgPath::getLoadPath(xpr_sint_t aType, xpr_tchar_t *aPath, xpr_size_t aMaxLen, const xpr_tchar_t *aRefName) const
{
    return getPath(aType, aPath, aMaxLen, aRefName);
}

xpr_bool_t CfgPath::getSavePath(xpr_sint_t aType, xpr_tchar_t *aPath, xpr_size_t aMaxLen, const xpr_tchar_t *aRefName) const
{
    if (getPath(aType, aPath, aMaxLen, aRefName) == XPR_FALSE)
        return XPR_FALSE;

    fxb::CreateDirectoryLevel(aPath, 0, XPR_FALSE);

    return XPR_TRUE;
}

xpr_bool_t CfgPath::getLoadDir(xpr_sint_t aType, xpr_tchar_t *aDir, xpr_size_t aMaxLen) const
{
    return getDir(aType, aDir, aMaxLen);
}

xpr_bool_t CfgPath::getSaveDir(xpr_sint_t aType, xpr_tchar_t *aDir, xpr_size_t aMaxLen) const
{
    if (getDir(aType, aDir, aMaxLen) == XPR_FALSE)
        return XPR_FALSE;

    if (fxb::IsExistFile(aDir) == XPR_TRUE)
        return XPR_TRUE;

    fxb::CreateDirectoryLevel(aDir);

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

xpr_bool_t CfgPath::checkChangedCfgPath(void)
{
    if (mBakOldCfgPathMap.size() != mBakNewCfgPathMap.size())
        return XPR_TRUE;

    xpr_bool_t sChanged = XPR_FALSE;

    CfgPathMap::const_iterator sIterator;
    CfgPathMap::const_iterator sFindIterator;

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

const xpr_tchar_t *CfgPath::getFileName(xpr_sint_t aType)
{
    switch (aType)
    {
    case TypeMain:       return XPR_STRING_LITERAL("main.conf");
    case TypeConfig:     return XPR_STRING_LITERAL("config.conf");
    case TypeBookmark:   return XPR_STRING_LITERAL("bookmark.conf");
    case TypeFileScrap:  return XPR_STRING_LITERAL("file_scrap.conf");
    case TypeSearchDir:  return XPR_STRING_LITERAL("search_dir.conf");
    case TypeViewSet:    return XPR_STRING_LITERAL("view_set.conf");
    case TypeDlgState:   return XPR_STRING_LITERAL("dlg_state.conf");
    case TypeAccel:      return XPR_STRING_LITERAL("accel.dat");
    case TypeCoolBar:    return XPR_STRING_LITERAL("coolbar.dat");
    case TypeToolBar:    return XPR_STRING_LITERAL("toolbar.dat");
    case TypeThumbnail:  return XPR_STRING_LITERAL("thumbnail.*");
    }

    return XPR_NULL;
}

const xpr_tchar_t *CfgPath::getDefRootDir(void)
{
    return kDefRootDir;
}

xpr_bool_t CfgPath::load(void)
{
    clear();

    std::tstring sPath;
    fxb::GetEnvRealPath(kPathConfPath, sPath);

    xpr_bool_t sResult = XPR_FALSE;

    fxb::IniFileEx sIniFile(sPath.c_str());
    if (sIniFile.readFile() == XPR_TRUE)
    {
        const xpr_tchar_t     *sValue;
        fxb::IniFile::Section *sSection;

        sSection = sIniFile.findSection(kCfgPathSection);
        if (XPR_IS_NOT_NULL(sSection))
        {
            sValue = sIniFile.getValueS(sSection, kPathKey, XPR_NULL);
            if (XPR_IS_NOT_NULL(sValue))
            {
                setRootDir(sValue);

                sResult = XPR_TRUE;
            }
        }
    }

    if (XPR_IS_FALSE(sResult))
    {
        const xpr_tchar_t *sDefRootDir = CfgPath::getDefRootDir();
        if (XPR_IS_NOT_NULL(sDefRootDir))
        {
            setRootDir(sDefRootDir);

            sResult = XPR_TRUE;
        }
    }

    return sResult;
}

xpr_bool_t CfgPath::save(void) const
{
    std::tstring sPath;
    fxb::GetEnvRealPath(kPathConfPath, sPath);

    fxb::IniFileEx sIniFile(sPath.c_str());
    sIniFile.setComment(XPR_STRING_LITERAL("flyExplorer configuration path file"));

    fxb::IniFile::Section *sSection;
    CfgPathMap::const_iterator sIterator;

    sSection = sIniFile.addSection(kCfgPathSection);
    XPR_ASSERT(sSection != XPR_NULL);

    sIniFile.setValueS(sSection, kPathKey, mRootDir.c_str());

    sIniFile.writeFile(xpr::CharSetUtf16);

    return XPR_TRUE;
}
