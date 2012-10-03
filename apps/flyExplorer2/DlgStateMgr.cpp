//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "DlgStateMgr.h"

#include "fxb/fxb_ini_file.h"

#include "DlgState.h"
#include "CfgPath.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

DlgStateMgr::DlgStateMgr(void)
    : mLoaded(XPR_FALSE)
{
}

DlgStateMgr::~DlgStateMgr(void)
{
    clear();
}

void DlgStateMgr::setPath(const xpr_tchar_t *aPath)
{
    if (XPR_IS_NOT_NULL(aPath))
        mPath = aPath;
}

xpr_bool_t DlgStateMgr::load(void)
{
    clear();

    fxb::IniFile sIniFile(mPath.c_str());

    if (sIniFile.readFileW() == XPR_FALSE)
        return XPR_FALSE;

    xpr_size_t i, j;
    xpr_size_t sKeyCount;
    xpr_size_t sEntryCount;
    const xpr_tchar_t *sKeyName;
    const xpr_tchar_t *sEntryName;
    const xpr_tchar_t *sValue;
    DlgState *sDlgState;
    DlgState::ValueMap sValueMap;
    DlgStateMap::iterator sIterator;

    sKeyCount = sIniFile.getKeyCount();
    for (i = 0; i < sKeyCount; ++i)
    {
        sKeyName = sIniFile.getKeyName(i);
        if (XPR_IS_NULL(sKeyName))
            continue;

        sIterator = mDlgStateMap.find(sKeyName);
        if (sIterator != mDlgStateMap.end())
        {
            sDlgState = sIterator->second;
            XPR_SAFE_DELETE(sDlgState);

            mDlgStateMap.erase(sIterator);
        }

        sDlgState = new DlgState(sKeyName);
        if (XPR_IS_NULL(sDlgState))
            continue;

        sValueMap.clear();

        sEntryCount = sIniFile.getEntryCount(i);
        for (j = 0; j < sEntryCount; ++j)
        {
            sEntryName = sIniFile.getEntryName(i, j);
            if (XPR_IS_NULL(sEntryName))
                continue;

            sValue = sIniFile.getValueS(sKeyName, sEntryName);
            if (XPR_IS_NOT_NULL(sValue))
            {
                sValueMap[sEntryName] = sValue;
            }
        }

        sDlgState->setValueMap(sValueMap);

        mDlgStateMap[sKeyName] = sDlgState;
    }

    return XPR_TRUE;
}

void DlgStateMgr::save(void) const
{
    xpr_tchar_t sPath[XPR_MAX_PATH] = {0};
    CfgPath::instance().getSavePath(CfgPath::TypeDlgState, sPath, XPR_MAX_PATH);

    fxb::IniFile sIniFile(sPath);

    const DlgState *sDlgState;
    DlgStateMap::const_iterator sIterator;
    DlgState::ValueMap::const_iterator sValueIterator;

    sIterator = mDlgStateMap.begin();
    for (; sIterator != mDlgStateMap.end(); ++sIterator)
    {
        sDlgState = sIterator->second;
        if (XPR_IS_NULL(sDlgState))
            continue;

        const std::tstring &sKey = sDlgState->getSection();
        const DlgState::ValueMap &sValueMap = sDlgState->getValueMap();

        sValueIterator = sValueMap.begin();
        for (; sValueIterator != sValueMap.end(); ++sValueIterator)
        {
            const std::tstring &sEntry = sValueIterator->first;
            const std::tstring &sValue = sValueIterator->second;

            sIniFile.setValueS(sKey.c_str(), sEntry.c_str(), sValue.c_str());
        }
    }

    sIniFile.writeFileW();
}

DlgState *DlgStateMgr::getDlgState(const xpr_tchar_t *aSection, xpr_bool_t aCreateIfNotExist)
{
    if (XPR_IS_NULL(aSection))
        return XPR_NULL;

    if (XPR_IS_FALSE(mLoaded))
    {
        xpr_tchar_t sPath[XPR_MAX_PATH] = {0};
        CfgPath::instance().getLoadPath(CfgPath::TypeDlgState, sPath, XPR_MAX_PATH);

        setPath(sPath);

        load();

        mLoaded = XPR_TRUE;
    }

    DlgStateMap::iterator sIterator = mDlgStateMap.find(aSection);
    if (sIterator == mDlgStateMap.end())
    {
        if (XPR_IS_TRUE(aCreateIfNotExist))
        {
            DlgState *sDlgState = new DlgState(aSection);
            if (XPR_IS_NULL(sDlgState))
                return XPR_NULL;

            mDlgStateMap[aSection] = sDlgState;

            return sDlgState;
        }

        return XPR_NULL;
    }

    return sIterator->second;
}

void DlgStateMgr::clear(void)
{
    DlgState *sDlgState;
    DlgStateMap::iterator sIterator;

    sIterator = mDlgStateMap.begin();
    for (; sIterator != mDlgStateMap.end(); ++sIterator)
    {
        sDlgState = sIterator->second;
        XPR_SAFE_DELETE(sDlgState);
    }

    mDlgStateMap.clear();
}
