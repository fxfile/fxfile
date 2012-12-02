//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "fxb_history.h"

#include "fxb_ini_file.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace fxb
{
History::History(void)
    : mMaxHistory(0xffffffff)
    , mMaxBackward(0xffffffff)
{
}

History::~History(void)
{
    clear();
}

static const xpr_tchar_t _fx_key_backward[] = XPR_STRING_LITERAL("Backward");
static const xpr_tchar_t _fx_key_forward[]  = XPR_STRING_LITERAL("Forward");
static const xpr_tchar_t _fx_key_history[]  = XPR_STRING_LITERAL("History");

xpr_bool_t History::loadFromFile(const xpr_tchar_t *aPath)
{
    IniFile sIniFile(aPath);
    if (sIniFile.readFile() == XPR_FALSE)
        return XPR_FALSE;

    xpr_size_t i, j;
    xpr_size_t sKeyCount;
    xpr_size_t sEntryCount;
    const xpr_tchar_t *sKey;
    const xpr_tchar_t *sEntry;
    const xpr_tchar_t *sValue;
    HistoryDeque *sHistoryDeque;
    LPITEMIDLIST sFullPidl;

    sKeyCount = sIniFile.getKeyCount();
    for (i = 0; i < sKeyCount; ++i)
    {
        sKey = sIniFile.getKeyName(i);
        if (XPR_IS_NULL(sKey))
            continue;

        sHistoryDeque = XPR_NULL;

        if (_tcsicmp(sKey, _fx_key_backward) == 0) sHistoryDeque = &mBackwardDeque;
        else if (_tcsicmp(sKey, _fx_key_forward) == 0) sHistoryDeque = &mForwardDeque;
        else if (_tcsicmp(sKey, _fx_key_history) == 0) sHistoryDeque = &mHistoryDeque;

        if (XPR_IS_NULL(sHistoryDeque))
            continue;

        sEntryCount = sIniFile.getEntryCount(i);
        for (j = 0; j < sEntryCount; ++j)
        {
            sEntry = sIniFile.getEntryName(i, j);
            if (XPR_IS_NULL(sEntry))
                break;

            sValue = sIniFile.getValueS(sKey, sEntry);
            if (XPR_IS_NULL(sValue))
                continue;

            sFullPidl = Path2Pidl(sValue);
            if (XPR_IS_NOT_NULL(sFullPidl))
            {
                sHistoryDeque->push_back(sFullPidl);
            }
        }
    }

    return XPR_TRUE;
}

void History::saveToFile(const xpr_tchar_t *aPath)
{
    IniFile sIniFile(aPath);
    sIniFile.setComment(XPR_STRING_LITERAL("flyExplorer History File"));

    xpr_size_t i;
    xpr_tchar_t sEntry[0xff];
    LPITEMIDLIST sFullPidl;
    HistoryDeque::iterator sIterator;
    xpr_tchar_t sPath[XPR_MAX_PATH * 2 + 1];

    sIterator = mBackwardDeque.begin();
    for (i = 0; sIterator != mBackwardDeque.end(); ++sIterator)
    {
        sFullPidl = *sIterator;
        if (XPR_IS_NULL(sFullPidl))
            continue;

        if (Pidl2Path(sFullPidl, sPath) == XPR_FALSE)
            continue;

        _stprintf(sEntry, XPR_STRING_LITERAL("%s%d"), _fx_key_backward, i++);
        sIniFile.setValueS(_fx_key_backward, sEntry, sPath);
    }

    sIterator = mForwardDeque.begin();
    for (i = 0; sIterator != mForwardDeque.end(); ++sIterator)
    {
        sFullPidl = *sIterator;
        if (XPR_IS_NULL(sFullPidl))
            continue;

        if (Pidl2Path(sFullPidl, sPath) == XPR_FALSE)
            continue;

        _stprintf(sEntry, XPR_STRING_LITERAL("%s%d"), _fx_key_forward, i++);
        sIniFile.setValueS(_fx_key_forward, sEntry, sPath);
    }

    sIterator = mHistoryDeque.begin();
    for (i = 0; sIterator != mHistoryDeque.end(); ++sIterator)
    {
        sFullPidl = *sIterator;
        if (XPR_IS_NULL(sFullPidl))
            continue;

        if (Pidl2Path(sFullPidl, sPath) == XPR_FALSE)
            continue;

        _stprintf(sEntry, XPR_STRING_LITERAL("%s%d"), _fx_key_history, i++);
        sIniFile.setValueS(_fx_key_history, sEntry, sPath);
    }

    sIniFile.writeFile(xpr::CharSetUtf16);
}

xpr_bool_t History::addBackward(LPITEMIDLIST aFullPidl)
{
    if (XPR_IS_NULL(aFullPidl))
        return XPR_FALSE;

    mBackwardDeque.push_back(CopyItemIDList(aFullPidl));

    if (mMaxBackward != -1 && mMaxBackward < mBackwardDeque.size())
    {
        HistoryDeque::iterator sIterator;
        LPITEMIDLIST sFullPidl;

        sIterator = mBackwardDeque.begin();
        while (sIterator != mBackwardDeque.end())
        {
            if (mBackwardDeque.size() <= mMaxBackward)
                break;

            sFullPidl = *sIterator;
            COM_FREE(sFullPidl);

            sIterator = mBackwardDeque.erase(sIterator);
        }
    }

    return XPR_TRUE;
}

xpr_bool_t History::addHistory(LPITEMIDLIST aFullPidl)
{
    if (XPR_IS_NULL(aFullPidl))
        return XPR_FALSE;

    mHistoryDeque.push_back(CopyItemIDList(aFullPidl));

    if (mMaxHistory != -1 && mMaxHistory < mHistoryDeque.size())
    {
        HistoryDeque::iterator sIterator;
        LPITEMIDLIST sFullPidl;

        sIterator = mHistoryDeque.begin();
        while (sIterator != mHistoryDeque.end())
        {
            if (mHistoryDeque.size() <= mMaxHistory)
                break;

            sFullPidl = *sIterator;
            COM_FREE(sFullPidl);

            sIterator = mHistoryDeque.erase(sIterator);
        }
    }

    return XPR_TRUE;
}

void History::clearBackward(void)
{
    HistoryDeque::iterator sIterator;
    LPITEMIDLIST sFullPidl;

    sIterator = mBackwardDeque.begin();
    for (; sIterator != mBackwardDeque.end(); ++sIterator)
    {
        sFullPidl = *sIterator;
        COM_FREE(sFullPidl);
    }

    mBackwardDeque.clear();
}

void History::clearForward(void)
{
    HistoryDeque::iterator sIterator;
    LPITEMIDLIST sFullPidl;

    sIterator = mForwardDeque.begin();
    for (; sIterator != mForwardDeque.end(); ++sIterator)
    {
        sFullPidl = *sIterator;
        COM_FREE(sFullPidl);
    }

    mForwardDeque.clear();
}

void History::clearHistory(void)
{
    HistoryDeque::iterator sIterator;
    LPITEMIDLIST sFullPidl;

    sIterator = mHistoryDeque.begin();
    for (; sIterator != mHistoryDeque.end(); ++sIterator)
    {
        sFullPidl = *sIterator;
        COM_FREE(sFullPidl);
    }

    mHistoryDeque.clear();
}

void History::clear(void)
{
    clearBackward();
    clearForward();
    clearHistory();
}

LPITEMIDLIST History::popBackward(LPITEMIDLIST aFullPidl)
{
    if (mBackwardDeque.empty())
        return XPR_NULL;

    LPITEMIDLIST sFullPidl;

    // Add Forward GoList
    if (XPR_IS_NOT_NULL(aFullPidl))
    {
        sFullPidl = CopyItemIDList(aFullPidl);
        mForwardDeque.push_back(sFullPidl);
    }

    // Get Back GoList and List
    sFullPidl = mBackwardDeque.back();
    mBackwardDeque.pop_back();

    return sFullPidl;
}

LPITEMIDLIST History::popBackward(LPITEMIDLIST aFullPidl, xpr_size_t aBack)
{
    if (!XPR_STL_IS_INDEXABLE(aBack, mBackwardDeque))
        return XPR_NULL;

    LPITEMIDLIST sFullPidl;

    if (XPR_IS_NOT_NULL(aFullPidl))
    {
        sFullPidl = CopyItemIDList(aFullPidl);
        mBackwardDeque.push_back(sFullPidl);
    }

    xpr_size_t i;
    for (i = 0; i <= aBack; ++i)
    {
        sFullPidl = mBackwardDeque.back();
        mBackwardDeque.pop_back();

        if (XPR_IS_NOT_NULL(sFullPidl))
            mForwardDeque.push_back(sFullPidl);
    }

    sFullPidl = mBackwardDeque.back();
    mBackwardDeque.pop_back();

    return sFullPidl;
}

LPITEMIDLIST History::popForward(LPITEMIDLIST aFullPidl)
{
    if (mForwardDeque.empty())
        return XPR_NULL;

    LPITEMIDLIST sFullPidl;

    // Add Back GoList
    if (XPR_IS_NOT_NULL(aFullPidl))
    {
        sFullPidl = CopyItemIDList(aFullPidl);
        mBackwardDeque.push_back(sFullPidl);
    }

    // Get Forward GoList and List
    sFullPidl = mForwardDeque.back();
    mForwardDeque.pop_back();

    return sFullPidl;
}

LPITEMIDLIST History::popForward(LPITEMIDLIST aFullPidl, xpr_size_t aForward)
{
    if (!XPR_STL_IS_INDEXABLE(aForward, mForwardDeque))
        return XPR_NULL;

    LPITEMIDLIST sFullPidl;

    if (XPR_IS_NOT_NULL(aFullPidl))
    {
        sFullPidl = CopyItemIDList(aFullPidl);
        mBackwardDeque.push_back(sFullPidl);
    }

    xpr_size_t i;
    for (i = 0; i < aForward; ++i)
    {
        sFullPidl = mForwardDeque.back();
        mForwardDeque.pop_back();

        if (XPR_IS_NOT_NULL(sFullPidl))
            mBackwardDeque.push_back(sFullPidl);
    }

    sFullPidl = mForwardDeque.back();
    mForwardDeque.pop_back();

    return sFullPidl;
}

LPITEMIDLIST History::getHistory(void)
{
    return getHistory(0);
}

LPITEMIDLIST History::getHistory(xpr_size_t aHistory)
{
    aHistory = mHistoryDeque.size() - aHistory - 1;

    if (!XPR_STL_IS_INDEXABLE(aHistory, mHistoryDeque))
        return XPR_NULL;

    return mHistoryDeque.at(aHistory);
}

xpr_size_t History::getBackwardCount(void)
{
    return mBackwardDeque.size();
}

xpr_size_t History::getForwardCount(void)
{
    return mForwardDeque.size();
}

xpr_size_t History::getHistoryCount(void)
{
    return mHistoryDeque.size();
}

HistoryDeque *History::getBackwardList(void)
{
    return &mBackwardDeque;
}

HistoryDeque *History::getForwardList(void)
{
    return &mForwardDeque;
}

HistoryDeque *History::getHistoryList(void)
{
    return &mHistoryDeque;
}

void History::setMaxBackward(xpr_size_t aBackwardCount)
{
    mMaxBackward = aBackwardCount;

    if (mHistoryDeque.size() < aBackwardCount)
        return;

    HistoryDeque::iterator sIterator;
    LPITEMIDLIST sFullPidl;

    sIterator = mBackwardDeque.begin();
    while (sIterator != mBackwardDeque.end())
    {
        if (mBackwardDeque.size() <= aBackwardCount)
            break;

        sFullPidl = *sIterator;
        COM_FREE(sFullPidl);

        sIterator = mBackwardDeque.erase(sIterator);
    }
}

void History::setMaxHistory(xpr_size_t aHistoryCount)
{
    mMaxHistory = aHistoryCount;

    if (mHistoryDeque.size() < aHistoryCount)
        return;

    HistoryDeque::iterator sIterator;
    LPITEMIDLIST sFullPidl;

    sIterator = mHistoryDeque.begin();
    while (sIterator != mHistoryDeque.end())
    {
        if (mHistoryDeque.size() <= aHistoryCount)
            break;

        sFullPidl = *sIterator;
        COM_FREE(sFullPidl);

        sIterator = mHistoryDeque.erase(sIterator);
    }
}
} // namespace fxb
