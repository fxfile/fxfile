//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "fxb_string.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace fxb
{
xpr_bool_t IsEmptyString(const xpr_tchar_t *aString)
{
    if (XPR_IS_NULL(aString))
        return XPR_TRUE;

    return aString[0] == XPR_STRING_LITERAL('\0');
}

void EmptyString(xpr_tchar_t *aString)
{
    if (XPR_IS_NULL(aString))
        return;

    aString[0] = XPR_STRING_LITERAL('\0');
}

void GetFormatedNumber(xpr_uint64_t aNumber, xpr_tchar_t *aFormatedNumber, xpr_size_t aMaxLen)
{
    if (XPR_IS_NULL(aFormatedNumber) || aMaxLen <= 0)
        return;

    NUMBERFMT sNumberFmt = {0};
    sNumberFmt.NumDigits     = 0;
    sNumberFmt.LeadingZero   = 0;
    sNumberFmt.Grouping      = 3;
    sNumberFmt.lpDecimalSep  = XPR_STRING_LITERAL(".");
    sNumberFmt.lpThousandSep = XPR_STRING_LITERAL(",");
    sNumberFmt.NegativeOrder = 0;

    xpr_tchar_t sNumberText[0xff] = {0};
    _stprintf(sNumberText, XPR_STRING_LITERAL("%I64u"), aNumber);

    ::GetNumberFormat(XPR_NULL, XPR_NULL, sNumberText, &sNumberFmt, aFormatedNumber, (xpr_sint_t)aMaxLen);
}
} // namespace fxb
