//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "fxb_file_ass.h"

#include "fxb_filter.h"
#include "fxb_ini_file.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace fxb
{
FileAssMgr::FileAssMgr(void)
{
}

FileAssMgr::~FileAssMgr(void)
{
}

FileAss::FileAss(void)
{
}

FileAss::~FileAss(void)
{
    clear();
}

void FileAss::addItem(FileAssItem *aFileAssItem)
{
    if (XPR_IS_NOT_NULL(aFileAssItem))
        mFileAssDeque.push_back(aFileAssItem);
}

FileAssItem *FileAss::getItem(xpr_size_t aIndex)
{
    if (!XPR_STL_IS_INDEXABLE(aIndex, mFileAssDeque))
        return XPR_NULL;

    return mFileAssDeque[aIndex];
}

FileAssItem *FileAss::getItemFromExt(const xpr_tchar_t *aExt, xpr_uint_t aType)
{
    xpr_size_t sIndex = indexFromExt(aExt, aType);
    if (sIndex == -1)
        return XPR_NULL;

    return getItem(sIndex);
}

FileAssItem *FileAss::getItemFromPath(const xpr_tchar_t *aPath, xpr_uint_t aType)
{
    xpr_size_t sIndex = indexFromPath(aPath, aType);
    if (sIndex == -1)
        return XPR_NULL;

    return getItem(sIndex);
}

xpr_size_t FileAss::indexFromExt(const xpr_tchar_t *aExt, xpr_uint_t aType)
{
    FilterMgr &sFilterMgr = FilterMgr::instance();

    const xpr_tchar_t *sExts;
    FilterItem *sFilterItem;
    FileAssItem *sFileAssItem;
    FileAssDeque::iterator sIterator;

    if (XPR_IS_NOT_NULL(aExt))
    {
        sIterator = mFileAssDeque.begin();
        for (; sIterator != mFileAssDeque.end(); ++sIterator)
        {
            sFileAssItem = *sIterator;
            if (XPR_IS_NULL(sFileAssItem))
                continue;

            if (!XPR_ANY_BITS(sFileAssItem->mType, aType))
                continue;

            sExts = sFileAssItem->mExts.c_str();
            if (sFileAssItem->mMethod == FileAssMethodFilter)
            {
                sFilterItem = sFilterMgr.getFilterFromName(sFileAssItem->mFilterName);
                if (XPR_IS_NOT_NULL(sFilterItem))
                    sExts = sFilterItem->mExts.c_str();
            }

            if (_tcsicmp(sExts, XPR_STRING_LITERAL("*.*")) == 0)
                continue;

            if (IsEqualFilter(sExts, aExt) == XPR_TRUE)
                return std::distance(mFileAssDeque.begin(), sIterator);
        }
    }

    sIterator = mFileAssDeque.begin();
    for (; sIterator != mFileAssDeque.end(); ++sIterator)
    {
        sFileAssItem = *sIterator;
        if (XPR_IS_NULL(sFileAssItem))
            continue;

        if (!XPR_ANY_BITS(sFileAssItem->mType, aType))
            continue;

        sExts = sFileAssItem->mExts.c_str();
        if (sFileAssItem->mMethod == FileAssMethodFilter)
        {
            sFilterItem = sFilterMgr.getFilterFromName(sFileAssItem->mFilterName);
            if (XPR_IS_NOT_NULL(sFilterItem))
                sExts = sFilterItem->mExts.c_str();
        }

        if (_tcsicmp(sExts, XPR_STRING_LITERAL("*.*")) != 0)
            continue;

        if (IsEqualFilter(sExts, aExt) == XPR_TRUE)
            return std::distance(mFileAssDeque.begin(), sIterator);
    }

    return -1;
}

xpr_size_t FileAss::indexFromPath(const xpr_tchar_t *aPath, xpr_uint_t aType)
{
    if (XPR_IS_NULL(aPath))
        return -1;

    const xpr_tchar_t *sExt = GetFileExt(aPath);
    if (XPR_IS_NULL(sExt))
        return -1;

    return indexFromExt(sExt, aType);
}

xpr_bool_t FileAss::getItemFromExt(const xpr_tchar_t *aExt, xpr_uint_t aType, FileAssDeque &aFileAssDeque)
{
    FilterMgr &sFilterMgr = FilterMgr::instance();

    const xpr_tchar_t *sExts;
    FilterItem *sFilterItem;
    FileAssItem *sFileAssItem;
    FileAssDeque::iterator sIterator;

    sIterator = mFileAssDeque.begin();
    for (; sIterator != mFileAssDeque.end(); ++sIterator)
    {
        sFileAssItem = *sIterator;
        if (XPR_IS_NULL(sFileAssItem))
            continue;

        if (!XPR_ANY_BITS(sFileAssItem->mType, aType))
            continue;

        sExts = sFileAssItem->mExts.c_str();
        if (sFileAssItem->mMethod == FileAssMethodFilter)
        {
            sFilterItem = sFilterMgr.getFilterFromName(sFileAssItem->mFilterName);
            if (XPR_IS_NOT_NULL(sFilterItem))
                sExts = sFilterItem->mExts.c_str();
        }

        if ((_tcsicmp(sExts, XPR_STRING_LITERAL("*.*")) == 0) ||
            (XPR_IS_NOT_NULL(aExt) && IsEqualFilter(sExts, aExt) == XPR_TRUE))
        {
            aFileAssDeque.push_back(sFileAssItem);
        }
    }

    return (aFileAssDeque.empty() == false) ? XPR_TRUE : XPR_FALSE;
}

xpr_bool_t FileAss::getItemFromPath(const xpr_tchar_t *aPath, xpr_uint_t aType, FileAssDeque &aFileAssDeque)
{
    if (XPR_IS_NULL(aPath))
        return -1;

    const xpr_tchar_t *sExt = GetFileExt(aPath);
    if (XPR_IS_NULL(sExt))
        return -1;

    return getItemFromExt(sExt, aType, aFileAssDeque);
}

xpr_size_t FileAss::getCount(void)
{
    return mFileAssDeque.size();
}

void FileAss::clear(void)
{
    FileAssItem *sFileAssItem;
    FileAssDeque::iterator sIterator;

    sIterator = mFileAssDeque.begin();
    for (; sIterator != mFileAssDeque.end(); ++sIterator)
    {
        sFileAssItem = *sIterator;
        XPR_SAFE_DELETE(sFileAssItem);
    }

    mFileAssDeque.clear();
}

void FileAss::initDefault(void)
{
    clear();

    FileAssItem *sFileAssItem;

    sFileAssItem = new FileAssItem;
    sFileAssItem->mName   = XPR_STRING_LITERAL("Default viewer");
    sFileAssItem->mType   = FileAssTypeViewer;
    sFileAssItem->mMethod = FileAssMethodExt;
    sFileAssItem->mExts   = XPR_STRING_LITERAL("*.*");
    sFileAssItem->mPath   = XPR_STRING_LITERAL("%Windows%\\notepad.exe");
    addItem(sFileAssItem);

    sFileAssItem = new FileAssItem;
    sFileAssItem->mName   = XPR_STRING_LITERAL("Default editor");
    sFileAssItem->mType   = FileAssTypeEditor;
    sFileAssItem->mMethod = FileAssMethodExt;
    sFileAssItem->mExts   = XPR_STRING_LITERAL("*.*");
    sFileAssItem->mPath   = XPR_STRING_LITERAL("%Windows%\\notepad.exe");
    addItem(sFileAssItem);
}

static const xpr_tchar_t _fx_Type[]       = XPR_STRING_LITERAL("Type");
static const xpr_tchar_t _fx_Method[]     = XPR_STRING_LITERAL("Method");
static const xpr_tchar_t _fx_Extension[]  = XPR_STRING_LITERAL("Extension");
static const xpr_tchar_t _fx_FilterName[] = XPR_STRING_LITERAL("Filter Name");
static const xpr_tchar_t _fx_Program[]    = XPR_STRING_LITERAL("Program");

xpr_bool_t FileAss::loadFromFile(const xpr_tchar_t *aPath)
{
    IniFile sIniFile(aPath);
    if (sIniFile.readFile() == XPR_FALSE)
        return XPR_FALSE;

    xpr_size_t sCount = sIniFile.getKeyCount();
    if (sCount <= 0)
        return XPR_FALSE;

    sCount = min(sCount, MAX_FILE_ASS);

    xpr_size_t i;
    const xpr_tchar_t *sKey;
    FileAssItem *sFileAssItem;

    for (i = 0; i < sCount; ++i)
    {
        sFileAssItem = new FileAssItem;

        sFileAssItem->mName = sIniFile.getKeyName(i);
        sKey = sFileAssItem->mName.c_str();

        sFileAssItem->mType    = sIniFile.getValueU(sKey, _fx_Type, FileAssTypeNone);
        sFileAssItem->mMethod  = sIniFile.getValueU(sKey, _fx_Method, FileAssMethodExt);

        if (sFileAssItem->mMethod == FileAssMethodExt)
            sFileAssItem->mExts = sIniFile.getValueS(sKey, _fx_Extension, XPR_STRING_LITERAL(""));
        else
            sFileAssItem->mExts = sIniFile.getValueS(sKey, _fx_FilterName, XPR_STRING_LITERAL(""));

        sFileAssItem->mPath = sIniFile.getValueS(sKey, _fx_Program, XPR_STRING_LITERAL(""));

        addItem(sFileAssItem);
    }

    return XPR_TRUE;
}

xpr_bool_t FileAss::saveToFile(const xpr_tchar_t *aPath)
{
    IniFile sIniFile(aPath);
    sIniFile.setComment(XPR_STRING_LITERAL("flyExplorer File Association File"));

    const xpr_tchar_t *sKey;
    FileAssItem *sFileAssItem;
    FileAssDeque::iterator sIterator;

    sIterator = mFileAssDeque.begin();
    for (; sIterator != mFileAssDeque.end(); ++sIterator)
    {
        sFileAssItem = *sIterator;
        if (XPR_IS_NULL(sFileAssItem))
            continue;

        sKey = sFileAssItem->mName.c_str();
        sIniFile.addKeyName(sKey);

        sIniFile.setValueU(sKey, _fx_Type,   sFileAssItem->mType);
        sIniFile.setValueU(sKey, _fx_Method, sFileAssItem->mMethod);

        if (sFileAssItem->mMethod == FileAssMethodExt)
            sIniFile.setValueS(sKey, _fx_Extension, sFileAssItem->mExts.c_str());
        else
            sIniFile.setValueS(sKey, _fx_FilterName, sFileAssItem->mFilterName.c_str());

        sIniFile.setValueS(sKey, _fx_Program, sFileAssItem->mPath.c_str());
    }

    sIniFile.writeFile(xpr::CharSetUtf16);

    return XPR_TRUE;
}
} // namespace fxb
