//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "fxb_history.h"

#include "fxb_ini_file_ex.h"

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

LPITEMIDLIST History::getHistory(void) const
{
    return getHistory(0);
}

LPITEMIDLIST History::getHistory(xpr_size_t aHistory) const
{
    aHistory = mHistoryDeque.size() - aHistory - 1;

    if (!XPR_STL_IS_INDEXABLE(aHistory, mHistoryDeque))
        return XPR_NULL;

    return mHistoryDeque.at(aHistory);
}

xpr_size_t History::getBackwardCount(void) const
{
    return mBackwardDeque.size();
}

xpr_size_t History::getForwardCount(void) const
{
    return mForwardDeque.size();
}

xpr_size_t History::getHistoryCount(void) const
{
    return mHistoryDeque.size();
}

const HistoryDeque *History::getBackwardDeque(void) const
{
    return &mBackwardDeque;
}

const HistoryDeque *History::getForwardDeque(void) const
{
    return &mForwardDeque;
}

const HistoryDeque *History::getHistoryDeque(void) const
{
    return &mHistoryDeque;
}

void History::setBackwardDeque(HistoryDeque *aBackwardDeque)
{
    clearBackward();

    if (XPR_IS_NOT_NULL(aBackwardDeque))
    {
        mBackwardDeque.insert(mBackwardDeque.begin(), aBackwardDeque->begin(), aBackwardDeque->end());

        aBackwardDeque->clear();
    }
}

void History::setForwardDeque(HistoryDeque *aForwardDeque)
{
    clearForward();

    if (XPR_IS_NOT_NULL(aForwardDeque))
    {
        mForwardDeque.insert(mForwardDeque.begin(), aForwardDeque->begin(), aForwardDeque->end());

        aForwardDeque->clear();
    }
}

void History::setHistoryDeque(HistoryDeque *aHistoryDeque)
{
    clearHistory();

    if (XPR_IS_NOT_NULL(aHistoryDeque))
    {
        mHistoryDeque.insert(mHistoryDeque.begin(), aHistoryDeque->begin(), aHistoryDeque->end());

        aHistoryDeque->clear();
    }
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
