//
// Copyright (c) 2012-2013 Leon Lee author. All rights reserved.
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "xpr_time.h"
#include "xpr_mutex.h"
#include "xpr_debug.h"
#include "xpr_system.h"
#include "xpr_math.h"

namespace xpr
{
#if defined(XPR_CFG_OS_WINDOWS)
namespace
{
const xpr_time_t k1601to1970Offset = XPR_SINT64_LITERAL(116444736000000000); // offset from Jan 1, 1601 (UTC) to Jan 1, 1970

const xpr_time_t kDeltaNanoUnit = 100; // FILETIME structure represent 100 nano-second unit

Mutex      gMutex; // TODO: change to spin lock
xpr_time_t gPrevTime = XPR_SINT64_LITERAL(0);
xpr_time_t gTimer1   = XPR_SINT64_LITERAL(0);
xpr_time_t gTimer2   = XPR_SINT64_LITERAL(0);

const xpr_sint_t kMonthTable    [12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
const xpr_sint_t kMonthTableLeap[12] = {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

xpr_sint_t getDaysSince1980(xpr_sint_t aYear, xpr_sint_t aMonth, xpr_sint_t aDay)
{
    if (aYear < 1980 || XPR_IS_OUT_OF_RANGE(1, aMonth, 12) || XPR_IS_OUT_OF_RANGE(1, aDay, 31))
    {
        return 0;
    }

    xpr_sint_t sDays = aDay;

    const xpr_sint_t *sMonthTable = isLeapYear(aYear) ? kMonthTableLeap : kMonthTable;

    xpr_sint_t i;
    for (i = 0; i < aMonth - 1; ++i)
    {
        sDays += sMonthTable[i];
    }

    for (i = 1980; i < aYear; ++i)
    {
        sDays += isLeapYear(i) ? 366 : 365;
    }

    return sDays;
}
} // namespace anonymous

XPR_INLINE void convertFileTimeToXprTime(const FILETIME &aFileTime, xpr_time_t &aTime)
{
    aTime  = aFileTime.dwHighDateTime;
    aTime  = aTime << 32;
    aTime |= aFileTime.dwLowDateTime;
    aTime -= k1601to1970Offset;
    aTime *= kDeltaNanoUnit;
}

XPR_INLINE void convertXprTimeToFileTime(const xpr_time_t &aTime, FILETIME &aFileTime)
{
    xpr_sint64_t sFileTime;

    sFileTime  = aTime;
    sFileTime /= kDeltaNanoUnit;
    sFileTime += k1601to1970Offset;

    aFileTime.dwLowDateTime  = (DWORD)sFileTime;
    aFileTime.dwHighDateTime = (DWORD)(sFileTime >> 32);
}

XPR_DL_API xpr_time_t getTimeNow(void)
{
    xpr_time_t sTimeNow;

    getTimeNow(sTimeNow);

    return sTimeNow;
}

XPR_DL_API void getTimeNow(xpr_time_t &aTimeNow)
{
    FILETIME sFileTime = {0};
    ::GetSystemTimeAsFileTime((LPFILETIME)&sFileTime);

    convertFileTimeToXprTime(sFileTime, aTimeNow);
}

XPR_DL_API void getTimeNow(TimeExpr &aTimeExprNow)
{
    xpr_time_t sTimeNow;
    getTimeNow(sTimeNow);

    xpr_bool_t sResult = toTimeExpr(sTimeNow, aTimeExprNow, 0);

    XPR_ASSERT(sResult == XPR_TRUE);
}

XPR_DL_API xpr_time_t getHighResTimeNow(void)
{
    xpr_time_t sTimeNow;

    getHighResTimeNow(sTimeNow);

    return sTimeNow;
}

XPR_DL_API void getHighResTimeNow(xpr_time_t &aTimeNow)
{
    getTimeNow(aTimeNow);

    // compensate for high resolution time
    {
        MutexGuard sMutexGuard(gMutex);

        if (gPrevTime != aTimeNow)
        {
            gTimer1 = timer_ns();
            gTimer2 = 0;
        }

        gPrevTime = aTimeNow;

        if (gTimer2 > 0)
        {
            xpr_sint64_t sDiff = gTimer2 - gTimer1;
            if (sDiff > 0)
            {
                aTimeNow += sDiff;
            }
        }

        gTimer2 = timer_ns();
    }
}

XPR_DL_API void getHighResTimeNow(TimeExpr &aTimeExprNow)
{
    xpr_time_t sTimeNow;
    getHighResTimeNow(sTimeNow);

    xpr_bool_t sResult = toTimeExpr(sTimeNow, aTimeExprNow, 0);

    XPR_ASSERT(sResult == XPR_TRUE);
}

XPR_DL_API xpr_time_t getLocalTimeNow(void)
{
    xpr_time_t sLocalTimeNow;

    getLocalTimeNow(sLocalTimeNow);

    return sLocalTimeNow;
}

XPR_DL_API void getLocalTimeNow(xpr_time_t &aLocalTimeNow)
{
    xpr_time_t sTimeNow;
    getTimeNow(sTimeNow);

    FILETIME sFileTime;
    FILETIME sLocalFileTime;

    convertXprTimeToFileTime(sTimeNow, sFileTime);

    if (getOsVer() >= xpr::kOsVerWinNT)
    {
        TIME_ZONE_INFORMATION sTimeZoneInformation;
        ::GetTimeZoneInformation(&sTimeZoneInformation);

        SYSTEMTIME sSystemTime;
        ::FileTimeToSystemTime(&sFileTime, &sSystemTime);

        SYSTEMTIME sLocalSystemTime;
        ::SystemTimeToTzSpecificLocalTime(&sTimeZoneInformation, &sSystemTime, &sLocalSystemTime);

        ::SystemTimeToFileTime(&sLocalSystemTime, &sLocalFileTime);

        // Because Win32 SYSTEMTIME structure represent milli-second unit.
        aLocalTimeNow += sTimeNow % kMillisecond;
    }
    else
    {
        ::FileTimeToLocalFileTime(&sFileTime, &sLocalFileTime);
    }

    convertFileTimeToXprTime(sLocalFileTime, aLocalTimeNow);
}

XPR_DL_API void getLocalTimeNow(TimeExpr &aLocalTimeExprNow)
{
    xpr_time_t sLocalTimeNow;
    getLocalTimeNow(sLocalTimeNow);

    xpr_bool_t sResult = toTimeExpr(sLocalTimeNow, aLocalTimeExprNow, 0);

    aLocalTimeExprNow.mUtcOffset = getTimeZoneOffset();

    XPR_ASSERT(sResult == XPR_TRUE);
}

XPR_DL_API void getTimeFromAnsiTime(time_t aAnsiTime, xpr_time_t &aTime)
{
    aTime = (xpr_time_t)aAnsiTime * kNanosecondPerSecond;
}

XPR_DL_API xpr_sint_t getTimeZoneOffset(void)
{
    TIME_ZONE_INFORMATION sTimeZoneInformation;
    ::GetTimeZoneInformation(&sTimeZoneInformation);

    return (xpr_sint_t)sTimeZoneInformation.Bias;
}

XPR_DL_API xpr_bool_t toTimeExpr(xpr_time_t aTime, TimeExpr &aTimeExpr, xpr_sint_t aOffset)
{
    xpr_bool_t sResult;
    FILETIME   sFileTime = {0};
    SYSTEMTIME sSystemTime = {0};

    convertXprTimeToFileTime(aTime + (aOffset * kNanosecondPerSecond), sFileTime);

    sResult = ::FileTimeToSystemTime(&sFileTime, &sSystemTime);
    if (XPR_IS_FALSE(sResult))
    {
        return XPR_FALSE;
    }

    aTimeExpr.mYear       = sSystemTime.wYear;
    aTimeExpr.mMonth      = sSystemTime.wMonth;
    aTimeExpr.mDay        = sSystemTime.wDay;
    aTimeExpr.mHour       = sSystemTime.wHour;
    aTimeExpr.mMinute     = sSystemTime.wMinute;
    aTimeExpr.mSecond     = sSystemTime.wSecond;
    aTimeExpr.mNanosecond = ((xpr_sint32_t)sSystemTime.wMilliseconds * kMillisecond) + (aTime % kMillisecond);

    aTimeExpr.mDayOfWeek  = getDayOfWeek(aTimeExpr.mYear, aTimeExpr.mMonth, aTimeExpr.mDay);
    aTimeExpr.mIsDst      = DstUnknown;
    aTimeExpr.mUtcOffset  = aOffset;

    return XPR_TRUE;
}

XPR_DL_API xpr_bool_t toTime(const TimeExpr &aTimeExpr, xpr_time_t &aTime)
{
    xpr_bool_t sResult;
    FILETIME   sFileTime = {0};
    SYSTEMTIME sSystemTime = {0};

    sSystemTime.wYear         = aTimeExpr.mYear;
    sSystemTime.wMonth        = aTimeExpr.mMonth;
    sSystemTime.wDay          = aTimeExpr.mDay;
    sSystemTime.wHour         = aTimeExpr.mHour;
    sSystemTime.wMinute       = aTimeExpr.mMinute;
    sSystemTime.wSecond       = aTimeExpr.mSecond;
    sSystemTime.wMilliseconds = (WORD)((aTimeExpr.mNanosecond % kSecond) / kMillisecond);

    sResult = ::SystemTimeToFileTime(&sSystemTime, &sFileTime);
    if (XPR_IS_FALSE(sResult))
    {
        return XPR_FALSE;
    }

    convertFileTimeToXprTime(sFileTime, aTime);

    aTime += aTimeExpr.mNanosecond % kMillisecond;
    aTime -= aTimeExpr.mUtcOffset * kNanosecondPerSecond;

    return XPR_TRUE;
}

XPR_DL_API xpr_bool_t isLeapYear(xpr_sint_t aYear)
{
    xpr_bool_t sLeap = XPR_FALSE;

    if ((aYear % 4) == 0)
    {
        sLeap = XPR_TRUE;

        if ((aYear % 100) == 0)
        {
            sLeap = (aYear % 400) ? XPR_FALSE : XPR_TRUE;
        }
    }

    return sLeap;
}

XPR_DL_API xpr_bool_t isValidDate(xpr_sint_t aYear, xpr_sint_t aMonth, xpr_sint_t aDay)
{
    xpr_sint_t sDays = getDays(aYear, aMonth);

    if (sDays == 0 || aDay > sDays)
    {
        return XPR_FALSE;
    }

    return XPR_TRUE;
}

XPR_DL_API xpr_sint_t getDayOfWeek(xpr_sint_t aYear, xpr_sint_t aMonth, xpr_sint_t aDay)
{
    xpr_size_t sDays = getDaysSince1980(aYear, aMonth, aDay);

    return ((sDays - 1) + 2) % 7;
}

XPR_DL_API xpr_sint_t getWeekOfMonth(xpr_sint_t aYear, xpr_sint_t aMonth, xpr_sint_t aDay)
{
    xpr_sint_t sFirstDayOfWeek = getDayOfWeek(aYear, aMonth, 1);
    if (sFirstDayOfWeek > 6)
    {
        return 0;
    }

    xpr_sint_t sLastDayFirstWeek = 7 - sFirstDayOfWeek;
    if (aDay <= sLastDayFirstWeek)
    {
        return 1;
    }

    xpr_sint_t sFirstDaySecondWeek = sLastDayFirstWeek + 1;

    return ((aDay - sFirstDaySecondWeek) / 7) + 2;
}

XPR_DL_API xpr_sint_t getDays(xpr_sint_t aYear, xpr_sint_t aMonth)
{
    if (XPR_IS_OUT_OF_RANGE(1, aMonth, 12))
    {
        return 0;
    }

    const xpr_sint_t *sMonthTable = isLeapYear(aYear) ? kMonthTableLeap : kMonthTable;

    return sMonthTable[aMonth - 1];
}

XPR_DL_API xpr_sint_t getDays(xpr_sint_t aYear1, xpr_sint_t aMonth1, xpr_sint_t aDay1, xpr_sint_t aYear2, xpr_sint_t aMonth2, xpr_sint_t aDay2)
{
    xpr_sint_t sDays1 = getDaysSince1980(aYear1, aMonth1, aDay1);
    xpr_sint_t sDays2 = getDaysSince1980(aYear2, aMonth2, aDay2);

    if (sDays1 > sDays2)
    {
        return 0;
    }

    return ((sDays2 - sDays1) + 1);
}

XPR_DL_API xpr_bool_t prevYear(xpr_sint_t &aYear)
{
    if (aYear == 0)
    {
        return XPR_FALSE;
    }

    --aYear;

    return XPR_TRUE;
}

XPR_DL_API xpr_bool_t nextYear(xpr_sint_t &aYear)
{
    ++aYear;

    return XPR_TRUE;
}

XPR_DL_API xpr_bool_t prevMonth(xpr_sint_t &aYear, xpr_sint_t &aMonth)
{
    if (aYear == 0 || XPR_IS_OUT_OF_RANGE(1, aMonth, 12))
    {
        return XPR_FALSE;
    }

    if (aMonth == 1)
    {
        --aYear;
        aMonth = 12;
    }
    else
    {
        --aMonth;
    }

    return XPR_TRUE;
}

XPR_DL_API xpr_bool_t nextMonth(xpr_sint_t &aYear, xpr_sint_t &aMonth)
{
    if (aYear == 0 || XPR_IS_OUT_OF_RANGE(1, aMonth, 12))
    {
        return XPR_FALSE;
    }

    if (aMonth == 12)
    {
        ++aYear;
        aMonth = 1;
    }
    else
    {
        ++aMonth;
    }

    return XPR_TRUE;
}

XPR_DL_API xpr_bool_t prevDay(xpr_sint_t &aYear, xpr_sint_t &aMonth, xpr_sint_t &aDay)
{
    if (aYear == 0 || XPR_IS_OUT_OF_RANGE(1, aMonth, 12))
    {
        return XPR_FALSE;
    }

    const xpr_sint_t *sMonthTable = isLeapYear(aYear) ? kMonthTableLeap : kMonthTable;

    if (XPR_IS_OUT_OF_RANGE(1, aDay, sMonthTable[aMonth - 1]))
    {
        return FALSE;
    }

    if (aDay == 1)
    {
        if (aMonth == 1)
        {
            --aYear;
            aMonth = 12;

            sMonthTable = isLeapYear(aYear) ? kMonthTableLeap : kMonthTable;

            aDay = sMonthTable[aMonth - 1];
        }
        else
        {
            --aMonth;
            aDay = sMonthTable[aMonth - 1];
        }
    }
    else
    {
        --aDay;
    }

    return TRUE;
}

XPR_DL_API xpr_bool_t nextDay(xpr_sint_t &aYear, xpr_sint_t &aMonth, xpr_sint_t &aDay)
{
    const xpr_sint_t *sMonthTable = isLeapYear(aYear) ? kMonthTableLeap : kMonthTable;

    if (XPR_IS_OUT_OF_RANGE(1, aDay, sMonthTable[aMonth - 1]))
    {
        return XPR_FALSE;
    }

    if (aDay == sMonthTable[aMonth - 1])
    {
        if (aMonth == 12)
        {
            ++aYear;
            aMonth = 1;
            aDay = 1;
        }
        else
        {
            ++aMonth;
            aDay = 1;
        }
    }
    else
    {
        ++aDay;
    }

    return XPR_TRUE;
}

class HighResTimer
{
public:
    HighResTimer(void)
        : mFreq(0)
        , mEnable(XPR_FALSE)
    {
        mEnable = ::QueryPerformanceFrequency((LARGE_INTEGER *)&mFreq);
    }

public:
    xpr_sint64_t mFreq;
    xpr_bool_t   mEnable;
};

namespace
{
HighResTimer gHighResTimer; // TODO: change to thread once
} // namespace anonymous

#define XPR_CALC_HIGH_RES_TIMER(aCur, aFreq, aUnit) \
    (((aCur / aFreq) * aUnit) + ((aCur % aFreq) * aUnit / aFreq))

XPR_DL_API xpr_time_t timer_ms(void)
{
    const xpr_sint64_t kMillisecondUnit = 1000;

    xpr_sint64_t sTimer = 0;
    ::QueryPerformanceCounter((LARGE_INTEGER *)&sTimer);

    return XPR_CALC_HIGH_RES_TIMER(sTimer, gHighResTimer.mFreq, kMillisecondUnit);
}

XPR_DL_API xpr_time_t timer_us(void)
{
    const xpr_sint64_t kMicrosecondUnit = 1000 * 1000;

    xpr_sint64_t sTimer = 0;
    ::QueryPerformanceCounter((LARGE_INTEGER *)&sTimer);

    return XPR_CALC_HIGH_RES_TIMER(sTimer, gHighResTimer.mFreq, kMicrosecondUnit);
}

XPR_DL_API xpr_time_t timer_ns(void)
{
    static const xpr_sint64_t kNanosecondUnit = 1000 * 1000 * 1000;

    xpr_sint64_t sTimer = 0;
    ::QueryPerformanceCounter((LARGE_INTEGER *)&sTimer);

    return XPR_CALC_HIGH_RES_TIMER(sTimer, gHighResTimer.mFreq, kNanosecondUnit);
}
#endif
} // namespace xpr
