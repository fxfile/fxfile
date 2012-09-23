//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#ifndef __FXB_REMOVABLE_DEVICE_H__
#define __FXB_REMOVABLE_DEVICE_H__
#pragma once

namespace fxb
{
class RemovableDevice;

class RemovableDeviceObserver
{
    friend class RemovableDevice;

public:
    typedef std::set<xpr_tchar_t> DriveSet;

public:
    RemovableDeviceObserver(void) {}
    virtual ~RemovableDeviceObserver(void) {}

protected:
    virtual xpr_bool_t OnQueryRemove(xpr_sint_t aDeviceNumber, const DriveSet &aDriveSet) = 0;
    virtual xpr_bool_t OnQueryRemoveFailed(xpr_sint_t aDeviceNumber, const DriveSet &aDriveSet) = 0;
    virtual void OnRemoveComplete(xpr_sint_t aDeviceNumber, const DriveSet &aDriveSet) = 0;
};

class RemovableDevice : public CWnd, public xpr::Singleton<RemovableDevice>
{
    friend class xpr::Singleton<RemovableDevice>;

protected: RemovableDevice(void);
public:   ~RemovableDevice(void);

public:
    void create(void);
    void destroy(void);

    void registerObserver(RemovableDeviceObserver *aObserver);
    void unregisterObserver(void);

protected:
    xpr_bool_t registerDeviceNotify(xpr_tchar_t aDriveChar);
    void scanRemovableDevice(void);

    void OnDeviceArrival(void *aData);
    xpr_bool_t OnDeviceQueryRemove(void *aData);
    void OnDeviceQueryRemoveFailed(void *aData);
    void OnDeviceQueryRemoveComplete(void *aData);

protected:
    struct Drive;

    typedef std::map<xpr_tchar_t, Drive *> DriveMap;
    DriveMap mDriveMap;

    RemovableDeviceObserver *mObserver;

protected:
    DECLARE_MESSAGE_MAP()
    afx_msg xpr_bool_t OnDeviceChange(xpr_uint_t nEventType, DWORD_PTR dwData);
};
} // namespace fxb

#endif // __FXB_REMOVABLE_DEVICE_H__
