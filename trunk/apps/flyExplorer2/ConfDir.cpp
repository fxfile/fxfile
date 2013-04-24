//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "ConfDir.h"

#include "fxb/fxb_ini_file_ex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define CFG_PATH_FILENAME                   XPR_STRING_LITERAL(".fxfile")

static const xpr_tchar_t kFxFilePath    [] = XPR_STRING_LITERAL("%AppData%\\fxfile\\")CFG_PATH_FILENAME;

static const xpr_tchar_t kProgramConfDir[] = XPR_STRING_LITERAL("%fxFile%\\conf");
static const xpr_tchar_t kDefConfDir    [] = XPR_STRING_LITERAL("%AppData%\\fxfile\\conf");

static const xpr_tchar_t kFxFileSection [] = XPR_STRING_LITERAL(".fxfile");
static const xpr_tchar_t kConfHomeKey   [] = XPR_STRING_LITERAL("conf_home");

ConfDir::ConfDir(void)
{
}

ConfDir::~ConfDir(void)
{
    clear();
}

void ConfDir::clear(void)
{
}

const xpr_tchar_t *ConfDir::getConfDir(void) const
{
    return mConfDir.c_str();
}

const xpr_tchar_t *ConfDir::getOldConfDir(void) const
{
    return mOldConfDir.c_str();
}

void ConfDir::setConfDir(const xpr_tchar_t *aConfDir)
{
    if (XPR_IS_NOT_NULL(aConfDir))
        mConfDir = aConfDir;
}

xpr_bool_t ConfDir::getDir(const xpr_tchar_t *aConfDir, xpr_tchar_t *aDir, xpr_size_t aMaxLen) const
{
    if (XPR_IS_NULL(aDir) || aMaxLen <= 0)
        return XPR_FALSE;

    std::tstring sDir;
    fxb::GetEnvRealPath(aConfDir, sDir);

    if (sDir.empty() == true)
        return XPR_FALSE;

    if (sDir.length() > aMaxLen)
        return XPR_FALSE;

    _tcscpy(aDir, sDir.c_str());

    return XPR_TRUE;
}

xpr_bool_t ConfDir::getDir(xpr_tchar_t *aDir, xpr_size_t aMaxLen) const
{
    const xpr_tchar_t *sConfDir;

    sConfDir = getConfDir();
    XPR_ASSERT(sConfDir != XPR_NULL);

    return getDir(sConfDir, aDir, aMaxLen);
}

