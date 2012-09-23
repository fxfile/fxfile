//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXB_FORMAT_PARSER_H__
#define __FXB_FORMAT_PARSER_H__
#pragma once

namespace fxb
{
class FormatParser
{
public:
    enum Result
    {
        PARSING_FORMAT_INVALID_FILENAME = -3,
        PARSING_FORMAT_INCORRECT_FORMAT = -2,
        PARSING_FORMAT_EXCEED_LENGTH    = -1,
        PARSING_FORMAT_SUCCEEDED        = 0,
    };

public:
    FormatParser(void);
    virtual ~FormatParser(void);

public:
    Result parse(
        const xpr_tchar_t *aOrgPath,
        const xpr_tchar_t *aPath,
        const xpr_tchar_t *aFileName,
        const xpr_tchar_t *aExt,
        const xpr_tchar_t *aFormat,
        std::tstring      &aDest,
        xpr_uint_t         aNumber,
        xpr_size_t         aMaxDestLen = XPR_MAX_PATH);

    Result parse(
        const std::tstring &aFormat,
        std::tstring       &aDest,
        xpr_uint_t         aNumber,
        xpr_size_t         aMaxDestLen = XPR_MAX_PATH);

    Result parse(
        const std::tstring &aOrgPath,
        const std::tstring &aPath,
        const std::tstring &aFileName,
        const std::tstring &aExt,
        const std::tstring &aFormat,
        std::tstring       &aDest,
        xpr_uint_t         aNumber,
        xpr_size_t         aMaxDestLen = XPR_MAX_PATH);

protected:
    const CTime mTime;
};
} // namespace fxb

#endif // __FXB_FORMAT_PARSER_H__
