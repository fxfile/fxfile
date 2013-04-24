//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "fxb_recent_file_list.h"

#include "fxb_ini_file_ex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace fxb
{
static const xpr_tchar_t kRecentFileListSection[] = XPR_STRING_LITERAL("recent_file_list");
static const xpr_tchar_t kFileKey              [] = XPR_STRING_LITERAL("recent_file%d");

RecentFileList::RecentFileList(void)
{
}

RecentFileList::~RecentFileList(void)
{
    clear();
}

void RecentFileList::addFile(const xpr_tchar_t *aPath)
{
    if (XPR_IS_NULL(aPath))
        return;

    mFileDeque.push_front(aPath);
}

xpr_size_t RecentFileList::getFileCount(void) const
{
    return mFileDeque.size();
}

const xpr_tchar_t *RecentFileList::getFile(xpr_size_t aIndex) const
{
    if (!FXFILE_STL_IS_INDEXABLE(aIndex, mFileDeque))
        return XPR_NULL;

    return mFileDeque[aIndex].c_str();
}

void RecentFileList::clear(void)
{
    mFileDeque.clear();
}

void RecentFileList::load(fxb::IniFileEx &aIniFile)
{
    clear();

    xpr_sint_t             i;
    xpr_tchar_t            sKey[XPR_MAX_PATH + 1];
    const xpr_tchar_t     *sValue;
    fxb::IniFile::Section *sSection;

    sSection = aIniFile.findSection(kRecentFileListSection);
    if (XPR_IS_NOT_NULL(sSection))
    {
        for (i = 0; ; ++i)
        {
            _stprintf(sKey, kFileKey, i + 1);

            sValue = aIniFile.getValueS(sSection, sKey, XPR_NULL);
            if (XPR_IS_NULL(sValue))
                break;

            mFileDeque.push_back(sValue);
        }
    }
}

void RecentFileList::save(fxb::IniFileEx &aIniFile) const
{
    xpr_sint_t             i;
    xpr_tchar_t            sKey[XPR_MAX_PATH + 1];
    fxb::IniFile::Section *sSection;
    FileDeque::const_iterator sIterator;

    sSection = aIniFile.addSection(kRecentFileListSection);
    XPR_ASSERT(sSection != XPR_NULL);

    sIterator = mFileDeque.begin();
    for (i = 0; sIterator != mFileDeque.end(); ++sIterator, ++i)
    {
        const std::tstring &sPath = *sIterator;

        _stprintf(sKey, kFileKey, i + 1);

        aIniFile.setValueS(sSection, sKey, sPath);
    }
}
} // namespace fxb
