//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_BASE_STRING_TABLE_H__
#define __FXFILE_BASE_STRING_TABLE_H__ 1
#pragma once

namespace fxfile
{
namespace base
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
    typedef std::tr1::unordered_map<xpr::tstring, xpr::tstring> StringTableMap;

    xpr_bool_t load(const xpr_tchar_t *aFileName, StringTableMap &aStringTable);

protected:
    StringTableMap mStringTable;
};
} // namespace base
} // namespace fxfile

#endif // __FXFILE_BASE_STRING_TABLE_H__
