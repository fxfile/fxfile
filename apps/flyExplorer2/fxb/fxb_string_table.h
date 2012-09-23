//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXB_STRING_TABLE_H__
#define __FXB_STRING_TABLE_H__
#pragma once

namespace fxb
{
class LanguagePack;

class StringTable
{
    friend class LanguagePack;

public:
    enum
    {
        kMaxIdLength     = 128,
        kMaxStringLength = 1024,
    };

public:
    StringTable(void);
    virtual ~StringTable(void);

public:
    const xpr_tchar_t *loadString(const xpr_tchar_t *aId, xpr_bool_t aNullAvailable = XPR_FALSE);
    xpr_bool_t loadString(const xpr_tchar_t *aId, xpr_tchar_t *aString, xpr_size_t aMaxStringLength);

public:
    void clear(void);

protected:
    typedef stdext::hash_map<std::tstring, std::tstring> StringTableMap;

    xpr_bool_t load(const xpr_tchar_t *aFileName, StringTableMap &aStringTable);

protected:
    StringTableMap mStringTable;
};
} // namespace fxb

#endif // __FXB_STRING_TABLE_H__
