//
// Copyright (c) 2012 Leon Lee author. All rights reserved.
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __XPR_TIME_H__
#define __XPR_TIME_H__
#pragma once

#include "xpr_types.h"

typedef xpr_sint64_t xpr_time_t; // nano-second unit, from Jan 1, 1970

namespace xpr
{
const xpr_time_t kMicrosecond = 1000;
const xpr_time_t kMillisecond = kMicrosecond * 1000;
const xpr_time_t kSecond      = kMillisecond * 1000;
const xpr_time_t kMinute      = kSecond * 60;
const xpr_time_t kHour        = kMinute * 60;
const xpr_time_t kDay         = kHour * 24;
const xpr_time_t kWeek        = kDay * 7;

enum
{
    DayOfWeekNone = -1,
    DayOfWeekSunday = 0,
    DayOfWeekMonday,
    DayOfWeekTuesday,
    DayOfWeekWednesday,
    DayOfWeekThursday,
    DayOfWeekFriday,
    DayOfWeekSaturday,
};

class Time
{
public:
    xpr_sint16_t mYear;
    xpr_sint16_t mMonth;
    xpr_sint16_t mDay;
    xpr_sint16_t mDayOfWeek;
    xpr_sint16_t mHour;
    xpr_sint16_t mMinute;
    xpr_sint16_t mSecond;
    xpr_sint32_t mNanosecond;
    xpr_bool_t   mDst;
};

const Time kInitTime = {0, 0, 0, DayOfWeekNone, 0, 0, 0, 0, XPR_FALSE};

xpr_time_t timer_ms(void);
xpr_time_t timer_us(void);
xpr_time_t timer_ns(void);
} // namespace xpr

#endif // __XPR_TIME_H__
