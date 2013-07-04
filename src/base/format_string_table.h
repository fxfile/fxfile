//
// Copyright (c) 2012-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_BASE_FORMAT_STRING_TABLE_H__
#define __FXFILE_BASE_FORMAT_STRING_TABLE_H__ 1
#pragma once

#include "string_table.h"

namespace fxfile
{
namespace base
{
#define MAX_REPLACE_FORMAT_SPECIFIER_COUNT 20

class FormatStringTable
{
public:
    FormatStringTable(StringTable &aStringTable);
    virtual ~FormatStringTable(void);

public:
    const xpr_tchar_t *loadString(const xpr_tchar_t *aId, xpr_bool_t aNullAvailable = XPR_FALSE);
    const xpr_tchar_t *loadString(const xpr_tchar_t *aId, const xpr_tchar_t *aReplaceFormatSpecifier, xpr_bool_t aNullAvailable = XPR_FALSE);
    xpr_bool_t loadString(const xpr_tchar_t *aId, xpr_tchar_t *aString, xpr_size_t aMaxStringLength);
    xpr_bool_t loadString(const xpr_tchar_t *aId, const xpr_tchar_t *aReplaceFormatSpecifier, xpr_tchar_t *aString, xpr_size_t aMaxStringLength);

protected:
    void replcaeFormatSpecifier(xpr_tchar_t *aFormatString, const xpr_tchar_t *aReplaceFormatSpecifier);
    xpr_size_t getDigits(xpr_sint_t aNumber) const;

protected:
    StringTable &mStringTable;

    struct ReplaceFormatSpecifier
    {
        const xpr_tchar_t *mFormatSpecifier;
        xpr_size_t         mFormatSpecifierLength;
    };

    ReplaceFormatSpecifier mReplaceFormatSpecifiers[MAX_REPLACE_FORMAT_SPECIFIER_COUNT];
    xpr_size_t             mReplaceFormatSpecifierCount;
    xpr_size_t             mMaxIndexSpecifierLength;

    xpr_tchar_t mFormatString[StringTable::kMaxStringLength + 1];
};
} // namespace base
} // namespace fxfile

#endif // __FXFILE_BASE_FORMAT_STRING_TABLE_H__
