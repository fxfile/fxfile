//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_DRIVE_SHCN_H__
#define __FXFILE_DRIVE_SHCN_H__ 1
#pragma once

#include "pattern.h"

namespace fxfile
{
class DriveShcn : public CWnd, public fxfile::base::Singleton<DriveShcn>
{
    friend class fxfile::base::Singleton<DriveShcn>;

protected: DriveShcn(void);
public:   ~DriveShcn(void);

public:
    void create(void);
    void destroy(void);

    void start(void);
    void stop(void);

public:
    void registerWatch(HWND aHwnd, xpr_uint_t aMsg, xpr_slong_t aEventMask);
    void registerWatch(CWnd *aWnd, xpr_uint_t aMsg, xpr_slong_t aEventMask);
    void unregisterWatch(HWND aHwnd);
    void unregisterWatch(CWnd *aWnd);
    xpr_bool_t isRegisteredWatch(HWND aHwnd);
    xpr_bool_t isRegisteredWatch(CWnd *aWnd);

protected:
    typedef struct NotifyInfo
    {
        xpr_uint_t  mMsg;
        xpr_slong_t mEventMask;
    } NotifyInfo;

    typedef std::map<HWND, NotifyInfo *> DriveShcnMap;
    DriveShcnMap mDriveShcnMap;

    xpr_uint_t mShcnId;

protected:
    DECLARE_MESSAGE_MAP()
    afx_msg LRESULT OnShellChangeNotify(WPARAM wParam, LPARAM lParam);
};
} // namespace fxfile

#endif // __FXFILE_DRIVE_SHCN_H__
