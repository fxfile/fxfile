//
// Copyright (c) 2012-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_BASE_LANGUAGE_PACK_H__
#define __FXFILE_BASE_LANGUAGE_PACK_H__ 1
#pragma once

namespace fxfile
{
namespace base
{
class StringTable;

class LanguagePack
{
public:
    enum
    {
        kMaxLanguageLength    = 128,
        kMaxAuthorLength      = 128,
        kMaxHomepageLength    = XPR_MAX_URL_LENGTH,
        kMaxEmailLength       = XPR_MAX_URL_LENGTH,
        kMaxDescriptionLength = 4096,
    };

    struct Desc
    {
        xpr::string mLanguage;
        xpr::string mFilePath;
        xpr::string mAuthor;
        xpr::string mHomepage;
        xpr::string mEmail;
        xpr::string mDescription;
    };

public:
    LanguagePack(void);
    virtual ~LanguagePack(void);

public:
    xpr_bool_t load(const xpr_tchar_t *aPath);

    xpr_bool_t loadStringTable(StringTable *aStringTable);

    const Desc *getLanguageDesc(void) const;
    xpr_bool_t equalLanguage(const xpr_tchar_t *aLanguage) const;

protected:
    xpr_bool_t load(const xpr_tchar_t *aPath, StringTable *aStringTable);

protected:
    xpr_tchar_t mPath[XPR_MAX_PATH + 1];

    Desc mDesc;
};
} // namespace base
} // namespace fxfile

#endif // __FXFILE_BASE_LANGUAGE_PACK_H__
