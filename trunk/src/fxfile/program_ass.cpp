//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "program_ass.h"
#include "filter.h"

#include "conf_file_ex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace fxfile;
using namespace fxfile::base;

namespace fxfile
{
static const xpr_tchar_t kProgramAssSection[] = XPR_STRING_LITERAL("program_ass");
static const xpr_tchar_t kNameKey          [] = XPR_STRING_LITERAL("program_ass.item%d.name");
static const xpr_tchar_t kTypeKey          [] = XPR_STRING_LITERAL("program_ass.item%d.type");
static const xpr_tchar_t kMethodKey        [] = XPR_STRING_LITERAL("program_ass.item%d.method");
static const xpr_tchar_t kExtensionKey     [] = XPR_STRING_LITERAL("program_ass.item%d.extension");
static const xpr_tchar_t kFilterKey        [] = XPR_STRING_LITERAL("program_ass.item%d.filter");
static const xpr_tchar_t kProgramKey       [] = XPR_STRING_LITERAL("program_ass.item%d.program");

ProgramAssMgr::ProgramAssMgr(void)
{
}

ProgramAssMgr::~ProgramAssMgr(void)
{
}

ProgramAss::ProgramAss(void)
{
}

ProgramAss::~ProgramAss(void)
{
    clear();
}

void ProgramAss::addItem(ProgramAssItem *aProgramAssItem)
{
    if (XPR_IS_NOT_NULL(aProgramAssItem))
        mProgramAssDeque.push_back(aProgramAssItem);
}

ProgramAssItem *ProgramAss::getItem(xpr_size_t aIndex)
{
    if (!FXFILE_STL_IS_INDEXABLE(aIndex, mProgramAssDeque))
        return XPR_NULL;

    return mProgramAssDeque[aIndex];
}

ProgramAssItem *ProgramAss::getItemFromExt(const xpr_tchar_t *aExt, xpr_uint_t aType)
{
    xpr_size_t sIndex = indexFromExt(aExt, aType);
    if (sIndex == -1)
        return XPR_NULL;

    return getItem(sIndex);
}

ProgramAssItem *ProgramAss::getItemFromPath(const xpr_tchar_t *aPath, xpr_uint_t aType)
{
    xpr_size_t sIndex = indexFromPath(aPath, aType);
    if (sIndex == -1)
        return XPR_NULL;

    return getItem(sIndex);
}

xpr_size_t ProgramAss::indexFromExt(const xpr_tchar_t *aExt, xpr_uint_t aType)
{
    FilterMgr &sFilterMgr = FilterMgr::instance();

    const xpr_tchar_t *sExts;
    FilterItem *sFilterItem;
    ProgramAssItem *sProgramAssItem;
    ProgramAssDeque::iterator sIterator;

    if (XPR_IS_NOT_NULL(aExt))
    {
        sIterator = mProgramAssDeque.begin();
        for (; sIterator != mProgramAssDeque.end(); ++sIterator)
        {
            sProgramAssItem = *sIterator;
            if (XPR_IS_NULL(sProgramAssItem))
                continue;

            if (!XPR_ANY_BITS(sProgramAssItem->mType, aType))
                continue;

            sExts = sProgramAssItem->mExts.c_str();
            if (sProgramAssItem->mMethod == ProgramAssMethodFilter)
            {
                sFilterItem = sFilterMgr.getFilterFromName(sProgramAssItem->mFilterName);
                if (XPR_IS_NOT_NULL(sFilterItem))
                    sExts = sFilterItem->mExts.c_str();
            }

            if (_tcsicmp(sExts, XPR_STRING_LITERAL("*.*")) == 0)
                continue;

            if (IsEqualFilter(sExts, aExt) == XPR_TRUE)
                return std::distance(mProgramAssDeque.begin(), sIterator);
        }
    }

    sIterator = mProgramAssDeque.begin();
    for (; sIterator != mProgramAssDeque.end(); ++sIterator)
    {
        sProgramAssItem = *sIterator;
        if (XPR_IS_NULL(sProgramAssItem))
            continue;

        if (!XPR_ANY_BITS(sProgramAssItem->mType, aType))
            continue;

        sExts = sProgramAssItem->mExts.c_str();
        if (sProgramAssItem->mMethod == ProgramAssMethodFilter)
        {
            sFilterItem = sFilterMgr.getFilterFromName(sProgramAssItem->mFilterName);
            if (XPR_IS_NOT_NULL(sFilterItem))
                sExts = sFilterItem->mExts.c_str();
        }

        if (_tcsicmp(sExts, XPR_STRING_LITERAL("*.*")) != 0)
            continue;

        if (IsEqualFilter(sExts, aExt) == XPR_TRUE)
            return std::distance(mProgramAssDeque.begin(), sIterator);
    }

    return -1;
}

xpr_size_t ProgramAss::indexFromPath(const xpr_tchar_t *aPath, xpr_uint_t aType)
{
    if (XPR_IS_NULL(aPath))
        return -1;

    const xpr_tchar_t *sExt = GetFileExt(aPath);
    if (XPR_IS_NULL(sExt))
        return -1;

    return indexFromExt(sExt, aType);
}

xpr_bool_t ProgramAss::getItemFromExt(const xpr_tchar_t *aExt, xpr_uint_t aType, ProgramAssDeque &aProgramAssDeque)
{
    FilterMgr &sFilterMgr = FilterMgr::instance();

    const xpr_tchar_t *sExts;
    FilterItem *sFilterItem;
    ProgramAssItem *sProgramAssItem;
    ProgramAssDeque::iterator sIterator;

    sIterator = mProgramAssDeque.begin();
    for (; sIterator != mProgramAssDeque.end(); ++sIterator)
    {
        sProgramAssItem = *sIterator;
        if (XPR_IS_NULL(sProgramAssItem))
            continue;

        if (!XPR_ANY_BITS(sProgramAssItem->mType, aType))
            continue;

        sExts = sProgramAssItem->mExts.c_str();
        if (sProgramAssItem->mMethod == ProgramAssMethodFilter)
        {
            sFilterItem = sFilterMgr.getFilterFromName(sProgramAssItem->mFilterName);
            if (XPR_IS_NOT_NULL(sFilterItem))
                sExts = sFilterItem->mExts.c_str();
        }

        if ((_tcsicmp(sExts, XPR_STRING_LITERAL("*.*")) == 0) ||
            (XPR_IS_NOT_NULL(aExt) && IsEqualFilter(sExts, aExt) == XPR_TRUE))
        {
            aProgramAssDeque.push_back(sProgramAssItem);
        }
    }

    return (aProgramAssDeque.empty() == false) ? XPR_TRUE : XPR_FALSE;
}

xpr_bool_t ProgramAss::getItemFromPath(const xpr_tchar_t *aPath, xpr_uint_t aType, ProgramAssDeque &aProgramAssDeque)
{
    if (XPR_IS_NULL(aPath))
        return -1;

    const xpr_tchar_t *sExt = GetFileExt(aPath);
    if (XPR_IS_NULL(sExt))
        return -1;

    return getItemFromExt(sExt, aType, aProgramAssDeque);
}

xpr_size_t ProgramAss::getCount(void)
{
    return mProgramAssDeque.size();
}

void ProgramAss::clear(void)
{
    ProgramAssItem *sProgramAssItem;
    ProgramAssDeque::iterator sIterator;

    sIterator = mProgramAssDeque.begin();
    for (; sIterator != mProgramAssDeque.end(); ++sIterator)
    {
        sProgramAssItem = *sIterator;
        XPR_SAFE_DELETE(sProgramAssItem);
    }

    mProgramAssDeque.clear();
}

void ProgramAss::initDefault(void)
{
    clear();

    ProgramAssItem *sProgramAssItem;

    sProgramAssItem = new ProgramAssItem;
    sProgramAssItem->mName   = XPR_STRING_LITERAL("Default viewer");
    sProgramAssItem->mType   = ProgramAssTypeViewer;
    sProgramAssItem->mMethod = ProgramAssMethodExt;
    sProgramAssItem->mExts   = XPR_STRING_LITERAL("*.*");
    sProgramAssItem->mPath   = XPR_STRING_LITERAL("%Windows%\\notepad.exe");
    addItem(sProgramAssItem);

    sProgramAssItem = new ProgramAssItem;
    sProgramAssItem->mName   = XPR_STRING_LITERAL("Default editor");
    sProgramAssItem->mType   = ProgramAssTypeEditor;
    sProgramAssItem->mMethod = ProgramAssMethodExt;
    sProgramAssItem->mExts   = XPR_STRING_LITERAL("*.*");
    sProgramAssItem->mPath   = XPR_STRING_LITERAL("%Windows%\\notepad.exe");
    addItem(sProgramAssItem);
}

xpr_bool_t ProgramAss::load(fxfile::base::ConfFileEx &aConfFile)
{
    xpr_size_t         i;
    xpr_tchar_t        sKey[0xff];
    const xpr_tchar_t *sValue;
    ConfFile::Section *sSection;
    ProgramAssItem    *sProgramAssItem;

    sSection = aConfFile.findSection(kProgramAssSection);
    if (XPR_IS_NULL(sSection))
        return XPR_FALSE;

    for (i = 0; i < MAX_PROGRAM_ASS; ++i)
    {
        _stprintf(sKey, kNameKey, i + 1);

        sValue = aConfFile.getValueS(sSection, sKey, XPR_NULL);
        if (XPR_IS_NULL(sValue))
            break;

        sProgramAssItem = new ProgramAssItem;
        XPR_ASSERT(sProgramAssItem != XPR_NULL);

        sProgramAssItem->mName = sValue;

        _stprintf(sKey, kTypeKey, i + 1);
        sProgramAssItem->mType = aConfFile.getValueI(sSection, sKey, ProgramAssTypeNone);

        _stprintf(sKey, kMethodKey, i + 1);
        sProgramAssItem->mMethod = aConfFile.getValueI(sSection, sKey, ProgramAssMethodExt);

        if (sProgramAssItem->mMethod == ProgramAssMethodExt)
        {
            _stprintf(sKey, kExtensionKey, i + 1);
            sProgramAssItem->mExts = aConfFile.getValueS(sSection, sKey, XPR_STRING_LITERAL(""));
        }
        else
        {
            _stprintf(sKey, kFilterKey, i + 1);
            sProgramAssItem->mExts = aConfFile.getValueS(sSection, sKey, XPR_STRING_LITERAL(""));
        }

        _stprintf(sKey, kProgramKey, i + 1);
        sProgramAssItem->mPath = aConfFile.getValueS(sSection, sKey, XPR_STRING_LITERAL(""));

        addItem(sProgramAssItem);
    }

    return XPR_TRUE;
}

void ProgramAss::save(fxfile::base::ConfFileEx &aConfFile) const
{
    xpr_sint_t         i;
    xpr_tchar_t        sKey[0xff];
    ConfFile::Section *sSection;
    ProgramAssItem    *sProgramAssItem;
    ProgramAssDeque::const_iterator sIterator;

    sSection = aConfFile.addSection(kProgramAssSection);
    XPR_ASSERT(sSection != XPR_NULL);

    sIterator = mProgramAssDeque.begin();
    for (i = 0; sIterator != mProgramAssDeque.end(); ++sIterator, ++i)
    {
        sProgramAssItem = *sIterator;
        XPR_ASSERT(sProgramAssItem != XPR_NULL);

        _stprintf(sKey, kNameKey, i + 1);
        aConfFile.setValueS(sSection, sKey, sProgramAssItem->mName);

        _stprintf(sKey, kTypeKey, i + 1);
        aConfFile.setValueI(sSection, sKey, sProgramAssItem->mType);

        _stprintf(sKey, kMethodKey, i + 1);
        aConfFile.setValueI(sSection, sKey, sProgramAssItem->mMethod);

        if (sProgramAssItem->mMethod == ProgramAssMethodExt)
        {
            _stprintf(sKey, kExtensionKey, i + 1);
            aConfFile.setValueS(sSection, sKey, sProgramAssItem->mExts);
        }
        else
        {
            _stprintf(sKey, kFilterKey, i + 1);
            aConfFile.setValueS(sSection, sKey, sProgramAssItem->mFilterName);
        }

        _stprintf(sKey, kProgramKey, i + 1);
        aConfFile.setValueS(sSection, sKey, sProgramAssItem->mPath);
    }
}
} // namespace fxfile
