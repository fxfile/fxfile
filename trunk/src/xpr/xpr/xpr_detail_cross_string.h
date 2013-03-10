//
// Copyright (c) 2013 Leon Lee author. All rights reserved.
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __XPR_DETAIL_CROSS_STRING_H__
#define __XPR_DETAIL_CROSS_STRING_H__
#pragma once

#include "xpr_types.h"
#include "xpr_dlsym.h"
#include "xpr_rcode.h"
#include "xpr_memory.h"

namespace xpr
{
namespace detail
{
class CrossString
{
public:
    CrossString(void)
        : mString(XPR_NULL)
        , mBytes(0)
        , mWideChar(XPR_FALSE)
    {
    }

    ~CrossString(void)
    {
        reset();
    }

public:
    inline void reset(void)
    {
        XPR_SAFE_DELETE_ARRAY(mString);
        mBytes = 0;
        mWideChar = XPR_FALSE;
    }

    inline xpr_rcode_t setString(const xpr_char_t *aString)
    {
        xpr_size_t sLen = strlen(aString);

        return setString(aString, sLen * sizeof(xpr_char_t), XPR_FALSE);
    }

    inline xpr_rcode_t setString(const xpr_wchar_t *aString)
    {
        xpr_size_t sLen = wcslen(aString);

        return setString(aString, sLen * sizeof(xpr_wchar_t), XPR_TRUE);
    }

    inline xpr_rcode_t setString(const void *aString, xpr_size_t aStringBytes, xpr_bool_t aWideChar)
    {
        if (XPR_IS_TRUE(aWideChar))
        {
            xpr_byte_t *sString = (xpr_byte_t *)new xpr_byte_t[aStringBytes + sizeof(xpr_wchar_t)];
            if (XPR_IS_NULL(sString))
            {
                return XPR_RCODE_ENOMEM;
            }

            wcscpy((xpr_wchar_t *)sString, (const xpr_wchar_t *)aString);

            reset();

            mString   = sString;
            mBytes    = aStringBytes;
            mWideChar = aWideChar;
        }
        else
        {
            xpr_byte_t *sString = (xpr_byte_t *)new xpr_byte_t[aStringBytes + sizeof(xpr_char_t)];
            if (XPR_IS_NULL(sString))
            {
                return XPR_RCODE_ENOMEM;
            }

            strcpy((xpr_char_t *)sString, (const xpr_char_t *)aString);

            reset();

            mString   = sString;
            mBytes    = aStringBytes;
            mWideChar = aWideChar;
        }

        return XPR_RCODE_SUCCESS;
    }

    xpr_bool_t getString(xpr_char_t *aString, xpr_size_t aMaxLen) const
    {
        if (XPR_IS_NULL(aString))
        {
            return XPR_FALSE;
        }

        if (XPR_IS_TRUE(mWideChar))
        {
            xpr_size_t sLen = wcslen(mStringW);
            if (sLen > aMaxLen)
            {
                return XPR_FALSE;
            }

            xpr_size_t sInputBytes = mBytes;
            xpr_size_t sOutputBytes = aMaxLen * sizeof(xpr_char_t);

            XPR_UTF16_TO_MBS(mString, &sInputBytes, aString, &sOutputBytes);

            aString[sOutputBytes / sizeof(xpr_char_t)] = 0;
        }
        else
        {
            xpr_size_t sLen = strlen(mStringA);
            if (sLen > aMaxLen)
            {
                return XPR_FALSE;
            }

            strcpy(aString, mStringA);
        }

        return XPR_TRUE;
    }

    xpr_bool_t getString(xpr_wchar_t *aString, xpr_size_t aMaxLen) const
    {
        if (XPR_IS_NULL(aString))
        {
            return XPR_FALSE;
        }

        if (XPR_IS_TRUE(mWideChar))
        {
            xpr_size_t sLen = wcslen(mStringW);
            if (sLen > aMaxLen)
            {
                return XPR_FALSE;
            }

            wcscpy(aString, mStringW);
        }
        else
        {
            xpr_size_t sLen = strlen(mStringA);
            if (sLen > aMaxLen)
            {
                return XPR_FALSE;
            }

            xpr_size_t sInputBytes = mBytes;
            xpr_size_t sOutputBytes = aMaxLen * sizeof(xpr_wchar_t);

            XPR_MBS_TO_UTF16(mString, &sInputBytes, aString, &sOutputBytes);

            aString[sOutputBytes / sizeof(xpr_wchar_t)] = 0;
        }

        return XPR_TRUE;
    }

    xpr_size_t getLen(void) const
    {
        if (XPR_IS_TRUE(mWideChar))
        {
            return mBytes / sizeof(xpr_wchar_t);
        }

        return mBytes / sizeof(xpr_char_t);
    }

public:
    union
    {
        xpr_byte_t  *mString;
        xpr_char_t  *mStringA;
        xpr_wchar_t *mStringW;
    };

    xpr_size_t mBytes;
    xpr_bool_t mWideChar;
};
} // namespace detail
} // namespace xpr

#endif // __XPR_DETAIL_CROSS_STRING_H__
