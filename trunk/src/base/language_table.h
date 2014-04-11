//
// Copyright (c) 2012-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_BASE_LANGUAGE_TABLE_H__
#define __FXFILE_BASE_LANGUAGE_TABLE_H__ 1
#pragma once

#include "language_pack.h"
#include "format_string_table.h"

namespace fxfile
{
namespace base
{
class LanguageTable
{
public:
    LanguageTable(void);
    virtual ~LanguageTable(void);

public:
    void setDir(const xpr_tchar_t *aDir);

public:
    xpr_bool_t scan(void);

    xpr_bool_t loadLanguage(const xpr_tchar_t *aLanguage);
    void unloadLanguage(void);

    xpr_size_t getLanguageCount(void) const;
    const LanguagePack::Desc *getLanguageDesc(xpr_size_t aIndex) const;
    const LanguagePack::Desc *getLanguageDesc(void) const;
    const LanguagePack::Desc *getLanguageDesc(const xpr_tchar_t *aLanguage) const;

    StringTable *getStringTable(void) const;
    FormatStringTable *getFormatStringTable(void) const;

    void clear(void);

protected:
    xpr_tchar_t mDir[XPR_MAX_PATH + 1];

    typedef std::tr1::unordered_map<xpr::string, LanguagePack *> LanguageMap;
    typedef std::deque<LanguagePack *> LanguageDeque;
    LanguageMap   mLanguageMap;
    LanguageDeque mLanguageDeque;

    LanguagePack      *mLoadedLanaguage;
    StringTable       *mStringTable;
    FormatStringTable *mFormatStringTable;
};
} // namespace base
} // namespace fxfile

#endif // __FXFILE_BASE_LANGUAGE_TABLE_H__
