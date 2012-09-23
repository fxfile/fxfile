//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXB_STRING_H__
#define __FXB_STRING_H__
#pragma once

namespace std
{
#if defined(XPR_CFG_UNICODE)
    typedef wstring tstring;
#else
    typedef string tstring;
#endif
} // std

namespace fxb
{
template <typename T>
void trim(T &aStr)
{
    xpr_size_t sOffset = 0;
    while (true)
    {
        sOffset = aStr.find(' ', sOffset);
        if (sOffset == std::tstring::npos)
            break;

        aStr.erase(sOffset, 1);
    }
}

template <typename T>
void trim_left(T &aStr)
{
    xpr_size_t sOffset = aStr.find_first_not_of(' ');
    if (sOffset != std::tstring::npos && sOffset != 0)
        aStr.erase(0, sOffset);
}

template <typename T>
void trim_right(T &aStr)
{
    xpr_size_t sOffset = aStr.find_last_not_of(' ');
    if (sOffset != std::tstring::npos && sOffset != aStr.length()-1)
        aStr.erase(sOffset+1, aStr.length()-sOffset);
}

template <typename T>
void trim_left_right(T &aStr)
{
    trim_left(aStr);
    trim_right(aStr);
}

template <typename T>
void upper(T &aStr)
{
    transform(aStr.begin(), aStr.end(), aStr.begin(), toupper);
}

template <typename T>
void lower(T &aStr)
{
    transform(aStr.begin(), aStr.end(), aStr.begin(), tolower);
}

xpr_bool_t IsEmptyString(const xpr_tchar_t *aString);
void EmptyString(xpr_tchar_t *aString);

void GetFormatedNumber(xpr_uint64_t aNumber, xpr_tchar_t *aFormatedNumber, xpr_size_t aMaxLen);
} // namespace fxb

#endif // __FXB_STRING_H__
