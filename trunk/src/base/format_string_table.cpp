//
// Copyright (c) 2012-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "format_string_table.h"

#include "string_table.h"

#include <cmath>

namespace fxfile
{
namespace base
{
FormatStringTable::FormatStringTable(StringTable &aStringTable)
    : mStringTable(aStringTable)
{
    mMaxIndexSpecifierLength = getDigits(MAX_REPLACE_FORMAT_SPECIFIER_COUNT);
}

FormatStringTable::~FormatStringTable(void)
{
}

const xpr_tchar_t *FormatStringTable::loadString(const xpr_tchar_t *aId, xpr_bool_t aNullAvailable)
{
    return mStringTable.loadString(aId, aNullAvailable);
}

const xpr_tchar_t *FormatStringTable::loadString(const xpr_tchar_t *aId,
                                                 const xpr_tchar_t *aReplaceFormatSpecifier,
                                                 xpr_bool_t         aNullAvailable)
{
    const xpr_tchar_t *sString = loadString(aId, aNullAvailable);

    if (sString == XPR_NULL)
        return XPR_NULL;

    _tcscpy(mFormatString, sString);

    replcaeFormatSpecifier(mFormatString, aReplaceFormatSpecifier);

    return mFormatString;
}

xpr_bool_t FormatStringTable::loadString(const xpr_tchar_t *aId, xpr_tchar_t *aString, xpr_size_t aMaxStringLength)
{
    return mStringTable.loadString(aId, aString, aMaxStringLength);
}

xpr_bool_t FormatStringTable::loadString(const xpr_tchar_t *aId,
                                         const xpr_tchar_t *aReplaceFormatSpecifier,
                                         xpr_tchar_t       *aString,
                                         xpr_size_t         aMaxStringLength)
{
    xpr_bool_t sResult = loadString(aId, aString, aMaxStringLength);
    if (sResult == XPR_FALSE)
        return XPR_FALSE;

    replcaeFormatSpecifier(aString, aReplaceFormatSpecifier);

    return XPR_TRUE;
}

/// @Brief replace index specifier to format specifier
/// 1 based index specifier.
/// maximum index specifier length is 99.
/// maximum format specifier length is 0xff.
void FormatStringTable::replcaeFormatSpecifier(xpr_tchar_t *aFormatString, const xpr_tchar_t *aReplaceFormatSpecifier)
{
    mReplaceFormatSpecifierCount = 0;

    const xpr_tchar_t *sSeparator;
    const xpr_tchar_t *sReplaceFormatSpecifierEnum = aReplaceFormatSpecifier;

    do
    {
        sSeparator = _tcschr(sReplaceFormatSpecifierEnum, XPR_STRING_LITERAL(','));
        if (XPR_IS_NULL(sSeparator))
        {
            mReplaceFormatSpecifiers[mReplaceFormatSpecifierCount].mFormatSpecifier       = sReplaceFormatSpecifierEnum;
            mReplaceFormatSpecifiers[mReplaceFormatSpecifierCount].mFormatSpecifierLength = _tcslen(sReplaceFormatSpecifierEnum);
            mReplaceFormatSpecifierCount++;
            break;
        }
        else
        {
            mReplaceFormatSpecifiers[mReplaceFormatSpecifierCount].mFormatSpecifier       = sReplaceFormatSpecifierEnum;
            mReplaceFormatSpecifiers[mReplaceFormatSpecifierCount].mFormatSpecifierLength = (xpr_size_t)(sSeparator - sReplaceFormatSpecifierEnum);
            mReplaceFormatSpecifierCount++;
        }

        sReplaceFormatSpecifierEnum = sSeparator + 1;
    }
    while (mReplaceFormatSpecifierCount <= MAX_REPLACE_FORMAT_SPECIFIER_COUNT);

    xpr_sint_t sFormatStringLength = (xpr_sint_t)_tcslen(aFormatString);
    xpr_tchar_t *sFormatStringEnum = aFormatString;
    xpr_sint_t sFormatSpecifierLength;
    xpr_tchar_t *sBeginIndexSpecifier = XPR_NULL;
    xpr_tchar_t *sEndIndexSpecifier;
    xpr_sint_t sIndexSpecifierLength;
    xpr_sint_t sIndexSpecifier;
    xpr_sint_t sDiffLength;

    while (true)
    {
        if (XPR_IS_NULL(sBeginIndexSpecifier))
        {
            sBeginIndexSpecifier = _tcschr(sFormatStringEnum, XPR_STRING_LITERAL('%'));
            if (XPR_IS_NULL(sBeginIndexSpecifier))
                break;
        }

        sEndIndexSpecifier = _tcschr(sBeginIndexSpecifier + 1, XPR_STRING_LITERAL('%'));
        if (XPR_IS_NULL(sEndIndexSpecifier))
            break;

        sEndIndexSpecifier++;

        sIndexSpecifierLength = (xpr_sint_t)(sEndIndexSpecifier - sBeginIndexSpecifier);
        if (sIndexSpecifierLength > ((xpr_sint_t)mMaxIndexSpecifierLength + 2)) // 2(start % + end %)
        {
            sFormatStringEnum = sBeginIndexSpecifier;
            sBeginIndexSpecifier = sEndIndexSpecifier - 1;
            continue;
        }

        sIndexSpecifier = _ttoi(sBeginIndexSpecifier + 1);
        if (sIndexSpecifier < 0 ||
            sIndexSpecifier > (xpr_sint_t)mReplaceFormatSpecifierCount ||
            getDigits(sIndexSpecifier) != (sIndexSpecifierLength - 2))
        {
            sFormatStringEnum = sBeginIndexSpecifier;
            sBeginIndexSpecifier = sEndIndexSpecifier - 1;
            continue;
        }

        sIndexSpecifier--;

        sFormatSpecifierLength = (xpr_sint_t)mReplaceFormatSpecifiers[sIndexSpecifier].mFormatSpecifierLength;
        sDiffLength = sFormatSpecifierLength - sIndexSpecifierLength;

        if (sDiffLength > 0 || sDiffLength < 0)
        {
            xpr_sint_t sMoveSize = sFormatStringLength * sizeof(xpr_tchar_t) - (xpr_sint_t)((xpr_byte_t *)(sBeginIndexSpecifier + sIndexSpecifierLength) - (xpr_byte_t *)aFormatString);
            memmove(sBeginIndexSpecifier + sIndexSpecifierLength + sDiffLength, sBeginIndexSpecifier + sIndexSpecifierLength, sMoveSize);
        }

        memcpy(sBeginIndexSpecifier, mReplaceFormatSpecifiers[sIndexSpecifier].mFormatSpecifier, sFormatSpecifierLength * sizeof(xpr_tchar_t));

        if (sDiffLength != 0)
        {
            sFormatStringLength += sDiffLength;

            aFormatString[sFormatStringLength] = 0;
        }

        sFormatStringEnum = sBeginIndexSpecifier + sFormatSpecifierLength;
        sBeginIndexSpecifier = XPR_NULL;
    }
}

xpr_size_t FormatStringTable::getDigits(xpr_sint_t aNumber) const
{
    if (XPR_IS_RANGE(0, aNumber, 999))
    {
        if (aNumber < 10)
            return 1;
        else if (aNumber >= 10)
            return 2;
        else if (aNumber >= 100)
            return 3;
    }

    xpr_bool_t sPositive = XPR_TRUE;
    if (aNumber < 0)
    {
        aNumber = -aNumber;
        sPositive = XPR_FALSE;
    }

    xpr_size_t sDigits = (xpr_size_t)floor(log10((xpr_double_t)abs(aNumber != 0 ? aNumber : 1))) + 1;

    if (XPR_IS_FALSE(sPositive))
        sDigits++;

    return sDigits;
}
} // namespace base
} // namespace fxfile
