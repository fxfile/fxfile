//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXB_WNET_MGR_H__
#define __FXB_WNET_MGR_H__
#pragma once

namespace fxb
{
class WnetMgr : public xpr::Singleton<WnetMgr>
{
    friend class xpr::Singleton<WnetMgr>;

protected: WnetMgr(void);
public:   ~WnetMgr(void);

public:
    DWORD connectNetDrive(HWND aHwnd);
    DWORD disconnectNetDrive(HWND aHwnd);

    xpr_bool_t checkServer(const xpr_tchar_t *aServer);
    xpr_bool_t getNetDesc(const xpr_tchar_t *aServer, xpr_tchar_t *aDesc);

    xpr_bool_t isConnectedServer(const std::tstring &aNetPath);

    void getNetFullPath(std::tstring &aFullPath);

protected:
    xpr_bool_t mSockInit;
};
} // namespace fxb

#endif // __FXB_WNET_MGR_H__
