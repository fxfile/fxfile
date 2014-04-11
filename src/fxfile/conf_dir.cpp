//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "conf_dir.h"

#include "conf_file_ex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace fxfile;
using namespace fxfile::base;

namespace fxfile
{
namespace
{
#define CFG_PATH_FILENAME             XPR_STRING_LITERAL(".fxfile")

const xpr_tchar_t kFxFilePath    [] = XPR_STRING_LITERAL("%AppData%\\fxfile\\")CFG_PATH_FILENAME;

const xpr_tchar_t kProgramConfDir[] = XPR_STRING_LITERAL("%fxFile%\\conf");
const xpr_tchar_t kDefConfDir    [] = XPR_STRING_LITERAL("%AppData%\\fxfile\\conf");

const xpr_tchar_t kFxFileSection [] = XPR_STRING_LITERAL(".fxfile");
const xpr_tchar_t kConfHomeKey   [] = XPR_STRING_LITERAL("conf_home");
} // namespace anonymous

ConfDir::ConfDir(void)
    : mReadOnly(XPR_FALSE)
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

void ConfDir::setConfDir(const xpr_tchar_t *aConfDir, xpr_bool_t aReadOnly)
{
    XPR_ASSERT(aConfDir != XPR_NULL);

    mConfDir  = aConfDir;
    mReadOnly = aReadOnly;
}

xpr_bool_t ConfDir::getDir(const xpr_tchar_t *aConfDir, xpr_tchar_t *aDir, xpr_size_t aMaxLen) const
{
    if (XPR_IS_NULL(aDir) || aMaxLen <= 0)
        return XPR_FALSE;

    xpr::string sDir;
    GetEnvRealPath(aConfDir, sDir);

    if (sDir.empty() == XPR_TRUE)
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

xpr_bool_t ConfDir::getPath(xpr_sint_t aType, const xpr_tchar_t *aDir, xpr_tchar_t *aPath, xpr_size_t aMaxLen) const
{
    const xpr_tchar_t *sFileName = ConfDir::getFileName(aType);
    if (XPR_IS_NULL(sFileName))
        return XPR_FALSE;

    xpr::string sPath(aDir);
    sPath += XPR_STRING_LITERAL('\\');
    sPath += sFileName;

    if (sPath.length() > aMaxLen)
        return XPR_FALSE;

    _tcscpy(aPath, sPath.c_str());

    return XPR_TRUE;
}

xpr_bool_t ConfDir::getPath(xpr_sint_t aType, xpr_tchar_t *aPath, xpr_size_t aMaxLen) const
{
    xpr_tchar_t sDir[XPR_MAX_PATH + 1] = {0};
    getDir(sDir, XPR_MAX_PATH + 1);

    return getPath(aType, sDir, aPath, aMaxLen);
}

xpr_bool_t ConfDir::getLoadPath(xpr_sint_t aType, xpr_tchar_t *aPath, xpr_size_t aMaxLen) const
{
    return getPath(aType, aPath, aMaxLen);
}

xpr_bool_t ConfDir::getSavePath(xpr_sint_t aType, xpr_tchar_t *aPath, xpr_size_t aMaxLen) const
{
    if (getPath(aType, aPath, aMaxLen) == XPR_FALSE)
        return XPR_FALSE;

    if (CreateDirectoryLevel(aPath, 0, XPR_FALSE) == XPR_FALSE)
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

    if (IsExistFile(aDir) == XPR_TRUE)
        return XPR_TRUE;

    if (CreateDirectoryLevel(aDir) == XPR_FALSE)
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

        if (xpr::FileSys::exist(sOldPath) == XPR_TRUE)
        {
            if (xpr::FileSys::exist(sNewPath) == XPR_TRUE)
            {
                xpr::FileSys::remove(sNewPath);
            }

            xpr::FileSys::rename(sOldPath, sNewPath);
        }
    }

    return XPR_TRUE;
}

const xpr_tchar_t *ConfDir::getFileName(xpr_sint_t aType)
{
    switch (aType)
    {
    case TypeMain:           return XPR_STRING_LITERAL("fxfile-main.conf");
    case TypeConfig:         return XPR_STRING_LITERAL("fxfile.conf");
    case TypeBookmark:       return XPR_STRING_LITERAL("fxfile-bookmark.conf");
    case TypeFileScrap:      return XPR_STRING_LITERAL("fxfile-file_scrap.conf");
    case TypeSearchDir:      return XPR_STRING_LITERAL("fxfile-search_dir.conf");
    case TypeViewSet:        return XPR_STRING_LITERAL("fxfile-view_set.conf");
    case TypeDlgState:       return XPR_STRING_LITERAL("fxfile-dlg_state.conf");
    case TypeAccel:          return XPR_STRING_LITERAL("fxfile-accel.dat");
    case TypeCoolBar:        return XPR_STRING_LITERAL("fxfile-coolbar.dat");
    case TypeToolBar:        return XPR_STRING_LITERAL("fxfile-toolbar.dat");
    case TypeThumbnailData:  return XPR_STRING_LITERAL("fxfile-thumbnail.dat");
    case TypeThumbnailIndex: return XPR_STRING_LITERAL("fxfile-thumbnail.idx");
    case TypeLauncher:       return XPR_STRING_LITERAL("fxfile-launcher.conf");
    case TypeUpdater:        return XPR_STRING_LITERAL("fxfile-updater.conf");
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

    xpr::string sPath;
    GetEnvRealPath(kFxFilePath, sPath);

    xpr_bool_t sResult = XPR_FALSE;

    fxfile::base::ConfFileEx sConfFile(sPath.c_str());
    if (sConfFile.load() == XPR_TRUE)
    {
        const xpr_tchar_t *sValue;
        ConfFile::Section *sSection;

        sSection = sConfFile.findSection(kFxFileSection);
        if (XPR_IS_NOT_NULL(sSection))
        {
            sValue = sConfFile.getValueS(sSection, kConfHomeKey, XPR_NULL);
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
    if (XPR_IS_TRUE(mReadOnly))
    {
        return XPR_TRUE;
    }

    xpr::string sPath;
    GetEnvRealPath(kFxFilePath, sPath);

    fxfile::base::ConfFileEx sConfFile(sPath.c_str());
    sConfFile.setComment(XPR_STRING_LITERAL("fxfile .fxfile configuration file"));

    ConfFile::Section *sSection;

    sSection = sConfFile.addSection(kFxFileSection);
    XPR_ASSERT(sSection != XPR_NULL);

    sConfFile.setValueS(sSection, kConfHomeKey, mConfDir.c_str());

    // remove hidden file attributes to save
    DWORD sFileAttributes = ::GetFileAttributes(sPath.c_str());
    sFileAttributes &= ~FILE_ATTRIBUTE_HIDDEN;
    ::SetFileAttributes(sPath.c_str(), sFileAttributes);

    sConfFile.save(xpr::CharSetUtf16);

    // set hidden file attributes
    sFileAttributes = ::GetFileAttributes(sPath.c_str());
    sFileAttributes |= FILE_ATTRIBUTE_HIDDEN;
    ::SetFileAttributes(sPath.c_str(), sFileAttributes);

    return XPR_TRUE;
}
} // namespace fxfile
