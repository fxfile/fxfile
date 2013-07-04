//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "removable_device.h"

#include <dbt.h>
#include <winioctl.h>
#include "removable_drive.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace fxfile
{
struct RemovableDevice::Drive
{
    xpr_sint_t  mDeviceType;
    xpr_sint_t  mDeviceNumber;

    xpr_tchar_t mDriveChar;
    HANDLE      mDeviceFile;
    xpr_bool_t  mCloseDevice;
    HDEVNOTIFY  mDevNotify;
};

RemovableDevice::RemovableDevice(void)
    : mObserver(XPR_NULL)
{
}

RemovableDevice::~RemovableDevice(void)
{
    destroy();
}

BEGIN_MESSAGE_MAP(RemovableDevice, CWnd)
    ON_WM_DEVICECHANGE()
END_MESSAGE_MAP()

void RemovableDevice::create(void)
{
    CreateEx(0, AfxRegisterWndClass(CS_GLOBALCLASS), XPR_STRING_LITERAL(""), 0,0,0,0,0,0,0);

    scanRemovableDevice();
}

void RemovableDevice::destroy(void)
{
    Drive *sDrive;
    DriveMap::iterator sIterator;

    sIterator = mDriveMap.begin();
    for (; sIterator != mDriveMap.end(); ++sIterator)
    {
        sDrive = sIterator->second;

        if (XPR_IS_FALSE(sDrive->mCloseDevice))
        {
            ::CloseHandle(sDrive->mDeviceFile);
            sDrive->mCloseDevice = XPR_TRUE;
        }
        ::UnregisterDeviceNotification(sDrive->mDevNotify);

        delete sDrive;
    }

    mDriveMap.clear();

    DestroyWindow();
}

void RemovableDevice::registerObserver(RemovableDeviceObserver *aObserver)
{
    mObserver = dynamic_cast<RemovableDeviceObserver *>(aObserver);
}

void RemovableDevice::unregisterObserver(void)
{
    mObserver = XPR_NULL;
}

xpr_bool_t RemovableDevice::registerDeviceNotify(xpr_tchar_t aDriveChar)
{
    if (mDriveMap.find(aDriveChar) != mDriveMap.end())
        return XPR_FALSE;

    xpr_tchar_t sDrivePath[0xff] = {0};
    _stprintf(sDrivePath, XPR_STRING_LITERAL("\\\\?\\%c:"), aDriveChar);

    HANDLE sDeviceFile = ::CreateFile(
        sDrivePath,
        0, 
        FILE_SHARE_READ, 
        XPR_NULL, OPEN_EXISTING, XPR_NULL, XPR_NULL);

    if (sDeviceFile == INVALID_HANDLE_VALUE)
        return XPR_FALSE;

    STORAGE_DEVICE_NUMBER sStorageDeviceNumber = {0};
    DWORD sBytesReturned = 0;

    xpr_bool_t sResult = ::DeviceIoControl(
        sDeviceFile,
        IOCTL_STORAGE_GET_DEVICE_NUMBER,
        XPR_NULL,
        0,
        &sStorageDeviceNumber,
        sizeof(sStorageDeviceNumber),
        &sBytesReturned,
        XPR_NULL);

    if (XPR_IS_FALSE(sResult))
    {
        ::CloseHandle(sDeviceFile);
        return XPR_FALSE;
    }

    DEV_BROADCAST_HANDLE aDevBroadcastHandle = {0};
    aDevBroadcastHandle.dbch_size       = sizeof(aDevBroadcastHandle);
    aDevBroadcastHandle.dbch_devicetype = DBT_DEVTYP_HANDLE;
    aDevBroadcastHandle.dbch_handle     = sDeviceFile;

    HDEVNOTIFY sDevNotify = ::RegisterDeviceNotification(m_hWnd, &aDevBroadcastHandle, DEVICE_NOTIFY_WINDOW_HANDLE);
    if (XPR_IS_NULL(sDevNotify))
    {
        ::CloseHandle(sDeviceFile);
        ::CloseHandle(sDevNotify);
        return XPR_FALSE;
    }

    Drive *sDrive = new Drive;
    if (XPR_IS_NULL(sDrive))
    {
        ::CloseHandle(sDeviceFile);
        ::CloseHandle(sDevNotify);
        return XPR_FALSE;
    }

    sDrive->mDeviceType   = sStorageDeviceNumber.DeviceType;
    sDrive->mDeviceNumber = sStorageDeviceNumber.DeviceNumber;
    sDrive->mDriveChar    = aDriveChar;
    sDrive->mDeviceFile   = sDeviceFile;
    sDrive->mCloseDevice  = XPR_FALSE;
    sDrive->mDevNotify    = sDevNotify;

    mDriveMap.insert(DriveMap::value_type(aDriveChar, sDrive));

    return XPR_TRUE;
}

void RemovableDevice::scanRemovableDevice(void)
{
    RemovableDrive::DriveDeque sDriveDeque;
    RemovableDrive::instance().getRemovableDriveList(sDriveDeque);

    RemovableDrive::DriveDeque::iterator sIterator = sDriveDeque.begin();
    for (; sIterator != sDriveDeque.end(); ++sIterator)
    {
        xpr_tchar_t sDriveChar = *sIterator;
        registerDeviceNotify(sDriveChar);
    }
}

xpr_bool_t RemovableDevice::OnDeviceChange(xpr_uint_t nEventType, DWORD_PTR dwData)
{
    DEV_BROADCAST_HDR *sDevBroadcastHdr = (DEV_BROADCAST_HDR *)dwData;

    switch (nEventType)
    {
    case DBT_DEVICEARRIVAL:
        {
            OnDeviceArrival(sDevBroadcastHdr);
            break;
        }

    case DBT_DEVICEQUERYREMOVE:
        {
            if (OnDeviceQueryRemove(sDevBroadcastHdr) == XPR_FALSE)
                return BROADCAST_QUERY_DENY;
            break;
        }

    case DBT_DEVICEQUERYREMOVEFAILED:
        {
            OnDeviceQueryRemoveFailed(sDevBroadcastHdr);
            break;
        }

    case DBT_DEVICEREMOVECOMPLETE:
        {
            OnDeviceQueryRemoveComplete(sDevBroadcastHdr);
            break;
        }
    }

    return CWnd::OnDeviceChange(nEventType, dwData);
}

void RemovableDevice::OnDeviceArrival(void *aData)
{
    if (XPR_IS_NULL(aData))
        return;

    DEV_BROADCAST_HDR *sDevBroadcastHdr = (DEV_BROADCAST_HDR *)aData;

    if (sDevBroadcastHdr->dbch_devicetype == DBT_DEVTYP_VOLUME)
    {
        DEV_BROADCAST_VOLUME *sDevBroadcastVolume = (PDEV_BROADCAST_VOLUME)sDevBroadcastHdr;

        if (!XPR_TEST_BITS(sDevBroadcastVolume->dbcv_flags, DBTF_MEDIA) &&
            !XPR_TEST_BITS(sDevBroadcastVolume->dbcv_flags, DBTF_NET))
        {
            //
            // register device notification
            //
            xpr_sint_t sDriveMask;
            xpr_tchar_t sDriveChar;

            for (sDriveChar = XPR_STRING_LITERAL('A'), sDriveMask = 0x1; sDriveChar <= XPR_STRING_LITERAL('Z'); ++sDriveChar, sDriveMask <<= 1)
            {
                if (sDevBroadcastVolume->dbcv_unitmask & sDriveMask)
                {
                    if (RemovableDrive::instance().isRemovableDrive(sDriveChar))
                    {
                        registerDeviceNotify(sDriveChar);
                    }
                }
            }
        }
    }
}

xpr_bool_t RemovableDevice::OnDeviceQueryRemove(void *aData)
{
    if (XPR_IS_NULL(aData))
        return XPR_TRUE;

    xpr_bool_t sAllowRemove = XPR_TRUE;
    DEV_BROADCAST_HDR *sDevBroadcastHdr = (DEV_BROADCAST_HDR *)aData;

    if (sDevBroadcastHdr->dbch_devicetype == DBT_DEVTYP_HANDLE)
    {
        DEV_BROADCAST_HANDLE *sDevBroadcastHandle = (DEV_BROADCAST_HANDLE *)sDevBroadcastHdr;

        xpr_sint_t sDeviceType;
        xpr_sint_t sDeviceNumber = -1;
        Drive *sDrive;
        DriveMap::iterator sIterator;

        sIterator = mDriveMap.begin();
        for (; sIterator != mDriveMap.end(); ++sIterator)
        {
            sDrive = sIterator->second;
            if (sDrive->mDeviceFile == sDevBroadcastHandle->dbch_handle)
            {
                sDeviceType = sDrive->mDeviceType;
                sDeviceNumber = sDrive->mDeviceNumber;
                break;
            }
        }

        if (sDeviceNumber >= 0)
        {
            RemovableDeviceObserver::DriveSet sDriveSet;

            sIterator = mDriveMap.begin();
            for (; sIterator != mDriveMap.end(); ++sIterator)
            {
                sDrive = sIterator->second;
                if (sDrive->mDeviceNumber == sDeviceNumber && sDrive->mDeviceType == sDeviceType)
                    sDriveSet.insert(sDrive->mDriveChar);
            }

            if (XPR_IS_NOT_NULL(mObserver))
            {
                if (mObserver->OnQueryRemove(sDeviceNumber, sDriveSet) == XPR_FALSE)
                {
                    sAllowRemove = XPR_FALSE;
                }
            }

            if (XPR_IS_TRUE(sAllowRemove))
            {
                //
                // close only handle for device notification
                //
                sIterator = mDriveMap.begin();
                for (; sIterator != mDriveMap.end(); ++sIterator)
                {
                    sDrive = sIterator->second;
                    if (sDrive->mDeviceNumber == sDeviceNumber && sDrive->mDeviceType == sDeviceType)
                    {
                        ::CloseHandle(sDrive->mDeviceFile);
                        sDrive->mCloseDevice = XPR_TRUE;
                    }
                }
            }

            sDriveSet.clear();
        }
    }

    return sAllowRemove;
}

void RemovableDevice::OnDeviceQueryRemoveFailed(void *aData)
{
    if (XPR_IS_NULL(aData))
        return;

    DEV_BROADCAST_HDR *sDevBroadcastHdr = (DEV_BROADCAST_HDR *)aData;

    if (sDevBroadcastHdr->dbch_devicetype == DBT_DEVTYP_HANDLE)
    {
        DEV_BROADCAST_HANDLE *sDevBroadcastHandle = (DEV_BROADCAST_HANDLE *)sDevBroadcastHdr;

        xpr_sint_t sDeviceType;
        xpr_sint_t sDeviceNumber = -1;
        Drive *sDrive;
        DriveMap::iterator sIterator;

        sIterator = mDriveMap.begin();
        for (; sIterator != mDriveMap.end(); ++sIterator)
        {
            sDrive = sIterator->second;
            if (sDrive->mDeviceFile == sDevBroadcastHandle->dbch_handle)
            {
                sDeviceType = sDrive->mDeviceType;
                sDeviceNumber = sDrive->mDeviceNumber;
                break;
            }
        }

        if (sDeviceNumber >= 0)
        {
            //
            // re-register device notification
            //
            RemovableDeviceObserver::DriveSet sDriveSet;

            sIterator = mDriveMap.begin();
            while (sIterator != mDriveMap.end())
            {
                sDrive = sIterator->second;
                if (sDrive->mDeviceNumber == sDeviceNumber && sDrive->mDeviceType == sDeviceType)
                {
                    sDriveSet.insert(sDrive->mDriveChar);

                    if (XPR_IS_FALSE(sDrive->mCloseDevice))
                    {
                        ::CloseHandle(sDrive->mDeviceFile);
                        sDrive->mCloseDevice = XPR_TRUE;
                    }
                    ::UnregisterDeviceNotification(sDrive->mDevNotify);
                    delete sDrive;

                    sIterator = mDriveMap.erase(sIterator);
                    continue;
                }
                sIterator++;
            }

            RemovableDeviceObserver::DriveSet::iterator sDriveIterator = sDriveSet.begin();
            for (; sDriveIterator != sDriveSet.end(); ++sDriveIterator)
            {
                xpr_tchar_t sDriveChar = *sDriveIterator;
                registerDeviceNotify(sDriveChar);
            }

            if (XPR_IS_NOT_NULL(mObserver))
            {
                if (mObserver->OnQueryRemoveFailed(sDeviceNumber, sDriveSet) == XPR_TRUE)
                {
                }
            }
        }
    }
}

void RemovableDevice::OnDeviceQueryRemoveComplete(void *aData)
{
    if (XPR_IS_NULL(aData))
        return;

    DEV_BROADCAST_HDR *sDevBroadcastHdr = (DEV_BROADCAST_HDR *)aData;

    if (sDevBroadcastHdr->dbch_devicetype == DBT_DEVTYP_HANDLE)
    {
        DEV_BROADCAST_HANDLE *sDevBroadcastHandle = (DEV_BROADCAST_HANDLE *)sDevBroadcastHdr;

        xpr_sint_t sDeviceType;
        xpr_sint_t sDeviceNumber = -1;
        Drive *sDrive;
        DriveMap::iterator sIterator;

        sIterator = mDriveMap.begin();
        for (; sIterator != mDriveMap.end(); ++sIterator)
        {
            sDrive = sIterator->second;
            if (sDrive->mDeviceFile == sDevBroadcastHandle->dbch_handle)
            {
                sDeviceType = sDrive->mDeviceType;
                sDeviceNumber = sDrive->mDeviceNumber;
                break;
            }
        }

        if (sDeviceNumber >= 0)
        {
            //
            // unregister device notification
            //
            RemovableDeviceObserver::DriveSet sDriveSet;

            sIterator = mDriveMap.begin();
            for (; sIterator != mDriveMap.end(); ++sIterator)
            {
                sDrive = sIterator->second;
                if (sDrive->mDeviceNumber == sDeviceNumber && sDrive->mDeviceType == sDeviceType)
                    sDriveSet.insert(sDrive->mDriveChar);
            }

            sIterator = mDriveMap.begin();
            while (sIterator != mDriveMap.end())
            {
                sDrive = sIterator->second;
                if (sDrive->mDeviceNumber == sDeviceNumber && sDrive->mDeviceType == sDeviceType)
                {
                    if (XPR_IS_FALSE(sDrive->mCloseDevice))
                    {
                        ::CloseHandle(sDrive->mDeviceFile);
                        sDrive->mCloseDevice = XPR_TRUE;
                    }
                    ::UnregisterDeviceNotification(sDrive->mDevNotify);
                    delete sDrive;

                    sIterator = mDriveMap.erase(sIterator);
                    continue;
                }
                sIterator++;
            }

            if (XPR_IS_NOT_NULL(mObserver))
            {
                mObserver->OnRemoveComplete(sDeviceNumber, sDriveSet);
            }

            sDriveSet.clear();
        }
    }
    else if (sDevBroadcastHdr->dbch_devicetype == DBT_DEVTYP_VOLUME)
    {
        DEV_BROADCAST_VOLUME *sDevBroadcastVolume = (PDEV_BROADCAST_VOLUME)sDevBroadcastHdr;

        if (!XPR_TEST_BITS(sDevBroadcastVolume->dbcv_flags, DBTF_MEDIA) &&
            !XPR_TEST_BITS(sDevBroadcastVolume->dbcv_flags, DBTF_NET))
        {
            //
            // unregister device notification
            //
            xpr_sint_t sDriveMask;
            xpr_tchar_t sDriveChar;
            xpr_sint_t nDeviceNumber = -1;
            Drive *sDrive;
            DriveMap::iterator sIterator;
            RemovableDeviceObserver::DriveSet sDriveSet;

            for (sDriveChar = XPR_STRING_LITERAL('A'), sDriveMask = 0x1; sDriveChar <= XPR_STRING_LITERAL('Z'); ++sDriveChar, sDriveMask <<= 1)
            {
                if (sDevBroadcastVolume->dbcv_unitmask & sDriveMask)
                {
                    sIterator = mDriveMap.find(sDriveChar);
                    if (sIterator != mDriveMap.end())
                    {
                        sDrive = sIterator->second;

                        nDeviceNumber = sDrive->mDeviceNumber;

                        if (XPR_IS_FALSE(sDrive->mCloseDevice))
                        {
                            ::CloseHandle(sDrive->mDeviceFile);
                            sDrive->mCloseDevice = XPR_TRUE;
                        }
                        ::UnregisterDeviceNotification(sDrive->mDevNotify);
                        delete sDrive;

                        sIterator = mDriveMap.erase(sIterator);

                        sDriveSet.insert(sDriveChar);
                    }
                }
            }

            if (nDeviceNumber >= 0)
            {
                if (XPR_IS_NOT_NULL(mObserver))
                {
                    mObserver->OnRemoveComplete(nDeviceNumber, sDriveSet);
                }
            }

            sDriveSet.clear();
        }
    }
}
} // namespace fxfile
