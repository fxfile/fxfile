//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "fnmatch.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// modified glibc fnmatch function
//  - unsinged char -> xpr_tchar_t
//  - reference: http://pubs.opengroup.org/onlinepubs/009695399/functions/fnmatch.html

#define __LowerCase_A__     97
#define __LowerCase_Z__     122
#define __UpperCase_A__     65
#define __UpperCase_Z__     90
#define __STRING_DISTANCE__ __LowerCase_A__ - __UpperCase_A__

inline xpr_tchar_t TOLOWER(xpr_tchar_t ConvertChar)
{
    if (__LowerCase_A__ <= ConvertChar && ConvertChar <= __LowerCase_Z__)
        return xpr_tchar_t(ConvertChar - 32);
    else
        return ConvertChar;
}

#define FOLD(c) ((flags & FNM_CASEFOLD) ? TOLOWER (c) : (c))

inline xpr_sint_t fnmatch(const xpr_tchar_t *pattern, const xpr_tchar_t *string, xpr_sint_t flags)
{
    register const xpr_tchar_t *p = pattern;
    register const xpr_tchar_t *n = string;
    register xpr_tchar_t c, c1;

    while ((c = *p++) != '\0')
    {
        c = FOLD(c);

        switch (c)
        {
        case '?':
            if (*n == '\0')
                return FNM_NOMATCH;
            break;

        case '*':
            for (c = *p++; c == '?' || c == '*'; c = *p++, ++n)
            {
                if (c == '?' && *n == '\0')
                    return FNM_NOMATCH;
            }

            if (c == '\0')
                return FNM_MATCH;

            c1 = FOLD(c);
            for (--p; *n != '\0'; ++n)
            {
                if ((FOLD((xpr_tchar_t)*n) == c1) && fnmatch(p, n, flags & ~FNM_PERIOD) == 0)
                    return FNM_MATCH;
            }
            return FNM_NOMATCH;
            break;

        default:
            if (c != FOLD((xpr_tchar_t)*n))
                return FNM_NOMATCH;
            break;
        }

        ++n;
    }

    if (*n == '\0')
        return FNM_MATCH;

    return FNM_NOMATCH;
}
