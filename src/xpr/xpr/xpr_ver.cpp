//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "xpr_ver.h"
#include "xpr_debug.h"

namespace xpr
{
namespace
{
const xpr_tchar_t kBuildDate[] = XPR_DATE;
const xpr_tchar_t kBuildTime[] = XPR_TIME;
} // namespace anonymous

XPR_DL_API void getVer(Ver &aVer)
{
    aVer.mMajor = XPR_MAJOR_VER;
    aVer.mMinor = XPR_MINOR_VER;
    aVer.mPatch = XPR_PATCH_VER;
}

XPR_DL_API void getVer(xpr::string &aVer)
{
    aVer = XPR_VER_STRING;
}

XPR_DL_API void getBuildTime(TimeExpr &aTimeExpr)
{
    xpr_tchar_t sMonthString[5] = {0,};
    xpr_sint_t  sYear   = 0;
    xpr_sint_t  sMonth  = 0;
    xpr_sint_t  sDay    = 0;
    xpr_sint_t  sHour   = 0;
    xpr_sint_t  sMinute = 0;
    xpr_sint_t  sSecond = 0;
    _stscanf(kBuildDate, XPR_STRING_LITERAL("%s %d %d"), sMonthString, &sDay, &sYear);

    if      (_tcscmp(sMonthString, XPR_STRING_LITERAL("Jan")) == 0) sMonth = 1;
    else if (_tcscmp(sMonthString, XPR_STRING_LITERAL("Feb")) == 0) sMonth = 2;
    else if (_tcscmp(sMonthString, XPR_STRING_LITERAL("Mar")) == 0) sMonth = 3;
    else if (_tcscmp(sMonthString, XPR_STRING_LITERAL("Apr")) == 0) sMonth = 4;
    else if (_tcscmp(sMonthString, XPR_STRING_LITERAL("May")) == 0) sMonth = 5;
    else if (_tcscmp(sMonthString, XPR_STRING_LITERAL("Jun")) == 0) sMonth = 6;
    else if (_tcscmp(sMonthString, XPR_STRING_LITERAL("Jul")) == 0) sMonth = 7;
    else if (_tcscmp(sMonthString, XPR_STRING_LITERAL("Aug")) == 0) sMonth = 8;
    else if (_tcscmp(sMonthString, XPR_STRING_LITERAL("Sep")) == 0) sMonth = 9;
    else if (_tcscmp(sMonthString, XPR_STRING_LITERAL("Oct")) == 0) sMonth = 10;
    else if (_tcscmp(sMonthString, XPR_STRING_LITERAL("Nov")) == 0) sMonth = 11;
    else if (_tcscmp(sMonthString, XPR_STRING_LITERAL("Dec")) == 0) sMonth = 12;
    else
    {
        XPR_ASSERT(0);
    }

    _stscanf(kBuildTime, XPR_STRING_LITERAL("%d:%d:%d"), &sHour, &sMinute, &sSecond);

    aTimeExpr.mYear       = sYear;
    aTimeExpr.mMonth      = sMonth;
    aTimeExpr.mDay        = sDay;
    aTimeExpr.mDayOfWeek  = getDayOfWeek(sYear, sMonth, sDay);
    aTimeExpr.mHour       = sHour;
    aTimeExpr.mMinute     = sMinute;
    aTimeExpr.mSecond     = sSecond;
    aTimeExpr.mNanosecond = 0;
    aTimeExpr.mIsDst      = DstUnknown;
    aTimeExpr.mUtcOffset  = getTimeZoneOffset();
}

XPR_DL_API void getBuildTime(xpr::string &aBuildTime)
{
    aBuildTime  = kBuildDate;
    aBuildTime += XPR_STRING_LITERAL(" ");
    aBuildTime += kBuildTime;
}
} // namespace xpr
