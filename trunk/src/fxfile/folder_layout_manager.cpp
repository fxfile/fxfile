//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "folder_layout_manager.h"

#include "conf_file_ex.h"
#include "md5.h"
#include "conf_dir.h"
#include "fxfile_def.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace fxfile;
using namespace fxfile::base;

namespace fxfile
{
namespace
{
#define EACH_FOLDER_LAYOUT_FILENAME   XPR_STRING_LITERAL(".folder_layout.conf")

const xpr_tchar_t kIndexSection       [] = XPR_STRING_LITERAL("folder_layout");
const xpr_tchar_t kIndexPathKey       [] = XPR_STRING_LITERAL("folder_layout.index%d.path");
const xpr_tchar_t kIndexKeyKey        [] = XPR_STRING_LITERAL("folder_layout.index%d.key");
const xpr_tchar_t kAllSubApplyKey     [] = XPR_STRING_LITERAL("folder_layout.index%d.all_sub_apply");

const xpr_tchar_t kBaseKey            [] = XPR_STRING_LITERAL("folder_layout");
const xpr_tchar_t kStyleKey           [] = XPR_STRING_LITERAL("style");
const xpr_tchar_t kSortFormatIdKey    [] = XPR_STRING_LITERAL("sort_format_id");
const xpr_tchar_t kSortPropertyIdKey  [] = XPR_STRING_LITERAL("sort_property_id");
const xpr_tchar_t kSortAscendingKey   [] = XPR_STRING_LITERAL("sort_ascending");
const xpr_tchar_t kColumnCountKey     [] = XPR_STRING_LITERAL("column_count");
const xpr_tchar_t kFormatIdKey        [] = XPR_STRING_LITERAL("column%d.format_id");
const xpr_tchar_t kPropertyIdKey      [] = XPR_STRING_LITERAL("column%d.property_id");
const xpr_tchar_t kWidthKey           [] = XPR_STRING_LITERAL("column%d.width");

const xpr_tchar_t kHashSection        [] = XPR_STRING_LITERAL("%s");
const xpr_tchar_t kFolderLayoutSection[] = XPR_STRING_LITERAL("folder_layout");
} // namespace anonymous

FolderLayoutManager::FolderLayoutManager(void)
    : mInstalledPath(XPR_TRUE)
{
}

FolderLayoutManager::~FolderLayoutManager(void)
{
    clear();
}

void FolderLayoutManager::clear(void)
{
    FolderLayout *sFolderLayout;
    HashMap::iterator sIterator;

    FXFILE_STL_FOR_EACH(sIterator, mHashMap)
    {
        sFolderLayout = sIterator->second;
        XPR_ASSERT(sFolderLayout != XPR_NULL);

        XPR_SAFE_DELETE(sFolderLayout);
    }

    mSubSet.clear();
    mIndexMap.clear();
    mHashMap.clear();
}

void FolderLayoutManager::load(void)
{
    xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
    ConfDir::instance().getLoadPath(ConfDir::TypeFolderLayout, sPath, XPR_MAX_PATH);

    fxfile::base::ConfFileEx sConfFile(sPath);
    if (sConfFile.load() == XPR_FALSE)
        return;

    FolderLayout *sFolderLayout = XPR_NULL;
    IndexMap::const_iterator sIndexIterator;
    HashMap::const_iterator  sHashIterator;
    xpr::string sHashValueSection;
    ConfFile::Section *sSection;

    // load index
    loadIndex(sConfFile);

    // load folder layout
    FXFILE_STL_FOR_EACH(sIndexIterator, mIndexMap)
    {
        const xpr_uint64_t &sHashValue = sIndexIterator->second;

        if (XPR_IS_NULL(sFolderLayout))
        {
            sFolderLayout = new FolderLayout;
            if (XPR_IS_NULL(sFolderLayout))
                break;
        }

        sHashIterator = mHashMap.find(sHashValue);
        if (sHashIterator == mHashMap.end())
        {
            sHashValueSection.format(XPR_STRING_LITERAL("%I64x"), sHashValue);

            sSection = sConfFile.findSection(sHashValueSection.c_str());
            if (XPR_IS_NOT_NULL(sSection))
            {
                if (FolderLayoutManager::loadFromConfFile(*sFolderLayout, sConfFile, sSection, kBaseKey) == XPR_TRUE)
                {
                    mHashMap[sHashValue] = sFolderLayout;
                    sFolderLayout = XPR_NULL;
                }
            }
        }
    }

    if (XPR_IS_NOT_NULL(sFolderLayout))
    {
        XPR_SAFE_DELETE(sFolderLayout);
    }
}

xpr_bool_t FolderLayoutManager::save(void) const
{
    xpr_tchar_t sPath[XPR_MAX_PATH + 1] = {0};
    ConfDir::instance().getSavePath(ConfDir::TypeFolderLayout, sPath, XPR_MAX_PATH);

    fxfile::base::ConfFileEx sConfFile(sPath);

    if (XPR_IS_TRUE(mInstalledPath))
    {
        sConfFile.setComment(XPR_STRING_LITERAL("fxfile folder layout file"));
    }
    else
    {
        sConfFile.setComment(XPR_STRING_LITERAL("fxfile folder layout index file"));
    }

    HashMap::const_iterator sIterator;
    xpr::string sHashValueSection;
    ConfFile::Section *sSection;

    // save index
    saveIndex(sConfFile);

    // save folder layout
    FXFILE_STL_FOR_EACH(sIterator, mHashMap)
    {
        const xpr_uint64_t &sHashValue = sIterator->first;
        FolderLayout        *sFolderLayout = sIterator->second;

        XPR_ASSERT(sFolderLayout != XPR_NULL);

        sHashValueSection.format(XPR_STRING_LITERAL("%I64x"), sHashValue);

        sSection = sConfFile.addSection(sHashValueSection.c_str());
        XPR_ASSERT(sSection != XPR_NULL);

        FolderLayoutManager::saveToConfFile(*sFolderLayout, sConfFile, sSection, kBaseKey);
    }

    return sConfFile.save(xpr::CharSetUtf16);
}

void FolderLayoutManager::loadIndex(fxfile::base::ConfFileEx &aConfFile)
{
    xpr_sint_t         i;
    xpr_tchar_t        sKey[0xff];
    const xpr_tchar_t *sPath;
    const xpr_tchar_t *sHashValueString;
    xpr_uint64_t       sHashValue;
    xpr_bool_t         sAllSubApply;
    ConfFile::Section *sSection;

    sSection = aConfFile.findSection(kIndexSection);
    if (XPR_IS_NULL(sSection))
        return;

    for (i = 0; ; ++i)
    {
        _stprintf(sKey, kIndexPathKey, i + 1);

        sPath = aConfFile.getValueS(sSection, sKey, XPR_NULL);
        if (XPR_IS_NULL(sPath))
            break;

        _stprintf(sKey, kIndexKeyKey, i + 1);

        sHashValueString = aConfFile.getValueS(sSection, sKey, XPR_NULL);
        if (XPR_IS_NULL(sHashValueString))
            break;

        _stscanf(sHashValueString, XPR_STRING_LITERAL("%I64x"), &sHashValue);

        _stprintf(sKey, kAllSubApplyKey, i + 1);
        sAllSubApply = aConfFile.getValueB(sSection, sKey, XPR_FALSE);

        mIndexMap[sPath] = sHashValue;

        if (XPR_IS_TRUE(sAllSubApply))
        {
            mSubSet.insert(sPath);
        }
    }
}

void FolderLayoutManager::saveIndex(fxfile::base::ConfFileEx &aConfFile) const
{
    xpr_sint_t          i;
    xpr_tchar_t         sKey[0xff];
    xpr::string         sHashValueString;
    ConfFile::Section  *sSection;
    IndexMap::const_iterator sIterator;
    SubSet::const_iterator  sSubIterator;

    sSection = aConfFile.addSection(kIndexSection);
    XPR_ASSERT(sSection != XPR_NULL);

    sIterator = mIndexMap.begin();
    for (i = 0; sIterator != mIndexMap.end(); ++sIterator, ++i)
    {
        const xpr::string  &sPath      = sIterator->first;
        const xpr_uint64_t &sHashValue = sIterator->second;

        _stprintf(sKey, kIndexPathKey, i + 1);
        aConfFile.setValueS(sSection, sKey, sPath);

        _stprintf(sKey, kIndexKeyKey, i + 1);
        sHashValueString.format(XPR_STRING_LITERAL("%I64x"), sHashValue);
        aConfFile.setValueS(sSection, sKey, sHashValueString);

        sSubIterator = mSubSet.find(sPath);
        if (sSubIterator == mSubSet.end())
            continue;

        _stprintf(sKey, kAllSubApplyKey, i + 1);
        aConfFile.setValueI(sSection, sKey, XPR_TRUE);
    }
}

xpr_bool_t FolderLayoutManager::load(const xpr::string &aKey, FolderLayout &aFolderLayout)
{
    xpr_bool_t sInstalledPath = XPR_TRUE;
    if (XPR_IS_FALSE(mInstalledPath) && aKey[1] == XPR_STRING_LITERAL(':'))
        sInstalledPath = XPR_FALSE;

    xpr_bool_t  sAllSubApply = XPR_FALSE;
    xpr::string sKey(aKey);
    SubSet::const_iterator sIterator;

    FXFILE_STL_FOR_EACH(sIterator, mSubSet)
    {
        const xpr::string &sPathForAllSubApply = *sIterator;

        if (_tcsnicmp(sPathForAllSubApply.c_str(), sKey.c_str(), sIterator->length()) == 0)
        {
            sKey = *sIterator;
            sAllSubApply = XPR_TRUE;
            break;
        }
    }

    if (XPR_IS_TRUE(sInstalledPath))
    {
        IndexMap::const_iterator sIndexIterator = mIndexMap.find(sKey);
        if (sIndexIterator == mIndexMap.end())
            return XPR_FALSE;

        const xpr_uint64_t &sHashValue = sIndexIterator->second;

        HashMap::const_iterator sHashIterator = mHashMap.find(sHashValue);
        if (sHashIterator == mHashMap.end())
            return XPR_FALSE;

        const FolderLayout *sFolderLayout = sHashIterator->second;

        sFolderLayout->clone(aFolderLayout);
    }
    else
    {
        xpr::string sIniPath;
        sIniPath.format(XPR_STRING_LITERAL("%s\\")EACH_FOLDER_LAYOUT_FILENAME, sKey.c_str());

        fxfile::base::ConfFileEx sConfFile(sIniPath);
        if (sConfFile.load() == XPR_FALSE)
            return XPR_FALSE;

        ConfFile::Section *sSection = sConfFile.findSection(kFolderLayoutSection);
        if (XPR_IS_NULL(sSection))
            return XPR_FALSE;

        return FolderLayoutManager::loadFromConfFile(aFolderLayout, sConfFile, sSection, kBaseKey);
    }

    aFolderLayout.mAllSubApply = sAllSubApply;

    return XPR_TRUE;
}

xpr_bool_t FolderLayoutManager::loadFromConfFile(FolderLayout &aFolderLayout, fxfile::base::ConfFileEx &aConfFile, base::ConfFile::Section *aSection, const xpr_tchar_t *aBaseKey)
{
    XPR_ASSERT(aSection != XPR_NULL);

    xpr_sint_t         i;
    xpr::string        sStyleKeyString(kStyleKey);
    xpr::string        sSortFormatIdKeyString(kSortFormatIdKey);
    xpr::string        sSortPropertyIdKeyString(kSortPropertyIdKey);
    xpr::string        sSortAscendingKeyString(kSortAscendingKey);
    xpr::string        sColumnCountKeyString(kColumnCountKey);
    xpr::string        sFormatIdKey;
    xpr::string        sPropertyIdKey;
    xpr::string        sWidthKey;
    const xpr_tchar_t *sValue;

    if (XPR_IS_NOT_NULL(aBaseKey))
    {
        sStyleKeyString         .insert(0, XPR_STRING_LITERAL("."));
        sSortFormatIdKeyString  .insert(0, XPR_STRING_LITERAL("."));
        sSortPropertyIdKeyString.insert(0, XPR_STRING_LITERAL("."));
        sSortAscendingKeyString .insert(0, XPR_STRING_LITERAL("."));
        sColumnCountKeyString   .insert(0, XPR_STRING_LITERAL("."));

        sStyleKeyString         .insert(0, aBaseKey);
        sSortFormatIdKeyString  .insert(0, aBaseKey);
        sSortPropertyIdKeyString.insert(0, aBaseKey);
        sSortAscendingKeyString .insert(0, aBaseKey);
        sColumnCountKeyString   .insert(0, aBaseKey);
    }

    ConfFile::Key *sStyleKey          = aConfFile.findKey(aSection, sStyleKeyString.c_str());
    ConfFile::Key *sSortFormatIdKey   = aConfFile.findKey(aSection, sSortFormatIdKeyString.c_str());
    ConfFile::Key *sSortPropertyIdKey = aConfFile.findKey(aSection, sSortPropertyIdKeyString.c_str());
    ConfFile::Key *sSortAscendingKey  = aConfFile.findKey(aSection, sSortAscendingKeyString.c_str());
    ConfFile::Key *sColumnCountKey    = aConfFile.findKey(aSection, sColumnCountKeyString.c_str());

    if (sStyleKey          == NULL &&
        sSortFormatIdKey   == NULL &&
        sSortPropertyIdKey == NULL &&
        sSortAscendingKey  == NULL &&
        sColumnCountKey    == NULL)
    {
        return XPR_FALSE;
    }

    sValue = aConfFile.getValueS(sSortFormatIdKey, XPR_STRING_LITERAL(""));

    aFolderLayout.mViewStyle                = aConfFile.getValueI(sStyleKey, VIEW_STYLE_DETAILS);
    aFolderLayout.mAllSubApply              = XPR_FALSE;
    aFolderLayout.mSortColumnId.mFormatId.fromString(sValue);
    aFolderLayout.mSortColumnId.mPropertyId = aConfFile.getValueI(sSortPropertyIdKey, 0);
    aFolderLayout.mSortAscending            = aConfFile.getValueI(sSortAscendingKey,  1);

    aFolderLayout.mColumnCount = aConfFile.getValueI(sColumnCountKey, 0);
    if (aFolderLayout.mColumnCount < 0)
        aFolderLayout.mColumnCount = 0;

    if (aFolderLayout.mColumnCount > 0)
    {
        aFolderLayout.mColumnItem = new FolderLayout::ColumnItem[aFolderLayout.mColumnCount];

        for (i = 0; i < aFolderLayout.mColumnCount; ++i)
        {
            sFormatIdKey  .format(kFormatIdKey,   i + 1);
            sPropertyIdKey.format(kPropertyIdKey, i + 1);
            sWidthKey     .format(kWidthKey,      i + 1);

            if (XPR_IS_NOT_NULL(aBaseKey))
            {
                sFormatIdKey  .insert(0, XPR_STRING_LITERAL("."));
                sPropertyIdKey.insert(0, XPR_STRING_LITERAL("."));
                sWidthKey     .insert(0, XPR_STRING_LITERAL("."));

                sFormatIdKey  .insert(0, aBaseKey);
                sPropertyIdKey.insert(0, aBaseKey);
                sWidthKey     .insert(0, aBaseKey);
            }

            sValue = aConfFile.getValueS(aSection, sFormatIdKey.c_str(), XPR_STRING_LITERAL(""));

            aFolderLayout.mColumnItem[i].mFormatId.fromString(sValue);
            aFolderLayout.mColumnItem[i].mPropertyId = aConfFile.getValueI(aSection, sPropertyIdKey.c_str(), -1);
            aFolderLayout.mColumnItem[i].mWidth      = aConfFile.getValueI(aSection, sWidthKey.c_str(),      -1);

            if (aFolderLayout.mColumnItem[i].mWidth <= 0 || aFolderLayout.mColumnItem[i].mWidth > MAX_COLUMN_SIZE)
                aFolderLayout.mColumnItem[i].mWidth = DEF_COLUMN_SIZE;
        }
    }

    return XPR_TRUE;
}

xpr_bool_t FolderLayoutManager::save(const xpr::string &aKey, const FolderLayout &aFolderLayout)
{
    xpr_bool_t sInstalledPath = XPR_TRUE;
    if (XPR_IS_FALSE(mInstalledPath) && aKey[1] == XPR_STRING_LITERAL(':'))
        sInstalledPath = XPR_FALSE;

    xpr::string sKey(aKey);

    if (XPR_IS_TRUE(aFolderLayout.mAllSubApply))
    {
        mSubSet.insert(sKey);
    }
    else
    {
        SubSet::iterator sIterator;

        sIterator = mSubSet.begin();
        for (; sIterator != mSubSet.end(); ++sIterator)
        {
            if (_tcsnicmp(sIterator->c_str(), sKey.c_str(), sIterator->length()) == 0)
            {
                sKey = *sIterator;
                break;
            }
        }
    }

    xpr_bool_t sResult = XPR_FALSE;

    if (XPR_IS_TRUE(sInstalledPath))
    {
        xpr_uint64_t sHashValue = generateHashValue(aFolderLayout);

        mIndexMap[sKey] = sHashValue;

        HashMap::iterator sHashIterator = mHashMap.find(sHashValue);
        if (sHashIterator != mHashMap.end())
        {
            FolderLayout *sFolderLayout = sHashIterator->second;
            XPR_ASSERT(sFolderLayout != XPR_NULL);

            aFolderLayout.clone(*sFolderLayout);

            sResult = XPR_TRUE;
        }
        else
        {
            FolderLayout *sFolderLayout = new FolderLayout;
            if (XPR_IS_NOT_NULL(sFolderLayout))
            {
                aFolderLayout.clone(*sFolderLayout);

                mHashMap[sHashValue] = sFolderLayout;

                sResult = XPR_TRUE;
            }
        }
    }
    else
    {
        xpr::string sIniPath;
        sIniPath.format(XPR_STRING_LITERAL("%s\\")EACH_FOLDER_LAYOUT_FILENAME, sKey.c_str());

        fxfile::base::ConfFileEx sConfFile(sIniPath);
        sConfFile.setComment(XPR_STRING_LITERAL("fxfile folder layout file"));

        ConfFile::Section *sSection = sConfFile.addSection(kFolderLayoutSection);
        XPR_ASSERT(sSection != XPR_NULL);

        FolderLayoutManager::saveToConfFile(aFolderLayout, sConfFile, sSection, kBaseKey);

        sResult = sConfFile.save(xpr::CharSetUtf16);

        if (XPR_IS_FALSE(sInstalledPath))
        {
            // set system and hidden file attributes
            DWORD sFileAttributes = ::GetFileAttributes(sIniPath.c_str());
            sFileAttributes |= FILE_ATTRIBUTE_HIDDEN;
            sFileAttributes |= FILE_ATTRIBUTE_SYSTEM;
            ::SetFileAttributes(sIniPath.c_str(), sFileAttributes);
        }
    }

    return sResult;
}

void FolderLayoutManager::saveToConfFile(const FolderLayout &aFolderLayout, fxfile::base::ConfFileEx &aConfFile, base::ConfFile::Section *aSection, const xpr_tchar_t *aBaseKey)
{
    XPR_ASSERT(aSection != XPR_NULL);

    xpr_sint_t   i;
    xpr::string  sKey;
    xpr::string  sStyleKey(kStyleKey);
    xpr::string  sSortFormatIdKey(kSortFormatIdKey);
    xpr::string  sSortPropertyIdKey(kSortPropertyIdKey);
    xpr::string  sSortAscendingKey(kSortAscendingKey);
    xpr::string  sColumnCountKey(kColumnCountKey);
    xpr::string  sFormatIdKey;
    xpr::string  sPropertyIdKey;
    xpr::string  sWidthKey;
    xpr::string  sValue;

    aFolderLayout.mSortColumnId.mFormatId.toString(sValue, XPR_FALSE);

    if (XPR_IS_NOT_NULL(aBaseKey))
    {
        sStyleKey         .insert(0, XPR_STRING_LITERAL("."));
        sSortFormatIdKey  .insert(0, XPR_STRING_LITERAL("."));
        sSortPropertyIdKey.insert(0, XPR_STRING_LITERAL("."));
        sSortAscendingKey .insert(0, XPR_STRING_LITERAL("."));
        sColumnCountKey   .insert(0, XPR_STRING_LITERAL("."));

        sStyleKey         .insert(0, aBaseKey);
        sSortFormatIdKey  .insert(0, aBaseKey);
        sSortPropertyIdKey.insert(0, aBaseKey);
        sSortAscendingKey .insert(0, aBaseKey);
        sColumnCountKey   .insert(0, aBaseKey);
    }

    aConfFile.setValueI(aSection, sStyleKey.c_str(),          aFolderLayout.mViewStyle);
    aConfFile.setValueS(aSection, sSortFormatIdKey.c_str(),   sValue);
    aConfFile.setValueI(aSection, sSortPropertyIdKey.c_str(), aFolderLayout.mSortColumnId.mPropertyId);
    aConfFile.setValueI(aSection, sSortAscendingKey.c_str(),  aFolderLayout.mSortAscending);
    aConfFile.setValueI(aSection, sColumnCountKey.c_str(),    aFolderLayout.mColumnCount);

    for (i = 0; i < aFolderLayout.mColumnCount; ++i)
    {
        if (aFolderLayout.mColumnItem[i].mPropertyId == -1 || aFolderLayout.mColumnItem[i].mWidth == -1)
            continue;

        sValue.clear();
        aFolderLayout.mColumnItem[i].mFormatId.toString(sValue, XPR_FALSE);

        sFormatIdKey  .format(kFormatIdKey,   i + 1);
        sPropertyIdKey.format(kPropertyIdKey, i + 1);
        sWidthKey     .format(kWidthKey,      i + 1);

        if (XPR_IS_NOT_NULL(aBaseKey))
        {
            sFormatIdKey  .insert(0, XPR_STRING_LITERAL("."));
            sPropertyIdKey.insert(0, XPR_STRING_LITERAL("."));
            sWidthKey     .insert(0, XPR_STRING_LITERAL("."));

            sFormatIdKey  .insert(0, aBaseKey);
            sPropertyIdKey.insert(0, aBaseKey);
            sWidthKey     .insert(0, aBaseKey);
        }

        aConfFile.setValueS(aSection, sFormatIdKey.c_str(),   sValue);
        aConfFile.setValueI(aSection, sPropertyIdKey.c_str(), aFolderLayout.mColumnItem[i].mPropertyId);
        aConfFile.setValueI(aSection, sWidthKey.c_str(),      aFolderLayout.mColumnItem[i].mWidth);
    }
}

void FolderLayoutManager::setSaveLocation(xpr_bool_t aInstalledPath)
{
    mInstalledPath = aInstalledPath;
}

void FolderLayoutManager::verify(void)
{
    if (XPR_IS_FALSE(mInstalledPath))
        return;

    {
        xpr_tchar_t sDriveChar;
        SubSet::iterator sIterator;

        sIterator = mSubSet.begin();
        while (sIterator != mSubSet.end())
        {
            const xpr::string &sPath = *sIterator;

            if (sPath.length() < 2)
            {
                mSubSet.erase(sIterator++);
                continue;
            }

            if (sPath.at(1) == XPR_STRING_LITERAL(':'))
            {
                sDriveChar = sPath.at(0);
                if (XPR_IS_RANGE(XPR_STRING_LITERAL('a'), sDriveChar, XPR_STRING_LITERAL('z')) ||
                    XPR_IS_RANGE(XPR_STRING_LITERAL('A'), sDriveChar, XPR_STRING_LITERAL('Z')))
                {
                    if (IsExistFile(sPath) == XPR_FALSE)
                    {
                        mSubSet.erase(sIterator++);
                        continue;
                    }
                }
            }

            sIterator++;
        }
    }

    {
        xpr_tchar_t sDriveChar;
        IndexMap::iterator sIterator;

        sIterator = mIndexMap.begin();
        while (sIterator != mIndexMap.end())
        {
            const xpr::string &sPath = sIterator->first;

            if (sPath.length() < 2)
            {
                mIndexMap.erase(sIterator++);
                continue;
            }

            if (sPath.at(1) == XPR_STRING_LITERAL(':'))
            {
                sDriveChar = sPath.at(0);
                if (XPR_IS_RANGE(XPR_STRING_LITERAL('a'), sDriveChar, XPR_STRING_LITERAL('z')) ||
                    XPR_IS_RANGE(XPR_STRING_LITERAL('A'), sDriveChar, XPR_STRING_LITERAL('Z')))
                {
                    if (IsExistFile(sPath) == XPR_FALSE)
                    {
                        mIndexMap.erase(sIterator++);
                        continue;
                    }
                }
            }

            sIterator++;
        }
    }

    typedef std::tr1::unordered_set<xpr_uint64_t> HashSet;
    HashSet sHashSet;

    {
        HashMap::iterator sIterator;

        FXFILE_STL_FOR_EACH(sIterator, mHashMap)
        {
            const xpr_uint64_t &sHashValue = sIterator->first;

            sHashSet.insert(sHashValue);
        }
    }

    {
        IndexMap::iterator sIterator;

        sIterator = mIndexMap.begin();
        while (sIterator != mIndexMap.end())
        {
            const xpr_uint64_t &sHashValue = sIterator->second;

            if (sHashSet.find(sHashValue) == sHashSet.end())
            {
                mIndexMap.erase(sIterator++);
                continue;
            }

            sIterator++;
        }
    }
}

xpr_uint64_t FolderLayoutManager::generateHashValue(const FolderLayout &aFolderLayout) const
{
    xpr_uint64_t sBaseHashValue = aFolderLayout.getHashValue();
    xpr_uint64_t sHashValue;
    xpr_uint64_t sId = 0;
    HashMap::const_iterator sIterator;

    while (XPR_TRUE)
    {
        sHashValue = sId++ << 32 | sBaseHashValue;

        sIterator = mHashMap.find(sHashValue);
        if (sIterator != mHashMap.end())
        {
            const FolderLayout *sFolderLayout = sIterator->second;
            if (*sFolderLayout == aFolderLayout)
            {
                // value is equal
                break;
            }
        }
        else
        {
            break;
        }
    }

    return sHashValue;
}
} // namespace fxfile
