//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXFILE_WNET_MGR_H__
#define __FXFILE_WNET_MGR_H__ 1
#pragma once

#include "pattern.h"

namespace fxfile
{
class WnetMgr : public fxfile::base::Singleton<WnetMgr>
{
    friend class fxfile::base::Singleton<WnetMgr>;

protected: WnetMgr(void);
public:   ~WnetMgr(void);

public:
    DWORD connectNetDrive(HWND aHwnd);
    DWORD disconnectNetDrive(HWND aHwnd);

    xpr_bool_t checkServer(const xpr_tchar_t *aServer);
    xpr_bool_t getNetDesc(const xpr_tchar_t *aServer, xpr_tchar_t *aDesc);

    xpr_bool_t isConnectedServer(const xpr::tstring &aNetPath);

    void getNetFullPath(xpr::tstring &aFullPath);

protected:
    xpr_bool_t mSockInit;
};
} // namespace fxfile

#endif // __FXFILE_WNET_MGR_H__
