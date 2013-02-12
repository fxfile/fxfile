//
// Copyright (c) 2012-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "fxb_language_table.h"

#include "xpr_xml.h"

namespace fxb
{
LanguageTable::LanguageTable(void)
    : mLoadedLanaguage(XPR_NULL)
    , mStringTable(XPR_NULL)
    , mFormatStringTable(XPR_NULL)
{
    mDir[0] = 0;
}

LanguageTable::~LanguageTable(void)
{
    clear();
}

void LanguageTable::setDir(const xpr_tchar_t *aDir)
{
    if (XPR_IS_NOT_NULL(aDir))
        _tcscpy(mDir, aDir);
}

xpr_bool_t LanguageTable::scan(void)
{
    clear();

    xpr_tchar_t sDir[XPR_MAX_PATH + 1] = {0};
    xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
    _tcscpy(sDir, mDir);

    if (sDir[_tcslen(sDir) - 1] != XPR_STRING_LITERAL('\\'))
        _tcscat(sDir, XPR_STRING_LITERAL("\\"));

    _tcscpy(sPath, sDir);
    _tcscat(sPath, XPR_STRING_LITERAL("*.*"));

    HANDLE sFile;
    WIN32_FIND_DATA sWin32FindData = {0};
    LanguagePack *sLanguagePack;

    sFile = ::FindFirstFile(sPath, &sWin32FindData);
    if (sFile == INVALID_HANDLE_VALUE)
        return XPR_FALSE;

    do
    {
        if (_tcscmp(sWin32FindData.cFileName, XPR_STRING_LITERAL(".")) == 0 || _tcscmp(sWin32FindData.cFileName, XPR_STRING_LITERAL("..")) == 0)
            continue;

        _tcscpy(sPath, mDir);
        _tcscat(sPath, XPR_STRING_LITERAL("\\"));
        _tcscat(sPath, sWin32FindData.cFileName);

        sLanguagePack = new LanguagePack;
        if (XPR_IS_NULL(sLanguagePack))
            continue;

        if (sLanguagePack->load(sPath) == XPR_FALSE)
        {
            XPR_SAFE_DELETE(sLanguagePack);
            continue;
        }

        mLanguageMap[sLanguagePack->getLanguageDesc()->mLanguage] = sLanguagePack;
        mLanguageDeque.push_back(sLanguagePack);
    }
    while (::FindNextFile(sFile, &sWin32FindData));

    ::FindClose(sFile);

    return XPR_TRUE;
}

xpr_bool_t LanguageTable::loadLanguage(const xpr_tchar_t *aLanguage)
{
    if (XPR_IS_NULL(aLanguage))
        return XPR_FALSE;

    if (XPR_IS_NOT_NULL(mLoadedLanaguage) && mLoadedLanaguage->equalLanguage(aLanguage) == XPR_TRUE)
        return XPR_TRUE;

    LanguageMap::iterator sIterator = mLanguageMap.find(aLanguage);
    if (sIterator == mLanguageMap.end())
        return XPR_FALSE;

    mLoadedLanaguage = sIterator->second;

    XPR_SAFE_DELETE(mStringTable);
    XPR_SAFE_DELETE(mFormatStringTable);

    mStringTable = new StringTable;
    if (XPR_IS_NULL(mStringTable))
    {
        mLoadedLanaguage = XPR_NULL;

        XPR_SAFE_DELETE(mStringTable);

        return XPR_FALSE;
    }

    if (mLoadedLanaguage->loadStringTable(mStringTable) == XPR_FALSE)
    {
        mLoadedLanaguage = XPR_NULL;

        XPR_SAFE_DELETE(mStringTable);

        return XPR_FALSE;
    }

    mFormatStringTable = new FormatStringTable(*mStringTable);
    if (XPR_IS_NULL(mFormatStringTable))
    {
        mLoadedLanaguage = XPR_NULL;

        XPR_SAFE_DELETE(mStringTable);

        return XPR_FALSE;
    }

    return XPR_TRUE;
}

void LanguageTable::unloadLanguage(void)
{
    mLoadedLanaguage = XPR_NULL;

    XPR_SAFE_DELETE(mStringTable);
    XPR_SAFE_DELETE(mFormatStringTable);
}

xpr_size_t LanguageTable::getLanguageCount(void) const
{
    return mLanguageDeque.size();
}

const LanguagePack::Desc *LanguageTable::getLanguageDesc(xpr_size_t aIndex) const
{
    if (!XPR_STL_IS_INDEXABLE(aIndex, mLanguageDeque))
        return XPR_NULL;

    return mLanguageDeque[aIndex]->getLanguageDesc();
}

const LanguagePack::Desc *LanguageTable::getLanguageDesc(const xpr_tchar_t *aLanguage) const
{
    if (XPR_IS_NULL(aLanguage))
        return XPR_NULL;

    LanguageMap::const_iterator sIterator = mLanguageMap.find(aLanguage);
    if (sIterator == mLanguageMap.end())
        return XPR_NULL;

    const LanguagePack *sLanguagePack = sIterator->second;
    const LanguagePack::Desc *sLanguagePackDesc = sLanguagePack->getLanguageDesc();

    return sLanguagePackDesc;
}

const LanguagePack::Desc *LanguageTable::getLanguageDesc(void) const
{
    if (XPR_IS_NULL(mLoadedLanaguage))
        return XPR_NULL;

    return mLoadedLanaguage->getLanguageDesc();
}

StringTable *LanguageTable::getStringTable(void) const
{
    return mStringTable;
}

FormatStringTable *LanguageTable::getFormatStringTable(void) const
{
    return mFormatStringTable;
}

void LanguageTable::clear(void)
{
    LanguagePack *sLanguagePack;
    LanguageDeque::iterator sIterator;

    sIterator = mLanguageDeque.begin();
    for (; sIterator != mLanguageDeque.end(); ++sIterator)
    {
        sLanguagePack = *sIterator;
        XPR_SAFE_DELETE(sLanguagePack);
    }

    mLanguageMap.clear();
    mLanguageDeque.clear();

    mLoadedLanaguage = XPR_NULL;
    XPR_SAFE_DELETE(mStringTable);
    XPR_SAFE_DELETE(mFormatStringTable);
}
} // namespace fxb
