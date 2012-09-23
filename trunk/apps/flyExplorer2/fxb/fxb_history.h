//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXB_HISTORY_H__
#define __FXB_HISTORY_H__
#pragma once

namespace fxb
{
typedef std::deque<LPITEMIDLIST> HistoryDeque;

class History
{
public:
    enum
    {
        TypeNone,
        TypeBackward,
        TypeForward,
        TypeHistory,
    };

public:
    History(void);
    virtual ~History(void);

public:
    xpr_bool_t loadFromFile(const xpr_tchar_t *aPath);
    void saveToFile(const xpr_tchar_t *aPath);

public:
    xpr_bool_t addBackward(LPITEMIDLIST aFullPidl);
    xpr_bool_t addHistory(LPITEMIDLIST aFullPidl);
    LPITEMIDLIST popBackward(LPITEMIDLIST aFullPidl);
    LPITEMIDLIST popBackward(LPITEMIDLIST aFullPidl, xpr_size_t aBack);
    LPITEMIDLIST popForward(LPITEMIDLIST aFullPidl);
    LPITEMIDLIST popForward(LPITEMIDLIST aFullPidl, xpr_size_t aForward);
    LPITEMIDLIST getHistory(void);
    LPITEMIDLIST getHistory(xpr_size_t aHistory);
    xpr_size_t getBackwardCount(void);
    xpr_size_t getForwardCount(void);
    xpr_size_t getHistoryCount(void);
    HistoryDeque *getBackwardList(void);
    HistoryDeque *getForwardList(void);
    HistoryDeque *getHistoryList(void);
    void setMaxBackward(xpr_size_t aBackwardCount);
    void setMaxHistory(xpr_size_t aBackwardCount);
    void clearBackward(void);
    void clearForward(void);
    void clearHistory(void);
    void clear(void);

protected:
    HistoryDeque mBackwardDeque;
    HistoryDeque mForwardDeque;
    HistoryDeque mHistoryDeque;
    xpr_size_t   mMaxBackward;
    xpr_size_t   mMaxHistory;
};
} // namespace fxb

#endif // __FXB_HISTORY_H__
