//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "fxb_program_ass.h"

#include "fxb_filter.h"
#include "fxb_ini_file.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace fxb
{
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
    if (!XPR_STL_IS_INDEXABLE(aIndex, mProgramAssDeque))
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

static const xpr_tchar_t _fx_Type[]       = XPR_STRING_LITERAL("Type");
static const xpr_tchar_t _fx_Method[]     = XPR_STRING_LITERAL("Method");
static const xpr_tchar_t _fx_Extension[]  = XPR_STRING_LITERAL("Extension");
static const xpr_tchar_t _fx_FilterName[] = XPR_STRING_LITERAL("Filter Name");
static const xpr_tchar_t _fx_Program[]    = XPR_STRING_LITERAL("Program");

xpr_bool_t ProgramAss::loadFromFile(const xpr_tchar_t *aPath)
{
    IniFile sIniFile(aPath);
    if (sIniFile.readFile() == XPR_FALSE)
        return XPR_FALSE;

    xpr_size_t sCount = sIniFile.getKeyCount();
    if (sCount <= 0)
        return XPR_FALSE;

    sCount = min(sCount, MAX_PROGRAM_ASS);

    xpr_size_t i;
    const xpr_tchar_t *sKey;
    ProgramAssItem *sProgramAssItem;

    for (i = 0; i < sCount; ++i)
    {
        sProgramAssItem = new ProgramAssItem;

        sProgramAssItem->mName = sIniFile.getKeyName(i);
        sKey = sProgramAssItem->mName.c_str();

        sProgramAssItem->mType    = sIniFile.getValueU(sKey, _fx_Type, ProgramAssTypeNone);
        sProgramAssItem->mMethod  = sIniFile.getValueU(sKey, _fx_Method, ProgramAssMethodExt);

        if (sProgramAssItem->mMethod == ProgramAssMethodExt)
            sProgramAssItem->mExts = sIniFile.getValueS(sKey, _fx_Extension, XPR_STRING_LITERAL(""));
        else
            sProgramAssItem->mExts = sIniFile.getValueS(sKey, _fx_FilterName, XPR_STRING_LITERAL(""));

        sProgramAssItem->mPath = sIniFile.getValueS(sKey, _fx_Program, XPR_STRING_LITERAL(""));

        addItem(sProgramAssItem);
    }

    return XPR_TRUE;
}

xpr_bool_t ProgramAss::saveToFile(const xpr_tchar_t *aPath)
{
    IniFile sIniFile(aPath);
    sIniFile.setComment(XPR_STRING_LITERAL("flyExplorer File Association File"));

    const xpr_tchar_t *sKey;
    ProgramAssItem *sProgramAssItem;
    ProgramAssDeque::iterator sIterator;

    sIterator = mProgramAssDeque.begin();
    for (; sIterator != mProgramAssDeque.end(); ++sIterator)
    {
        sProgramAssItem = *sIterator;
        if (XPR_IS_NULL(sProgramAssItem))
            continue;

        sKey = sProgramAssItem->mName.c_str();
        sIniFile.addKeyName(sKey);

        sIniFile.setValueU(sKey, _fx_Type,   sProgramAssItem->mType);
        sIniFile.setValueU(sKey, _fx_Method, sProgramAssItem->mMethod);

        if (sProgramAssItem->mMethod == ProgramAssMethodExt)
            sIniFile.setValueS(sKey, _fx_Extension, sProgramAssItem->mExts.c_str());
        else
            sIniFile.setValueS(sKey, _fx_FilterName, sProgramAssItem->mFilterName.c_str());

        sIniFile.setValueS(sKey, _fx_Program, sProgramAssItem->mPath.c_str());
    }

    sIniFile.writeFile(xpr::CharSetUtf16);

    return XPR_TRUE;
}
} // namespace fxb
