//
// Copyright (c) 2014 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "format_short_parser.h"
#include "format_factory.h"
#include "format_clear.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace fxfile
{
namespace cmd
{
ShortFormatParser::ShortFormatParser(void)
{
}

ShortFormatParser::~ShortFormatParser(void)
{
}

xpr_bool_t ShortFormatParser::parse(const xpr::tstring &aFormat, FormatSequence &aFormatSequence) const
{
    xpr_bool_t     sSuccess = XPR_TRUE;
    xpr_size_t     i = 0, sLen;
    xpr_size_t     sSeparatedFormatPos1 = -1;
    xpr_size_t     sSeparatedFormatPos2 = -1;
    xpr::tstring   sSeparatedFormat;
    xpr::tstring   sString;
    Format        *sFormat;
    FormatFactory &sFormatFactory = SingletonManager::get<FormatFactory>();

    sLen = aFormat.length();
    while (i < sLen)
    {
        if (aFormat[i] == XPR_STRING_LITERAL('<'))
        {
            sSeparatedFormatPos1 = i;
            ++i;

            for (; i < sLen; ++i)
            {
                if (aFormat[i] == XPR_STRING_LITERAL('>'))
                {
                    sSeparatedFormatPos2 = i;
                    break;
                }
            }

            if (sSeparatedFormatPos2 == -1)
            {
                sSuccess = XPR_FALSE;
                break;
            }

            sSeparatedFormat = aFormat.substr(sSeparatedFormatPos1, sSeparatedFormatPos2 - sSeparatedFormatPos1 + 1);
            sSeparatedFormatPos1 = sSeparatedFormatPos2 = -1;
            ++i;

            if (sString.empty() == XPR_FALSE)
            {
                sFormatFactory.parseShortAsString(sString, sFormat);

                aFormatSequence.add(sFormat);

                sString.clear();
            }

            sSuccess = sFormatFactory.parseShort(sSeparatedFormat, sFormat);
            if (XPR_IS_FALSE(sSuccess))
            {
                break;
            }

            aFormatSequence.add(sFormat);
        }
        else if (aFormat[i] == XPR_STRING_LITERAL('\\') ||
                 aFormat[i] == XPR_STRING_LITERAL('/')  ||
                 aFormat[i] == XPR_STRING_LITERAL(':')  ||
                 aFormat[i] == XPR_STRING_LITERAL('*')  ||
                 aFormat[i] == XPR_STRING_LITERAL('?')  ||
                 aFormat[i] == XPR_STRING_LITERAL('\"') ||
                 aFormat[i] == XPR_STRING_LITERAL('<')  ||
                 aFormat[i] == XPR_STRING_LITERAL('>')  ||
                 aFormat[i] == XPR_STRING_LITERAL('|'))
        {
            sSuccess = XPR_FALSE;
            break;
        }
        else
        {
            sString += aFormat[i];
            ++i;
        }
    }

    if (sString.empty() == XPR_FALSE)
    {
        sFormatFactory.parseShortAsString(sString, sFormat);

        aFormatSequence.add(sFormat);
    }

    if (XPR_IS_FALSE(sSuccess))
    {
        aFormatSequence.clear();
    }

    return sSuccess;
}
} // namespace cmd
} // namespace fxfile
