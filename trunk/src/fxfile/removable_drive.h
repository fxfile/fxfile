//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_REMOVABLE_DRIVE_H__
#define __FXFILE_REMOVABLE_DRIVE_H__ 1
#pragma once

#include "pattern.h"

namespace fxfile
{
class RemovableDrive : public fxfile::base::Singleton<RemovableDrive>
{
    friend class fxfile::base::Singleton<RemovableDrive>;

protected: RemovableDrive(void);
public:   ~RemovableDrive(void);

public:
    typedef std::deque<xpr_tchar_t> DriveDeque;

    enum Result
    {
        ResultNone,
        ResultSucceeded,
        ResultUnknownError,
        ResultInvalidParameter,
        ResultRemoveVetoed,
    };

public:
    xpr_bool_t isRemovableDrive(xpr_tchar_t aDriveChar);
    void getRemovableDriveList(DriveDeque &aDriveDeque);
    Result removeSafetyRemovableDrive(xpr_tchar_t aDriveChar);
};
} // namespace fxfile

#endif // __FXFILE_REMOVABLE_DRIVE_H__
