//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "dlg_state_manager.h"

#include "conf_file_ex.h"

#include "dlg_state.h"
#include "conf_dir.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace fxfile;
using namespace fxfile::base;

namespace fxfile
{
DlgStateManager::DlgStateManager(void)
    : mLoaded(XPR_FALSE)
{
}

DlgStateManager::~DlgStateManager(void)
{
    clear();
}

void DlgStateManager::setPath(const xpr_tchar_t *aPath)
{
    if (XPR_IS_NOT_NULL(aPath))
        mPath = aPath;
}

xpr_bool_t DlgStateManager::load(void)
{
    clear();

    fxfile::base::ConfFileEx sConfFile(mPath.c_str());

    if (sConfFile.load() == XPR_FALSE)
        return XPR_FALSE;

    xpr_size_t                 i, j;
    const xpr_tchar_t         *sSectionName;
    const xpr_tchar_t         *sKeyName;
    const xpr_tchar_t         *sValue;
    ConfFile::Section         *sSection;
    ConfFile::Key             *sKey;
    ConfFile::SectionIterator  sSectionIterator;
    ConfFile::KeyIterator      sKeyIterator;
    DlgState                  *sDlgState;
    DlgState::ValueMap         sValueMap;
    DlgStateMap::iterator      sIterator;

    sSectionIterator = sConfFile.beginSection();
    for (i = 0; sSectionIterator != sConfFile.endSection(); ++i, ++sSectionIterator)
    {
        sSection = *sSectionIterator;
        XPR_ASSERT(sSection != XPR_NULL);

        sSectionName = sConfFile.getSectionName(sSection);
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

        sKeyIterator = sConfFile.beginKey(sSection);
        for (j = 0; sKeyIterator != sConfFile.endKey(sSection); ++j, ++sKeyIterator)
        {
            sKey = *sKeyIterator;
            XPR_ASSERT(sKey != XPR_NULL);

            sKeyName = sConfFile.getKeyName(sKey);
            XPR_ASSERT(sKeyName != XPR_NULL);

            sValue = sConfFile.getValueS(sSection, sKeyName, XPR_NULL);
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

void DlgStateManager::save(void) const
{
    xpr_tchar_t sPath[XPR_MAX_PATH] = {0};
    ConfDir::instance().getSavePath(ConfDir::TypeDlgState, sPath, XPR_MAX_PATH);

    fxfile::base::ConfFileEx sConfFile(sPath);

    ConfFile::Section *sSection;
    const DlgState    *sDlgState;
    DlgStateMap::const_iterator sIterator;
    DlgState::ValueMap::const_iterator sValueIterator;

    sIterator = mDlgStateMap.begin();
    for (; sIterator != mDlgStateMap.end(); ++sIterator)
    {
        sDlgState = sIterator->second;
        if (XPR_IS_NULL(sDlgState))
            continue;

        const xpr::string &sSectionName = sDlgState->getSection();
        const DlgState::ValueMap &sValueMap = sDlgState->getValueMap();

        sSection = sConfFile.addSection(sSectionName.c_str());
        XPR_ASSERT(sSection != XPR_NULL);

        sValueIterator = sValueMap.begin();
        for (; sValueIterator != sValueMap.end(); ++sValueIterator)
        {
            const xpr::string &sKey   = sValueIterator->first;
            const xpr::string &sValue = sValueIterator->second;

            sConfFile.setValueS(sSection, sKey.c_str(), sValue);
        }
    }

    sConfFile.save(xpr::CharSetUtf16);
}

DlgState *DlgStateManager::getDlgState(const xpr_tchar_t *aSection, xpr_bool_t aCreateIfNotExist)
{
    if (XPR_IS_NULL(aSection))
        return XPR_NULL;

    if (XPR_IS_FALSE(mLoaded))
    {
        xpr_tchar_t sPath[XPR_MAX_PATH] = {0};
        ConfDir::instance().getLoadPath(ConfDir::TypeDlgState, sPath, XPR_MAX_PATH);

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

void DlgStateManager::clear(void)
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
} // namespace fxfile
