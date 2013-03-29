//
// Copyright (c) 2012-2013 Leon Lee author. All rights reserved.
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __XPR_TIME_H__
#define __XPR_TIME_H__
#pragma once

#include "xpr_types.h"
#include "xpr_dlsym.h"
#include "xpr_math.h"

typedef xpr_sint64_t xpr_time_t; // nano-second unit, from Jan 1, 1970

namespace xpr
{
const xpr_time_t kNanosecond  = 1;
const xpr_time_t kMicrosecond = 1000 * kNanosecond;
const xpr_time_t kMillisecond = 1000 * kMicrosecond;
const xpr_time_t kSecond      = 1000 * kMillisecond;
const xpr_time_t kMinute      =   60 * kSecond;
const xpr_time_t kHour        =   60 * kMinute;
const xpr_time_t kDay         =   24 * kHour;
const xpr_time_t kWeek        =    7 * kDay;
const xpr_time_t kYear        =  365 * kDay;

const xpr_time_t kNanosecondPerSecond = kSecond;

enum
{
    kSunday = 0,
    kMonday,
    kTuesday,
    kWednesday,
    kThursday,
    kFriday,
    kSaturday,
};

enum
{
    kJaunuary = 0,
    kFebruary,
    kMarch,
    kApril,
    kMay,
    kJune,
    kJuly,
    kAugust,
    kSeptember,
    kOctober,
    kNovember,
    kDecember,
};

class TimeExpr
{
public:
    inline xpr_sint_t getMicrosecond(void) const
    {
        return mNanosecond / kMicrosecond;
    }

    inline xpr_sint_t getMillisecond(void) const
    {
        return mNanosecond / kMillisecond;
    }

    inline xpr_bool_t validate(void) const
    {
        if (XPR_IS_OUT_OF_RANGE(0, mMonth,                   11) ||
            XPR_IS_OUT_OF_RANGE(1, mDay,                     31) ||
            XPR_IS_OUT_OF_RANGE(0, mHour,                    23) ||
            XPR_IS_OUT_OF_RANGE(0, mMinute,                  59) ||
            XPR_IS_OUT_OF_RANGE(0, mSecond,                  59) ||
            XPR_IS_OUT_OF_RANGE(0, mNanosecond, kNanosecond - 1))
        {
            return XPR_FALSE;
        }

        return XPR_TRUE;
    }

public:
    xpr_sint_t mYear;
    xpr_sint_t mMonth;
    xpr_sint_t mDay;
    xpr_sint_t mDayOfWeek;
    xpr_sint_t mHour;
    xpr_sint_t mMinute;
    xpr_sint_t mSecond;
    xpr_sint_t mNanosecond;
    xpr_bool_t mIsDst;       // daylight saving time
    xpr_sint_t mGmtOff;      // seconds east of UTC
};

const TimeExpr kInitTimeExpr = {0, 0, 0, 0, 0, 0, 0, 0, XPR_FALSE};

XPR_DL_API xpr_time_t getTimeNow(void);
XPR_DL_API void getTimeNow(xpr_time_t &aTimeNow);
XPR_DL_API void getTimeNow(TimeExpr &aTimeExprNow);

XPR_DL_API xpr_time_t getHighResTimeNow(void);
XPR_DL_API void getHighResTimeNow(xpr_time_t &aTimeNow);
XPR_DL_API void getHighResTimeNow(TimeExpr &aTimeExprNow);

XPR_DL_API xpr_time_t getLocalTimeNow(void);
XPR_DL_API void getLocalTimeNow(xpr_time_t &aLocalTimeNow);
XPR_DL_API void getLocalTimeNow(TimeExpr &aLocalTimeExprNow);

XPR_DL_API void getTimeFromAnsiTime(time_t aAnsiTime, xpr_time_t &aTime);

XPR_DL_API xpr_bool_t toTimeExpr(xpr_time_t aTime, TimeExpr &aTimeExpr, xpr_sint_t aOffset);
XPR_DL_API xpr_bool_t toTime(const TimeExpr &aTimeExpr, xpr_time_t &aTime);

XPR_DL_API xpr_bool_t isLeapYear(xpr_sint_t aYear);
XPR_DL_API xpr_bool_t isValidDate(xpr_sint_t aYear, xpr_sint_t aMonth, xpr_sint_t aDay);

XPR_DL_API xpr_sint_t getDayOfWeek(xpr_sint_t aYear, xpr_sint_t aMonth, xpr_sint_t aDay);
XPR_DL_API xpr_sint_t getWeekOfMonth(xpr_sint_t aYear, xpr_sint_t aMonth, xpr_sint_t aDay);

XPR_DL_API xpr_sint_t getDays(xpr_sint_t aYear, xpr_sint_t aMonth);
XPR_DL_API xpr_sint_t getDays(xpr_sint_t aYear1, xpr_sint_t aMonth1, xpr_sint_t aDay1,
                              xpr_sint_t aYear2, xpr_sint_t aMonth2, xpr_sint_t aDay2);

XPR_DL_API xpr_bool_t prevYear(xpr_sint_t &aYear);
XPR_DL_API xpr_bool_t nextYear(xpr_sint_t &aYear);
XPR_DL_API xpr_bool_t prevMonth(xpr_sint_t &aYear, xpr_sint_t &aMonth);
XPR_DL_API xpr_bool_t nextMonth(xpr_sint_t &aYear, xpr_sint_t &aMonth);
XPR_DL_API xpr_bool_t prevDay(xpr_sint_t &aYear, xpr_sint_t &aMonth, xpr_sint_t &aDay);
XPR_DL_API xpr_bool_t nextDay(xpr_sint_t &aYear, xpr_sint_t &aMonth, xpr_sint_t &aDay);

XPR_DL_API xpr_time_t timer_ms(void);
XPR_DL_API xpr_time_t timer_us(void);
XPR_DL_API xpr_time_t timer_ns(void);
} // namespace xpr

#endif // __XPR_TIME_H__