xpr_bool_t ConfDir::getPath(xpr_sint_t aType, const xpr_tchar_t *aDir, xpr_tchar_t *aPath, xpr_size_t aMaxLen, const xpr_tchar_t *aRefName) const
{
    const xpr_tchar_t *sFileName = ConfDir::getFileName(aType);
    if (XPR_IS_NULL(sFileName))
        return XPR_FALSE;

    std::tstring sPath(aDir);
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

xpr_bool_t ConfDir::getPath(xpr_sint_t aType, xpr_tchar_t *aPath, xpr_size_t aMaxLen, const xpr_tchar_t *aRefName) const
{
    xpr_tchar_t sDir[XPR_MAX_PATH + 1] = {0};
    getDir(sDir, XPR_MAX_PATH + 1);

    return getPath(aType, sDir, aPath, aMaxLen, aRefName);
}

xpr_bool_t ConfDir::getLoadPath(xpr_sint_t aType, xpr_tchar_t *aPath, xpr_size_t aMaxLen, const xpr_tchar_t *aRefName) const
{
    return getPath(aType, aPath, aMaxLen, aRefName);
}

xpr_bool_t ConfDir::getSavePath(xpr_sint_t aType, xpr_tchar_t *aPath, xpr_size_t aMaxLen, const xpr_tchar_t *aRefName) const
{
    if (getPath(aType, aPath, aMaxLen, aRefName) == XPR_FALSE)
        return XPR_FALSE;

    if (fxb::CreateDirectoryLevel(aPath, 0, XPR_FALSE) == XPR_FALSE)
        return XPR_FALSE;

    return XPR_TRUE;
}

xpr_bool_t ConfDir::getLoadDir(xpr_tchar_t *aDir, xpr_size_t aMaxLen) const
{
    return getDir(aDir, aMaxLen);
}

xpr_bool_t ConfDir::getSaveDir(xpr_tchar_t *aDir, xpr_size_t aMaxLen) const
{
    if (getDir(aDir, aMaxLen) == XPR_FALSE)
        return XPR_FALSE;

    if (fxb::IsExistFile(aDir) == XPR_TRUE)
        return XPR_TRUE;

    if (fxb::CreateDirectoryLevel(aDir) == XPR_FALSE)
        return XPR_FALSE;

    return XPR_TRUE;
}

void ConfDir::setBackup(void)
{
    mOldConfDir = mConfDir;
}

xpr_bool_t ConfDir::checkChangedConfDir(void)
{
    return (_tcsicmp(mOldConfDir.c_str(), mConfDir.c_str()) != 0) ? XPR_TRUE : XPR_FALSE;
}

xpr_bool_t ConfDir::moveToNewConfDir(void)
{
    xpr_sint_t  sType;
    xpr_tchar_t sOldConfDir[XPR_MAX_PATH + 1] = {0};
    xpr_tchar_t sNewConfDir[XPR_MAX_PATH + 1] = {0};
    xpr_tchar_t sOldPath[XPR_MAX_PATH + 1] = {0};
    xpr_tchar_t sNewPath[XPR_MAX_PATH + 1] = {0};

    getDir(mOldConfDir.c_str(), sOldConfDir, XPR_MAX_PATH);

    if (getSaveDir(sNewConfDir, XPR_MAX_PATH) == XPR_FALSE)
    {
        // sNewConfDir cannot set since one does not exist or cannot create.
        return XPR_FALSE;
    }

    for (sType = TypeBegin; sType < TypeEnd; ++sType)
    {
        sOldPath[0] = XPR_STRING_LITERAL('\0');
        sNewPath[0] = XPR_STRING_LITERAL('\0');

        getPath(sType, sOldConfDir, sOldPath, XPR_MAX_PATH);
        getSavePath(sType, sNewPath, XPR_MAX_PATH);

        if (::MoveFile(sOldPath, sNewPath) == XPR_FALSE)
        {
            return XPR_FALSE;
        }
    }

    return XPR_TRUE;
}

const xpr_tchar_t *ConfDir::getFileName(xpr_sint_t aType)
{
    switch (aType)
    {
    case TypeMain:       return XPR_STRING_LITERAL("fxfile-main.conf");
    case TypeConfig:     return XPR_STRING_LITERAL("fxfile.conf");
    case TypeBookmark:   return XPR_STRING_LITERAL("fxfile-bookmark.conf");
    case TypeFileScrap:  return XPR_STRING_LITERAL("fxfile-file_scrap.conf");
    case TypeSearchDir:  return XPR_STRING_LITERAL("fxfile-search_dir.conf");
    case TypeViewSet:    return XPR_STRING_LITERAL("fxfile-view_set.conf");
    case TypeDlgState:   return XPR_STRING_LITERAL("fxfile-dlg_state.conf");
    case TypeAccel:      return XPR_STRING_LITERAL("fxfile-accel.dat");
    case TypeCoolBar:    return XPR_STRING_LITERAL("fxfile-coolbar.dat");
    case TypeToolBar:    return XPR_STRING_LITERAL("fxfile-toolbar.dat");
    case TypeThumbnail:  return XPR_STRING_LITERAL("fxfile-thumbnail.*");
    case TypeLauncher:   return XPR_STRING_LITERAL("fxfile-launcher.conf");
    case TypeUpdater:    return XPR_STRING_LITERAL("fxfile-updater.conf");
    }

    return XPR_NULL;
}

const xpr_tchar_t *ConfDir::getDefConfDir(void)
{
    return kDefConfDir;
}

const xpr_tchar_t *ConfDir::getProgramConfDir(void)
{
    return kProgramConfDir;
}

xpr_bool_t ConfDir::load(void)
{
    clear();

    std::tstring sPath;
    fxb::GetEnvRealPath(kFxFilePath, sPath);

    xpr_bool_t sResult = XPR_FALSE;

    fxb::IniFileEx sIniFile(sPath.c_str());
    if (sIniFile.readFile() == XPR_TRUE)
    {
        const xpr_tchar_t     *sValue;
        fxb::IniFile::Section *sSection;

        sSection = sIniFile.findSection(kFxFileSection);
        if (XPR_IS_NOT_NULL(sSection))
        {
            sValue = sIniFile.getValueS(sSection, kConfHomeKey, XPR_NULL);
            if (XPR_IS_NOT_NULL(sValue))
            {
                setConfDir(sValue);

                sResult = XPR_TRUE;
            }
        }
    }

    if (XPR_IS_FALSE(sResult))
    {
        const xpr_tchar_t *sDefConfDir = ConfDir::getDefConfDir();
        if (XPR_IS_NOT_NULL(sDefConfDir))
        {
            setConfDir(sDefConfDir);

            sResult = XPR_TRUE;
        }
    }

    return sResult;
}

xpr_bool_t ConfDir::save(void) const
{
    std::tstring sPath;
    fxb::GetEnvRealPath(kFxFilePath, sPath);

    fxb::IniFileEx sIniFile(sPath.c_str());
    sIniFile.setComment(XPR_STRING_LITERAL("flyExplorer .fxfile configuration file"));

    fxb::IniFile::Section *sSection;

    sSection = sIniFile.addSection(kFxFileSection);
    XPR_ASSERT(sSection != XPR_NULL);

    sIniFile.setValueS(sSection, kConfHomeKey, mConfDir.c_str());

    sIniFile.writeFile(xpr::CharSetUtf16);

    // set system and hidden file attributes
    DWORD sFileAttributes = ::GetFileAttributes(sPath.c_str());
    sFileAttributes |= FILE_ATTRIBUTE_HIDDEN;
    ::SetFileAttributes(sPath.c_str(), sFileAttributes);

    return XPR_TRUE;
}
