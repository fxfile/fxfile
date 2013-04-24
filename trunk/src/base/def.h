//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_BASE_DEF_H__
#define __FXFILE_BASE_DEF_H__
#pragma once

#include "xpr.h"

//
// C++ standard libraries
//
#include <stack>
#include <queue>
#include <deque>
#include <vector>
#include <list>
#include <set>
#include <map>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>
#include <string>
#include <cctype>

namespace std
{
#if defined(XPR_CFG_UNICODE)
typedef wstring tstring;
#else
typedef string tstring;
#endif
} // namespace std

#define FXFILE_STL_FOR_EACH(aIterator, aContainer) \
    for ((aIterator) = (aContainer).begin(); (aIterator) != (aContainer).end(); ++(aIterator))

#define FXFILE_STL_FOR_EACH_REVERSE(aIterator, aContainer) \
    for ((aIterator) = (aContainer).rbegin(); (aIterator) != (aContainer).rend(); ++(aIterator))

#define FXFILE_STL_IS_INDEXABLE(aIndex, aContainer) \
    ((0 <= ((xpr_sint_t)(aIndex))) && (((xpr_sint_t)(aIndex)) < ((xpr_sint_t)(aContainer).size())))

#if defined(XPR_CFG_OS_WINDOWS)
#include "def_win.h"
#else // not XPR_CFG_OS_WINDOWS
#error Please, support your OS. <def.h>
#endif // XPR_CFG_OS_WINDOWS

#endif // __FXFILE_BASE_DEF_H__
