//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "fxb_string_table.h"

#include "fxb_language_pack.h"
#include "fxb_xml.h"

namespace fxb
{
static const xpr_tchar_t mNoId[] = XPR_STRING_LITERAL("<<ID don't exist!!!>>");

StringTable::StringTable(void)
{
}

StringTable::~StringTable(void)
{
    clear();
}

void StringTable::clear(void)
{
    mStringTable.clear();
}

const xpr_tchar_t *StringTable::loadString(const xpr_tchar_t *aId, xpr_bool_t aNullAvailable)
{
    XPR_ASSERT(aId != XPR_NULL);

    if (aId == XPR_NULL)
    {
        if (aNullAvailable == XPR_TRUE)
            return XPR_NULL;

        return mNoId;
    }

    StringTableMap::iterator sIterator = mStringTable.find(aId);
    if (sIterator == mStringTable.end())
    {
        XPR_TRACE(XPR_STRING_LITERAL("[StringTable] \'%s\' string is not loaded.\n"), aId);

        if (aNullAvailable == XPR_TRUE)
            return XPR_NULL;

        return aId;
    }

    return sIterator->second.c_str();
}

xpr_bool_t StringTable::loadString(const xpr_tchar_t *aId, xpr_tchar_t *aString, xpr_size_t aMaxStringLength)
{
    if (aString == XPR_NULL || aMaxStringLength == 0)
        return XPR_FALSE;

    aString[0] = 0;

    if (aId == XPR_NULL)
    {
        static xpr_size_t sNoIdLength = _tcslen(mNoId);

        if (aMaxStringLength < sNoIdLength)
            return XPR_FALSE;

        _tcscpy(aString, mNoId);
    }
    else
    {
        const xpr_tchar_t *sString = loadString(aId);

        if (_tcslen(sString) > aMaxStringLength)
            return XPR_FALSE;

        if (sString == XPR_NULL)
            _tcscpy(aString, aId);
        else
            _tcscpy(aString, sString);
    }

    return XPR_TRUE;
}
} // namespace fxb
