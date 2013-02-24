//
// Copyright (c) 2001-2013 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FX_ACCEL_TABLE_H__
#define __FX_ACCEL_TABLE_H__
#pragma once

class AccelTable
{
public:
    AccelTable(void);
    virtual ~AccelTable(void);

public:
    xpr_bool_t load(const xpr_tchar_t *aPath, ACCEL *aAccel, xpr_sint32_t *aCount, xpr_sint32_t aMaxCount);
    xpr_bool_t save(xpr_tchar_t *aPath, ACCEL *aAccel, xpr_sint32_t aCount) const;
};

#endif // __FX_ACCEL_TABLE_H__