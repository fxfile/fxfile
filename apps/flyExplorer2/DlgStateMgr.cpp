//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "DlgStateMgr.h"

#include "fxb/fxb_ini_file_ex.h"

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

    fxb::IniFileEx sIniFile(mPath.c_str());

    if (sIniFile.readFile() == XPR_FALSE)
        return XPR_FALSE;

    xpr_size_t                     i, j;
    const xpr_tchar_t             *sSectionName;
    const xpr_tchar_t             *sKeyName;
    const xpr_tchar_t             *sValue;
    fxb::IniFile::Section         *sSection;
    fxb::IniFile::Key             *sKey;
    fxb::IniFile::SectionIterator  sSectionIterator;
    fxb::IniFile::KeyIterator      sKeyIterator;
    DlgState                      *sDlgState;
    DlgState::ValueMap             sValueMap;
    DlgStateMap::iterator          sIterator;

    sSectionIterator = sIniFile.beginSection();
    for (i = 0; sSectionIterator != sIniFile.endSection(); ++i, ++sSectionIterator)
    {
        sSection = *sSectionIterator;
        XPR_ASSERT(sSection != XPR_NULL);

        sSectionName = sIniFile.getSectionName(sSection);
        XPR_ASSERT(sSectionName != XPR_NULL);

        sIterator = mDlgStateMap.find(sSectionName);
        if (sIterator != mDlgStateMap.end())
        {
            sDlgState = sIterator->second;
            XPR_SAFE_DELETE(sDlgState);

            mDlgStateMap.erase(sIterator);
        }

        sDlgState = new DlgState(sSectionName);
        if (XPR_IS_NULL(sDlgState))
            continue;

        sValueMap.clear();

        sKeyIterator = sIniFile.beginKey(sSection);
        for (j = 0; sKeyIterator != sIniFile.endKey(sSection); ++j, ++sKeyIterator)
        {
            sKey = *sKeyIterator;
            XPR_ASSERT(sKey != XPR_NULL);

            sKeyName = sIniFile.getKeyName(sKey);
            XPR_ASSERT(sKeyName != XPR_NULL);

            sValue = sIniFile.getValueS(sSection, sKeyName, XPR_NULL);
            if (XPR_IS_NOT_NULL(sValue))
            {
                sValueMap[sKeyName] = sValue;
            }
        }

        sDlgState->setValueMap(sValueMap);

        mDlgStateMap[sSectionName] = sDlgState;
    }

    return XPR_TRUE;
}

void DlgStateMgr::save(void) const
{
    xpr_tchar_t sPath[XPR_MAX_PATH] = {0};
    CfgPath::instance().getSavePath(CfgPath::TypeDlgState, sPath, XPR_MAX_PATH);

    fxb::IniFileEx sIniFile(sPath);

    fxb::IniFile::Section *sSection;
    const DlgState        *sDlgState;
    DlgStateMap::const_iterator sIterator;
    DlgState::ValueMap::const_iterator sValueIterator;

    sIterator = mDlgStateMap.begin();
    for (; sIterator != mDlgStateMap.end(); ++sIterator)
    {
        sDlgState = sIterator->second;
        if (XPR_IS_NULL(sDlgState))
            continue;

        const std::tstring &sSectionName = sDlgState->getSection();
        const DlgState::ValueMap &sValueMap = sDlgState->getValueMap();

        sSection = sIniFile.addSection(sSectionName.c_str());
        XPR_ASSERT(sSection != XPR_NULL);

        sValueIterator = sValueMap.begin();
        for (; sValueIterator != sValueMap.end(); ++sValueIterator)
        {
            const std::tstring &sKey   = sValueIterator->first;
            const std::tstring &sValue = sValueIterator->second;

            sIniFile.setValueS(sSection, sKey.c_str(), sValue);
        }
    }

    sIniFile.writeFile(xpr::CharSetUtf16);
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
