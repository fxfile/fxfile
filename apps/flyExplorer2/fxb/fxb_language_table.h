//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXB_LANGUAGE_TABLE_H__
#define __FXB_LANGUAGE_TABLE_H__
#pragma once

#include "fxb_language_pack.h"
#include "fxb_format_string_table.h"

namespace fxb
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

    StringTable *getStringTable(void) const;
    FormatStringTable *getFormatStringTable(void) const;

    void clear(void);

protected:
    xpr_tchar_t mDir[XPR_MAX_PATH + 1];

    typedef stdext::hash_map<std::tstring, LanguagePack *> LanguageMap;
    typedef std::deque<LanguagePack *> LanguageDeque;
    LanguageMap   mLanguageMap;
    LanguageDeque mLanguageDeque;

    LanguagePack      *mLoadedLanaguage;
    StringTable       *mStringTable;
    FormatStringTable *mFormatStringTable;
};
} // namespace fxb

#endif // __FXB_LANGUAGE_TABLE_H__
